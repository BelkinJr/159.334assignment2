//159.334 - Networks
// CLIENT: prototype for assignment 2. 
//Note that this progam is not yet cross-platform-capable
// This code is different than the one used in previous semesters...
//************************************************************************/
//RUN WITH: Rclient_UDP 127.0.0.1 1235 0 0 
//RUN WITH: Rclient_UDP 127.0.0.1 1235 0 1 
//RUN WITH: Rclient_UDP 127.0.0.1 1235 1 0 
//RUN WITH: Rclient_UDP 127.0.0.1 1235 1 1 
//************************************************************************/

//---
#if defined __unix__ || defined __APPLE__
	#include <unistd.h>
	#include <errno.h>
	#include <stdlib.h>
	#include <stdio.h> 
	#include <string.h>
	#include <sys/types.h> 
	#include <sys/socket.h> 
	#include <arpa/inet.h>
	#include <netinet/in.h> 
	#include <netdb.h>
    #include <iostream>
#elif defined _WIN32 


	//Ws2_32.lib
	#define _WIN32_WINNT 0x501  //to recognise getaddrinfo()

	//"For historical reasons, the Windows.h header defaults to including the Winsock.h header file for Windows Sockets 1.1. The declarations in the Winsock.h header file will conflict with the declarations in the Winsock2.h header file required by Windows Sockets 2.0. The WIN32_LEAN_AND_MEAN macro prevents the Winsock.h from being included by the Windows.h header"
	#ifndef WIN32_LEAN_AND_MEAN
	#define WIN32_LEAN_AND_MEAN
	#endif

	#include <winsock2.h>
	#include <ws2tcpip.h>
	#include <stdio.h> 
	#include <string.h>
	#include <stdlib.h>
	#include <iostream>	
	#include <ctime>
	#include <cmath>
#endif

#include "myrandomizer.h"

using namespace std;

#define WSVERS MAKEWORD(2,0)
#define BUFFER_SIZE 80  //used by receive_buffer and send_buffer
                        //the BUFFER_SIZE has to be at least big enough to receive the packet
#define SEGMENT_SIZE 78
//segment size, i.e., if fgets gets more than this number of bytes it segments the message into smaller parts.

#define GENERATOR 0x8005 //0x8005, generator for polynomial division
//for the assignments you can copy and past CRCpolynomial() directly in your C code.

#define LIMIT 500 //might change later

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

WSADATA wsadata;
const int ARG_COUNT=5;
//---
int numOfPacketsDamaged=0;
int numOfPacketsLost=0;
int numOfPacketsUncorrupted=0;

int packets_damagedbit=0;
int packets_lostbit=0;

/////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[]) {
//*******************************************************************
// Initialization
//*******************************************************************
   clock_t time;	 
   int counterCheck;
   unsigned int CRCresult;
   char CRCstr[BUFFER_SIZE];

   struct sockaddr_storage localaddr, remoteaddr;
   char portNum[NI_MAXSERV];
   struct addrinfo *result = NULL;
   struct addrinfo hints;
	
   memset(&localaddr, 0, sizeof(localaddr));  //clean up
   memset(&remoteaddr, 0, sizeof(remoteaddr));//clean up  
   randominit();
   SOCKET s;
   char send_buffer[BUFFER_SIZE],receive_buffer[BUFFER_SIZE];
   int n,bytes,addrlen;
	 
   
	addrlen=sizeof(struct sockaddr);
	
	
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET; //AF_INET6;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	
//********************************************************************
// WSSTARTUP
//********************************************************************
   if (WSAStartup(WSVERS, &wsadata) != 0) {
      WSACleanup();
      printf("WSAStartup failed\n");
   }
//*******************************************************************
//	Dealing with user's arguments
//*******************************************************************
   if (argc != ARG_COUNT) {
	   printf("USAGE: Rclient_UDP remote_IP-address remoteport allow_corrupted_bits(0 or 1) allow_packet_loss(0 or 1)\n");
	   exit(1);
   }
	
	int iResult=0;
	
	sprintf(portNum,"%s", argv[2]);
	iResult = getaddrinfo( argv[1], portNum, &hints, &result);
   
   packets_damagedbit=atoi(argv[3]);
   packets_lostbit=atoi(argv[4]);
   if (packets_damagedbit < 0 || packets_damagedbit > 1 || packets_lostbit< 0 || packets_lostbit>1){
	   printf("USAGE: Rclient_UDP remote_IP-address remoteport allow_corrupted_bits(0 or 1) allow_packet_loss(0 or 1)\n");
	   exit(0);
   }
   
//*******************************************************************
//CREATE CLIENT'S SOCKET 
//*******************************************************************
   s = INVALID_SOCKET; 
   s = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

   if (s == INVALID_SOCKET) {
      printf("socket failed\n");
   	exit(1);
   }
    //nonblocking option
	// Set the socket I/O mode: In this case FIONBIO
	// enables or disables the blocking mode for the 
	// socket based on the numerical value of iMode.
	// If iMode = 0, blocking is enabled; 
	// If iMode != 0, non-blocking mode is enabled.
   u_long iMode=1;
	
   iResult=ioctlsocket(s,FIONBIO,&iMode);
   if (iResult != NO_ERROR){
         printf("ioctlsocket failed with error: %d\n", iResult);
		 closesocket(s);
		 WSACleanup();
		 exit(0);
   }

   cout << "==============<< UDP CLIENT >>=============" << endl;
   cout << "channel can damage packets=" << packets_damagedbit << endl;
   cout << "channel can lose packets=" << packets_lostbit << endl;
	
