//
// Created by daniel on 19/06/18.
//

#include "Tools.hpp"

void printN (byte * bytes, int N) {
    cout << hex;
    for (int i = 0; i<N; i++ )
        cout << (int)bytes[i];
    cout << dec << endl;
}

void printN (vector<byte> & bytes) {
    cout << hex;
    for (int i = 0; i < bytes.size(); i++)
        cout << (int)(bytes[i]);
    cout << dec << endl;
}

vector<vector<byte>> vectorConversion(vector<byte> & input, int nrows, int ncols){
    assert (input.size() == nrows * ncols);
    vector<vector<byte> > output;
    output.resize(nrows, vector<byte>(ncols));
    for(int i=0; i<nrows; i++)
    {
        //vector<int>::const_iterator first = b.begin() + 2*i;
        //vector<int>::const_iterator last = b.begin() + 2*(i+1);
        copy_n(input.begin() + ncols*i, ncols,output[i].begin());
    }
    return output;
}

vector<byte> inToBytes(int paramInt)
{
    vector<byte> AoB(16,0x00);
    uint64_t param64 = (uint64_t ) paramInt;
    auto p = (uint64_t *)AoB.data();
    *p = param64;

    return AoB;

}

vector<byte> prfCall0()
{
    PseudorandomFunction *prf = new OpenSSLAES();
    int outputLength = (CONST_pwr * (CONST_n + CONST_k_))/8;
    int keySize = 128;
    SecretKey secretkey  = prf->generateKey(keySize);

//    SecretKey secretkey(key,"AES");

    vector<byte> output(outputLength);
    vector<byte> in;

    for(int ctr=0;ctr<outputLength/16;ctr++)
    {
        in = inToBytes(ctr);
        prf->setKey(secretkey);

        prf->computeBlock(in,0,in.size(),output,16*ctr);

        //printing secret.

    }
    for(int i=0;i<output.size();i++)
        cout<<hex<<(int)output[i];
    cout<<endl;

    return output;
}

vector<byte> prfCall(PseudorandomFunction *prf,vector<byte> &key)
{
    int outputLength = (CONST_pwr * (CONST_n + CONST_k_))/8;
    int keySize = 128;
    SecretKey secretkey  = prf->generateKey(keySize);

//    SecretKey secretkey(key,"AES");

    vector<byte> output(outputLength);
    vector<byte> in;

    for(int ctr=0;ctr<outputLength/16;ctr++)
    {
        in = inToBytes(ctr);
        prf->setKey(secretkey);

        prf->computeBlock(in,0,in.size(),output,16*ctr);

        //printing secret.

    }
    for(int i=0;i<output.size();i++)
        cout<<hex<<(int)output[i];
    cout<<endl;

    return output;

}

//harsh 6/18/2018
//void runPRF(vector<vector<byte>> &keys0, vector<vector<byte>> &keys1, vector<vector<byte>> &T0, vector<vector<byte>> &T1)
//{
//    PseudorandomFunction *prf = new OpenSSLAES();
//
//    int maxCount = (CONST_pwr * (CONST_n + CONST_k_))/128;
//
//    T0.resize(maxCount,vector<byte>(16));
//    T1.resize(maxCount,vector<byte>(16));
//
//
//
//    for (int i = 0;i<CONST_k;i++) {
//        copy_n(prfCall(prf,keys0[i],maxCount).begin(), 16,T0.begin()+16*i);
//        copy_n(prfCall(prf,keys1[i],maxCount).begin(), 16,T1.begin()+16*i);
//        //T0 = prfCall(prf,keys0[i],maxCount);
//        //T1 = prfCall(prf,keys1[i],maxCount);
//    }
//
//}
