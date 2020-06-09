// init 
#ifndef MY_COMMON_HEADER
#define MY_COMMON_HEADER
inline int positive_modulo(int i, int n) {
	return (i % n + n) % n;
}

#define TO_STR(str) #str
#endif
