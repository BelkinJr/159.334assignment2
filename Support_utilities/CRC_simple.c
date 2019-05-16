//159.334 Computer Network
//this is a simple 16 bits CRC function
//it does not uses reverse data or reverse result
//the Initial remainder is 0x0000, the generator is 0x8005
//Usage:
//the input should be just a normal string, finishing in '\0' (NOT '\n')
//From the sender's side, you can just use the function, and concatenate the result of the CRC
//The CRC needs to be sent with the message, as a text (in decimal notation, or any other notation you want)
//The receiver needs to be able to split the CRC and the message, recompute the CRC and compare.
#include <stdio.h>
#include <string.h>
#include <iomanip>
#include <string>

using namespace std; 


#define GENERATOR 0x8005 //0x8005, generator for polynomial division

//for the assignments you can copy and past CRCpolynomial() directly in your C code.
unsigned int CRCpolynomial(char *buffer){
	unsigned char i;
	unsigned int rem=0x0000;
    unsigned int bufsize=strlen(buffer);
	
	while(bufsize--!=0){
		for(i=0x80;i!=0;i/=2){
			if((rem&0x8000)!=0){
				rem=rem<<1;
				rem^=GENERATOR;
			} else{
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



int main(int argc, char *argv[]){
	char buffer[1025];
	char CRCstr[122222];
	strcpy(buffer, "NAPOLEON");
	unsigned int CRCresult;
	CRCresult=CRCpolynomial(buffer);
	printf("The CRC hex value for: '%s' is ",buffer);
	printf(" 0x%X\n\n",CRCresult);
	
	sprintf(CRCstr,"%x", CRCresult);
	CRCresult=stoi(CRCstr, nullptr, 16);
	printf("%x\n\n", CRCresult);

	CRCresult=~CRCresult;
	CRCresult=CRCresult&0xffff;
	char str[100];
	char CRCnum[100];
	strcpy(str, "bhes lol ");

	sprintf(CRCnum, "%x lol", CRCresult);

	strcat(str, CRCnum);



	//printf("CRCresult inverted is 0x%X\n", CRCresult);
	sprintf(CRCstr,"%x", CRCresult);
	CRCresult=stoi(CRCstr, nullptr, 16);
	printf("%x", CRCresult);
	//printf("%s\n", str);

	return 0;
}
