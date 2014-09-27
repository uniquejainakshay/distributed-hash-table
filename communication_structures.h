#ifndef COMMUNICATION_STRUCTURES_H 
#define COMMUNICATION_STRUCTURES_H 
#define OK 			1
#define JOIN 			2
#define PUT 			3
#define GET 			4
#define FIND_SUCCESSOR 		5//
#define FIND_PREDECESSOR 	6//
#define GET_PREDECESSOR 	7//
#define GET_SUCCESSOR  		8//
#define SET_PREDECESSOR 	9//
#define CLOSEST_PRECEDING_FINGER 10 //


typedef struct header_t{
	int type;
}header_t;

#endif
