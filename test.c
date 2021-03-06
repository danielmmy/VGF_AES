#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "aes.h"

#define BUFFSIZE 16

static void phex(uint8_t* str);
static void test_encrypt(void);
static void test_decrypt(void);
static void test_encrypt_gf8(void);
static void test_decrypt_gf8(void);
static void test_encrypt_gf4(void);
static void test_decrypt_gf4(void);
static void test_cipher_files(int argc, char **argv);


int main(int argc, char **argv){
	
	if(argc==6){
		test_cipher_files(argc,argv);
	}else{
		initialize_boxes(8,0x11b);
		test_encrypt();
		test_decrypt();
		test_encrypt_gf8();
		printf("\n");
		test_decrypt_gf8();
		printf("\n");
		initialize_boxes(4,0x13);
		test_encrypt_gf4();
		printf("\n");
		test_decrypt_gf4();

		printf("\n\nUse \"test [CcDd] filenamein filenameout GF(hex) characteristical_polynomial(hex)\" to cipher or decipher a file\n");
	}
	return 0;
}

static void test_cipher_files(int argc, char **argv){
	char *endptr;
	unsigned long w=strtoul(argv[4], &endptr, 16);
	if(endptr==argv[4]){
		printf("%s is not a valid GF\nExiting...\n",argv[4]);
		exit(0);
	}else if(*endptr!='\0'){
	// String begins with a valid number, but also contains something else after the number
		printf("%s is not a valid GF\nExiting...\n",argv[4]);
                exit(0);
	}
	unsigned long poly=strtoul(argv[5], &endptr, 16);
        if(endptr==argv[5]){
                printf("%s is not a valid GF\nExiting...\n",argv[5]);
                exit(0);
        }else if(*endptr!='\0'){
        // String begins with a valid number, but also contains something else after the number
                printf("%s is not a valid GF\nExiting...\n",argv[5]);
                exit(0);
        }
	initialize_boxes(w,poly);	
        FILE *fpr = fopen(argv[2], "rb");
        FILE *fpw = fopen(argv[3], "wb");
        uint8_t buff[BUFFSIZE];
        uint8_t buf[BUFFSIZE];
        uint8_t buf2[BUFFSIZE];
        size_t readb;
        if (fpr!=NULL && fpw!=NULL) {
		uint8_t *key;
		if(w==8){
			key=(uint8_t[16]){(uint8_t)0x2b,(uint8_t)0x7e,(uint8_t)0x15,(uint8_t)0x16,(uint8_t)0x28,(uint8_t)0xae,(uint8_t)0xd2,(uint8_t)0xa6,(uint8_t)0xab,(uint8_t)0xf7,(uint8_t)0x15,(uint8_t)0x88,(uint8_t)0x09,(uint8_t)0xcf,(uint8_t)0x4f,(uint8_t)0x3c};
		}else if(w==4){
			key=(uint8_t[16]){(uint8_t)0xf,(uint8_t)0xe,(uint8_t)0xd,(uint8_t)0xc,(uint8_t)0xb,(uint8_t)0xa,(uint8_t)0x9,(uint8_t)0x8,(uint8_t)0x7,(uint8_t)0x6,(uint8_t)0x5,(uint8_t)0x4,(uint8_t)0x3,(uint8_t)0x2,(uint8_t)0x1,(uint8_t)0x0};
		}else{
			printf("GF not yet implemented, exiting...\n");
			exit(0);
		}
		memset(buf, 0, 16);
        	memset(buf2, 0, 16);
                struct AES_ctx ctx;
                AES_init_ctx(&ctx, key);
                if(argv[1][0]=='C'||argv[1][0]=='c'){
			readb = fread(buff,1,BUFFSIZE,fpr);
                        while(readb==BUFFSIZE){
				if(w==4){
					int i,j;
					uint8_t tmp[16];
					uint8_t tmp2[8];
					for(i=0;i<8;++i){
						tmp[i*2]=buff[i]<<4;
						tmp[i*2]=tmp[i*2]>>4;
						tmp[i*2+1]=buff[i]>>4;
					}
					AES_encrypt(&ctx,tmp,w,poly);
					for(i=0;i<8;++i){
						tmp2[i]=tmp[i*2+1]<<4;
						tmp2[i]=tmp2[i]^tmp[i*2];
					}
					fwrite(tmp2,8,1,fpw);
//					fwrite(tmp,BUFFSIZE,1,fpw);
					for(i=8,j=0;i<16;++i, ++j){
						tmp[j*2]=buff[i]<<4;
                                                tmp[j*2]=tmp[j*2]>>4;
                                                tmp[j*2+1]=buff[i]>>4;
                                        }
                                        AES_encrypt(&ctx,tmp,w,poly);
					for(i=0;i<8;++i){
						tmp2[i]=tmp[i*2+1]<<4;
						tmp2[i]=tmp2[i]^tmp[i*2];
                                        }
                                        fwrite(tmp2,8,1,fpw);
//					fwrite(tmp,BUFFSIZE,1,fpw);
				}else{
                        		AES_encrypt(&ctx,buff,w,poly);
	                                fwrite(buff,BUFFSIZE,1,fpw);
				}
        	                readb = fread(buff,1,BUFFSIZE,fpr);
                        }
		}else if(argv[1][0]=='D'||argv[1][0]=='d'){
                	readb = fread(buff,1,BUFFSIZE,fpr);
                        while(readb==BUFFSIZE){
				if(w==4){
					int i,j;
                                        uint8_t tmp[16];
                                        uint8_t tmp2[8];
                                        for(i=0;i<8;++i){
                                                tmp[i*2]=buff[i]<<4;
                                                tmp[i*2]=tmp[i*2]>>4;
                                                tmp[i*2+1]=buff[i]>>4;
                                        }
                                        AES_decrypt(&ctx,tmp,w,poly);
                                        for(i=0;i<8;++i){
                                                tmp2[i]=tmp[i*2+1]<<4;
                                                tmp2[i]=tmp2[i]^tmp[i*2];
                                        }
                                        fwrite(tmp2,8,1,fpw);
                                        for(i=8,j=0;i<16;++i, ++j){
                                                tmp[j*2]=buff[i]<<4;
                                                tmp[j*2]=tmp[j*2]>>4;
                                                tmp[j*2+1]=buff[i]>>4;
                                        }
                                        AES_decrypt(&ctx,tmp,w,poly);
                                        for(i=0;i<8;++i){
                                                tmp2[i]=tmp[i*2+1]<<4;
                                                tmp2[i]=tmp2[i]^tmp[i*2];
                                        }
                                        fwrite(tmp2,8,1,fpw);
#if 0
                                        int i;
                                        uint8_t tmp[8]={[0 ... 7]=0};
					AES_decrypt(&ctx,buff,w,poly);
                                        for(i=0;i<8;++i){
						tmp[i]=buff[i*2+1]<<4;
                                                tmp[i]=tmp[i]^buff[i*2];
                                        }
                                        fwrite(tmp,8,1,fpw);
#endif
                                }else{
                                        AES_decrypt(&ctx,buff,w,poly);
                                        fwrite(buff,BUFFSIZE,1,fpw);
                                }

                                readb = fread(buff,1,BUFFSIZE,fpr);
			}
		}else{
                	printf("\n\nUse test [CcDd] filenamein filenameout GF(hex) characteristical_polynomial(hex)\n");
		}
                fclose(fpr);
                fclose(fpw);
	}
}


