#include "OT.hpp"

int main() {

    string partyNum;
    cout <<"Enter Party Id : "<<endl;
    cin >> partyNum;

    int my_num = stoi(partyNum);

    if(my_num == 0)
    {
        // Sender
        PartyOT * party = new PartyOT(0);

        OT(my_num,party->getChannel());

        /* ------------- Sending and blocking ------------- */

        char message[6] = "hello";
        party->getChannel().get()->write((byte *)message, 6);

    }

    else if(my_num == 1) {

        // Receiver
        PartyOT * party = new PartyOT(1);

        OT(my_num,party->getChannel());

        byte message_received[6];
        party->getChannel().get()->read(message_received, 6);
        cout << string((char *)message_received) << endl;
    }

    else{cout<< "Wrong PartyId"<<endl;}



}
