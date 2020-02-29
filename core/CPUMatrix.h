#include "SimpleMatrix.h"

struct CPUMatrix
{
    SimpleMatrix Matrix;
    CPUMatrix(const CPUMatrix& copy);
    CPUMatrix(CPUMatrix&& move);
    CPUMatrix& operator=(const CPUMatrix& copy);
    CPUMatrix& operator=(CPUMatrix&& move);
    //~CPUMatrix(); seems no need...

    CPUMatrix(char* FileName);
    CPUMatrix(int w, int h);
    
    float& operator()(int i, int j);
    const float& operator()(int i, int j) const;
    float& operator()(int i);
    const float& operator()(int i) const;
    void reshape(int new_w, int new_h);

    void dump(){
        Matrix.dump();
        }
    void dump_plane(){
        Matrix.dump_plane();
        }

    friend void multiply( CPUMatrix& res, const CPUMatrix& lhs, const CPUMatrix& rhs);
    friend void addition( CPUMatrix& res, const CPUMatrix& lhs, const CPUMatrix& rhs);
    friend void sub( CPUMatrix& res, const CPUMatrix& lhs, const CPUMatrix& rhs);
    friend float dot(const CPUMatrix& lhs, int str, const CPUMatrix& rhs, int col);

};


