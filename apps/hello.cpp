#include <iostream>
#include "HandMadeCPUMatrix.h"

int main(int ac, char *av[])
{
    HandMadeCPUMatrix a(6, 3);
    a.random_fill();
    a.dump();

    HandMadeCPUMatrix b(3, 8);
    b.random_fill();
    std::cerr<< std::endl;
    b.dump();
    HandMadeCPUMatrix c(6,8);
    simple_multiply(c, a, b);
    std::cerr<<"a*b="<< std::endl;
    c.dump();
    block_multiply(c, a, b, 1);
    std::cerr<<"a*b="<< std::endl;
    c.dump();
    HandMadeCPUMatrix d(6, 3);
    d.random_fill();
    std::cerr<< "d:"<< std::endl;
    d.dump();
    std::cerr << "a+d"<< std::endl;

    HandMadeCPUMatrix e(6, 3);
    e.random_fill();
    addition(e, a, d);
    e.dump();
    sub(e, a, d);
    e.dump();

    return 0;
}
