#include <iostream>
#include <libscapi/include/interactive_mid_protocols/OTExtensionBristol.hpp>
#include <libscapi/include/primitives/Prg.hpp>

// Number of OTs
#define CONST_k 128
// Length for the correlated OTs
#define CONST_n 1000
// Length of masking correlated vector
#define CONST_k_ 1
// Length of base OTs
#define SIZE_OT CONST_k/8

void OT(int my_num, shared_ptr<CommParty> channel);

struct data_t
{
    data_t(string s){
        memset(bytes,0,sizeof(data_t));
        memcpy(bytes, s.c_str(), s.length());
    }
    byte bytes[SIZE_OT];

    data_t(){
        memset(bytes,0,sizeof(data_t));
    }
};

class PartyOT {
private:
    int id;
    shared_ptr<CommParty> channel;  // Channel between this party to every other party in the protocol.
    void setChannel();

public:
    PartyOT(int id)
            : id(id), prg0(new PrgFromOpenSSLAES()), prg1(new PrgFromOpenSSLAES()) {
        // Sets the channel
        setChannel();

        SecretKey secretKey0 = prg0->generateKey(256); // Sets the key for the PRG
        prg0->setKey(secretKey0);
        SecretKey secretKey1 = prg1->generateKey(256); // Sets the key for the PRG
        prg1->setKey(secretKey1);
    }

    int getID() { return id; }

    shared_ptr<CommParty> getChannel() { return channel; }

protected:
    PrgFromOpenSSLAES * prg0;
    PrgFromOpenSSLAES * prg1;
};

class ReceiverOT : public PartyOT{
public:
    ReceiverOT() : PartyOT(1) {}

    void runInitialize();

private:
    vector<data_t> choice_bits; // Holds the receiver's choice bits for the OTs

    void generateChoiceBitsOT();

};

class SenderOT : public PartyOT{
public:
    SenderOT() : PartyOT(0), keys0_bOT(CONST_k), keys1_bOT(CONST_k) {
        generateKeysOT();
    }

    void runInitialize();

private:
    vector<data_t> keys0_bOT; // Holds the sender's inputs to the OTs
    vector<data_t> keys1_bOT;

    void generateKeysOT();
};