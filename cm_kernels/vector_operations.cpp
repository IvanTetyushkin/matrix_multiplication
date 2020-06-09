
#include "cm/cm.h"

const unsigned int part_size = 32;


extern "C" _GENX_MAIN_ void vector_add(SurfaceIndex res, // res cm_vector
				const SurfaceIndex lhs,// rhs cm_vector
				const SurfaceIndex rhs// lhs cm_vector
				) {
    uint linear_loc = cm_group_id(0);

	vector<float, part_size> part_rhs;
	vector<float, part_size> part_lhs;
	vector<float, part_size> part_res;
	read(lhs, linear_loc * part_size * sizeof(float), part_lhs);
	read(rhs, linear_loc * part_size * sizeof(float), part_rhs);
	part_res = part_lhs + part_rhs;
	write(res, linear_loc * part_size * sizeof(float), part_res);
}

extern "C" _GENX_MAIN_ void vector_sub(SurfaceIndex res, // res cm_vector
				const SurfaceIndex lhs,// rhs cm_vector
				const SurfaceIndex rhs// lhs cm_vector
				) {
    uint linear_loc = cm_group_id(0);

	vector<float, part_size> part_rhs;
	vector<float, part_size> part_lhs;
	vector<float, part_size> part_res;
	read(lhs, linear_loc * part_size * sizeof(float), part_lhs);
	read(rhs, linear_loc * part_size * sizeof(float), part_rhs);
	part_res = part_lhs - part_rhs;
	write(res, linear_loc * part_size * sizeof(float), part_res);
}
