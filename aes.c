/*
This implemenations is basically the same avaiable at https://github.com/kokke/tiny-AES-c.
The changes in the code are to use the transformations instead of subtitutions tables for example during the S-box stage.
This is to implement variations of the AES with Varied Galois Field. 
*/

/*
The implementation is verified against the test vectors in:
  National Institute of Standards and Technology Special Publication 800-38A 2001 ED

ECB-AES128
----------

  plain-text:
    6bc1bee22e409f96e93d7e117393172a
    ae2d8a571e03ac9c9eb76fac45af8e51
    30c81c46a35ce411e5fbc1191a0a52ef
    f69f2445df4f9b17ad2b417be66c3710

  key:
    2b7e151628aed2a6abf7158809cf4f3c

  resulting cipher
    3ad77bb40d7a3660a89ecaf32466ef97 
    f5d3d58503b9699de785895a96fdbaaf 
    43b1cd7f598ece23881b00e3ed030688 
    7b0c785e27e8ad3f8223207104725dd4 


NOTE:   String length must be evenly divisible by 16byte (str_len % 16 == 0)
        You should pad the end of the string with zeros if this is not the case.
        For AES192/256 the key size is proportionally larger.

*/


/*****************************************************************************/
/* Includes:                                                                 */
/*****************************************************************************/
#include <stdint.h>
#include <string.h> // CBC mode, for memset
#include "aes.h"
#include "VGF_AES.h"
#include<stdio.h>

/*****************************************************************************/
/* Defines:                                                                  */
/*****************************************************************************/
// The number of columns comprising a state in AES. This is a constant in AES. Value=4
#define Nb 4
#define Nk 4        // The number of 32 bit words in a key.
#define Nr 10       // The number of rounds in AES Cipher.

#if defined(WITH_TRANSFORMATION)
	#define with_transformation 1
#else
	#define with_transformation 0
#endif

// jcallan@github points out that declaring Multiply as a function 
// reduces code size considerably with the Keil ARM compiler.
// See this link for more information: https://github.com/kokke/tiny-AES-C/pull/3
#ifndef MULTIPLY_AS_A_FUNCTION
  #define MULTIPLY_AS_A_FUNCTION 0
#endif




/*****************************************************************************/
/* Private variables:                                                        */
/*****************************************************************************/
// state - array holding the intermediate results during decryption.
typedef uint8_t state_t[4][4];
static uint8_t sbox[256];
static uint8_t rsbox[256];

// The round constant word array, Rcon[i], contains the values given by 
// x to the power (i-1) being powers of x (x is denoted as {02}) in the field GF(2^8)
static uint8_t Rcon[11];

/*
 * Jordan Goulder points out in PR #12 (https://github.com/kokke/tiny-AES-C/pull/12),
 * that you can remove most of the elements in the Rcon array, because they are unused.
 *
 * From Wikipedia's article on the Rijndael key schedule @ https://en.wikipedia.org/wiki/Rijndael_key_schedule#Rcon
 * 
 * "Only the first some of these constants are actually used – up to rcon[10] for AES-128 (as 11 round keys are needed), 
 *  up to rcon[8] for AES-192, up to rcon[7] for AES-256. rcon[0] is not used in AES algorithm."
 */


/*****************************************************************************/
/* Private functions:                                                        */
/*****************************************************************************/
/*
static uint8_t getSBoxValue(uint8_t num)
{
  return sbox[num];
}
*/
#define getSBoxValue(num) (sbox[(num)])
/*
static uint8_t getSBoxInvert(uint8_t num)
{
  return rsbox[num];
}
*/
#define getSBoxInvert(num) (rsbox[(num)])

