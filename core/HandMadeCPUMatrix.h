#pragma once
#ifndef HandMadeCPUMartix
#define HandMadeCPUMartix
#include <vector>
#include <utility>
#include <algorithm>
#include <string>
#include <cassert>

class HandMadeCPUMatrix
{
    std::vector<float> _raw_data;
    // in element aij from wiki, can get it in raw_data, but...
    int Str;// i is str
    int Col; // j is col
 
    public:
    HandMadeCPUMatrix(const HandMadeCPUMatrix& copy);
    HandMadeCPUMatrix(HandMadeCPUMatrix&& move) :
        _raw_data(std::move(move._raw_data)),
        Str(move.Str),
        Col(move.Col)
    {}
    HandMadeCPUMatrix& operator=(const HandMadeCPUMatrix& copy);
    HandMadeCPUMatrix& operator=(HandMadeCPUMatrix&& move)
    {
        std::swap(_raw_data, move._raw_data);
        Str = move.Str;
        Col = move.Col;
    }
    //~HandMadeCPUMatrix(); seems no need...

    HandMadeCPUMatrix(const std::string& FileName);
    HandMadeCPUMatrix(int str, int col);
#if 0
    std::vector<float>& operator[](int str)
    {
        assert( str < Str && "Error opertator[]");
        return _raw_data[str];   
    } 
    const std::vector<float>& operator[](int str) const
    {
        assert( str < Str && "Error opertator[] const");
        return _raw_data[str];    
    } 
#endif
	float& operator()(int str, int col)
	{
		assert((str * Col + col) < Str*Col);
		assert(str < Str && col < Col);
		return _raw_data[str * Col + col] ;
	}
	const float& operator()(int str, int col) const
	{
		assert((str * Col + col) < Str*Col);
		assert(str < Str && col < Col);
		return _raw_data[str * Col + col];
	}
    // as_plane()....

    void dump() const;

    friend void block_multiply(HandMadeCPUMatrix& res, 
                            const HandMadeCPUMatrix& lhs,
                            const HandMadeCPUMatrix& rhs,
                            int block_size = 16); 

    friend void simple_multiply( HandMadeCPUMatrix& res,
                            const HandMadeCPUMatrix& lhs,
                            const HandMadeCPUMatrix& rhs);
   
    friend void addition( HandMadeCPUMatrix& res, const HandMadeCPUMatrix& lhs, const HandMadeCPUMatrix& rhs);
    friend void sub( HandMadeCPUMatrix& res, const HandMadeCPUMatrix& lhs, const HandMadeCPUMatrix& rhs);
    void fill_with_zeros()
    {
        
        std::for_each(_raw_data.begin(), 
                _raw_data.end(),
                [](float& value)
                {
					value = 0;
                }
       );
   }
   void random_fill()
   {
	   std::generate(_raw_data.begin(), _raw_data.end(),
		   []() {
		   return std::rand() % 10;
	   });
   }

};

#endif
