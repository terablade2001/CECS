// MIT License

// Copyright (c) 2016-2019 Vasileios Kon. Pothos (terablade2001)
// https://github.com/terablade2001/CECS

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

// EXPECTED RESULTS:
// //////////////////////////////////////////////////////////////////////////////
// (*) Exception occured:
//   --> ------- ECS:: 2 Errors of ALL Types recorded! -------
// [ET:2]-> #Main: 0 | [main.cpp], 53:: Testing the ECS
// [ET:0]-> #Main: -9999 | [main.cpp], 47:: Error! variable[x] has value (5).
// -------------------------------------------------------
// 
// 
// Now get again the recorded ERRORS only as an external user:
// ------- ECS:: 1 Errors of Type {0} recorded! -------
// -> #Main: -9999 | [main.cpp], 47:: Error! variable[x] has value (5).
// -------------------------------------------------------
// //////////////////////////////////////////////////////////////////////////////


#include "include/CECS.hpp"
using namespace std;

CECS ECS("ECS","Main");

void throwError() {
	int x = 5;
	CECS_ERR(ECS, x == 5, "Error! variable[x] has value (%i).", x)
}

int main(int argc, char **argv)
{
	try {
		CECS_INFO(ECS, true, "Testing the ECS")
		throwError();
		cout << "This message should NOT be shown!" << endl;
	} catch(std::exception &e) {
   std::cout<< std::endl<<"(*) Exception occured: "<< std::endl << "  --> " << e.what() << std::endl;
	}
	
	cout << "Now get again the recorded ERRORS only as an external user:" << endl;
	const int ET = 0; // Error Type = ERROR
	string Errors(ECS.str(ET));
	cout << Errors << endl;
}
