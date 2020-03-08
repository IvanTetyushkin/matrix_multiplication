#include "HandMadeCPUMatrix.h"

#include <utility>
#include <iostream>
#include <fstream>

HandMadeCPUMatrix::HandMadeCPUMatrix(const HandMadeCPUMatrix& copy):
    _raw_data(copy._raw_data),
    Str(copy.Str),
    Col(copy.Col)
    {}
HandMadeCPUMatrix& HandMadeCPUMatrix::operator=(const HandMadeCPUMatrix& copy)
{
    _raw_data = copy._raw_data;
    Col = copy.Col;
    Str = copy.Str;
    return *this;  
}

HandMadeCPUMatrix::HandMadeCPUMatrix(const std::string& FileName)
{
        
    std::fstream f(FileName);
    f >> Str >> Col;
    _raw_data.resize(Str*Col);
       
    std::for_each(_raw_data.begin(), 
                  _raw_data.end(),
                [&f](float& value)
                {
                    f >> value;    
                }
    );
    return;    
}
HandMadeCPUMatrix::HandMadeCPUMatrix(int str, int col):
    _raw_data(str * col),
    Str(str),
    Col(col){}

void HandMadeCPUMatrix::dump() const
{
	int current_col = 0;
	std::cerr << "Sizes:" << std::endl;
	std::cerr << "Str = " << Str << std::endl;
	std::cerr << "Col = " << Col << std::endl;
    std::for_each(_raw_data.begin(), 
                  _raw_data.end(),
        [&current_col, this](const float& value)
        {
		    current_col++;
            std::cerr<< value <<"\t";
			if (current_col % (Col) == 0)
			{
				std::cerr << std::endl;
			}
			
			
        });                    
}


void simple_multiply( HandMadeCPUMatrix& res,
                    const HandMadeCPUMatrix& lhs,
                    const HandMadeCPUMatrix& rhs)
{
    assert(res.Str == lhs.Str &&
        res.Col == rhs.Col &&
        lhs.Col == rhs.Str);
    
    for( int i = 0; i< res.Str; ++i)
        for( int j = 0; j< res.Col; ++j)
        {
            res(i,j) = 0;
            for( int k = 0; k < rhs.Str; ++k)
                res(i,j) += lhs(i,k) * rhs(k,j);
        }
    return;
}

void block_multiply( HandMadeCPUMatrix& res,
                    const HandMadeCPUMatrix& lhs,
                    const HandMadeCPUMatrix& rhs,
                    int block_size)
{
    assert(res.Str == lhs.Str &&
        res.Col == rhs.Col &&
        lhs.Col == rhs.Str);
    res.fill_with_zeros();   
    for( int i = 0; i< res.Str; i+= block_size)
        for( int j = 0; j< res.Col; j += block_size)
        {
            for( int k = 0; k < rhs.Str; k += block_size)
                for( int ii = 0; ii < block_size; ++ii)
                    for(int jj = 0; jj < block_size; ++jj)
                        for( int kk = 0; kk < block_size; ++kk)
                            res(i+ii, j + jj) += lhs(i+ii, k + kk) 
                                               * rhs(k + kk,j + jj);

        }
    return;
        
}

void addition( HandMadeCPUMatrix& res, const HandMadeCPUMatrix& lhs, const HandMadeCPUMatrix& rhs)
{
    res.fill_with_zeros();
    assert(res.Str == lhs.Str && 
           lhs.Str == rhs.Str);
    assert(res.Col == lhs.Col && 
           lhs.Col == rhs.Col);
    for(int i = 0; i < res.Str;++i)
        for(int j = 0; j < res.Col;++j)
            res(i,j) = lhs(i,j) + rhs(i,j);
}
void sub( HandMadeCPUMatrix& res, const HandMadeCPUMatrix& lhs, const HandMadeCPUMatrix& rhs)
{
    assert(res.Str == lhs.Str && 
           lhs.Str == rhs.Str);
    assert(res.Col == lhs.Col && 
           lhs.Col == rhs.Col);
    res.fill_with_zeros();
    for(int i = 0; i < res.Str;++i)
        for(int j = 0; j < res.Col;++j)
            res(i,j) = lhs(i,j) - rhs(i,j);
}
