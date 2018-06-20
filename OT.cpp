#include "OT.hpp"

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

void ReceiverOT::runCreateCorrelation() {

}
