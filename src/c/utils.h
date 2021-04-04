#ifndef UTILS_H
#define UTILS_H

#define CATCH_CALLBACK(instruction, error_message) { \
	if(instruction < 0) {                            \
		LOG(error_message);                          \
	}                                                \
}


#define MIN(a, b) ((a) < (b)) ?  a : b
#define MAX(a, b) ((a) > (b)) ?  a : b

#endif