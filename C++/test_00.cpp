#include "../C/src/include/CECS.h"
#include "src/include/CECS.hpp"

#include <iostream>
using namespace std;

// #define RUNTEST_00
// #define RUNTEST_01
// #define RUNTEST_02
#define RUNTEST_03






/* Expected Results RUNTEST_00
------- GlobalECS:: 0 Errors of ALL Types recorded! -------

Name of CECS: GlobalECS
Mod of CECS: Mod-0
------- GlobalECS:: 5 Errors of ALL Types recorded! -------
[ET:0]-> #Mod-0: -9999 | [test_00.cpp], 14:: Error #1
[ET:1]-> #Mod-0: -9998 | [test_00.cpp], 15:: Warn #2
[ET:2]-> #Mod-0: 0 | [test_00.cpp], 16:: Info #3
[ET:4]-> #Mod-0: 0 | [test_00.cpp], 17:: Debug #4
[ET:0]-> #Mod-Global: -1 | [test_00.cpp], 19:: test00 returned an error!
-------------------------------------------------------
*/
#ifdef RUNTEST_00
CECS GCE("GlobalECS","Mod-Global");
int test00(void) {
	CECS CE("TestECS", "Mod-0");
	cout << CE.str() << endl;
	cout << "Name of CECS: " << CE.name() << endl;
	cout << "Mod of CECS: " << CE.modname() << endl;
    
    CECS_ERR(CE,1,"Error #1");
	CECS_WARN(CE,1,"Warn #2");
	CECS_INFO(CE,1,"Info #3");
	CECS_DEBUG(CE,1,"Debug #4");

	CECS_IERR(GCE,1,-1,"test00 returned an error!");

	return 0;
}
#endif






/* Expected Results RUNTEST_01
------- GlobalECS:: 4 Errors of ALL Types recorded! -------
[ET:0]-> #Mod-Global: -9999 | [test_00.cpp], 43:: Error #1 - GlobalEcs
[ET:0]-> #Mod-0: -9999 | [test_00.cpp], 44:: Error #1 - TempCE
[ET:0]-> #Mod-Global: -1 | [test_00.cpp], 45:: Return Error #3 - GlobalEcs
[ET:0]-> #Mod-0: -9999 | [test_00.cpp], 51:: Error #4 - TempCE
-------------------------------------------------------
*/
/* Expected Results RUNTEST_02
------- GlobalECS:: 4 Errors of ALL Types recorded! -------
[ET:0]-> #Mod-Global: -9999 | [test_00.cpp], 70:: Error #1 - GlobalEcs
[ET:0]-> #Mod-0: -9999 | [test_00.cpp], 71:: Error #1 - TempCE
[ET:0]-> #Mod-Global: -1 | [test_00.cpp], 72:: Return Error #3 - GlobalEcs
[ET:0]-> #Mod-0: -9999 | [test_00.cpp], 78:: Error #4 - TempCE
-------------------------------------------------------

------- ECS-2:: 1 Errors of ALL Types recorded! -------
[ET:0]-> #Mod-2: -9999 | [test_00.cpp], 109:: Error #5
-------------------------------------------------------
*/
/* Expected Results RUNTEST_03
------- GlobalECS:: 4 Errors of ALL Types recorded! -------
[ET:0]-> #Mod-Global: -9999 | [test_00.cpp], 79:: Error #1 - GlobalEcs
[ET:0]-> #Mod-0: -9999 | [test_00.cpp], 80:: Error #1 - TempCE
[ET:0]-> #Mod-Global: -1 | [test_00.cpp], 81:: Return Error #3 - GlobalEcs
[ET:0]-> #Mod-0: -9999 | [test_00.cpp], 87:: Error #4 - TempCE
-------------------------------------------------------

------- GlobalECS:: 1 Errors of ALL Types recorded! -------
[ET:0]-> #Mod-2: -9999 | [test_00.cpp], 132:: Error #5
-------------------------------------------------------
*/
#if defined(RUNTEST_01) || defined(RUNTEST_02) || defined(RUNTEST_03)
CECS* Keep;
int test01a(void) {
	// Create the Main-Object. This creates the CECS-Internal
	CECS GCE("GlobalECS", "Mod-Global");
	/* Create a second object. Passing NULL at cecs it will use the 
CECS-Internal. Because CECS-Internal is already used by the GCE object, the 
RefCounter will only inrease without any change to CECS-Internal. */
	Keep = new CECS("TempCE", "Mod-0");

	/* Now GCE and *Keep are using the same CECS-Internal and write their 
errors there */
	CECS_ERR(GCE,1,"Error #1 - GlobalEcs"); // Add Error #1
	CECS_ERR(*Keep,1,"Error #1 - TempCE"); // Add Error #2
	CECS_IERR(GCE,1,-1,"Return Error #3 - GlobalEcs"); // Add Error #3 and return -1 ErrorId
	/* GCE Object destructs, and reducing RefCounter by 1. As *Keep is still 
using the CECS-Internal, the CECS-Internal does not clean from memory */
}
int test01b(void) {
	// *Keep understands the CECS-C object is dead, and re-initialize it!
	CECS_ERR(*Keep,1,"Error #4 - TempCE");
	return 0;
}
#endif












int main(int argc, char **argv) {
#ifdef RUNTEST_00
	test00();
	cout << GCE.str() << endl;
#endif

#ifdef RUNTEST_01
	test01a(); test01b();
	// *Keep is seeing the CECS-Internal which has all errors with it.
	cout << Keep->str() << endl;
	delete Keep;
	/* Shutdown() for CECS-Internal is called, RefCounter is reaching 0, and 
thus the CECS-Internal is clean-up from memory */
#endif


#ifdef RUNTEST_02
	test01a(); test01b();
	// *Keep is seeing the CECS-Internal which has all errors with it.
	cout << Keep->str() << endl;
	delete Keep;
	/* Shutdown() for CECS-Internal is called, RefCounter is reaching 0, and 
thus the CECS-Internal is clean-up from memory */
	/* CE2 in now using a new CECS-Internal object (Named ECS-2 and not 
GlobalECS anymore) */ 
	CECS CE2("ECS-2", "Mod-2");
	CECS_ERR(CE2,1,"Error #5");
	cout << CE2.str() << endl;
#endif

#ifdef RUNTEST_03
	test01a(); test01b();
	// *Keep is seeing the CECS-Internal which has all errors with it.
	cout << Keep->str() << endl;
	/* If CE2 is initialized before destruction of *Keep, then CE2 will use 
the CECS-Internal of *Keep (with initial name: GlobalECS)*/
	CECS CE2("ECS-2", "Mod-2");
	delete Keep;
	/* Shutdown() of Keep now does not destroy the CECS-Internal, as ECS-2 
is using it */
	CECS_ERR(CE2,1,"Error #5");
	cout << CE2.str() << endl;
#endif

	return 0;
}