// prints string as hex
static void phex(uint8_t* str){

#ifdef AES128
    uint8_t len = 16;
#elif defined(AES192)
    uint8_t len = 24;
#elif defined(AES256)
    uint8_t len = 32;
#endif

    unsigned char i;
    for (i = 0; i < len; ++i)
        printf("%.2x", str[i]);
    printf("\n");
}

static void test_encrypt()
{
#ifdef AES128
    uint8_t key[] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
    uint8_t out[] = { 0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60, 0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97 };
#elif defined(AES192)
    uint8_t key[] = { 0x8e, 0x73, 0xb0, 0xf7, 0xda, 0x0e, 0x64, 0x52, 0xc8, 0x10, 0xf3, 0x2b, 0x80, 0x90, 0x79, 0xe5,
                      0x62, 0xf8, 0xea, 0xd2, 0x52, 0x2c, 0x6b, 0x7b };
    uint8_t out[] = { 0xbd, 0x33, 0x4f, 0x1d, 0x6e, 0x45, 0xf2, 0x5f, 0xf7, 0x12, 0xa2, 0x14, 0x57, 0x1f, 0xa5, 0xcc };
#elif defined(AES256)
    uint8_t key[] = { 0x60, 0x3d, 0xeb, 0x10, 0x15, 0xca, 0x71, 0xbe, 0x2b, 0x73, 0xae, 0xf0, 0x85, 0x7d, 0x77, 0x81,
                      0x1f, 0x35, 0x2c, 0x07, 0x3b, 0x61, 0x08, 0xd7, 0x2d, 0x98, 0x10, 0xa3, 0x09, 0x14, 0xdf, 0xf4 };
    uint8_t out[] = { 0xf3, 0xee, 0xd1, 0xbd, 0xb5, 0xd2, 0xa0, 0x3c, 0x06, 0x4b, 0x5a, 0x7e, 0x3d, 0xb1, 0x81, 0xf8 };
#endif

    uint8_t in[]  = { 0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a };
    struct AES_ctx ctx;

    AES_init_ctx(&ctx, key);
    AES_encrypt(&ctx, in,8,0x11b);

    printf("encrypt: ");

    if (0 == memcmp((char*) out, (char*) in, 16))
    {
        printf("SUCCESS!\n");
    }
    else
    {
        printf("FAILURE!\n");
    }
}


