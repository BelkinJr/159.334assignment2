//159.334 Computer Network
//this is a simple 16 bits CRC function
//it does not use reverse data or reverse result
//the Initial remainder is 0x0000, the generator is 0x8005
//Usage:
//the input should be just a normal string, finishing in '\0' (NOT '\n')
//From the sender's side, you can just use the function, and concatenate the result of the CRC
//The CRC needs to be sent with the message, as a text (in decimal notation, or any other notation you want)
//The receiver needs to be able to split the CRC and the message, recompute the CRC and compare.
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define GENERATOR 0x8005 //0x8005, generator for polynomial division

//for the assignments you can copy and paste CRCpolynomial() directly in your C code.
unsigned int CRCpolynomial(char *buffer){
	unsigned char i;
	unsigned int rem=0x0000;
        unsigned int bufsize=strlen(buffer);
	while(bufsize--!=0){
		for(i=0x80;i!=0;i/=2){
			if((rem&0x8000)!=0){
				rem=rem<<1;
				rem^=GENERATOR;
			}
     		else{
	   	   rem=rem<<1;
		   }
	  		if((*buffer&i)!=0){
			   rem^=GENERATOR;
			}
		}
		buffer++;
	}
	rem=rem&0xffff;
	return rem;
}

int random_char(void){
	return (int) (255.0 * (rand() / (RAND_MAX + 1.0))); //number between [0, 255]
}

int damage_bit(void){
	return (int) (10.0 * (rand() / (RAND_MAX + 1.0))); //number between [0, 10]
}

void testCRC()
{
	char buffer[78];
	char corrupted_buffer[78];
	
	strcpy(buffer, "line 0 aaaaaaaaaaaaaaaaaaaa");
	strcpy(corrupted_buffer, "line 0 aaaaaaaaaaaaaaaaaaaa");
	
	corrupted_buffer[damage_bit()]=random_char();
	corrupted_buffer[damage_bit()]=random_char();
	
	unsigned int CRCresult1, CRCresult2;
	
	CRCresult1=CRCpolynomial(buffer);
	printf("The CRC hex value for: '%s' is ",buffer);
	printf(" 0x%X\n\n",CRCresult1);
	
	CRCresult2=CRCpolynomial(corrupted_buffer);
	printf("The CRC hex value for: '%s' is ",corrupted_buffer);
	printf(" 0x%X\n\n",CRCresult2);
	
	if(CRCresult1 != CRCresult2) {
		printf("DATA INTEGRITY VALIDATION:  bits corrupted!\n");
	} else {
		printf("DATA INTEGRITY VALIDATION:  data is in perfect condition\n"); 
	}
}

int main(int argc, char *argv[]){
	
	/*
	char buffer[1025];
	strcpy(buffer, "A");
	unsigned int CRCresult;
	CRCresult=CRCpolynomial(buffer);
	printf("The CRC hex value for: '%s' is ",buffer);
	printf(" 0x%X\n\n",CRCresult);
	*/
	
	testCRC();
	return 0;
}
