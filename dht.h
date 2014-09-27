#ifndef DHT_H
#define DHT_H
#define M 3
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
#include <math.h>
#include <errno.h>
#include "communication_structures.h"



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
typedef struct hash_table_t{
	char *keys[50];
	char *values[50];
}hash_table_t;
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
	finger_t finger[M+1];
	hash_table_t hash_table;

}nodeinfo_t ; 

void print_finger_table(nodeinfo_t * );
/**
 * Hashes the "size"  no of characters from c and returns the prefix 
 * no of bits from the SHA1 hash of the input data
 * prefix is restricted to 32
 * */

dhtkey_t getHashID(char * c, int size , int prefix){

	unsigned int result =0 , i; 
	//unsigned char hash[SHA_DIGEST_LENGTH];
	char str[32];
	char  hash[21];
	/*
	printf("here %s %d %d\n", c, size , strlen(c));
	//SHA1(c , size, hash);
	hash = SHA1(str, size, NULL);
	printf("here\n");
*/

	FILE * pf ;
	strcpy(str,"./mysha ");
	strcat(str, c);
	pf = popen(str, "r");
	fgets(hash, 21, pf);
	hash[20] = 0;
	//printf("hash : %s", hash);

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
	char add_string[32];
	int i;
	addr = inet_ntoa(address.sin_addr);
	strcpy(add_string, addr);
	strcat(add_string, port);
	//
	address.sin_port = htons(atoi(port));
	address.sin_family = AF_INET;
	// set self 
	np ->self.key = getHashID(add_string, strlen(add_string), M);
	np ->self.address = address; 
	for ( i = 1; i < 1+M; i++){
		finger_t f ; 
		f.start = ((int)(np -> self.key +  pow(2, i -1))) % ((int)pow( 2, M)); 
		f.interval.start = f.start;
		f.interval.end = ((int)(np -> self.key +  pow(2, i))) % ((int)pow( 2, M));
		np -> finger[i]  =f; 
	}
	printf("Fetched IP address : %s:%s\n", inet_ntoa((*np).self.address.sin_addr), port);
	printf("Hashed address %d \n", (*np).self.key);
}

void create_ring(nodeinfo_t * np){
	int i ; 
	extern int isConnectedToRing;

	isConnectedToRing = 1;

	for ( i = 1; i < 1+M; i++){
		np -> finger[i].successor = np -> self; 
	}
	np -> successor = np -> self; 
	np -> predecessor= np -> self; 
	print_finger_table(np);
	startServer();
}