static void test_decrypt(void){
	uint8_t key[] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };
	uint8_t in[]  = { 0x3a, 0xd7, 0x7b, 0xb4, 0x0d, 0x7a, 0x36, 0x60, 0xa8, 0x9e, 0xca, 0xf3, 0x24, 0x66, 0xef, 0x97 };
	uint8_t out[]   = { 0x6b, 0xc1, 0xbe, 0xe2, 0x2e, 0x40, 0x9f, 0x96, 0xe9, 0x3d, 0x7e, 0x11, 0x73, 0x93, 0x17, 0x2a };
	struct AES_ctx ctx;
    
	AES_init_ctx(&ctx, key);
	AES_decrypt(&ctx, in,8,0x11b);

	printf("decrypt: ");

	if (0 == memcmp((char*) out, (char*) in, 16)){
		printf("SUCCESS!\n");
	}else{
		printf("FAILURE!\n");
	}
}


static void test_encrypt_gf8(void){
// Example of more verbose verification
	uint8_t i, buf[64], buf2[64];
	// 128bit key
	uint8_t key[16] =        { (uint8_t) 0x2b, (uint8_t) 0x7e, (uint8_t) 0x15, (uint8_t) 0x16, (uint8_t) 0x28, (uint8_t) 0xae, (uint8_t) 0xd2, (uint8_t) 0xa6, (uint8_t) 0xab, (uint8_t) 0xf7, (uint8_t) 0x15, (uint8_t) 0x88, (uint8_t) 0x09, (uint8_t) 0xcf, (uint8_t) 0x4f, (uint8_t) 0x3c };
	// 512bit text
	uint8_t plain_text[64] = { (uint8_t) 0x6b, (uint8_t) 0xc1, (uint8_t) 0xbe, (uint8_t) 0xe2, (uint8_t) 0x2e, (uint8_t) 0x40, (uint8_t) 0x9f, (uint8_t) 0x96, (uint8_t) 0xe9, (uint8_t) 0x3d, (uint8_t) 0x7e, (uint8_t) 0x11, (uint8_t) 0x73, (uint8_t) 0x93, (uint8_t) 0x17, (uint8_t) 0x2a,(uint8_t) 0xae, (uint8_t) 0x2d, (uint8_t) 0x8a, (uint8_t) 0x57, (uint8_t) 0x1e, (uint8_t) 0x03, (uint8_t) 0xac, (uint8_t) 0x9c, (uint8_t) 0x9e, (uint8_t) 0xb7, (uint8_t) 0x6f, (uint8_t) 0xac, (uint8_t) 0x45, (uint8_t) 0xaf, (uint8_t) 0x8e, (uint8_t) 0x51,(uint8_t) 0x30, (uint8_t) 0xc8, (uint8_t) 0x1c, (uint8_t) 0x46, (uint8_t) 0xa3, (uint8_t) 0x5c, (uint8_t) 0xe4, (uint8_t) 0x11, (uint8_t) 0xe5, (uint8_t) 0xfb, (uint8_t) 0xc1, (uint8_t) 0x19, (uint8_t) 0x1a, (uint8_t) 0x0a, (uint8_t) 0x52, (uint8_t) 0xef,(uint8_t) 0xf6, (uint8_t) 0x9f, (uint8_t) 0x24, (uint8_t) 0x45, (uint8_t) 0xdf, (uint8_t) 0x4f, (uint8_t) 0x9b, (uint8_t) 0x17, (uint8_t) 0xad, (uint8_t) 0x2b, (uint8_t) 0x41, (uint8_t) 0x7b, (uint8_t) 0xe6, (uint8_t) 0x6c, (uint8_t) 0x37, (uint8_t) 0x10 };

	memset(buf, 0, 64);
	memset(buf2, 0, 64);

	// print text to encrypt, key and IV
	printf("\nencrypt GF8:\n");
	printf("plain text:\n");
	for (i = (uint8_t) 0; i < (uint8_t) 4; ++i){
		phex(plain_text + i * (uint8_t) 16);
	}
	printf("key:\n");
	phex(key);

    // print the resulting cipher as 4 x 16 byte strings
    printf("cipher:\n");
    
    struct AES_ctx ctx;
    AES_init_ctx(&ctx, key);

    for (i = 0; i < 4; ++i)
    {
      AES_encrypt(&ctx, plain_text + (i * 16),8,0x11b);
      phex(plain_text + (i * 16));
    }
	printf("\n");
}

