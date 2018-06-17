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

void OT(int my_num, shared_ptr<CommParty> channel)
{// TODO: Use run_baseOT to run the OT
    int nOT = 2;
    int elementSize = 16 * 8; // Size in bits

    if(my_num == 0)
    {
        vector<byte> m0{'h','a','r','s','h','c'};
        vector<byte> m1{'d','a','n','i','e','l'};

        OTExtensionBristolSender sender(12001, true, channel);
        OTBatchSInput * input = new OTExtensionGeneralSInput(m0,m1,nOT);

        auto output = sender.transfer(input);
    }

    else if(my_num == 1)
    {
        OTExtensionBristolReceiver receiver("localhost", 12001,true,channel);

        vector<byte> sigma;
        sigma.resize(nOT);

        //choice bit.
        sigma[0] = 0;
        sigma[1] = 1;


        OTBatchRInput * input = new OTExtensionGeneralRInput(sigma, elementSize);
        auto output = receiver.transfer(input);
        vector<byte> outputbytes = ((OTOnByteArrayROutput *)output.get())->getXSigma();
    }
}

void PartyOT::setChannel(){
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

void SenderOT::generateKeysOT() {
    // Fills in the buffers keys0_bOT and keys1_bOT with (pseudo)random bytes
    size_t size = sizeof(data_t)* keys0_bOT.size();
    memcpy(keys0_bOT[0].bytes,prg0->getPRGBytesEX(size),size);
    memcpy(keys1_bOT[0].bytes,prg1->getPRGBytesEX(size),size);
 }

void SenderOT::run_baseOT(vector<data_t> data0, vector<data_t> data1, size_t nOT) {
    byte * p0 = data0[0].bytes;
    byte * p1 = data1[0].bytes;

    vector<byte> data0_;
    vector<byte> data1_;
    copy(p0, p0 + data0.size() * sizeof(data_t), back_inserter(data0_));
    copy(p1, p1 + data1.size() * sizeof(data_t), back_inserter(data1_));

    OTExtensionBristolSender sender(12001, true, this->getChannel());
    OTBatchSInput * input = new OTExtensionGeneralSInput(data0_,data1_,nOT);

    auto output = sender.transfer(input);

}

void ReceiverOT::runInitialize() {

}

