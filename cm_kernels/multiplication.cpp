#include "cm/cm.h"
#include "cm/cmtl.h"

const  int part_size = 16;

inline int positive_mod(int a, int n)
{
    return (a % n + n) % n;
}


extern "C" _GENX_MAIN_ void mult_simple(SurfaceIndex res,// vector
        const SurfaceIndex lhs,// diag matrix
        const SurfaceIndex rhs,// vector
        int vec_size,
        int total_diag_num
        ) 
{
    uint linear_loc = cm_group_id(0);

    vector<float, 1> diag_num;

    vector<float, part_size> part_res;
    cmtl::cm_vector_assign(part_res.select<part_size, 1>(0), 0,0);

    int answer_offset = linear_loc * part_size;

    vector<float, part_size> part_lhs;
    vector<float, part_size> part_rhs;


    vector<uint, 1> offset_diag = 0;

    vector<uint, part_size> offset;



    for(int raw_diag = 0; raw_diag < total_diag_num; raw_diag++)
    {
        read(lhs, (vec_size + (1 + vec_size ) * raw_diag), offset_diag,
        diag_num);
        
        cmtl::cm_vector_assign(offset.select<part_size, 1>(0), 
                positive_mod( answer_offset - diag_num(0), vec_size), 
                1);
        offset = offset % vec_size;

        read(lhs, vec_size + raw_diag * (vec_size + 1) + 1, offset, part_lhs);
        
        read(rhs, 0, offset , part_rhs);
        part_res += part_lhs * part_rhs;
    }

    write(res, answer_offset* sizeof(float), part_res);

}
