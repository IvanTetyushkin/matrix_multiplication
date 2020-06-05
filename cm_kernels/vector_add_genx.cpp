
#include "cm/cm.h"

const unsigned int part_size = 16;


extern "C" _GENX_MAIN_ void vector_add(SurfaceIndex res, // res cm_vector
				const SurfaceIndex lhs,// rhs cm_vector
				const SurfaceIndex rhs// lhs cm_vector
				) {
#if 1
    uint linear_loc = cm_linear_global_id();

	vector<float, part_size> part_rhs;
	vector<float, part_size> part_lhs;
	vector<float, part_size> part_res;
	read(lhs, linear_loc * part_size * sizeof(float), part_lhs);
	read(rhs, linear_loc * part_size * sizeof(float), part_rhs);
	part_res = part_lhs + part_rhs;
	write(res, linear_loc * part_size * sizeof(float), part_res);
#endif
}
