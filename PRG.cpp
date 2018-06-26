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

inline void print128(char *str, const byte * b) {

    cout << str << " " << hex << std::setfill( '0' );
    for (int i =0; i < 16; i++) {
        cout << (int)b[i];
    }
    cout << dec << endl;
}

class COTSK_Prg {

   public:

      ~COTSK_Prg() {
         _mm_free(_buff);
         _mm_free(_ctr);
      }

      COTSK_Prg() {
         int maxBytes = 100000 * 8;
         assert (maxBytes % 16 == 0);
         _ctr_inc = 1;
         _buff = (byte *)_mm_malloc(maxBytes,16);
         _ctr = (byte *)_mm_malloc(maxBytes,16);

         memset(_iv, 0x03, 16);
         memset(_buff, 0, maxBytes);
         memset(_ctr, 0, maxBytes);

         _ctr_u64 = (uint64_t *) _ctr;
         }

      void init(const byte *key128bit , uint32_t maxBytes) {

         if (_aes != nullptr) {
            EVP_CIPHER_CTX_init(_aes);
         }
            _ctr_inc = 1;

         memcpy(_key, key128bit, 16);
         //print128("prg key",_key);
           // print128("iv",_iv);

            _aes = new EVP_CIPHER_CTX();
           int rc = EVP_EncryptInit(_aes, EVP_aes_128_ecb(), _key, _iv);
            assert (rc == 1);

      }

      const byte *getBytes(uint32_t sizeBytes) {
         return doGetBytes (_buff,sizeBytes);
      }
        const byte *getBytes(byte *client_buff, uint32_t sizeBytes) {
            return doGetBytes (client_buff,sizeBytes);
        }

private:
      const byte *doGetBytes(byte *buffer, uint32_t sizeBytes) {
         for (uint32_t i = 0; i < (sizeBytes/16)+1; i++) {
             _ctr_u64[2*i] = (++_ctr_inc);
         }
         int tmp;
           if (sizeBytes < 16) {
            sizeBytes = 16;
           }
          int rc = EVP_EncryptUpdate(_aes, buffer, &tmp, _ctr, sizeBytes);
          assert (rc == 1);

         return buffer;
      }

   private:
      byte *      _buff = nullptr;
      byte *      _ctr = nullptr;
      uint64_t *  _ctr_u64 = nullptr;
      byte        _key[16];
      uint64_t    _ctr_inc;
      EVP_CIPHER_CTX *_aes = nullptr;
      byte       _iv[16];

};

