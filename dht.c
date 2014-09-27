#include "dht.h" 
#include "history.h" 
#include "communication_structures.h" 
#include "mythread.h" 
nodeinfo_t nodeinfo ; 

int isConnectedToRing  = 0 ; 
char port[32] = "3410";

void shell(){

	char *command[10];
	char command_string[250];
	int i ;
	char * token;


	do { 
		// scan the command
		fgets(command_string, 250, stdin);
		if ( 1 != strlen(command_string))
			command_string[strlen(command_string) - 1 ] =0;
		i = 0 ; 
		token = strtok ( command_string , " ")  ;

		while ( token != NULL){
			command[i] = token; 
			i++; 
			token = strtok(NULL, " "); 
		}
		command[i] = NULL;

		//printf("%s " , command[0]);
		if ( command[0] == NULL)
			continue;
		if ( 0 == strcmp( command[0] , "help"))
		{
			printf("help\t: Displays the help contents \n");
			printf("port\t: Displays the port to which the server is currently listening to. \n");
			printf("port <portno>\t: Sets the port no to the specified port on which the server will listen\n");
			printf("create\t: Creates a new ring and starts listening to the requests on the specified/default port\n");
			printf("join <ip> <port>\t: Joins the ring of which the node with specified ip port is a member\n");
			printf("quit\t: Quit the interactive shell\n");
			printf("put <key> <value>\t: Stores the key-value pair in the hash table\n");
			printf("get <key>\t: Get the value of the specified key if present in the hash table \n");
		}else if ( 0 == strcmp( command[0] , "port")){
			if ( command[1] == NULL){
				printf("Port set to : %s\n" , port) ; 
			}else{
				if ( i < 2)
				{
					printf("Insufficient arguments \n");
					continue;
				}
				if ( isConnectedToRing) {
					printf("Cannot set after the node has joined the ring \n");
					continue;
				}
				else
				{
					strcpy(port, command[1]);
					init_node_info(&nodeinfo, port);
				}
			}


		}else if ( 0 == strcmp( command[0] , "create"))
		{

			if ( isConnectedToRing){
				printf("Already connected to a ring. Can't create another one \n");
			}else{
				int tid = createWithArgs(create_ring, &nodeinfo);
				run(tid);
				//create_ring(&nodeinfo);
			}

		}else if ( 0 == strcmp( command[0] , "join")){
			printf("join command\n");
			if ( i < 3)
			{
				printf("Insufficient arguments \n");
				continue;
			}
			if ( isConnectedToRing)
			{
				printf("Already connected to a ring \n");
				continue;
			}
			join_ring(command[1], command[2]);

		}else if ( 0 == strcmp( command[0] , "quit")){
			printf("quit\n");
			exit(0);

		}else if ( 0 == strcmp( command[0] , "put")){
			if ( i < 3)
			{
				printf("Insufficient arguments \n");
				continue;
			}
			printf("put\n");
		}else if ( 0 == strcmp( command[0] , "get")){
			if ( i < 2)
			{
				printf("Insufficient arguments \n");
				continue;
			}
			printf("get\n");
		}else{
			printf("Unrecognised command. How about using 'help' ? \n");
		}

		command[0] = NULL;
	}while(1);
}


int main(int argc, char * argv[]) {

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

	//printf("___________CSP Assignment 2\n");
	//printf("___________Distributed hash table\n");
	//printf("___________Akshay Jain and Shashank Khasare\n");

	init_node_info(&nodeinfo, port);
	int tid = create(shell);
	start();

}
