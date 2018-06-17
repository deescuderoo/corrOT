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

void printN (byte * bytes, int N);
void printN (vector<byte> & bytes);
vector<vector<byte>> vectorConversion(vector<byte> & input, int nrows, int ncols);


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
    void createChannel();

public:
    PartyOT(int id)
            : id(id), prg(new PrgFromOpenSSLAES()) {
        // Sets the channel
        createChannel();

        SecretKey secretKey0 = prg->generateKey(256); // Sets the key for the PRG
        prg->setKey(secretKey0);
    }

    int getID() { return id; }
    shared_ptr<CommParty> getChannel() { return channel; }
    virtual void runInitialize() = 0;

protected:
    PrgFromOpenSSLAES * prg;
};

class ReceiverOT : public PartyOT{
public:
    ReceiverOT() : PartyOT(1), choice_bits(CONST_k) {
        generateChoiceBitsOT();
    }

    vector<byte> choice_bits;// Holds the receiver's choice bits for the OTs
    void run_baseOT(vector<byte> sigma, int nOT, int elementSize);
    void runInitialize() override;

//private:

    vector<vector<byte>> keys_bOT;

    void generateChoiceBitsOT();

};

class SenderOT : public PartyOT{
public:
    SenderOT() : PartyOT(0), keys0_bOT(CONST_k, vector<byte>(SIZE_OT)), keys1_bOT(CONST_k, vector<byte>(SIZE_OT)) {}

    void run_baseOT(vector<vector<byte>> data0, vector<vector<byte>> data1, int nOT, int elementSizeBits);
    void runInitialize() override;
    //
//private:
    vector<vector<byte>> keys0_bOT; // Holds the sender's inputs to the OTs
    vector<vector<byte>> keys1_bOT;
};