static void test_decrypt_gf8(void)
{
    // Example of more verbose verification

    uint8_t i, buf[64], buf2[64];

    // 128bit key
    uint8_t key[16] =        { (uint8_t) 0x2b, (uint8_t) 0x7e, (uint8_t) 0x15, (uint8_t) 0x16, (uint8_t) 0x28, (uint8_t) 0xae, (uint8_t) 0xd2, (uint8_t) 0xa6, (uint8_t) 0xab, (uint8_t) 0xf7, (uint8_t) 0x15, (uint8_t) 0x88, (uint8_t) 0x09, (uint8_t) 0xcf, (uint8_t) 0x4f, (uint8_t) 0x3c };
    // 512bit text
    uint8_t cipher[64] = { (uint8_t) 0x3a,(uint8_t) 0xd7,(uint8_t) 0x7b,(uint8_t) 0xb4,(uint8_t) 0x0d,(uint8_t) 0x7a,(uint8_t) 0x36,(uint8_t) 0x60,(uint8_t) 0xa8,(uint8_t) 0x9e,(uint8_t) 0xca,(uint8_t) 0xf3,(uint8_t) 0x24,(uint8_t) 0x66,(uint8_t) 0xef,(uint8_t) 0x97,(uint8_t) 0xf5,(uint8_t) 0xd3,(uint8_t) 0xd5,(uint8_t) 0x85,(uint8_t) 0x03,(uint8_t) 0xb9,(uint8_t) 0x69,(uint8_t) 0x9d,(uint8_t) 0xe7,(uint8_t) 0x85,(uint8_t) 0x89,(uint8_t) 0x5a,(uint8_t) 0x96,(uint8_t) 0xfd,(uint8_t) 0xba,(uint8_t) 0xaf,(uint8_t) 0x43,(uint8_t) 0xb1,(uint8_t) 0xcd,(uint8_t) 0x7f,(uint8_t) 0x59,(uint8_t) 0x8e,(uint8_t) 0xce,(uint8_t) 0x23,(uint8_t) 0x88,(uint8_t) 0x1b,(uint8_t) 0x00,(uint8_t) 0xe3,(uint8_t) 0xed,(uint8_t) 0x03,(uint8_t) 0x06,(uint8_t) 0x88,(uint8_t) 0x7b,(uint8_t) 0x0c,(uint8_t) 0x78,(uint8_t) 0x5e,(uint8_t) 0x27,(uint8_t) 0xe8,(uint8_t) 0xad,(uint8_t) 0x3f,(uint8_t) 0x82,(uint8_t) 0x23,(uint8_t) 0x20,(uint8_t) 0x71,(uint8_t) 0x04,(uint8_t) 0x72,(uint8_t) 0x5d,(uint8_t) 0xd4};

    memset(buf, 0, 64);
    memset(buf2, 0, 64);

    // print text to encrypt, key and IV
    printf("decrypt GF8:\n");
    printf("cipher:\n");
    for (i = (uint8_t) 0; i < (uint8_t) 4; ++i)
    {
        phex(cipher + i * (uint8_t) 16);
    }
    printf("key:\n");
    phex(key);

    // print the resulting cipher as 4 x 16 byte strings
    printf("plain text:\n");
    
    struct AES_ctx ctx;
    AES_init_ctx(&ctx, key);

    for (i = 0; i < 4; ++i)
    {
      AES_decrypt(&ctx, cipher + (i * 16),8,0x11b);
      phex(cipher + (i * 16));
    }
}

