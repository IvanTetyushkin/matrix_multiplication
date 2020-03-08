#pragma once
#ifndef HandMadeOpenCLMartix
#define HandMadeOpenCLMartix
#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 200

#include <vector>
#include <utility>
#include <algorithm>
#include <string>
#include <cassert>
#include "CL/cl2.hpp"
namespace prepare {
	int prepare_OpenCL();
	void exit_OpenCL();
}


class HandMadeOpenCLMatrix
{
	std::vector<float, cl::SVMAllocator<float, cl::SVMTraitCoarse<>>> _raw_data;
    // in element aij from wiki
    int Str;// i is str
    int Col; // j is col

    public:
    HandMadeOpenCLMatrix(const HandMadeOpenCLMatrix& copy);
    HandMadeOpenCLMatrix(HandMadeOpenCLMatrix&& move) :
        _raw_data(std::move(move._raw_data)),
        Str(move.Str),
        Col(move.Col)
    {}
    HandMadeOpenCLMatrix& operator=(const HandMadeOpenCLMatrix& copy);
    HandMadeOpenCLMatrix& operator=(HandMadeOpenCLMatrix&& move)
    {
        std::swap(_raw_data, move._raw_data);
        Str = move.Str;
        Col = move.Col;
    }
    //~HandMadeOpenCLMatrix(); seems no need...

    HandMadeOpenCLMatrix(const std::string& FileName);
    HandMadeOpenCLMatrix(int str, int col);


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
		return _raw_data[str * Col + col];
	}
	const float& operator()(int str, int col) const
	{
		assert((str * Col + col) < Str*Col);
		assert(str < Str && col < Col);
		return _raw_data[str * Col + col];
	}
    // as_plane()....

	void prepare()
	{
		cl::unmapSVM(_raw_data);
	}

	void getResult()
	{
		cl::mapSVM(_raw_data);
	}

    void dump() const;

    friend void simple_multiply( HandMadeOpenCLMatrix& res,
                            const HandMadeOpenCLMatrix& lhs,
                            const HandMadeOpenCLMatrix& rhs);
   
    friend void addition( HandMadeOpenCLMatrix& res, const HandMadeOpenCLMatrix& lhs, const HandMadeOpenCLMatrix& rhs);

    friend void sub( HandMadeOpenCLMatrix& res, const HandMadeOpenCLMatrix& lhs, const HandMadeOpenCLMatrix& rhs);
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
		std::generate(_raw_data.begin(), 
			_raw_data.end(), 
            [](){
				return std::rand() % 10;
			}
		);                             
   }

};

#endif
