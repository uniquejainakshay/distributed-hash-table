#include "dht.h" 
#include "history.h" 
nodeinfo_t nodeinfo ; 

int isConnectedToRing ; 
int main(int argc, char * argv[]) {
{

	if ( argc == 1) 
	{
		enable_history();

	}else if ( argc == 2 ) {
		if ( 0 != strcmp ( argv[1] , "--enabled" )) {
			printf("Unknown argument : %s " , argv[1]);
			exit(0);
		}
	}else 
	{
		printf("Too many arguments passed \n");
		exit(0);

	}



	printf("Distributed hash table\n");
do { 



}while (1);


