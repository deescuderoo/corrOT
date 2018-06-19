#include "OT.hpp"
#include "IntMod.hpp"

int main() {

    string partyNum;
    cout <<"Enter Party Id : "<<endl;
    cin >> partyNum;

    int my_num = stoi(partyNum);





    if(my_num == 0)
    {
        // Sender
        auto * party = new SenderOT();
//        party->runInitialize();

// ------------- Sending and blocking -------------


//        char message[6] = "hello";
//        party->getChannel().get()->write((byte *)message, 6);

    }

    else if(my_num == 1) {

        // Receiver
        auto * party = new ReceiverOT();
//        party->runInitialize();

    }

    else{cout<< "Wrong PartyId"<<endl;}



    /// TESTS

    // Test numbers
    int a = 10;
    int b = 20;
    auto number1 = new Z2k<int, 32>(a);
    auto number2 = new Z2k<int, 32>(b);
    auto number3 = *number1 + *number2;

    cout << "Number1: " << number1->m_data << endl;
    cout << "Number2: " << number2->m_data << endl;
    cout << "Number3: " << number3.m_data << endl;

    // Construction from byte * and from vector<byte>
    vector<byte> bytes = {0x10,0x00,0x00,0x00}; //{first byte, second byte, third byte, fourth byte}
    byte c[4];
    copy(bytes.begin(), bytes.end(), c);

    auto test1 = new Z2k<int, 32>(bytes);
    auto test2 = new Z2k<int, 32>(c);
    cout << "Other constructors: " << test1->m_data << " " << test2->m_data << endl;


    // Test vectors
    auto number4 = new Z2k<int, 32>(15);
    auto vec1 = new vZ2k<int, 32>(vector<Z2k<int, 32>>({*number1, *number2}));
    auto vec2 = new vZ2k<int, 32>(vector<Z2k<int, 32>>({number3, *number4}));
    cout << "Vec1: " << vec1->m_data[0].m_data << " " << vec1->m_data[1].m_data << endl;
    cout << "Vec2: " << vec2->m_data[0].m_data << " " << vec2->m_data[1].m_data << endl;
    auto vec3 = *vec1 + *vec2;
    cout << "Vec3: " << vec3.m_data[0].m_data << " " << vec3.m_data[1].m_data << endl;

    // Vectors from vector<vector<bytes>>
    vector<byte> bytes2 = {0x01,0x00,0x00,0x00};
    vector<vector<byte>> vecvec = {bytes,bytes2};
    auto vec4 = new vZ2k<int, 32>(vecvec);
    cout << "Vec4: " << vec4->m_data[0].m_data << " " << vec4->m_data[1].m_data << endl;


}


