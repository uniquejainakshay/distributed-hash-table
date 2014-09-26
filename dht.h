#define M 30
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>
#include<openssl/sha.h>
#include<stdio.h>
#include<netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <ifaddrs.h>
typedef unsigned int dhtkey_t; 
typedef struct node_t {

	// key 
	// structure storing ip  addres 
	dhtkey_t key ; 
	struct sockaddr_in address; 

}node_t ; 
typedef struct interval_t {

	dhtkey_t start , end; 

}interval_t ; 
typedef struct finger_t{

	// start 
	// interval s
	// successor 
	dhtkey_t start ;
	interval_t interval ; 
	node_t successor; 


}finger_t; 
typedef struct nodeinfo_t 
{
	// 1. self ip address
	// 2. self key 
	// 3. successor
	// 4. predecessor
	// 5. fingertable 
	
	node_t self;
	node_t successor;
	node_t predecessor;
	finger_t finger[M];


}nodeinfo_t ; 

/**
 * Hashes the "size"  no of characters from c and returns the prefix 
 * no of bits from the SHA1 hash of the input data
 * prefix is restricted to 32
 * */

dhtkey_t getHashID(char * c, int size , int prefix){

	unsigned int result =0 , i; 
	unsigned char hash[SHA_DIGEST_LENGTH];
	SHA1(c , size, hash);


	for ( i =0 ; i < 4; i++)
	{
		result += (unsigned int ) hash[i];
		result = result << 8; 
	}


	result = result >> ( 32 - prefix ) ; 

	return result ; 

}
struct sockaddr_in getSelfAddress(){
	struct ifaddrs *ifap, *ifa;
	struct sockaddr_in *sa;
	struct sockaddr_in ret;

	getifaddrs (&ifap);
	for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr->sa_family==AF_INET) {
			sa = (struct sockaddr_in *) ifa->ifa_addr;
			ret = *sa; 
		}
	}

	freeifaddrs(ifap);
	return ret; 
}


/**
 * Called before joining/creating a ring 
 * It initialises the ip address and hashes to it to get the key. 
 */
void init_node_info(nodeinfo_t * np, char *port){
	struct sockaddr_in address = getSelfAddress();
	char *addr; 
	addr = inet_ntoa(address.sin_addr);
	strcat(addr, port);

	// set self 
	np ->self.key = getHashID(addr, strlen(addr), M);
	np ->self.address = address; 
	printf("Fetched IP address : %s ", inet_ntoa((*np).self.address.sin_addr));
	printf("Hashed address %d ", (*np).self.key);
}

void createRing(){

}
