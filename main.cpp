#include "OT.hpp"

int main() {

    string partyNum;
    cout <<"Enter Party Id : "<<endl;
    cin >> partyNum;

    int my_num = stoi(partyNum);

    if(my_num == 0)
    {
        // Sender
        auto * party = new SenderOT();
        party->run_baseOT(party->keys0_bOT, party->keys1_bOT, CONST_k);

        /* ------------- Sending and blocking ------------- */

//        char message[6] = "hello";
//        party->getChannel().get()->write((byte *)message, 6);

    }

    else if(my_num == 1) {

        // Receiver
        auto * party = new ReceiverOT();
        party->generateChoiceBitsOT();
        OT(my_num,party->getChannel());

//        byte message_received[6];
//        party->getChannel().get()->read(message_received, 6);
//        cout << string((char *)message_received) << endl;
    }

    else{cout<< "Wrong PartyId"<<endl;}
}
