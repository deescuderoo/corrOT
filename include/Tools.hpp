//
// Created by daniel on 19/06/18.
//

#ifndef OT1_TOOLS_HPP
#define OT1_TOOLS_HPP

// (k' + n) must be divisible by 4

// Number of OTs
#define CONST_k 128
// Length for the correlated OTs
#define CONST_n 999
// Length of masking correlated vector
#define CONST_k_ 1
// Length of base OTs
#define SIZE_OT CONST_k/8

#define CONST_pwr 32

#define HASH_OUTPUT_LENGTH 32

typedef unsigned int CONST_T;
typedef unsigned char byte;

#include <iostream>
#include <openssl/sha.h>
#include <vector>
#include <cassert>
#include <algorithm>
using namespace std;

void printN (byte * bytes, int N);
void printN (vector<byte> & bytes);

vector<vector<byte>> vectorConversion(vector<byte> & input, int nrows, int ncols);

vector<byte> inToBytes(int paramInt);
void Hash_value(SHA256_CTX * hashAlgorithm, byte* in, byte * digest, int inSize);



#endif //OT1_TOOLS_HPP
