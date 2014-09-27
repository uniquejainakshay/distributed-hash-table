#ifndef COMMUNICATION_STRUCTURES_H 
#define COMMUNICATION_STRUCTURES_H 
#define OK 	1
#define JOIN 	2
#define PUT 	3
#define GET 	4
#define FIND_SUCCESSOR 5


typedef struct header_t{
	int type;
}header_t;

#endif
