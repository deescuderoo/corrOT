#include "Tools.hpp"
#include "IntMod.hpp"
#include "PRG.cpp" //TODO: generate header file

/*struct data_t
{
    data_t(string s){
        memset(bytes,0,sizeof(data_t));
        memcpy(bytes, s.c_str(), s.length());
    }
    byte bytes[SIZE_OT];

    data_t(){
        memset(bytes,0,sizeof(data_t));
    }
};*/

class PartyOT {
private:
    int id;
    shared_ptr<CommParty> channel;  // Channel between this party to every other party in the protocol.
    void createChannel();

public:
    PartyOT(int id)
            : id(id), prg(new COTSK_Prg()), prf(new COTSK_Prg()), hash(new SHA256_CTX) {
        // Sets the channel
        createChannel();

//        SecretKey secretKey0 = prg->generateKey(256); // Sets the key for the PRG
//        prg->setKey(secretKey0);

        byte seed_key[16];
        RAND_bytes(seed_key, sizeof(seed_key));
        prg->init(seed_key, 100000 * 8);

    }

    int getID() { return id; }
    shared_ptr<CommParty> getChannel() { return channel; }
    virtual void runInitialize() = 0;
    virtual void runCreateCorrelation() = 0;
    virtual void runConsistencyCheck() = 0;


protected:
    COTSK_Prg * prg;
    COTSK_Prg * prf;
    SHA256_CTX * hash;

};

template<class T, int pwr>
class ReceiverOT : public PartyOT{
public:
    ReceiverOT() : PartyOT(1), choice_bits(CONST_k), t(CONST_k,vZ2k<T,pwr>(CONST_n + CONST_k_)),
                   ui(CONST_k,vZ2k<T,pwr>(CONST_n + CONST_k_)), ai(CONST_k,vZ2k<T,pwr>(CONST_n + CONST_k_)){};

    vector<byte> choice_bits;// Holds the receiver's choice bits for the OTs
    void run_baseOT(vector<byte> sigma, int nOT, int elementSize);
    void runInitialize() override;
    void runCreateCorrelation() override;
    void runConsistencyCheck() override;


    //private:

    vector<vector<byte>> keys_bOT;
    vector<vZ2k<T,pwr>> t;
    vector<vZ2k<T,pwr>> ui;
    vector<vZ2k<T,pwr>> ai;

    byte h00[HASH_OUTPUT_LENGTH * CONST_k * CONST_k];
    byte h01[HASH_OUTPUT_LENGTH * CONST_k * CONST_k];
    byte h10[HASH_OUTPUT_LENGTH * CONST_k * CONST_k];
    byte h11[HASH_OUTPUT_LENGTH * CONST_k * CONST_k];

    void generateChoiceBitsOT();
    void applyPRF();
    void receiveUi();
    void computeAi();
    void receiveHashes();
    void checkHashes();
};

template<class T, int pwr>
class SenderOT : public PartyOT{
public:
    SenderOT() : PartyOT(0), keys0_bOT(CONST_k, vector<byte>(SIZE_OT)), keys1_bOT(CONST_k, vector<byte>(SIZE_OT)),
                 correlation(CONST_n + CONST_k_), t0(CONST_k,vZ2k<T,pwr>(CONST_n + CONST_k_)), t1(CONST_k,vZ2k<T,pwr>(CONST_n + CONST_k_))
    /*             ,h00(CONST_k * CONST_k, vector<byte>(HASH_OUTPUT_LENGTH)), h01(CONST_k * CONST_k, vector<byte>(HASH_OUTPUT_LENGTH)),
                 h10(CONST_k * CONST_k, vector<byte>(HASH_OUTPUT_LENGTH)), h11(CONST_k * CONST_k, vector<byte>(HASH_OUTPUT_LENGTH))
    */{}

    void runInitialize() override;
    void runCreateCorrelation() override;
    void runConsistencyCheck() override;


    //
//private:
    vector<vector<byte>> keys0_bOT; // Holds the sender's inputs to the OTs
    vector<vector<byte>> keys1_bOT;
    vZ2k<T,pwr> correlation;
    vector<vZ2k<T,pwr>> t0;
    vector<vZ2k<T,pwr>> t1;

    byte h00[HASH_OUTPUT_LENGTH * CONST_k * CONST_k];
    byte h01[HASH_OUTPUT_LENGTH * CONST_k * CONST_k];
    byte h10[HASH_OUTPUT_LENGTH * CONST_k * CONST_k];
    byte h11[HASH_OUTPUT_LENGTH * CONST_k * CONST_k];

private:
    void run_baseOT(vector<vector<byte>> data0, vector<vector<byte>> data1, int nOT, int elementSizeBits);
    void sampleCorrelation();
    void applyPRF();
    void sendUi();
    void generateHashes();
    void sendHashes();
};

#include "Sender.tpp" // Implementations of template methods must be in the header file
#include "Receiver.tpp" // Implementations of template methods must be in the header file