// TOKENIZER EXAMPLE
//
//
//NOTES
//
//strtok() doesn't create a new string and return it; 
//it returns a pointer to the token within the string you pass as argument to strtok(). 
//Therefore the original string gets affected.
//strtok() breaks the string means it replaces the delimiter character with NULL and 
//returns a pointer to the beginning of that token. 
//Therefore after you run strtok() the delim characters will be replaced by NULL characters. 

#include <stdio.h>
#include <string.h>
#include <cstdlib> //used by atoi()
#include <iostream> 
#include <vector> 

using namespace std;



void extractTokens(char *str,  char *CRC, char *command, int &packetNumber, char *data){
	char * pch;
     
  int tokenCounter=0;
  printf ("Splitting string \"%s\" into tokens:\n\n",str);
  
  while (1)
  {
	 if(tokenCounter ==0){ 
       pch = strtok (str, " ,.-'\r\n'");
    } else {
		 pch = strtok (NULL, " ,.-'\r\n'");
	 }
	 if(pch == NULL) break;
	 printf ("Token[%d], with %d characters = %s\n",tokenCounter,int(strlen(pch)),pch);
	 
    switch(tokenCounter){
    case 0: strcpy(CRC,pch); //copy CRC to the variable
	          printf("\tcode = %s\n",CRC);
			     break;
      case 1: //command = new char[strlen(pch)];
			      strcpy(command, pch);
		
		        printf("\tcommand = %s, %d characters\n", command, int(strlen(command)));
              break;			
		  case 2: packetNumber = atoi(pch);
		        break;
		  case 3: //data = new char[strlen(pch)];
			     strcpy(data, pch);
		
		        printf("\tdata = %s, %d characters\n", data, int(strlen(data)));
              break;			
    }	
	  
	 tokenCounter++;
  }
}

void test_extractTokens(){
  char str[256] ="2310 PACKET 9 data56789\r\n";

char CRC[10];  	
  
  int packetNumber;
  char data[256];
  char command[256];
  //---
  //  CRC=-1;
  packetNumber=-1;
  memset(data,0,sizeof(data));  
  memset(command,0,sizeof(command));  
 
  printf("\n===<< CORRECT PACKET >>=====================================\n");

  extractTokens(str, CRC, command, packetNumber, data);
  
  printf("\n==== << Extracted Information >> =======\n");
  printf("CRC = %s\n", CRC);
  printf("command = %s, size = %d\n", command, int(strlen(command)));
  printf("packetNumber = %d\n", packetNumber);
  printf("data = [%s], size = %d\n", data,int(strlen(data)));
  // printf("original_str = [%s], size = %d\n", str,int(strlen(str))); 
}

void test_extractCorruptedTokens(){
  

  char CRC[10];    
  
  int packetNumber;
  char data[256];
  char command[256];
 
  //  CRC=-1;
  packetNumber=-1;
  memset(data,0,sizeof(data));  
  memset(command,0,sizeof(command));  

  // printf("\n========================================\n");
  // char corruptStr[256] ="12345%^^*PACKET!@9 data56789\r\n";
  // char corruptStr[256] ="125PACKET6789\r\n";
  // char corruptStr[256] ="%^%^%9\r\n";
  // char corruptStr[256] ="#7s";
  char corruptStr[256] ="PACKET 12345 x data56789\n";

  printf("\n===<< CORRUPTED PACKET >>=====================================\n");

  extractTokens(corruptStr, CRC, command, packetNumber, data);
  
  printf("\n==== << Extracted Information >> =======\n");
  printf("CRC = %s\n", CRC);
  printf("command = %s, size = %d\n", command, int(strlen(command)));
  printf("packetNumber = %d\n", packetNumber);
  printf("data = [%s], size = %d\n", data,int(strlen(data)));
  // printf("original_str = [%s], size = %d\n", corruptStr,int(strlen(corruptStr)));
   
}


int main ()
{
  test_extractTokens();
  test_extractCorruptedTokens();
  return 0;
}
