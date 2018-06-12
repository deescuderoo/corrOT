#include "OT.hpp"

int main() {

    string partyNum;
    cout <<"Enter Party Id : "<<endl;
    cin>>partyNum;

    int my_num = stoi(partyNum);

    //auto channel = setComm(my_num);


    if(my_num == 0)
    {
        //sender
        boost::asio::io_service io_service;
        SocketPartyData me(IpAddress::from_string("127.0.0.1"), 1212);
        SocketPartyData other(IpAddress::from_string("127.0.0.1"), 1213);
        shared_ptr<CommParty> channel = make_shared<CommPartyTCPSynced>(io_service, me, other);

        // connect to party One
        channel->join(500, 5000);
        cout << "Connection Established" << endl;

       OT(my_num,channel);


    }

    else if(my_num == 1) {

        //receiver
        boost::asio::io_service io_service;
        SocketPartyData me(IpAddress::from_string("127.0.0.1"), 1213);
        SocketPartyData other(IpAddress::from_string("127.0.0.1"), 1212);
        shared_ptr<CommParty> channel = make_shared<CommPartyTCPSynced>(io_service, me, other);

        // connect to party Zero
        channel->join(500, 5000);
        cout<<"Connection Established"<<endl;

        OT(my_num,channel);

    }

    else{cout<< "Wrong PartyId"<<endl;}



}
