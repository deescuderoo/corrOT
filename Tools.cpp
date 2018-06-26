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
    vector<vector<byte>> output(nrows, vector<byte>(ncols));
    for(int i=0; i<nrows; i++)
    {
//        *(output[i].data()) = *(input.data() + i*ncols);
        copy_n(input.begin() + ncols*i, ncols, output[i].begin());
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

void Hash_value(SHA256_CTX * hashAlgorithm, byte* in, byte * digest, int inSize){
    SHA256_Init(hashAlgorithm);
    SHA256_Update(hashAlgorithm, in, inSize);
    SHA256_Final(digest, hashAlgorithm);


//    cout<<"--------------------------------Hash------------------\n";
//    for(int i = 0; i<32; i++) {
//        cout << hex;
//        cout << (int) digest[i];
//    }
//    cout << dec;
//    cout << endl;
}