/*
This implemenations is basically the same avaiable at https://github.com/kokke/tiny-AES-c.
The changes in the code are to use the transformations instead of subtitutions tables for example during the s-box stage.
This is to implement variations of the AES with Varied Galois Field. 
*/


#ifndef _AES_H_
#define _AES_H_

#include <stdint.h>

#define AES128 1
#define WITH_TRANSFORMATION
#define AES_BLOCKLEN 16 //Block length in bytes AES is 128b block only
#define AES_KEYLEN 16   // Key length in bytes
#define AES_keyExpSize 176

struct AES_ctx
{
  uint8_t RoundKey[AES_keyExpSize];
};

void AES_init_ctx(struct AES_ctx* ctx, const uint8_t* key);

// buffer size is exactly AES_BLOCKLEN bytes; 
// you need only AES_init_ctx as IV is not used in ECB 
// NB: ECB is considered insecure for most uses
void AES_encrypt(struct AES_ctx* ctx, const uint8_t* buf,int w, uint64_t poly);
void AES_decrypt(struct AES_ctx* ctx, const uint8_t* buf,int w, uint64_t poly);




/*
function to get SBox value by implementing transformation
*/
uint8_t getSBoxValueWithTransformation(uint8_t num);
/*
function to get SBox Inverted value by implementing transformation
*/
uint8_t getSBoxInvertWithTransformation(uint8_t num);
/*
function to get SBox Inverted value by implementing transformation
*/
uint8_t getInv(uint8_t);

int GCDExtended(int num,int x,int *y,int *z);


/* Add two numbers in a GF(2^8) finite field */
uint8_t gadd(uint8_t a, uint8_t b);

/* Subtract two numbers in a GF(2^8) finite field */
uint8_t gsub(uint8_t a, uint8_t b);

//Populates sbox and rsbox
//w -> GF size
//poly -> characteristic polynomial
void initialize_boxes(int w, uint64_t poly);
#endif //_AES_H_
