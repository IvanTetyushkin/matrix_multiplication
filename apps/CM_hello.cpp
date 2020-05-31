#include "CM_diag.hpp"
#include <iostream>
using namespace std;
int main()
{
	cout << "hi there\n";
	CM_vector lhs(9);
	CM_vector res(9);
	CM_vector rhs(9);
	CM_diag_matrix matrix(9, 9);
	add(res, lhs, rhs);
	multiply(res, matrix, lhs);
}