// This function produces Nb(Nr+1) round keys. The round keys are used in each round to decrypt the states. 
static void KeyExpansion(uint8_t* RoundKey, const uint8_t* Key)
{
  unsigned i, j, k;
  uint8_t tempa[4]; // Used for the column/row operations
  
  // The first round key is the key itself.
  for (i = 0; i < Nk; ++i)
  {
    RoundKey[(i * 4) + 0] = Key[(i * 4) + 0];
    RoundKey[(i * 4) + 1] = Key[(i * 4) + 1];
    RoundKey[(i * 4) + 2] = Key[(i * 4) + 2];
    RoundKey[(i * 4) + 3] = Key[(i * 4) + 3];
  }

  // All other round keys are found from the previous round keys.
  for (i = Nk; i < Nb * (Nr + 1); ++i)
  {
    {
      k = (i - 1) * 4;
      tempa[0]=RoundKey[k + 0];
      tempa[1]=RoundKey[k + 1];
      tempa[2]=RoundKey[k + 2];
      tempa[3]=RoundKey[k + 3];

    }

    if (i % Nk == 0)
    {
      // This function shifts the 4 bytes in a word to the left once.
      // [a0,a1,a2,a3] becomes [a1,a2,a3,a0]

      // Function RotWord()
      {
        k = tempa[0];
        tempa[0] = tempa[1];
        tempa[1] = tempa[2];
        tempa[2] = tempa[3];
        tempa[3] = k;
      }

      // SubWord() is a function that takes a four-byte input word and 
      // applies the S-box to each of the four bytes to produce an output word.

      // Function Subword()
      {
        tempa[0] = getSBoxValue(tempa[0]);
        tempa[1] = getSBoxValue(tempa[1]);
        tempa[2] = getSBoxValue(tempa[2]);
        tempa[3] = getSBoxValue(tempa[3]);
      }

      tempa[0] = tempa[0] ^ Rcon[i/Nk];
    }
    j = i * 4; k=(i - Nk) * 4;
    RoundKey[j + 0] = RoundKey[k + 0] ^ tempa[0];
    RoundKey[j + 1] = RoundKey[k + 1] ^ tempa[1];
    RoundKey[j + 2] = RoundKey[k + 2] ^ tempa[2];
    RoundKey[j + 3] = RoundKey[k + 3] ^ tempa[3];
  }
}

void AES_init_ctx(struct AES_ctx* ctx, const uint8_t* key)
{
  KeyExpansion(ctx->RoundKey, key);
}

// This function adds the round key to state.
// The round key is added to the state by an XOR function.
static void AddRoundKey(uint8_t round,state_t* state,uint8_t* RoundKey)
{
  uint8_t i,j;
  for (i = 0; i < 4; ++i)
  {
    for (j = 0; j < 4; ++j)
    {
      (*state)[i][j] ^= RoundKey[(round * Nb * 4) + (i * Nb) + j];
    }
  }
}

// The SubBytes Function Substitutes the values in the
// state matrix with values in an S-box.
static void SubBytes(state_t* state){
  uint8_t i, j;
	if(with_transformation){
		for (i = 0; i < 4; ++i){
                        for (j = 0; j < 4; ++j){
                                (*state)[j][i] = getSBoxValueWithTransformation((*state)[j][i]);
                        }
                }
	}else{
		for (i = 0; i < 4; ++i){
			for (j = 0; j < 4; ++j){
				(*state)[j][i] = getSBoxValue((*state)[j][i]);
	    		}
  		}
	}
}

// The ShiftRows() function shifts the rows in the state to the left.
// Each row is shifted with different offset.
// Offset = Row number. So the first row is not shifted.
static void ShiftRows(state_t* state)
{
  uint8_t temp;

  // Rotate first row 1 columns to left  
  temp           = (*state)[0][1];
  (*state)[0][1] = (*state)[1][1];
  (*state)[1][1] = (*state)[2][1];
  (*state)[2][1] = (*state)[3][1];
  (*state)[3][1] = temp;

  // Rotate second row 2 columns to left  
  temp           = (*state)[0][2];
  (*state)[0][2] = (*state)[2][2];
  (*state)[2][2] = temp;

  temp           = (*state)[1][2];
  (*state)[1][2] = (*state)[3][2];
  (*state)[3][2] = temp;

  // Rotate third row 3 columns to left
  temp           = (*state)[0][3];
  (*state)[0][3] = (*state)[3][3];
  (*state)[3][3] = (*state)[2][3];
  (*state)[2][3] = (*state)[1][3];
  (*state)[1][3] = temp;
}

// MixColumns function mixes the columns of the state matrix
static void MixColumns(state_t* state,int w, uint64_t poly)
{
  uint8_t i;
  uint8_t Tmp, Tm, t;
	 gf_t gf;
        gf_init_hard(&gf, w, GF_MULT_DEFAULT, GF_REGION_DEFAULT, GF_DIVIDE_DEFAULT,poly, 0, 0,NULL, NULL);
  for (i = 0; i < 4; ++i)
  { 
	t   = (*state)[i][0];
	Tmp = (*state)[i][0] ^ (*state)[i][1] ^ (*state)[i][2] ^ (*state)[i][3] ;
	Tm  = (*state)[i][0] ^ (*state)[i][1] ;
	Tm = gf.multiply.w32(&gf,Tm,2);
	(*state)[i][0] ^= Tm ^ Tmp ;
	Tm  = (*state)[i][1] ^ (*state)[i][2] ;
	Tm = gf.multiply.w32(&gf,Tm,2);
	(*state)[i][1] ^= Tm ^ Tmp ;
	Tm  = (*state)[i][2] ^ (*state)[i][3];
	Tm = gf.multiply.w32(&gf,Tm,2);
	(*state)[i][2] ^= Tm ^ Tmp ;
	Tm  = (*state)[i][3] ^ t ;
	Tm = gf.multiply.w32(&gf,Tm,2);
	(*state)[i][3] ^= Tm ^ Tmp ;
	
  }
}

