#ifndef _WIN32
#include "OT/OTExtensionBristol.hpp"


void OTExtensionBristolBase::init(const string& senderAddress, int port, int my_num, bool isSemiHonest, const shared_ptr<CommParty> & channel)
{

	this->channel = channel;

	OT_ROLE ot_role;

	if (my_num == 0)
		ot_role = SENDER;
	else
		ot_role = RECEIVER;


	//Set the host names. The sender is the listener.
	vector<string> names(2);
	names[my_num] = "localhost";
	names[1-my_num] = senderAddress;
	pParty.reset(new TwoPartyPlayer(Names(my_num, 0, names), 1 - my_num, port));

	//init the base OT with 128 ot's with 128 bit length for the relevant role.
	BaseOT baseOT(128, 128, 1 - my_num, pParty.get(), INV_ROLE(ot_role));

	//run the base OT
	baseOT.exec_base();

	BitVector baseReceiverInput(128);
	for (int i = 0; i < 128; i++)
	{
		baseReceiverInput.set_bit(i, baseOT.receiver_inputs[i]);
	}


	//set the unique pointer to t he ot extension object.
	pOtExt.reset(new OTExtensionWithMatrix(128, baseOT.length(),
									   1, 1,
									   pParty.get(),
									   baseReceiverInput,
									   baseOT.sender_inputs,
									   baseOT.receiver_outputs,
									   ot_role,
									   isSemiHonest));
}




void OTExtensionBristolBase::transfer(int nOTs, const BitVector& receiverInput) {

	//call the transfer using the OT extension object of the underlying library.
	pOtExt->transfer(nOTs, receiverInput);

}

void OTExtensionBristolBase::shrinkArray(int sourceElementSize, int targetElementSize, int numOfElements, byte* srcArr, byte* targetArr){

    //for each element, copy the required bytes.
    for(int i=0; i<numOfElements; i++){

        for(int j=0; j<targetElementSize/8; j++){

            //copy only the needed data to the smaller array
            targetArr[targetElementSize*i/8 + j] = srcArr[sourceElementSize*i/8 + j];
        }
    }
}

OTExtensionBristolSender::OTExtensionBristolSender(int port, bool isSemiHonest,
                                                   const shared_ptr<CommParty> &channel) {

	//Call the init of the base class. The host name is hard coded to localhost since the sender is the  listener.
	init("localhost", port, 0, isSemiHonest, channel);
}


