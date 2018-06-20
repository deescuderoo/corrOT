//
// Created by daniel on 19/06/18.
//

#ifndef OT1_TOOLS_HPP
#define OT1_TOOLS_HPP

// Number of OTs
#define CONST_k 128
// Length for the correlated OTs
#define CONST_n 999
// Length of masking correlated vector
#define CONST_k_ 1
// Length of base OTs
#define SIZE_OT CONST_k/8

#define CONST_T uint
#define CONST_pwr 32

#include <iostream>
#include <libscapi/include/interactive_mid_protocols/OTExtensionBristol.hpp>
#include <libscapi/include/primitives/Prg.hpp>
#include <libscapi/include/primitives/PrfOpenSSL.hpp>


void printN (byte * bytes, int N);
void printN (vector<byte> & bytes);
vector<vector<byte>> vectorConversion(vector<byte> & input, int nrows, int ncols);

vector<byte> inToBytes(int paramInt);
vector<byte> prfCall0();
void prfCall(PseudorandomFunction *prf, vector<byte> &key, vector<byte> &output, int outputBytesLength);
void runPRF(vector<vector<byte>> &keys0, vector<vector<byte>> &keys1, vector<vector<byte>> &T0, vector<vector<byte>> &T1);



#endif //OT1_TOOLS_HPP
