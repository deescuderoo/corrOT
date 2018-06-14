#include "OT.hpp"

void OT(int my_num, shared_ptr<CommParty> channel)
{
    int nOT = 2;
    int elementSize = 3 * 8; // Size in bits

    if(my_num == 0)
    {
        vector<byte> m0{'h','a','r','s','h','c'};
        vector<byte> m1{'d','a','n','i','e','l'};

        OTExtensionBristolSender sender(12001,true,channel);
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

        //vector<byte> outputbytes = ((OTOnByteArrayROutput *)output.get())->getXSigma();
        vector<byte> outputbytes = ((OTOnByteArrayROutput *)output.get())->getXSigma();

        cout<<outputbytes.size()<<endl;
        for (unsigned char outputbyte : outputbytes) {
            cout << outputbyte;
        }
        cout << endl;
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

/*
constexpr char hexmap[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                           '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

std::string hexStr(byte * data, int len)
{
    std::string s(len * 2, ' ');
    for (int i = 0; i < len; ++i) {
        s[2 * i]     = hexmap[(data[i] & 0xF0) >> 4];
        s[2 * i + 1] = hexmap[data[i] & 0x0F];
    }
    return s;
}
*/
void printN (byte * bytes, int N) {
    cout << hex;
    for (int i = 0; i<N; i++ )
        cout << (int)bytes[i];
    cout << dec << endl;
}

void SenderOT::generateKeysOT() {
    // Fills in the buffers keys0_bOT and keys1_bOT with (pseudo)random bytes
//    for (uint i = 0; i < keys0_bOT.size(); i++) {
//        byte * p0 = prg->getPRGBytesEX(sizeof(data_t));
//        memcpy(keys0_bOT[i].bytes, p0, sizeof(data_t));
////        cout << (long int)keys0_bOT[i].bytes << endl;
//        cout << hexStr(p0, sizeof(data_t)) << endl;
//
//        byte * p1 = prg->getPRGBytesEX(sizeof(data_t));
//        memcpy(keys1_bOT[i].bytes, p1, sizeof(data_t));
////        cout << (long int)keys1_bOT[i].bytes << endl;
//        cout << hexStr(p1, sizeof(data_t)) << endl;
    size_t size = sizeof(data_t)* keys0_bOT.size();
    cout << "size " << size << endl;
    memcpy(keys0_bOT[0].bytes,prg0->getPRGBytesEX(size),size);
    memcpy(keys1_bOT[0].bytes,prg1->getPRGBytesEX(size),size);

//    for (int i = 0; i < 5; i++) {
//        printN(keys0_bOT[i].bytes, 16);
//    }
//
//    for (int i = 0; i < 5; i++) {
//        printN(keys1_bOT[i].bytes, 16);
//    }

    //printN(keys1_bOT[0].bytes, 32);



    /*
    for (uint i = 0; i < keys0_bOT.size(); i++) {

        memcpy (keys0_bOT[i].bytes , prg->getPRGBytesEX(sizeof(data_t)), sizeof(data_t));
         cout << hexStr(keys0_bOT[i].bytes, sizeof(data_t)) << endl;

        byte * p1 = prg->getPRGBytesEX(sizeof(data_t));
        memcpy(keys1_bOT[i].bytes, p1, sizeof(data_t));


*/

 }

void ReceiverOT::runInitialize() {

}