shared_ptr<OTBatchSOutput> OTExtensionBristolSender::transfer(OTBatchSInput * input){

	if(input->getType()!= OTBatchSInputTypes::OTExtensionRandomizedSInput &&
	   input->getType()!= OTBatchSInputTypes::OTExtensionGeneralSInput &&
	   input->getType()!= OTBatchSInputTypes::OTExtensionCorrelatedSInput){
		throw invalid_argument("input should be instance of OTExtensionRandomizedSInput or OTExtensionGeneralSInput or OTExtensionCorrelatedSInput.");
	}
	else {
        int nOTsReal, elementSize;

        //get the number OTs and element size from the input object.
        //need to cast to the right class according to the type.
        if (input->getType() == OTBatchSInputTypes::OTExtensionGeneralSInput) {

            nOTsReal = ((OTExtensionGeneralSInput *) input)->getNumOfOts();
            elementSize = 8 * (((OTExtensionGeneralSInput *) input)->getX0Arr().size() / nOTsReal);

        } else if (input->getType() == OTBatchSInputTypes::OTExtensionRandomizedSInput) {

            nOTsReal = ((OTExtensionRandomizedSInput *) input)->getNumOfOts();
            elementSize = ((OTExtensionRandomizedSInput *) input)->getElementSize();

        } else {

            nOTsReal = ((OTExtensionCorrelatedSInput *) input)->getNumOfOts();
            elementSize = 8 * (((OTExtensionCorrelatedSInput *) input)->getDeltaArr().size() / nOTsReal);
        }

        //round to the nearest 128 multiplication
        int nOTs = ((nOTsReal + 128 - 1) / 128) * 128;
        //number of 128 bit ot's needed.
        int factor = (elementSize + 127) / 128;

        //we create a bitvector since the transfer of the bristol library demands that. There is no use of it and thus
        //we do not require that the user inputs that.
        BitVector receiverInput(nOTs);

        //call the base class transfer that eventually calls the ot extension of the bristol library
        OTExtensionBristolBase::transfer(nOTs, receiverInput);

        /* Convert the result of the transfer function to the required size */
        int size = elementSize / 8 * nOTsReal;
        vector<byte> aesX0(size);
        vector<byte> aesX1(size);

        //There is no need to change the element size, copy only the required OTs
        if (elementSize == 128){
            copy_byte_array_to_byte_vector((byte*) pOtExt->senderOutputMatrices[0].squares.data(), nOTsReal*16, aesX0, 0);
            copy_byte_array_to_byte_vector((byte*) pOtExt->senderOutputMatrices[1].squares.data(), nOTsReal*16, aesX1, 0);

        //The required size is smaller than the output. Shrink x0 and x1.
        } else if (elementSize < 128) {

            shrinkArray(128, elementSize, nOTsReal, (byte*) pOtExt->senderOutputMatrices[0].squares.data(), aesX0.data());
            shrinkArray(128, elementSize, nOTsReal, (byte*) pOtExt->senderOutputMatrices[1].squares.data(), aesX1.data());

        //The required size is bigger than the output. Expand x0 and x1.
        } else {

            byte * counters = createCountersArray(factor);

            auto aes = new EVP_CIPHER_CTX();
            auto output = new byte[factor * 16];
            for (int i=0; i < nOTsReal; i++){
                //Expand the x0[i] to the required size.
                //Put the result in x0.
                expandOutput(elementSize, (byte*) &pOtExt->senderOutputMatrices[0].squares[i/128].rows[i % 128],
                             aesX0, factor, counters, aes, output, i);

                //Expand the x0[i] to the required size.
                //Put the result in x0.
                expandOutput(elementSize, (byte*) &pOtExt->senderOutputMatrices[1].squares[i/128].rows[i % 128],
                             aesX1, factor, counters, aes, output, i);
            }
            delete [] counters;
            delete [] output;
            delete aes;
        }

        //If this if the general case we need another round of communication using the channel member, since OT bristol only works on random case.
		if(input->getType()== OTBatchSInputTypes::OTExtensionGeneralSInput) {

            if (channel == NULL) {
                throw runtime_error("In order to execute a general ot extension the channel must be given");
            }

            auto x0Vec = ((OTExtensionGeneralSInput *) input)->getX0Arr();
            auto x1Vec = ((OTExtensionGeneralSInput *) input)->getX1Arr();

            //Xor the given x0 and x1 with the OT output
            for (int i = 0; i < size; i++) {

                aesX0[i] = x0Vec[i] ^ aesX0[i];
                aesX1[i] = x1Vec[i] ^ aesX1[i];
            }

            //send the xored arrrays over the channel.
            channel->write(aesX0.data(), size);
            channel->write(aesX1.data(), size);

            return nullptr;

        //If this if the correlated case we need another round of communication using the channel member, since OT bristol only works on random case.
        } else if (input->getType()== OTBatchSInputTypes::OTExtensionCorrelatedSInput){

			if (channel == NULL) {
				throw runtime_error("In order to execute a correlated ot extension the channel must be given");
			}

			auto delta = ((OTExtensionCorrelatedSInput *)input)->getDeltaArr();

            vector<byte> newX1(size);
            byte* newDelta = new byte[size];

            //X0 = x0
            //X1 - delta ^ x0
			for(int i=0; i<size; i++){

                newX1[i] = delta[i] ^ aesX0[i];

				//we use delta in order not to create an additional array
                newDelta[i] =  newX1[i] ^ aesX1[i];
			}

			//send R1^R0^delta over the channel.
			channel->write(newDelta, size);
            delete [] newDelta;

			//the output for the sender is x0 and x0^delta
			return make_shared<OTExtensionCorrelatedSOutput>(aesX0, newX1);

		}

		else{
			//return a shared pointer of the output as it taken from the ot object of the library
			return make_shared<OTExtensionBristolRandomizedSOutput>(aesX0, aesX1);
		}

	}
}


void OTExtensionBristolBase::expandOutput(int elementSize, byte * key, vector<byte> & output, int factor, const byte *counters,
                                           EVP_CIPHER_CTX *aes, byte * aesOutput, int i) const {
    //init the aes prp with the given key
    EVP_CIPHER_CTX_init(aes);
    EVP_EncryptInit(aes, EVP_aes_128_ecb(), key, nullptr);

    //Compute AES on the counters array
    int outLength;
    EVP_EncryptUpdate(aes, aesOutput, &outLength, (byte *)counters, factor * 16);

    //Copy the result to the given location
    memcpy(output.data() + i*elementSize/8, aesOutput, elementSize/8);

    //Cleaning the environment before setting the next key.
    EVP_CIPHER_CTX_cleanup(aes);
}

byte* OTExtensionBristolBase::createCountersArray(int factor) const {//Create the array of indices to be the plaintext for the AES
    auto counters = new byte[factor * 16];
    //assign zero to the array of indices which are set as the plaintext.
    memset(counters, 0, 16 * factor);
    long *countersArray = (long *)counters;
    //go over the array and set the 64 list significant bits for evey 128 bit value, we use only half of the 128 bit variables
    for (long i = 0; i < factor; i++) {
        countersArray[i * 2 + 1] = i;
    }
    return counters;
}

OTExtensionBristolReceiver::OTExtensionBristolReceiver(const string& senderAddress, int port,bool isSemiHonest, const shared_ptr<CommParty> & channel) {

	init(senderAddress, port, 1, isSemiHonest, channel);
}

