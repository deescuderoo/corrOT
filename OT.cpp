#include "OT.hpp"

void OT(int my_num, shared_ptr<CommParty> channel)
{
    int nOT = 2;
    int elementSize = 3 * 8; // Size in bits

    if(my_num == 0)
    {
        vector<byte> m0{'h','a','r','s','h','c'};
        vector<byte> m1{'d','a','n','i','e','l'};

        OTExtensionBristolSender sender(12001,true,channel);
        OTBatchSInput *input = new OTExtensionGeneralSInput(m0,m1,nOT);

        auto output = sender.transfer(input);
    }

    else if(my_num == 1)
    {
        OTExtensionBristolReceiver receiver("localhost", 12001,true,channel);

        vector<byte> sigma;
        sigma.resize(nOT);

        //choice bit.
        sigma[0] = 0;
        sigma[1] = 1;


        OTBatchRInput * input = new OTExtensionGeneralRInput(sigma, elementSize);
        auto output = receiver.transfer(input);

        //vector<byte> outputbytes = ((OTOnByteArrayROutput *)output.get())->getXSigma();
        vector<byte> outputbytes = ((OTOnByteArrayROutput *)output.get())->getXSigma();

        cout<<outputbytes.size()<<endl;
        for (int i =0 ;i<outputbytes.size();i++)
        {
            cout <<outputbytes[i] << endl;
        }
    }
}