// MixColumns function mixes the columns of the state matrix.
// The method used to multiply may be difficult to understand for the inexperienced.
// Please use the references to gain more information.
static void InvMixColumns(state_t* state,int w, uint64_t poly)
{
  int i;
  uint8_t a, b, c, d;
	gf_t gf;
	gf_init_hard(&gf, w, GF_MULT_DEFAULT, GF_REGION_DEFAULT, GF_DIVIDE_DEFAULT,poly, 0, 0,NULL, NULL);	
  for (i = 0; i < 4; ++i)
  { 
    a = (*state)[i][0];
    b = (*state)[i][1];
    c = (*state)[i][2];
    d = (*state)[i][3];

    (*state)[i][0] = gf.multiply.w32(&gf,a, 0x0e) ^ gf.multiply.w32(&gf,b, 0x0b) ^ gf.multiply.w32(&gf,c, 0x0d) ^ gf.multiply.w32(&gf,d, 0x09);
    (*state)[i][1] = gf.multiply.w32(&gf,a, 0x09) ^ gf.multiply.w32(&gf,b, 0x0e) ^ gf.multiply.w32(&gf,c, 0x0b) ^ gf.multiply.w32(&gf,d, 0x0d);
    (*state)[i][2] = gf.multiply.w32(&gf,a, 0x0d) ^ gf.multiply.w32(&gf,b, 0x09) ^ gf.multiply.w32(&gf,c, 0x0e) ^ gf.multiply.w32(&gf,d, 0x0b);
    (*state)[i][3] = gf.multiply.w32(&gf,a, 0x0b) ^ gf.multiply.w32(&gf,b, 0x0d) ^ gf.multiply.w32(&gf,c, 0x09) ^ gf.multiply.w32(&gf,d, 0x0e);
  }
}


// The SubBytes Function Substitutes the values in the
// state matrix with values in an S-box.
static void InvSubBytes(state_t* state){
	uint8_t i, j;
	if(with_transformation){
		for (i = 0; i < 4; ++i){
			for (j = 0; j < 4; ++j){
				(*state)[j][i] = getSBoxInvertWithTransformation((*state)[j][i]);
			}
  		}
	}else{
		for (i = 0; i < 4; ++i){
                        for (j = 0; j < 4; ++j){
                                (*state)[j][i] = getSBoxInvert((*state)[j][i]);
                        }
                }
	}
}

static void InvShiftRows(state_t* state)
{
  uint8_t temp;

  // Rotate first row 1 columns to right  
  temp = (*state)[3][1];
  (*state)[3][1] = (*state)[2][1];
  (*state)[2][1] = (*state)[1][1];
  (*state)[1][1] = (*state)[0][1];
  (*state)[0][1] = temp;

  // Rotate second row 2 columns to right 
  temp = (*state)[0][2];
  (*state)[0][2] = (*state)[2][2];
  (*state)[2][2] = temp;

  temp = (*state)[1][2];
  (*state)[1][2] = (*state)[3][2];
  (*state)[3][2] = temp;

  // Rotate third row 3 columns to right
  temp = (*state)[0][3];
  (*state)[0][3] = (*state)[1][3];
  (*state)[1][3] = (*state)[2][3];
  (*state)[2][3] = (*state)[3][3];
  (*state)[3][3] = temp;
}


// Cipher is the main function that encrypts the PlainText.
static void Cipher(state_t* state, uint8_t* RoundKey, int w, uint64_t poly)
{
  uint8_t round = 0;

  // Add the First round key to the state before starting the rounds.
  AddRoundKey(0, state, RoundKey);
  
  // There will be Nr rounds.
  // The first Nr-1 rounds are identical.
  // These Nr-1 rounds are executed in the loop below.
  for (round = 1; round < Nr; ++round)
  {
    SubBytes(state);
    ShiftRows(state);
    MixColumns(state,w,poly);
    AddRoundKey(round, state, RoundKey);
  }
  
  // The last round is given below.
  // The MixColumns function is not here in the last round.
  SubBytes(state);
  ShiftRows(state);
  AddRoundKey(Nr, state, RoundKey);
}

