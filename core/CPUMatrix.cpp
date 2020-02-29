#include "CPUMatrix.h"

#include <utility>


CPUMatrix::CPUMatrix(const CPUMatrix& copy):
    Matrix(copy.Matrix){}
CPUMatrix::CPUMatrix(CPUMatrix&& move):
    Matrix(std::move(move.Matrix)){}
CPUMatrix& CPUMatrix::operator=(const CPUMatrix& copy)
{
    Matrix = copy.Matrix;
    return *this;  
}
CPUMatrix& CPUMatrix::operator=(CPUMatrix&& move)
{
    Matrix = std::move(move.Matrix);
    return *this;    
}


CPUMatrix::CPUMatrix(char* FileName):
    Matrix(FileName){}
CPUMatrix::CPUMatrix(int w, int h):
    Matrix(w, h){}

float& CPUMatrix::operator()(int i, int j)
{
    return Matrix(i,j);    
}
const float& CPUMatrix::operator()(int i, int j) const
{
    return Matrix(i,j);    
}
float& CPUMatrix::operator()(int i)
{
    return Matrix(i);    
}
const float& CPUMatrix::operator()(int i) const
{
    return Matrix(i);    
}

void  CPUMatrix::reshape( int new_w, int new_h)
{
    Matrix.reshape(new_w, new_h);    
}
float dot(const CPUMatrix& lhs, int str, const CPUMatrix& rhs, int col)
{
    assert( lhs.Matrix.getWidth() == rhs.Matrix.getHeight());
    float res = 0;
    for( int i = 0; i < lhs.Matrix.getWidth();++i)
        res += lhs(str, i) * rhs(i, col);
    return res;    
}

void multiply( CPUMatrix& res, const CPUMatrix& lhs, const CPUMatrix& rhs)
{
 assert(check_multiply_shapes(res.Matrix, lhs.Matrix, rhs.Matrix));
 for( int i = 0; i< res.Matrix.getHeight(); ++i)
    for( int j = 0; j< res.Matrix.getWidth(); ++j)
        res(i,j) = dot(lhs, i, rhs, j);
 return;
        
}

void addition( CPUMatrix& res, const CPUMatrix& lhs, const CPUMatrix& rhs)
{
    assert(check_shapes(res.Matrix, lhs.Matrix) && 
           check_shapes(lhs.Matrix,rhs.Matrix));
    for(int i = 0; i < res.Matrix.getHeight();++i)
        for(int j = 0; j < res.Matrix.getWidth();++j)
            res(i,j) = lhs(i,j) + rhs(i,j);
}
void sub( CPUMatrix& res, const CPUMatrix& lhs, const CPUMatrix& rhs)
{
    assert(check_shapes(res.Matrix, lhs.Matrix) && 
            check_shapes(lhs.Matrix, rhs.Matrix));
    for(int i = 0; i < res.Matrix.getHeight();++i)
        for(int j = 0; j < res.Matrix.getWidth();++j)
            res(i,j) = lhs(i,j) - rhs(i,j);
}
