#include <iostream>
#include <libscapi/include/interactive_mid_protocols/OTExtensionBristol.hpp>
#include <libscapi/include/primitives/Prg.hpp>

void OT(int my_num, shared_ptr<CommParty> channel);

class PartyOT {
private:
    int id;
    shared_ptr<CommParty> channel;  // Channel between this party to every other party in the protocol.
    PrgFromOpenSSLAES* prg;

public:
    PartyOT(int id)
            : id(id), prg(new PrgFromOpenSSLAES()){
        // Sets the channel
        setChannel();

        SecretKey secretKey = prg->generateKey(256); // Sets the key for the PRG
        prg->setKey(secretKey);
    }

    int getID() { return id; }
    void setChannel();
    shared_ptr<CommParty> getChannel() { return channel; }


//    byte * p = prg->getPRGBytesEX(sizeof(data256));
//    memcpy(iotape[m_partyId].bytes, p, sizeof(data256));
};