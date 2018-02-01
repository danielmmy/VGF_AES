#include <stdio.h>

#include "VGF_AES.h"

unsigned int gf8_at_multiply(uint8_t num, uint8_t S){
	int i=0;
	uint8_t counter;
//	uint8_t S=248;
	uint8_t retval=0;
	uint8_t tmp;
	for(i=7;i>=0;--i){
		tmp=S&num;
		counter=0;
    		while(tmp){
			counter+=tmp%2;
        		tmp>>=1;
    		}
		if(counter%2){
			retval|=1UL<<i;
		}

	

		//TODO need to fix using logical shift
		if(S%2){
			S>>=1;
			S|=128;
		}else{
			S>>=1;
		}
		//ENDTODO
	}
	return retval;
}


unsigned int gf6_at_multiply(uint8_t num, uint8_t S){
        int i=0;
	int j=0;
        uint8_t counter;
        uint8_t retval=0;
        uint8_t tmp;
        for(i=5;i>=0;--i){
                tmp=S&num;
                counter=0;
                for(j=0;j<6;++j){
                        counter+=tmp%2;
                        tmp>>=1;
                }
                if(counter%2){
                        retval|=1UL<<i;
                }



                //TODO need to fix using logical shift
                if(S%2){
                        S>>=1;
                        S|=32;
                }else{
                        S>>=1;
                }
                //ENDTODO
        }
        return retval&63;
}


unsigned int gf4_at_multiply(uint8_t num, uint8_t S){
        int i=0;
        int j=0;
        uint8_t counter;
        uint8_t retval=0;
        uint8_t tmp;
        for(i=3;i>=0;--i){
                tmp=S&num;
                counter=0;
                for(j=0;j<4;++j){
                        counter+=tmp%2;
                        tmp>>=1;
                }
                if(counter%2){
                        retval|=1UL<<i;
                }



                //TODO need to fix using logical shift
                if(S%2){
                        S>>=1;
                        S|=8;
                }else{
                        S>>=1;
                }
                //ENDTODO
        }
        return retval&15;
}


unsigned int gf2_at_multiply(uint8_t num, uint8_t S){
        int i=0;
        int j=0;
        uint8_t counter;
        uint8_t retval=0;
        uint8_t tmp;
        for(i=1;i>=0;--i){
                tmp=S&num;
                counter=0;
                for(j=0;j<2;++j){
                        counter+=tmp%2;
                        tmp>>=1;
                }
                if(counter%2){
                        retval|=1UL<<i;
                }



                //TODO need to fix using logical shift
                if(S%2){
                        S>>=1;
                        S|=2;
                }else{
                        S>>=1;
                }
                //ENDTODO
        }
        return retval&3;
}


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
                        mult=gf8_at_multiply(i,82);
                        add=mult^5;
                        inv=gf.inverse.w32(&gf, add);
                        printf("%2x ",inv);
                }
        }
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
        printf("\n5 inv %d\n5X11=%d\n",gf.inverse.w32(&gf, 5),gf.multiply.w32(&gf,5,11));
        printf("\n\n");
	for(i=0;i<16;++i){
                if(i%4==0){
                        printf("\n");
                }
                if(i==0){
                        printf("%2x ",11);
                }else{
                        mult=gf4_at_multiply(i,14);
                        add=mult^11;
                        inv=gf.inverse.w32(&gf, add);
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
                        printf("%2x ",1);
                }else{
                        inv=gf.inverse.w32(&gf, i);
                        mult=gf2_at_multiply(inv,1);
                        add=mult^1;
                        printf("%2x ",add);
                }
        }
        printf("\n2 inv %d\n2X3=%d\n",gf.inverse.w32(&gf, 2),gf.multiply.w32(&gf,2,3));
        printf("\n\n");
        for(i=0;i<4;++i){
                if(i%2==0){
                        printf("\n");
                }
                if(i==0){
                        printf("%2x ",2);
                }else{
                        mult=gf4_at_multiply(i,14);
                        add=mult^10;
                        inv=gf.inverse.w32(&gf, add);
                        printf("%2x ",inv);
                }
        }
        printf("\n\n");



}

