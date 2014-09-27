#include<openssl/sha.h>
#include<stdio.h>
#include<string.h>
/**
 * Hashes the "size"  no of characters from c and returns the prefix 
 * no of bits from the SHA1 hash of the input data
 * prefix is restricted to 32
 * */

unsigned int getHashID(char * c, int size ){

	unsigned int result =0 , i; 
	unsigned char hash[SHA_DIGEST_LENGTH];
	SHA1(c , size, hash);
	

	for ( i =0 ; i < 4; i++)
	{
		result += (unsigned int ) hash[i];
		result = result << 8; 
	}



	return result ; 

}
int main(int argc, char *argv[])
{


	unsigned char hash[SHA_DIGEST_LENGTH + 1];
	SHA1(argv[1] , strlen(argv[1]), hash);
	hash[20] =0;
	printf("%s", hash);

	return 0; 
}







	

