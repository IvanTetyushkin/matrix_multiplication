#include "CPUMatrix.h"

#include <benchmark/benchmark.h>

#include <iostream>


// part benchmarks
auto Multiply100 =  []( benchmark::State &state, 
std::pair<int, long int> param)
{
    // preparations part
    CPUMatrix res(param.first, param.first);
    CPUMatrix lhs(param.first, param.first);
    CPUMatrix rhs(param.first, param.first);
    for( auto _: state)
    {
        for(int i = 0 ; i < param.second; i++)
            multiply(res, lhs, rhs);
            addition(lhs, lhs, rhs);
            sub(lhs, lhs, rhs);
            
    }
};

void RegisterBenchmark( const std::vector<std::pair<int, long int>>& values){
    for( const auto& v : values){
        (benchmark::RegisterBenchmark((std::string("CPU_Matrix")+":"+
        std::to_string(v.first)+ ":"+ std::to_string(v.second)).c_str(),
        Multiply100, v));
    }    
}
// end benchmarks



int main(int argc, char *argv[])
{
        std::vector<std::pair<int, long int>> param = {
            {1, 100},
            {2, 100}, 
            {3, 100},
            {4, 100},
            {1, 1000},
            {2, 1000},
            {3, 1000},
            {4, 1000},
            {4, 4634},
            {1, 4634},
            {4, 1000},
            {1, 1000},
            {8, 2000},
            { 32, 1000},
            { 32, 1000},
            { 100, 1000},
        };
        benchmark::Initialize(&argc, argv);
        
        RegisterBenchmark(param);

        benchmark::RunSpecifiedBenchmarks();

}
