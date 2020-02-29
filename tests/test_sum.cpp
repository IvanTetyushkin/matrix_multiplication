#include <gtest/gtest.h>

#include "add.h"

#include <benchmark/benchmark.h>

#include <iostream>

double realHarm(long int until)
{
    double res = 0;
    for( int i = until ; i>0 ; i--)
        res+= 1.0d/i;
    return res;    
};

//for gtests
class MyFixture : public ::testing::Test{};

class MyTest: public MyFixture
{
    private:
    int _threads;
    long int _count;
    int _bunch_size;
    public:
    explicit MyTest(int thread, long int  count, int bunch_size):
    _threads(thread), _count(count), _bunch_size(bunch_size){}
    void TestBody() override
    {
        EXPECT_EQ( getHarm(_threads, _count, _bunch_size), getHarm(_threads, _count, _bunch_size));
        EXPECT_EQ( realHarm(_count), getHarm(_threads, _count, 1));
    }
};

void RegisterTests( const std::vector<std::pair<int, long int>>& values)
{
    for(const auto& v: values)
    {
        ::testing::RegisterTest(
        "MyTest", ("Test" + std::to_string(v.first)+ ":"+ 
        std::to_string(v.second)+":"+ 
        std::to_string(10)).c_str(), nullptr,
        nullptr,
        __FILE__, __LINE__,
        [=]()->MyFixture* {return new MyTest(v.first,v.second, 100);});    
    }    
}
// end gtest part

// part benchmarks
auto getTimes =  []( benchmark::State &state, 
std::pair<int, long int> param)
{
    // preparations part - no now;
    for( auto _: state)
    {
        getHarm( param.first, param.second, 100);    
    }
};

auto getHarmTime = [](benchmark::State& state, long int until)
{
   for( auto _: state)
   {
        realHarm( until); 
   }    
};

void RegisterBenchmark( const std::vector<std::pair<int, long int>>& values){
    for( const auto& v : values){
        (benchmark::RegisterBenchmark((std::string("getTime")+":"+
        std::to_string(v.first)+ ":"+ std::to_string(v.second)+":"+
        std::to_string(100)).c_str(),
        getTimes, v));
        (benchmark::RegisterBenchmark((std::string("Simple")+":"+ std::to_string(v.second)+":").c_str(),
        getHarmTime, v.second));
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
            {4, 46340},
            {1, 463400},
            {4, 100000},
            {1, 100000},
            {8, 20000},
            { 32, 1000000000},
            { 32, 1000000000},
        };
        ::testing::InitGoogleTest(&argc, argv);
        benchmark::Initialize(&argc, argv);
        RegisterTests(param);
        
        RegisterBenchmark(param);

        benchmark::RunSpecifiedBenchmarks();

        std::cout<< "testing gtests"<< std::endl;
        return RUN_ALL_TESTS();

}
