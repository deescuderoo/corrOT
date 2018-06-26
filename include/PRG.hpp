//
// Created by daniel on 24/06/18.
//
#include <cstring> //memcpy memset
#include <cstdlib>
#include <openssl/rand.h>
#include <openssl/evp.h>
#include <emmintrin.h>
#include <iostream>
#include <iomanip> //setfill
#include <cassert>

using namespace std;

typedef unsigned char byte;

inline void print128(char *str, const byte * b);

class COTSK_Prg {

   public:

  ~COTSK_Prg();
  
  COTSK_Prg();

  void init(const byte *key128bit , uint32_t maxBytes);
  const byte * getBytes(uint32_t sizeBytes);
  const byte * getBytes(byte *client_buff, uint32_t sizeBytes);
  
private:
  const byte *doGetBytes(byte *buffer, uint32_t sizeBytes);

   private:
      byte *      _buff = nullptr;
      byte *      _ctr = nullptr;
      uint64_t *  _ctr_u64 = nullptr;
      byte        _key[16];
      uint64_t    _ctr_inc;
      EVP_CIPHER_CTX *_aes = nullptr;
      byte       _iv[16];


};
