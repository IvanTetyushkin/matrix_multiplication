#include "cm/cm.h"
#include "cm/cmtl.h"

const unsigned int part_size = 8;

inline int positive_mod(int a, int n)
{
    return (a % n + n) % n;
}


extern "C" _GENX_MAIN_ void mult_simple(SurfaceIndex res,// vector
        const SurfaceIndex lhs,// diag matrix
        const SurfaceIndex rhs,// vector
        int vec_size
        ) 
{
#if 1
    uint linear_loc = cm_group_id(0);

    vector<float, part_size> part_res;
    cmtl::cm_vector_assign(part_res.select<part_size, 1>(0), 0,0);

    int answer_offset = linear_loc * part_size;

    vector<float, part_size> part_lhs;
    vector<float, part_size> part_rhs;

    int passed_diags = -1;

    vector<float, part_size> buffer;

    vector<uint, part_size> offset;
    for(int i = 0; i < vec_size/ part_size; i += part_size)
    {
        read(lhs, i * part_size *sizeof(float), buffer);
        for(int j = 0; j < part_size; j++)
        {
            if(buffer[j] == 1)
            {
                passed_diags++;
                int current_diag_num = i * part_size + j;
                cmtl::cm_vector_assign(offset.select<part_size, 1>(0), 
                    positive_mod( answer_offset - current_diag_num, vec_size), 
                    1);
                offset = offset % vec_size;

                read(lhs, (passed_diags + 1) * vec_size, offset, part_lhs);
                read(rhs, 0, offset , part_rhs);
                part_res += part_lhs * part_rhs;
                    
            }
        }
    }
    write(res, answer_offset* sizeof(float), part_res);
#endif
}
