#include <stdio.h>

#include "VGF_AES.h"


int main(int argc, char **argv){
	int i,n;
        gf_t gf;
	uint8_t inv, mult, add;

        /* Create the proper instance of the gf_t object using the polynomial x^8+x^4+x^3+x+1: */
        gf_init_hard(&gf, 8, GF_MULT_DEFAULT, GF_REGION_DEFAULT, GF_DIVIDE_DEFAULT,
                      0x11b, 0, 0,NULL, NULL);

	printf("S-BOX and inverted S-BOX for GF X^8+X^4+X^3+X+1");

	for(i=0;i<256;++i){
		if(i%16==0){
			printf("\n");
		}
		if(i==0){
			printf("%2x ",99);
		}else{
			inv=gf.inverse.w32(&gf, i);
			mult=gf8_at_multiply(inv,248);
			add=mult^99;
			printf("%2x ",add);
		}
	}
	printf("\n\n");

        for(i=0;i<256;++i){
                if(i%16==0){
                        printf("\n");
                }
                if(i==0){
                        printf("%2x ",82);
                }else{
                        add=i^99;
                        mult=gf8_at_multiply(add,82);
                        inv=gf.inverse.w32(&gf, mult);
                        printf("%2x ",inv);
                }
        }
	printf("AQUI:%2x\n\n\n",gf.divide.w32(&gf,256,0x11b));
//	printf("\n99 inv %d\n99X211=%d\n",gf.inverse.w32(&gf, 99),gf.multiply.w32(&gf,99,211));
        printf("\n\n");

	gf_init_hard(&gf, 6, GF_MULT_DEFAULT, GF_REGION_DEFAULT, GF_DIVIDE_DEFAULT,
                      0x43, 0, 0,NULL, NULL);
	
	printf("S-BOX and inverted S-BOX for GF X^6+X+1");

        for(i=0;i<64;++i){
                if(i%8==0){
                        printf("\n");
                }
                if(i==0){
                        printf("%2x ",38);
                }else{
                        inv=gf.inverse.w32(&gf, i);
                        mult=gf6_at_multiply(inv,37);
                        add=mult^38;
                        printf("%2x ",add);
                }
        }
//	printf("\n37 inv %d\n37X9=%d\n",gf.inverse.w32(&gf, 37),gf.multiply.w32(&gf,37,9));
	printf("\n\n");
        for(i=0;i<64;++i){
                if(i%8==0){
                        printf("\n");
                }
                if(i==0){
                        printf("%2x ",49);
                }else{
                        add=i^38;
                        mult=gf6_at_multiply(add,25);
                        inv=gf.inverse.w32(&gf,mult);
                        printf("%2x ",inv);
                }
        }



        printf("\n\n");
	
        printf("\n\n");

	gf_init_hard(&gf, 4, GF_MULT_DEFAULT, GF_REGION_DEFAULT, GF_DIVIDE_DEFAULT,
                      0x13, 0, 0,NULL, NULL);

        printf("S-BOX and inverted S-BOX for GF X^4+X+1");

        for(i=0;i<16;++i){
                if(i%4==0){
                        printf("\n");
                }
                if(i==0){
                        printf("%2x ",10);
                }else{
                        inv=gf.inverse.w32(&gf, i);
                        mult=gf4_at_multiply(inv,11);
                        add=mult^10;
                        printf("%2x ",add);
                }
        }
        //printf("\n5 inv %d\n5X11=%d\n",gf.inverse.w32(&gf, 5),gf.multiply.w32(&gf,5,11));
        printf("\n\n");
	for(i=0;i<16;++i){
                if(i%4==0){
                        printf("\n");
                }
                if(i==0){
                        printf("%2x ",11);
                }else{
                        add=i^10;
                        mult=gf4_at_multiply(add,14);
                        inv=gf.inverse.w32(&gf,mult);
                        printf("%2x ",inv);
                }
        }
        printf("\n\n");

	printf("\n\n");

        gf_init_hard(&gf, 2 , GF_MULT_DEFAULT, GF_REGION_DEFAULT, GF_DIVIDE_DEFAULT,
                      0x7, 0, 0,NULL, NULL);

        printf("S-BOX and inverted S-BOX for GF X^2+X+1");

        for(i=0;i<4;++i){
                if(i%2==0){
                        printf("\n");
                }
                if(i==0){
                        printf("%2x ",2);
                }else{
                        inv=gf.inverse.w32(&gf, i);
                        mult=gf2_at_multiply(inv,2);
                        add=mult^2;
                        printf("%2x ",add);
                }
        }
        //printf("\n2 inv %d\n2X3=%d\n",gf.inverse.w32(&gf, 2),gf.multiply.w32(&gf,2,3));
        printf("\n\n");
        for(i=0;i<4;++i){
                if(i%2==0){
                        printf("\n");
                }
                if(i==0){
                        printf("%2x ",3);
                }else{
                        add=i^2;
                        mult=gf2_at_multiply(add,2);
                        inv=gf.inverse.w32(&gf,mult);
                        printf("%2x ",inv);
                }
        }
        printf("\n\n");



}

