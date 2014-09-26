#define M 30
typedef unsigned int key_t; 
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


typedef struct finger_t{

	// start 
	// interval s
	// successor 
	key_t start ;
	interval_t interval ; 
	node_t successor; 


}finger_t; 

typedef struct interval_t {

	key_t start , end; 

}interval_t ; 


typedef struct node_t {

	// key 
	// structure storing ip  addres 
	key_t key ; 
	struct sockaddr_in address; 

}node_t ; 

/**
 * Hashes the "size"  no of characters from c and returns the prefix 
 * no of bits from the SHA1 hash of the input data
 * prefix is restricted to 32
 * */

key_t getHashID(char * c, int size , int prefix){

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