static void test_encrypt_gf4(void){

	// Example of more verbose verification
	uint8_t i, buf[64], buf2[64];

	// 128bit key
	uint8_t key[16] =        { (uint8_t) 0xf, (uint8_t) 0xe, (uint8_t) 0xd, (uint8_t) 0xc, (uint8_t) 0xb, (uint8_t) 0xa, (uint8_t) 0x9, (uint8_t) 0x8, (uint8_t) 0x7, (uint8_t) 0x6, (uint8_t) 0x5, (uint8_t) 0x4, (uint8_t) 0x3, (uint8_t) 0x2, (uint8_t) 0x1, (uint8_t) 0x0 };
	// 512bit text
	uint8_t plain_text[64] = { (uint8_t) 0x0, (uint8_t) 0x1, (uint8_t) 0x2, (uint8_t) 0x3, (uint8_t) 0x4, (uint8_t) 0x5, (uint8_t) 0x6, (uint8_t) 0x7, (uint8_t) 0x8, (uint8_t) 0x9, (uint8_t) 0xa, (uint8_t) 0xb, (uint8_t) 0xc, (uint8_t) 0xd, (uint8_t) 0xe, (uint8_t) 0xf,
	(uint8_t) 0xf, (uint8_t) 0xe, (uint8_t) 0xd, (uint8_t) 0xc, (uint8_t) 0xb, (uint8_t) 0xa, (uint8_t) 0x9, (uint8_t) 0x8, (uint8_t) 0x7, (uint8_t) 0x6, (uint8_t) 0x5, (uint8_t) 0x4, (uint8_t) 0x3, (uint8_t) 0x2, (uint8_t) 0x1, (uint8_t) 0x0,
	(uint8_t) 0x1, (uint8_t) 0x2, (uint8_t) 0x2, (uint8_t) 0x5, (uint8_t) 0xa, (uint8_t) 0x8, (uint8_t) 0x6, (uint8_t) 0x7, (uint8_t) 0xe, (uint8_t) 0x9, (uint8_t) 0xa, (uint8_t) 0xf, (uint8_t) 0xe, (uint8_t) 0xd, (uint8_t) 0xe, (uint8_t) 0xf,
	(uint8_t) 0xf, (uint8_t) 0x1, (uint8_t) 0xd, (uint8_t) 0xc, (uint8_t) 0xb, (uint8_t) 0x8, (uint8_t) 0x9, (uint8_t) 0x9, (uint8_t) 0x7, (uint8_t) 0xc, (uint8_t) 0x5, (uint8_t) 0x4, (uint8_t) 0x1, (uint8_t) 0xf, (uint8_t) 0x3, (uint8_t) 0xa};

	memset(buf, 0, 64);
	memset(buf2, 0, 64);


	// print text to encrypt, key and IV
	printf("encrypt GF4:\n");
	printf("plain text:\n");
	for (i = (uint8_t) 0; i < (uint8_t) 4; ++i){
		phex(plain_text + i * (uint8_t) 16);
    	}

	printf("key:\n");
	phex(key);

	// print the resulting cipher as 4 x 16 byte strings
	printf("cipher:\n");

	struct AES_ctx ctx;
	AES_init_ctx(&ctx, key);

	for (i = 0; i < 4; ++i){
		AES_encrypt(&ctx, plain_text + (i * 16),4,0x13);
		phex(plain_text + (i * 16));
	}
}

