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
        party->runInitialize();

// ------------- Sending and blocking -------------


//        char message[6] = "hello";
//        party->getChannel().get()->write((byte *)message, 6);

    }

    else if(my_num == 1) {

        // Receiver
        auto * party = new ReceiverOT();
        party->runInitialize();
    }

    else{cout<< "Wrong PartyId"<<endl;}
}







/*
    int nOTs = 12;

    int elementSize = 128;

    if (my_num == 1) {
        auto * party = new SenderOT();
        party->run_baseOT(party->keys0_bOT, party->keys1_bOT, CONST_k, 0);

        cout<<"nOTS: "<< nOTs<<endl;
        OTExtensionBristolSender sender(12001,true,party->getChannel());

        OTBatchSInput *input = new OTExtensionRandomizedSInput(nOTs, elementSize);

        auto start = scapi_now();
        auto output = sender.transfer(input);
        print_elapsed_ms(start, "Transfer for random");

        vector<byte> outputbytes = ((OTExtensionRandomizedSOutput *) output.get())->getR0Arr();

        cout << "the size is :" << outputbytes.size() << " r0Arr " << endl;
        for (int i = 0; i < nOTs * elementSize / 8; i++) {

            if (i % (elementSize / 8) == 0) {
                cout << endl;
            }
            cout << (int) outputbytes[i] << "--";

        }

        outputbytes = ((OTExtensionRandomizedSOutput *) output.get())->getR1Arr();

        cout << "the size is :" << outputbytes.size() << " r1Arr " << endl;
        for (int i = 0; i < nOTs * elementSize / 8; i++) {

            if (i % (elementSize / 8) == 0) {
                cout << endl;
            }
            cout << (int) outputbytes[i] << "--";

        }


    } else {
        auto * party = new ReceiverOT();
        party->run_baseOT(party->choice_bits, CONST_k, CONST_k);

        OTExtensionBristolReceiver receiver("localhost", 12001,true,party->getChannel());

        vector<byte> sigma;
        sigma.resize(nOTs);
        sigma[0] = 1;
        sigma[1] = 1;


        OTBatchRInput * input = new OTExtensionRandomizedRInput(sigma, elementSize);

        auto start = scapi_now();
        auto output = receiver.transfer(input);
        print_elapsed_ms(start, "Transfer for random");

        vector<byte> outputbytes = ((OTOnByteArrayROutput *)output.get())->getXSigma();

        cout<<"the size is :" <<outputbytes.size()<<endl;
        for(int i=0; i<nOTs*(elementSize/8); i++){

            if (i%(elementSize/8)==0){
                cout<<endl;
            }
            cout<< (int)outputbytes[i]<<"--";

        }

    }


    cout<<"Done running randomized"<<endl;
}
*/
