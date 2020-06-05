
#include "cm/cm.h"
#include "cm/cmtl.h"

const unsigned int part_size = 16;

inline int positive_mod(int a, int n)
{
	return (a % n + n) % n;
}


extern "C" _GENX_MAIN_ void mult_simple(SurfaceIndex res,// vector
				const SurfaceIndex lhs,// diag
			       	const SurfaceIndex rhs,// vector
				int vec_size,
				int total_diag
				) 
{
#if 1
    uint linear_loc = cm_linear_global_id();
    int current_diag_num = -1;
    int passed_diags = -1;
    vector<float, part_size> part_res;
    cmtl::cm_vector_assign(part_res.select<part_size, 1>(0), 0,0);
    vector<float, part_size> part_lhs;
    vector<float, part_size> part_rhs;
    vector<uint, part_size> offset;

    int answer_offset = part_size * linear_loc;	
		    passed_diags++;
		    current_diag_num = 0;
		    // calculations starts...
		    // offsets
		    cmtl::cm_vector_assign(offset.select<part_size, 1>(0), positive_mod( answer_offset - current_diag_num, vec_size), 1);
		    offset = offset % vec_size;
		    read(lhs, (passed_diags + 1) * vec_size, offset, part_lhs);
		    read(rhs, 0, offset, part_rhs);
		    part_res += part_lhs * part_rhs;
    write(res, answer_offset* sizeof(float), part_res);
#endif
}