void print_finger_table(nodeinfo_t * np){
	int i; 
	printf("Finger table of node %s:%d\n", inet_ntoa(np -> self.address.sin_addr), ntohs(np -> self.address.sin_port));
	printf("start\tinterval\tsuccessor\n" );
	for ( i =1 ; i <= M; i ++)
	{
		finger_t f = np -> finger[i];
		printf("%u\t[%u, %u)\t%u\n", f.start,f.interval.start, f.interval.end, f.successor.key);
	}
}
int getSocket(char *ip, char * port){

	int sock_fd;
	struct sockaddr_in server;

	if ( ( sock_fd = socket(AF_INET, SOCK_STREAM, 0) ) < 0 ){
		perror("Socket : Unable to create socket.\n");
		return errno;
	}


	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr( ip );
	server.sin_port = htons(atoi(port));

	if( connect(sock_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
		printf("Invalid ip or port %s:%s\n", ip, port);
		return -1;
	}
	return sock_fd;

}
int getSocketFromNode(node_t n){
	
	char * ip;
	char port[10];
	ip = inet_ntoa(n.address.sin_addr);
	sprintf(port, "%d", inet_ntohs(n.address.sin_port));

	return getSocket( ip , port);
}

void join_ring(char *ip , char * port){

	extern int isConnectedToRing;
	isConnectedToRing = 1;
	int sock_fd = getSocket(ip, port);
	if ( socket == -1)
		return;

	init_finger_table(char * ip, char * port );
	update_others(char * ip, char * port);

	close(sock_fd);		
}

node_t find_predecessor_rpc(int sockfd, key_t k){
	header_t h ; 
	node_t predecessor;
	h.type = FIND_PREDECESSOR;
	send(sock_fd, &h, sizeof(h), 0);
	send(sock_fd, &k, sizeof(k), 0);
	recv(sock_fd, &h, sizeof(h), 0);
	if ( h.type == OK){
		if ( debug)
			printf("--Received OK signal\n"); 

		recv(sock_fd, &predecessor, sizeof(node_t), 0);
	}else
	{
		printf("Unknown response from server\n");
		exit(0);
	}

	return predecessor;
}
node_t find_successor_rpc(int sockfd, key_t k){
	header_t h ; 
	node_t successor;
	h.type = FIND_SUCCESSOR;
	send(sock_fd, &h, sizeof(h), 0);
	send(sock_fd, &k, sizeof(k), 0);
	recv(sock_fd, &h, sizeof(h), 0);
	if ( h.type == OK){
		if ( debug)
			printf("--Received OK signal\n"); 

		recv(sock_fd, &successor, sizeof(node_t), 0);
	}else
	{
		printf("Unknown response from server\n");
		exit(0);
	}

	return successor;
}

node_t get_predecessor_rpc(int sockfd){
	header_t h ; 
	node_t predecessor;
	h.type = GET_PREDECESSOR; 
	send(sock_fd, &h, sizeof(h), 0);
	recv(sock_fd, &h, sizeof(h), 0);
	if ( h.type == OK){
		if ( debug)
			printf("--Received OK signal\n"); 

		recv(sock_fd, &predecessor, sizeof(node_t), 0);
	}else
	{
		printf("Unknown response from server\n");
		exit(0);
	}

	return predecessor;
}

node_t get_successor_rpc(int sockfd){
	header_t h ; 
	node_t successor;
	h.type = GET_SUCCESSOR;
	send(sock_fd, &h, sizeof(h), 0);
	recv(sock_fd, &h, sizeof(h), 0);
	if ( h.type == OK){
		if ( debug)
			printf("--Received OK signal\n"); 

		recv(sock_fd, &successor, sizeof(node_t), 0);
	}else
	{
		printf("Unknown response from server\n");
		exit(0);
	}

	return successor;
}
int belongsToRange(key_t key, interval_t interval){
	if ( interval.start < interval.end ) {
		if (   interval.start <= key && key < interval.end ) 
			return 1;
	}else{

		if ( (interval.start <= key && key <= (pow(2, M) -1)  ) || ( 0 <= key && key < interval.end ) ) 
			return 1;
	}
	return 0;
}

void set_predecessor_rpc(node_t successor , node_t self){

	int sockfd = getSocketFromNode(successor);
	header_t header; 
	h.type = SET_PREDECESSOR;

	send(sockfd, &header, sizeof(header_t) , 0);
	send(sockfd, &self, sizeof(self) , 0);
	recv(sockfd, &header, sizeof(header_t) , 0);
	if ( h.type == OK){
		if ( debug)
			printf("--Received OK signal\n"); 
	}else
	{
		printf("Unknown response from server\n");
		exit(0);
	}

	close(sockfd);

}
void init_finger_table(char * ip, char * port){
	extern nodeinfo_t nodeinfo; 

	int sock_fd = getSocket(ip, port);
	nodeinfo.finger[1].successor =  find_successor_rpc(sock_fd, nodeinfo.finger[1].start);

	nodeinfo.successor = nodeinfo.finger[1].successor;
	nodeinfo.predecessor = find_predecessor_rpc(sock_fd, nodeinfo.successor);

	set_predecessor_rpc(nodeinfo.successor, nodeinfo.self);
	int i; 
	for( i = 1 ; i <= M - 1 ; i++)
	{
		interval_t interval ; 
		interval.start = nodeinfo.self;
		interval.end = nodeinfo.finger[i].successor.key;
		if ( belongsToRange(nodeinfo.finger[i + 1].start, interval){
			nodeinfo.finger[i + 1].successor = nodeinfo.finger[i].successor; 
		}else{

			nodeinfo.finger[i + 1].successor =find_successor_rpc(sock_fd, nodeinfo.finger[i + 1].start); 

		}

	}



	close(sock_fd);
}
void update_fingertable_rpc(int sockfd, key_t s, int i)
{
	header_t header;
	header.type = UPDATE_FINGERTABLE; 
	send(sockfd, &header, sizeof(header_t) , 0);
	send(sockfd, &s, sizeof(key_t) , 0);
	send(sockfd, &i, sizeof(int) , 0);
	recv(sockfd, &header, sizeof(header_t) , 0);
	if ( header.type == OK)
	{
		if ( debug)
			printf("--Received OK signal\n"); 



	}else
	{
		printf("Unknown response from server \n");
		exit(0);
	}
}

void update_others(char * ip, char * port)
{
	int sock_fd = getSocket(ip, port);
	int i; 
	for ( i = 1 ; i <= M; i++ ) {
		node_t p = find_predecessor_rpc(sock_fd, nodeinfo.self.key - (int) pow( 2 , i - 1 ) ) ; 
		int psock = getSocketFromNode(p);
		update_fingertable_rpc(psock, nodeinfo.self , i);

	}

}
void startServer(){
	extern nodeinfo_t nodeinfo ; 
	int sock_fd, client_sock, i = 0, addr_size, k;
	struct sockaddr_in addr, client;
	char buf[1024];

	if ( ( sock_fd = socket(AF_INET, SOCK_STREAM, 0) ) < 0 ){
		perror("socket() : Failed to create socket. ");
		return errno;
	}


	bzero(&addr,sizeof(addr));
	/*
	   addr.sin_family = AF_INET;
	   addr.sin_addr.s_addr = nodeinfo.self.address.sin_addr.s_addr;
	   addr.sin_port = nodeinfo.self.address.sin_port;
	   */

	addr = nodeinfo.self.address;

	if( bind(sock_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0 ){
		perror("bind() : Failed to bind. ");
		return errno;
	}


	if( listen(sock_fd, M) < 0 ) {
		perror("Failed to listen on socket. ");
		return errno;
	}
	while(1){
		client_sock = accept(sock_fd, (struct sockaddr *)&client, &addr_size);
		header_t h;
		recv(client_sock, &h, sizeof(h), 0);

		if ( h.type == JOIN){
			printf("Join request received\n" ) ; 
			h.type = OK;
			send(client_sock, &h, sizeof(h), 0);

		}else if ( h.type == PUT){
			printf("Put request receive\n " ) ; 
			h.type = OK;
			send(client_sock, &h, sizeof(h), 0);
		}else if ( h.type == GET){
			printf("Get request received\n" ) ; 
			h.type = OK;
			send(client_sock, &h, sizeof(h), 0);
		}else if ( h.type == FIND_SUCCESSOR ){
			printf("Find successor request received\n" ) ; 
			h.type = OK;
			send(client_sock, &h, sizeof(h), 0);
			key_t id ; 
			recv(client_sock, &id, sizeof(id), 0);
			node_t n = find_successor(id);
			send(client_sock, &n, sizeof(n), 0);

		}else if ( h.type == FIND_PREDECESSOR){
			printf("Find successor request received\n" ) ; 
			h.type = OK;
			send(client_sock, &h, sizeof(h), 0);
			key_t id ; 
			recv(client_sock, &id, sizeof(id), 0);
			node_t n = find_predecessor(id);
			send(client_sock, &n, sizeof(n), 0);
		}else if ( h.type == GET_SUCCESSOR){
			printf("Find successor request received\n" ) ; 
			h.type = OK;
			send(client_sock, &h, sizeof(h), 0);
			send(client_sock, &(nodeinfo.successor), sizeof(node_t), 0);
		}else if ( h.type == GET_PREDECESSOR){
			printf("Find successor request received\n" ) ; 
			h.type = OK;
			send(client_sock, &h, sizeof(h), 0);
			send(client_sock, &(nodeinfo.predecessor), sizeof(node_t), 0);
		}else if ( h.type == SET_PREDECESSOR){
			printf("Find successor request received\n" ) ; 
			h.type = OK;
			send(client_sock, &h, sizeof(h), 0);
			node_t p ; 
			recv(client_sock, &p, sizeof(p), 0);
			nodeinfo.predecessor = p;
		}else if ( h.type == CLOSEST_PRECEDING_FINGER){
			printf("Find successor request received\n" ) ; 
			h.type = OK;
			send(client_sock, &h, sizeof(h), 0);
			key_t id  ;
			recv(client_sock, &id, sizeof(id), 0);
			node_t n = closest_preceding_finger(id);
			send(client_sock, &n, sizeof(n), 0);
		}else{
			printf("Unknown request\n" ) ; 
		}
	}
}
node_t closest_preceding_finger_rpc(int sockfd, key_t id){
	header_t h ; 
	node_t cpf;
	h.type = CLOSEST_PRECEDING_FINGER;
	send(sock_fd, &h, sizeof(h), 0);
	send(sock_fd, &id, sizeof(key_t), 0);
	recv(sock_fd, &h, sizeof(h), 0);
	if ( h.type == OK){
		if ( debug)
			printf("--Received OK signal\n"); 

		recv(sock_fd, &cpf, sizeof(node_t), 0);
	}else
	{
		printf("Unknown response from server\n");
		exit(0);
	}

	return cpf;

}
node_t find_successor(key_t id){

	node_t n = find_predecessor(id);
	return get_successor_rpc(getSocketFromNode(n));
}

}
node_t find_predecessor(key_t id){

	extern nodeinfo_t nodeinfo;
	node_t n1; 
	n1 = nodeinfo.self;
	interval_t interval ; 
	interval.start = n1.key; 
	interval.end = nodeinfo.successor.key ; 

	while ( !belongsToRange(id , interval))
	{
		n1 = closest_preceding_finger_rpc(getSocketFromNode(n1), id);
		interval.start = n1.key; 
		interval.end = get_successor_rpc(getSocketFromNode(n1)).key;
	}
	return n1;
}





}
node_t closest_preceding_finger(key_t id){

	int i; 
	interval_t interval ; 
	extern nodeinfo_t nodeinfo;


	interval.start = nodeinfo.self.key; 
	interval.end = id; 
	for  ( i = M  ; i >= 1; i -- ) 
	{
		if ( belongsToRange(nodeinfo.finger[i].successor.key , interval ) )
		{
			return nodeinfo.finger[i].successor; 
		}
	}
	return nodeinfo.self;
}



#endif
