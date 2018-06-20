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
    }
    //printing secret.
    for(int i=0;i<output.size();i++)
        cout<<hex<<(int)output[i];
    cout<<endl;

    return output;
}
/// Writes (F_k(0), ... ,F_k(c)) into output, where c is such that this vector has length outputBytesLength
/// (it has to be divisible by 16)
void prfCall(PseudorandomFunction *prf, vector<byte> &key, vector<byte> &output, int outputBytesLength)
{
    output.resize(outputBytesLength);
//    outputBytesLength = (CONST_pwr * (CONST_n + CONST_k_))/8;
//    int keySize = 128;
//    SecretKey secretkey  = prf->generateKey(keySize);
    SecretKey secretkey(key,"AES");
    prf->setKey(secretkey);

    vector<byte> in;

    for(int ctr=0; ctr < outputBytesLength/16; ctr++) // The output of AES is 16 bytes long
    {
        in = inToBytes(ctr);
        prf->computeBlock(in, 0, in.size(), output, 16*ctr);
    }
/*    for(int i=0;i<output.size();i++)
        cout<<hex<<(int)output[i];
    cout<<endl;*/
}