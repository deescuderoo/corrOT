#include "OT.hpp"

void printN (byte * bytes, int N) {
    cout << hex;
    for (int i = 0; i<N; i++ )
        cout << (int)bytes[i];
    cout << dec << endl;
}

void printN (vector<byte> & bytes) {
    cout << hex;
    for (int i = 0; i < bytes.size(); i++)
        cout << (int)(bytes[i]);
    cout << dec << endl;
}

vector<vector<byte>> vectorConversion(vector<byte> & input, int nrows, int ncols){
    assert (input.size() == nrows * ncols);
    vector<vector<byte> > output;
    output.resize(nrows, vector<byte>(ncols));
    for(int i=0; i<nrows; i++)
    {
        //vector<int>::const_iterator first = b.begin() + 2*i;
        //vector<int>::const_iterator last = b.begin() + 2*(i+1);
        copy_n(input.begin() + ncols*i, ncols,output[i].begin());
    }
    return output;
}

void PartyOT::createChannel(){
    if(id == 0)
    {
        //sender
        boost::asio::io_service io_service;
        SocketPartyData me(IpAddress::from_string("127.0.0.1"), 1212);
        SocketPartyData other(IpAddress::from_string("127.0.0.1"), 1213);
        shared_ptr<CommParty> channel = make_shared<CommPartyTCPSynced>(io_service, me, other);

        // connect to party One
        channel->join(500, 5000);
        cout << "Connection Established" << endl;

        this->channel = channel;
    }

    else if(id == 1) {

        //receiver
        boost::asio::io_service io_service;
        SocketPartyData me(IpAddress::from_string("127.0.0.1"), 1213);
        SocketPartyData other(IpAddress::from_string("127.0.0.1"), 1212);
        shared_ptr<CommParty> channel = make_shared<CommPartyTCPSynced>(io_service, me, other);

        // connect to party Zero
        channel->join(500, 5000);
        cout<<"Connection Established"<<endl;

        this->channel = channel;
    }
};

void SenderOT::run_baseOT(vector<vector<byte>> data0, vector<vector<byte>> data1, int nOT, int elementSizeBits) {
//    byte * p0 = data0[0].bytes;
//    byte * p1 = data1[0].bytes;
//
//    vector<byte> data0_;
//    vector<byte> data1_;
//    copy(p0, p0 + data0.size() * sizeof(data_t), back_inserter(data0_));
//    copy(p1, p1 + data1.size() * sizeof(data_t), back_inserter(data1_));

    OTExtensionBristolSender sender(12001, true, this->getChannel());
    OTBatchSInput * input = new OTExtensionRandomizedSInput(nOT, elementSizeBits);

    auto output = sender.transfer(input);

    auto output_ = ((OTExtensionRandomizedSOutput *) output.get());
    vector<byte> outputbytes0 = output_->getR0Arr();
    vector<byte> outputbytes1 = output_->getR1Arr();

//    cout << "Strings " << outputbytes0.size() << endl;
//    printN(outputbytes0);
//    printN(outputbytes1);

    keys0_bOT = vectorConversion(outputbytes0, CONST_k, SIZE_OT);
    keys1_bOT = vectorConversion(outputbytes1, CONST_k, SIZE_OT);


}

void ReceiverOT::generateChoiceBitsOT() {
    size_t size = choice_bits.size();
    memcpy(&choice_bits[0], prg->getPRGBytesEX(size), size);
    for (unsigned char &choice_bit : choice_bits) {
        choice_bit %= 2; // Project byte to bit
//        cout << (int)choice_bits[i] << endl;
    }
//    cout << "Choice bits" << endl;
//    printN(choice_bits);
}

void ReceiverOT::run_baseOT(vector<byte> sigma, int nOT, int elementSizeBits) {

    OTExtensionBristolReceiver receiver("localhost", 12001, true, getChannel());
    sigma.resize(nOT);
    OTBatchRInput * input = new OTExtensionRandomizedRInput(sigma, elementSizeBits);
    auto output = receiver.transfer(input);
    vector<byte> outputbytes = ((OTOnByteArrayROutput *)output.get())->getXSigma();

//    cout << "Output strings" << endl;
//    printN(outputbytes);

    keys_bOT = vectorConversion(outputbytes, CONST_k, SIZE_OT);
//    for (auto row : keys_bOT){
//        printN(row);
//    }

}

void ReceiverOT::runInitialize() {
    run_baseOT(choice_bits, CONST_k, CONST_k);
}

void SenderOT::runInitialize() {
    run_baseOT(keys0_bOT, keys1_bOT, CONST_k, CONST_k);
}