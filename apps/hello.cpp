#include <iostream>
//#include "SimpleMatrix.h"
#include "CPUMatrix.h"


int main(int ac, char *av[])
{
#if 0
    SimpleMatrix A(1,1);
    SimpleMatrix B("../matrix/simple4x1.txt");
    B.dump();
    A.dump();
    A = B;
    A.dump();
    B.dump();
    A = std::move(B);
    A.dump();
   // B.dump(); segfault...
#endif
    CPUMatrix A(1,1);
    CPUMatrix B("../matrix/simple4x1.txt");
    CPUMatrix C("../matrix/simple1x4.txt");
    A.dump();
    B.dump();
    C.dump();
    multiply(A, B, C);
    A.dump();
    B.dump();
    C.dump();
    A = std::move(B);
    A.dump();
    A.dump_plane();
   // B.dump(); segfault...
    return 0;
}
