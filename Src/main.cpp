/*
 * main.cpp
 *
 *  Created on: 11 Jul 2021
 *      Author: barbarossa
 */


#include <iostream>
#include "ExampleFile.hpp"
#include "MatrixVectorMultiplication.hpp"
#include <vector>
#include <string>

class foo
{
public:
	int Seegras;
	std::string name;
	foo(int s, std::string n) : Seegras(s), name (n){};
};

int main()
{
//	const int size = 8;
//	std::vector<foo> test;
//	test.reserve(size);
//	for (int i = 0; i < 8;i++)
//	{
//		test.push_back(foo(i,"A");
//	}
	RunMatrixMultiplication();
}