static void InvCipher(state_t* state,uint8_t* RoundKey, int w, uint64_t poly)
{
  uint8_t round = 0;

  // Add the First round key to the state before starting the rounds.
  AddRoundKey(Nr, state, RoundKey); 

  // There will be Nr rounds.
  // The first Nr-1 rounds are identical.
  // These Nr-1 rounds are executed in the loop below.
  for (round = (Nr - 1); round > 0; --round)
  {
    InvShiftRows(state);
    InvSubBytes(state);
    AddRoundKey(round, state, RoundKey);
    InvMixColumns(state,w,poly);
  }
  
  // The last round is given below.
  // The MixColumns function is not here in the last round.
  InvShiftRows(state);
  InvSubBytes(state);
  AddRoundKey(0, state, RoundKey);
}


/*****************************************************************************/
/* Public functions:                                                         */
/*****************************************************************************/
void AES_encrypt(struct AES_ctx *ctx,const uint8_t* buf,int w, uint64_t poly)
{
  // The next function call encrypts the PlainText with the Key using AES algorithm.
  Cipher((state_t*)buf, ctx->RoundKey,w,poly);
}

void AES_decrypt(struct AES_ctx* ctx,const uint8_t* buf,int w, uint64_t poly)
{
  // The next function call decrypts the PlainText with the Key using AES algorithm.
  InvCipher((state_t*)buf, ctx->RoundKey,w,poly);
}

uint8_t getSBoxValueWithTransformation(uint8_t num){
	return sbox[num];
}

uint8_t getSBoxInvertWithTransformation(uint8_t num){
        return rsbox[num];
}

int GCDExtended(int num,int x,int *y,int *z){
	if(num==0){
		*y=0;
		*z=1;
		return x;
	}

	int y1, z1;
	int gcd= GCDExtended(x%num,num,&y1,&z1);
	*y = z1 - (x/num) * y1;
	*z = y1;

    return gcd;
}

uint8_t getInv(uint8_t num){
	int x,y,z;
	x=283;//(x^8+x^4+x^3+x+1)
	GCDExtended(num,x,&y,&z);
	return y+x;
}


/* Add two numbers in a GF(2^8) finite field */
uint8_t gadd(uint8_t a, uint8_t b) {
	return a ^ b;
}

/* Subtract two numbers in a GF(2^8) finite field */
uint8_t gsub(uint8_t a, uint8_t b) {
	return a ^ b;
}


//Populates sbox and rsbox
void initialize_boxes(int w, uint64_t poly){
        int i;
        gf_t gf;
        uint8_t inv, mult, add;
//Create the proper instance of the gf_t object using the polynomial x^8+x^4+x^3+x+1: */
        gf_init_hard(&gf, w, GF_MULT_DEFAULT, GF_REGION_DEFAULT, GF_DIVIDE_DEFAULT,
                      poly, 0, 0,NULL, NULL);

//initialize The round constant word array, Rcon[i], contains the values given by 
// x to the power (i-1) being powers of x (x is denoted as {02}) in the field GF(2^w)
//the complete array is filled depending on the GF size
	Rcon[0]=0x00;//Actually 0x8d for gf8 in Rijndael, but never actually used
	Rcon[1]=0x01;

switch(w){
	case(8):
		Rcon[2]=0x02;
		Rcon[3]=0x04;
		Rcon[4]=0x08;
		Rcon[5]=0x10;
		Rcon[6]=0x20;
		Rcon[7]=0x40;
		Rcon[8]=0x80;
		Rcon[9]=0x1b;
		Rcon[10]=0x36;
		for(i=0;i<256;++i){
                	if(i==0){
                        	sbox[i]=99;
	                        rsbox[i]=82;
        	        }else{
                	        inv=gf.inverse.w32(&gf, i);
                        	mult=gf8_at_multiply(inv,248);
	                        add=mult^99;
        	                sbox[i]=add;
				add=i^99;
                        	mult=gf8_at_multiply(add,82);
	                        inv=gf.inverse.w32(&gf, mult);
        	                rsbox[i]=inv;
                	}
        	}
	break;
	case(4):
		Rcon[2]=0x02;
                Rcon[3]=0x04;
                Rcon[4]=0x08;
                Rcon[5]=0x03;
                Rcon[6]=0x06;
                Rcon[7]=0x0c;
                Rcon[8]=0x0b;
                Rcon[9]=0x05;
                Rcon[10]=0x0a;
		for(i=0;i<16;++i){
                	if(i==0){
                        	sbox[i]=10;
				rsbox[i]=11;
        	        }else{
                	        inv=gf.inverse.w32(&gf, i);
                        	mult=gf4_at_multiply(inv,11);
	                        add=mult^10;
        	                sbox[i]=add;
				add=i^10;
                        	mult=gf4_at_multiply(add,14);
	                        inv=gf.inverse.w32(&gf,mult);
        	                rsbox[i]=inv;
                	}
        	}
	break;
	}
}