//*******************************************************************
//SEND A TEXT FILE 
//*******************************************************************
   int counter=0;
   int lineCounter=0;
   char temp_buffer[BUFFER_SIZE];
   FILE *fin=fopen("data_for_transmission.txt","rb"); //original
	
//In text mode, carriage return–linefeed combinations 
//are translated into single linefeeds on input, and 
//linefeed characters are translated to carriage return–linefeed combinations on output. 
	
   if(fin==NULL){
	   printf("cannot open data_for_transmission.txt\n");
	   closesocket(s);
   	   WSACleanup();
	   exit(0);
   } else {
	   printf("data_for_transmission.txt is now open for sending\n");
	}
   while (1){
		if (!feof(fin)) {
			if (strncmp(receive_buffer,"ACK", 3)==0) {  //if ACK is received
				sscanf(receive_buffer,"ACK %d", &counterCheck);
				printf("%d\n\n", counterCheck);
				if (counterCheck==counter) {
					counter++;
				} else {
					send_unreliably(s,send_buffer,(result->ai_addr)); //resend the packet 
					time= clock();	//reset timer
				}
			}
			if (lineCounter==counter) { 
				memset(send_buffer, 0, sizeof(send_buffer));
				fgets(send_buffer,SEGMENT_SIZE,fin); //get one line of data from the file
				lineCounter++;
				sprintf(temp_buffer,"PACKET %d ",counter);  //create packet header with Sequence number
				//counter++;
				strcat(temp_buffer,send_buffer);   //append data to packet header
				printf("%s <-- this is string before CRC\n\n", temp_buffer);
				CRCresult=CRCpolynomial(temp_buffer); //making hex out of the packet
				CRCresult=~CRCresult; // inverting
				CRCresult=CRCresult&0xffff;
				sprintf(CRCstr,"%x ", CRCresult); //hexCRC to stringCRC
				strcat(CRCstr, temp_buffer); //adding CRC in front of the packet
				strcpy(send_buffer,CRCstr);   //the complete packet
				printf("\n======================================================\n");
				cout << "calling send_unreliably, to deliver data of size " << strlen(send_buffer) << endl;
				time=clock(); //starting timer
				send_unreliably(s,send_buffer,(result->ai_addr)); //send the packet to the unreliable data channel
			}	
			if( ((clock()-time)>LIMIT) ) { //if timeout 
			printf("\n======================================================\n");
			cout << "calling send_unreliably, to deliver data of size " << strlen(send_buffer) << endl;
			send_unreliably(s,send_buffer,(result->ai_addr)); //resend the packet 
			time= clock();	//reset timer
			} 
			Sleep(1);  //sleep for 1 millisecond																			
//********************************************************************
//RECEIVE
//********************************************************************
			addrlen = sizeof(remoteaddr); //IPv4 & IPv6-compliant
			bytes = recvfrom(s, receive_buffer, 78, 0,(struct sockaddr*)&remoteaddr,&addrlen);
//********************************************************************
//IDENTIFY server's IP address and port number.     
//********************************************************************      
	char serverHost[NI_MAXHOST]; 
    char serverService[NI_MAXSERV];	
    memset(serverHost, 0, sizeof(serverHost));
    memset(serverService, 0, sizeof(serverService));


    getnameinfo((struct sockaddr *)&remoteaddr, addrlen,
                  serverHost, sizeof(serverHost),
                  serverService, sizeof(serverService),
                  NI_NUMERICHOST);



    printf("\nReceived a packet of size %d bytes from <<<UDP Server>>> with IP address:%s, at Port:%s\n",bytes,serverHost, serverService); 	   
	
//********************************************************************
//PROCESS REQUEST
//********************************************************************
			//Remove trailing CR and LN
			if( bytes != SOCKET_ERROR ){	
				n=0;
				while (n<bytes){
					n++;
					if ((bytes < 0) || (bytes == 0)) break;	
					if (receive_buffer[n] == '\n') { /*end on a LF*/
						receive_buffer[n] = '\0';
						break;
					}
					if (receive_buffer[n] == '\r') /*ignore CRs*/
					receive_buffer[n] = '\0';
				}
				printf("RECEIVED --> %s, %d elements\n",receive_buffer, int(strlen(receive_buffer)));
			}
			
		} else {
			fclose(fin);
			printf("End-of-File reached. \n"); 
			memset(send_buffer, 0, sizeof(send_buffer)); 
			sprintf(send_buffer,"CLOSE \r\n"); //send a CLOSE command to the RECEIVER (Server)
			printf("\n======================================================\n");
			
			send_unreliably(s,send_buffer,(result->ai_addr));
			break;
		}		
   } //while loop
//*******************************************************************
//CLOSESOCKET   
//*******************************************************************
   closesocket(s);
   printf("Closing the socket connection and Exiting...\n");
   cout << "==============<< STATISTICS >>=============" << endl;
   cout << "numOfPacketsDamaged=" << numOfPacketsDamaged << endl;
   cout << "numOfPacketsLost=" << numOfPacketsLost << endl;
   cout << "numOfPacketsUncorrupted=" << numOfPacketsUncorrupted << endl;
   cout << "===========================================" << endl;

   exit(0);
}
	