static void test_decrypt_gf4(void){
	        // Example of more verbose verification
        uint8_t i, buf[64], buf2[64];

        // 128bit key
        uint8_t key[16] =        { (uint8_t) 0xf, (uint8_t) 0xe, (uint8_t) 0xd, (uint8_t) 0xc, (uint8_t) 0xb, (uint8_t) 0xa, (uint8_t) 0x9, (uint8_t) 0x8, (uint8_t) 0x7, (uint8_t) 0x6, (uint8_t) 0x5, (uint8_t) 0x4, (uint8_t) 0x3, (uint8_t) 0x2, (uint8_t) 0x1, (uint8_t) 0x0 };
        // 512bit text
        uint8_t cypher_text[64] = {(uint8_t)0x0a,(uint8_t)0x0c,(uint8_t)0x00,(uint8_t)0x02,(uint8_t)0x08,(uint8_t)0x0d,(uint8_t)0x02,(uint8_t)0x09,(uint8_t)0x07,(uint8_t)0x04,(uint8_t)0x0f,(uint8_t)0x04,(uint8_t)0x00,(uint8_t)0x0f,(uint8_t)0x0c,(uint8_t)0x0d,(uint8_t)0x0f,(uint8_t)0x04,(uint8_t)0x01,(uint8_t)0x06,(uint8_t)0x0b,(uint8_t)0x0e,(uint8_t)0x0a,(uint8_t)0x06,(uint8_t)0x08,(uint8_t)0x0e,(uint8_t)0x0f,(uint8_t)0x0e,(uint8_t)0x09,(uint8_t)0x08,(uint8_t)0x06,(uint8_t)0x03,(uint8_t)0x08,(uint8_t)0x04,(uint8_t)0x0b,(uint8_t)0x0a,(uint8_t)0x00,(uint8_t)0x08,(uint8_t)0x0b,(uint8_t)0x00,(uint8_t)0x08,(uint8_t)0x08,(uint8_t)0x0c,(uint8_t)0x0f,(uint8_t)0x00,(uint8_t)0x07,(uint8_t)0x09,(uint8_t)0x01,(uint8_t)0x05,(uint8_t)0x03,(uint8_t)0x04,(uint8_t)0x03,(uint8_t)0x00,(uint8_t)0x06,(uint8_t)0x0e,(uint8_t)0x0e,(uint8_t)0x0d,(uint8_t)0x0a,(uint8_t)0x0d,(uint8_t)0x00,(uint8_t)0x0e,(uint8_t)0x08,(uint8_t)0x0e,(uint8_t)0x0b};

        memset(buf, 0, 64);
        memset(buf2, 0, 64);


        // print cypher, key and IV
        printf("decrypt GF4:\n");
        printf("cypher text:\n");
        for (i = (uint8_t) 0; i < (uint8_t) 4; ++i){
                phex(cypher_text + i * (uint8_t) 16);
        }

        printf("key:\n");
        phex(key);

        // print the plain text as 4 x 16 byte strings
        printf("plain text:\n");

        struct AES_ctx ctx;
        AES_init_ctx(&ctx, key);

        for (i = 0; i < 4; ++i){
                AES_decrypt(&ctx, cypher_text + (i * 16),4,0x13);
                phex(cypher_text + (i * 16));
        }
}