shared_ptr<OTBatchROutput> OTExtensionBristolReceiver::transfer(OTBatchRInput * input){


	if (input->getType() != OTBatchRInputTypes::OTExtensionGeneralRInput &&
		input->getType() != OTBatchRInputTypes::OTExtensionRandomizedRInput &&
		input->getType() != OTBatchRInputTypes::OTExtensionCorrelatedRInput){
		throw invalid_argument("input should be instance of OTExtensionGeneralRInput or OTExtensionRandomizedRInput or OTExtensionCorrelatedRInput.");
	}
	else{

		auto sigmaArr = ((OTExtensionRInput *)input)->getSigmaArr();

		auto nOTsReal = sigmaArr.size();

        //make the number of ot's to be a multiplication of 128
		auto nOTs = ((nOTsReal + 128 - 1) / 128) * 128;

		//Convert the given sigma to BitVector
		BitVector inputBits(nOTs);
		inputBits.assign_zero();

		//fill the bit vector that bristol needs from the sigma array
		for(size_t i=0; i<nOTsReal; i++){

			if(sigmaArr[i]==1)
				inputBits.set_bit(i,1);
		}

        //Call the bristol's transfer function
		OTExtensionBristolBase::transfer(nOTs,inputBits);

        auto elementSize = (((OTExtensionRInput*)input)->getElementSize());

        /* Convert the result of the transfer function to the required size */
        vector<byte> aesOutput(nOTsReal * elementSize / 8);

        //There is no need to change the element size, copy only the required OTs.
        if (elementSize == 128){
            copy_byte_array_to_byte_vector((byte*) pOtExt->receiverOutputMatrix.squares.data(), nOTsReal*16, aesOutput, 0);

        //The required size is smaller than the output. Shrink it.
        } else if (elementSize <= 128) {
            shrinkArray(128, elementSize, nOTsReal, (byte*) pOtExt->receiverOutputMatrix.squares.data(), aesOutput.data());

        //The required size is bigger than the output. Expand it.
        } else {
            //number of 128 bit ot's needed.
            int factor = (elementSize + 127)/128;
            //Create the array of indices to be the plaintext for the AES
            auto counters = createCountersArray(factor);

            auto aes = new EVP_CIPHER_CTX();
            int outLength;
            auto outputArr = new byte[16 * factor];

            for (int i=0; i < (int) nOTsReal; i++){

                //Expand the output to the required size.
                expandOutput(elementSize, (byte*) &pOtExt->receiverOutputMatrix.squares[i/128].rows[i % 128], aesOutput,
                             factor, counters, aes, outputArr, i);
            }
            delete [] counters;
            delete [] outputArr;
            delete aes;
        }

        int size = elementSize/8 * nOTsReal;

        //If this if the general case we need another round of communication using the channel member, since OT bristol only works on random case.
		//we need to get the xor of the randomized and real data from the sender.
		if(input->getType() == OTBatchRInputTypes::OTExtensionGeneralRInput){

			if (channel == NULL) {
				throw runtime_error("In order to execute a general ot extension the channel must be given");
			}

            //Read x0 and x1 from the sender
			byte* x0Arr = new byte[size];
            byte* x1Arr = new byte[size];

			channel->read(x0Arr, size);
			channel->read(x1Arr, size);

            //xor each randomized output with the relevant xored sent from the sender
			for(int i=0; i < size; i++){
                 if (sigmaArr[i/(elementSize/8)] == 0) {
                     aesOutput[i] = x0Arr[i] ^ aesOutput[i];
                 } else {
                     aesOutput[i] = x1Arr[i] ^ aesOutput[i];
                 }
			}

            delete [] x0Arr;
            delete [] x1Arr;

			return make_shared<OTOnByteArrayROutput>(aesOutput);
		}
        //If this if the correlated case we need another round of communication using the channel member, since OT bristol only works on random case.
		//we need to get the xor of the randomized and real data from the sender.
		else if(input->getType() == OTBatchRInputTypes::OTExtensionCorrelatedRInput){

			if (channel == NULL) {
				throw runtime_error("In order to execute a correlated ot extension the channel must be given");
			}

			byte* delta = new byte[size];

			channel->read(delta, size);

			//xor each randomized output with the relevant xored sent from the sender
			for(int i=0; i < size; i++){
                if (sigmaArr[i/(elementSize/8)] != 0) {
                    //aesOutput[i] = aesOutput[i];
               // } else {
                    //x1 = delta^x1 = x1^ x0^delta^x1 = xo^delta=x1
                    aesOutput[i] = delta[i] ^ aesOutput[i];
                }
			}

			 delete [] delta;

			return make_shared<OTOnByteArrayROutput>(aesOutput);
		} else{
            return make_shared<OTOnByteArrayROutput>(aesOutput);
		}


	}

}
#endif
