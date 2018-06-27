#include <Tools.hpp>
#include <IntMod.hpp>
#include <PRG.hpp>
#include <OT/OTExtensionBristol.hpp>

/**
 * Base Party class from which Sender and Receiver class are derived.
 * Contains the Party information such as channel, party Id etc.
 */
class PartyOT {
private:

    /**
     * Stores Party ID.
     */
    int id;

    /**
     * Channel between this party to every other party in the protocol.
     */
    shared_ptr<CommParty> channel;

    /**
     *  Sets up a channel with the another party.
     */
    void createChannel();

public:
    /**
     *
     * @param id It's own Party Id.
     * Create/Defines an object for PRG,PRF and HASH on the heap.
     * Sets up channel using createChannel().
     *
     */
    PartyOT(int id)
            : id(id), prg(new COTSK_Prg()), prf(new COTSK_Prg()), hash(new SHA256_CTX) {

        createChannel();

        /**
         * Generates a key usign RAND_bytes(,).
         * Uses it as a seed for the PRG using init(key,maxSize).
         */
        byte seed_key[16];
        RAND_bytes(seed_key, sizeof(seed_key));
        prg->init(seed_key, 100000 * 8);

    }

    /**
     * @return Party Id
     */
    int getID() { return id; }

    /**
     *
     * @return channel
     */
    shared_ptr<CommParty> getChannel() { return channel; }

    /**
     * virutal function which runs the Initialize Phase of the protocol.
     */
    virtual void runInitialize() = 0;

    /**
     * virutal function which runs the Correlation Phase of the protocol.
     */
    virtual void runCreateCorrelation() = 0;

    /**
     * virutal function which runs the Consistency Check Phase of the protocol.
     */
    virtual void runConsistencyCheck() = 0;


protected:
    COTSK_Prg * prg;
    COTSK_Prg * prf;
    SHA256_CTX * hash;

};

/**
 * Receiver Class derived from Party OT class.
 * @tparam T Data type
 * @tparam pwr power of the field size (eg: 16,32,64)
 */
template<class T, int pwr>
class ReceiverOT : public PartyOT{
public:
    ReceiverOT() : PartyOT(1), choice_bits(CONST_k), t(CONST_k,vZ2k<T,pwr>(CONST_n + CONST_k_)),
                   ui(CONST_k,vZ2k<T,pwr>(CONST_n + CONST_k_)), ai(CONST_k,vZ2k<T,pwr>(CONST_n + CONST_k_)){};

    // Holds the receiver's choice bits for the OTs
    vector<byte> choice_bits;

    void run_baseOT(vector<byte> sigma, int nOT, int elementSize);
    void runInitialize() override;
    void runCreateCorrelation() override;
    void runConsistencyCheck() override;



    /**
    * keys_bOT and holds the inputs received from sender.
    * ti holds the PRF output received from sender.
    * ui holds the calculated correlation received from sender.
    * ai holds the output calculated  by the reciever using the uis and tis.
    */
    vector<vector<byte>> keys_bOT;
    vector<vZ2k<T,pwr>> t;
    vector<vZ2k<T,pwr>> ui;
    vector<vZ2k<T,pwr>> ai;

    /**
     * Holds the hashes for the Consistency Check phase.
     */
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

/**
* Receiver Class derived from Party OT class.
* @tparam T Data type
* @tparam pwr power of the field size (eg: 16,32,64)
*/
template<class T, int pwr>
class SenderOT : public PartyOT{
public:
    /**
     * Sender Party Constructor.
     * Allocates necessary sizes to the vectors.
     *
     */
    SenderOT() : PartyOT(0), keys0_bOT(CONST_k, vector<byte>(SIZE_OT)), keys1_bOT(CONST_k, vector<byte>(SIZE_OT)),
                 correlation(CONST_n + CONST_k_), t0(CONST_k,vZ2k<T,pwr>(CONST_n + CONST_k_)), t1(CONST_k,vZ2k<T,pwr>(CONST_n + CONST_k_)) {}
    /**
     * function which overrides  runInitialize() function of PartyOT class.
     */
    void runInitialize() override;

    /**
     * function which overrides  runCreateCorrelation() function of PartyOT class.
     */
    void runCreateCorrelation() override;

    /**
     * function which overrides  runConsistencyCheck() function of PartyOT class.
     */
    void runConsistencyCheck() override;

    /**
     * keys0_bOT and keys1_bOT holds the senders inputs.
     * correlation holds the correlation generated from PRG.
     * t0 and t1 holds the outputs of the PRFs.
     */
    vector<vector<byte>> keys0_bOT; // Holds the sender's inputs to the OTs
    vector<vector<byte>> keys1_bOT;
    vZ2k<T,pwr> correlation;
    vector<vZ2k<T,pwr>> t0;
    vector<vZ2k<T,pwr>> t1;

    /**
     * Holds the hashes for the Consistency Check phase.
     */
    byte h00[HASH_OUTPUT_LENGTH * CONST_k * CONST_k];
    byte h01[HASH_OUTPUT_LENGTH * CONST_k * CONST_k];
    byte h10[HASH_OUTPUT_LENGTH * CONST_k * CONST_k];
    byte h11[HASH_OUTPUT_LENGTH * CONST_k * CONST_k];

private:
    /**
     *
     * @param data0 Sender's Input-0 to the OT.
     * @param data1 Sender's Input-1 to the OT
     * @param nOT Number of OTs
     * @param elementSizeBits Size of each Input.
     */
    void run_baseOT(vector<vector<byte>> data0, vector<vector<byte>> data1, int nOT, int elementSizeBits);
    /**
     * Uses PRG to generate correlation using PRG.
     */
    void sampleCorrelation();
    /**
     * Used to create PRF outputs which are stored in t0 and t1.
     */
    void applyPRF();
    /**
     * Used to send the calculated ui's to the receiver.
     */
    void sendUi();
    /**
     * Used to generate Hashes in the Correlation Phase.
     */
    void generateHashes();
    /**
     * Used to send the Hashes to the receiver.
     */
    void sendHashes();
};

#include "corrOT/src/Sender.tpp" // Implementations of template methods must be in the header file
#include "corrOT/src/Receiver.tpp" // Implementations of template methods must be in the header file