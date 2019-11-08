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
#include "../src/include/CECS.hpp"

#ifndef __ECSOBJ__
	#define __ECSOBJ__ ECS
#endif
CECS_MAIN_MODULE("Main-Module","Main-ECS")
using namespace std;

int TableAt = 3; // 1 for success.

bool boolCheck(int _tableat) {
	_ERRB(3==_tableat,"TableAt == 3. Boolean error procuced!")
	return true;
}
int TableIsAt(int _tableat) {
	_ERRI((_tableat < 0) || (_tableat > 3),"TableIsAt():: Works only for input 0, 1, 2 or 3. Got input = (%i)",_tableat)
	_ERRL(_tableat==0,"TableIsAt():: Zero value for table!")
	_ERRO(_tableat==2,  { cout<<"User Code Executed!"<<endl; return 8; }, "TableIsAt():: User code execution detected!")
	_ERRI(!boolCheck(_tableat),"boolCheck() returned false!...")
	return 7;
}

int PersonIsInTable(const char* name) {
	string namestr(name);
	_ERRI(namestr.compare("Vasileios"),"Unknown name on tables!")
	int TableNo = TableIsAt(TableAt);
	_ERRI(TableNo < 0,"Wrong position of Table given the person [%s]",name)
	_WARN(TableNo==8, "Warning: TableId == 8!")
	_CHECKRI_
	_ERRI(TableAt==0,"error recorded as [ERR-LOG] above. Here it's captured!")
	return TableNo;
}

const char* BallIsInHandsOf(const string& nickname) {
	_ERRN(nickname.compare("terablade2001"),"Unknown nickname (%s)",nickname.c_str())
	static string name("Vasileios");
	
	int TableNo = PersonIsInTable(name.c_str());
	_ERRN(TableNo < 0,"Return of PersonIsInTable(%s) was negative",nickname.c_str())
	return name.c_str();
}

void FindTheBall() {
	const string nickname("terablade2001");
	const char* name = BallIsInHandsOf(nickname);
	_ERR(name == NULL,"Return of BallIsInHandsOf() is NULL!")
	cout << "The ball is at person ["<< name << "]" << endl;
}

int main(int argc, char **argv)
{
	_ECSFORMAT(1,0,0,1,1,1,1,1)
	try {
		FindTheBall();
		_CHECKRO_(_ERRSTR((TableAt!=0)&&(TableAt!=2),{
			ss << ">> ..." << endl << "+  This is a custom user message." << endl;
			ss << "+  This message will be printed with [ERRSTR] tag." << endl;
			ss << "+  In such custom messages, the develeper can add any kind of" << endl;
			ss << "information in the \"ss\" stringstream object that _ERRSTR()" << endl;
			ss << "macro provides." << endl;
			ss << "+  That means that he can export whole debug data if need in" << endl;
			ss << "the error log! Like this multiline text!" << endl;
			for (int i=0; i < 5; i++) {
				ss << "i: " << i << endl;
			}
			ss << "... debug data done!";
		}))
		_CERRT("Errors captured. Program failed!")
		cout << "Test Completed Succesfully!" << endl;
	} catch(std::exception &e) {
		 std::cout<< std::endl<<"(*) Exception occured: "<< std::endl << e.what() << std::endl;
	}
	return 0;
}
