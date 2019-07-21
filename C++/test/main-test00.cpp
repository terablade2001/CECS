#include "CECS/include/CECS.hpp"

#define __ECSOBJ__ ECS
static CECS ECS("Main-Module","Main-CECS");
using namespace std;



int TableAt = 1;



int TableIsAt(int _tableat) {
	_ERRI((_tableat < 0) || (_tableat > 2),"TableIsAt():: Works only for input 0, 1 or 2. Got input = (%i)",_tableat)
	_ERRL(_tableat==0,"TableIsAt():: Zero value for table!")
	_ERRO(_tableat==2,  { cout<<"User Code Executed!"<<endl; return 8; }, "TableIsAt():: User code execution detected!")
	return 7;
}



int PersonIsInTable(const char* name) {
	string namestr(name);
	_ERRI(namestr.compare("Vasileios"),"Unknown name on tables!")
	int TableNo = TableIsAt(TableAt);
	_ERRI(TableNo < 0,"Wrong position of Table given the person [%s]",name)
	_WARN(TableNo==8, "Warning: TableId == 8!")
	_CHECKRI_
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
	_ERRT(name == NULL,"Return of BallIsInHandsOf() is NULL!")
	cout << "The ball is at person ["<< name << "]" << endl;
}



int main(int argc, char **argv)
{
	try {
		FindTheBall();
		_CHECKRT_
		cout << "Test Completed Succesfully!" << endl;
	} catch(std::exception &e) {
		 std::cout<< std::endl<<"(*) Exception occured: "<< std::endl << "  --> " << e.what() << std::endl;
	}
}
