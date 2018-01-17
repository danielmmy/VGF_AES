#include <stdio.h>
#include <getopt.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "gf_complete.h"


unsigned int gf8_at_multiply(unsigned int num){
	int i=0;
	uint8_t counter;
	uint8_t S=248;
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

int main(int argc, char **argv){
        int w=8;
	int i,n;
        gf_t gf;
	uint8_t inv, mult, add;

        /* Create the proper instance of the gf_t object using the polynomial x^8+x^4+x^3+x+1: */
        gf_init_hard(&gf, w, GF_MULT_DEFAULT, GF_REGION_DEFAULT, GF_DIVIDE_DEFAULT,
                      0x11b, 0, 0,NULL, NULL);

	for(i=0;i<256;++i){
		if(i%16==0){
			printf("\n");
		}
		if(i==0){
			printf(" 99 ");
		}else{
			inv=gf.inverse.w32(&gf, i);
			mult=gf8_at_multiply(inv);
			add=mult^99;
			printf("%3d ",add);
		}
	}
	printf("\n");
}

