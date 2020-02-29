#include "SimpleMatrix.h"
#include <iostream>
#include <fstream>
#include <utility>
using namespace std;


void SimpleMatrix::copy_elements(const SimpleMatrix& from)
{
    assert(data.height == from.data.height &&
            data.width == from.data.width &&
            "assert: wrong sizes for copy elements..");
    for( int i = 0; i< data.height; ++i)
        for( int j = 0; j < data.width; ++j)
            operator()(i, j) = from(i,j);
    return;    
}

// SimpleMatrix start...
SimpleMatrix::SimpleMatrix(const SimpleMatrix& copy)
{
    data.height = copy.data.height;
    data.width = copy.data.width;
    data.array = static_cast<float*> (malloc( sizeof(float) * data.width
                                                *data.height));
    assert(data.array != nullptr && "malloc not worked...");
    copy_elements(copy);
    return;       
}

SimpleMatrix::SimpleMatrix(SimpleMatrix&& move)
{
    swap(data, move.data);// need checkss...
    move.data.array = nullptr;
}
SimpleMatrix::SimpleMatrix(char* FileName)
{
    fstream f(FileName);
    f >> data.width >> data.height;
    assert( data.width > 0 && data.height > 0);
        
    data.array = static_cast<float*> (malloc( sizeof(float) * data.width
                                                *data.height));
    assert(data.array != nullptr && "malloc not worked...");

    for( int i = 0; i< data.height; ++i)
        for( int j = 0; j < data.width; ++j)
            f >> operator()(i, j) ;
    return;    
}
SimpleMatrix::SimpleMatrix(int w, int h)
{
    data.height = h;
    data.width = w;
    data.array = static_cast<float*> (malloc( sizeof(float) * data.width
                                                *data.height));
    assert(data.array != nullptr && "malloc not worked...");
    return;
}
using  ::SimpleMatrix;
SimpleMatrix& SimpleMatrix::operator=(const SimpleMatrix& copy)
{
    delete(data.array);
    data.height = copy.data.height;
    data.width = copy.data.width;
    data.array = static_cast<float*> (malloc( sizeof(float) * data.width
                                                *data.height));
    assert(data.array != nullptr && "malloc not worked...");
    copy_elements(copy);
}

SimpleMatrix& SimpleMatrix::operator=(SimpleMatrix&& move)
{
    swap(data, move.data);// need checkss...
    move.data.array = nullptr;
    return *this;
}

SimpleMatrix::~SimpleMatrix()
{
    delete( data.array);    
}


float& SimpleMatrix::operator()(int i)
{
    assert(i < data.width*data.height);
    return *(data.array + i);    
}

const float& SimpleMatrix::operator()(int i) const
{
    assert(i < data.width*data.height);
    return *(data.array + i);    
}


float& SimpleMatrix::operator()(int i, int j)
{
    assert(( i * data.width +j)  < data.width*data.height);
    return *(data.array + i * data.width + j);    
}
const float& SimpleMatrix::operator()(int i, int j) const
{
    assert(( i * data.width +j)  < data.width*data.height);
    return *(data.array + i * data.width + j);    
}


void SimpleMatrix::dump()
{
    cerr<< "Dumping matrix...\n";
    cerr<< "Sizes:\nwidth: "<< data.width<<"\theight: "<< data.height<<"\n";
    cerr<< "data_begin:\n";
    for(int i = 0; i < data.height; ++i)
    {
        for( int j = 0; j < data.width; ++j)
            cerr<< operator()(i, j)<<"\t";
        cerr<< "\n";       
    }
    cerr<< "data_end\n";
}

void SimpleMatrix::dump_plane()
{
    cerr<< "Dump_plane matrix...\n";
    cerr<< "Sizes:\nwidth: "<< data.width<<"\theight: "<< data.height<<"\n";
    cerr<< "data_begin:\n";
    for(int i = 0; i < data.height*data.width; ++i)
            cerr<< operator()(i)<<"\t";
        cerr<< "\n";       
    cerr<< "data_end\n";
}

void SimpleMatrix::reshape(int new_w, int new_h)
{
    assert( new_w * new_h == data.height * data.width);
    data.width = new_w;
    data.height = new_h;    
}
