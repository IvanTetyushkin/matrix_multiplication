// init 
#ifndef MY_COMMON_HEADER
#define MY_COMMON_HEADER
template<class InputIt1, class InputIt2, class OutputIt, class BinaryOperation>
OutputIt transform_saved(InputIt1 first1, InputIt1 last1, InputIt2 first2,
	OutputIt d_first, BinaryOperation binary_op)
{
	while (first1 != last1) {
		*d_first++ += binary_op(*first1++, *first2++);
	}
	return d_first;
}

inline int positive_modulo(int i, int n) {
	return (i % n + n) % n;
}

#define TO_STR(str) #str
#endif
