#include <iostream>
#include "HandMadeOpenCLMatrix.h"
#include <vector>
#include <algorithm>


int main(void)
{
	if (prepare::prepare_OpenCL() != 0)
		return - 1;// compile all needed...

	HandMadeOpenCLMatrix a(6, 3);
	a.random_fill();
	a.dump();

	HandMadeOpenCLMatrix b(3, 8);
	b.random_fill();
	std::cerr << std::endl;
	b.dump();
	HandMadeOpenCLMatrix c(6, 8);

	c.prepare();
	a.prepare();
	b.prepare();
	simple_multiply(c, a, b);
	c.getResult();
	a.getResult();
	b.getResult();

	std::cerr << "a*b=" << std::endl;
	c.dump();
	HandMadeOpenCLMatrix d(6, 3);
	d.random_fill();
	std::cerr << "d:" << std::endl;
	d.dump();
	std::cerr << "a+d" << std::endl;

	HandMadeOpenCLMatrix e(6, 3);
	e.random_fill();

	e.prepare();
	a.prepare();
	d.prepare();
	addition(e, a, d);
	c.getResult();
	a.getResult();
	d.getResult();

	e.dump();

	e.prepare();
	a.prepare();
	d.prepare();
	sub(e, a, d);
	c.getResult();
	a.getResult();
	d.getResult();
	e.dump();

	prepare::exit_OpenCL();
	return 0;
}