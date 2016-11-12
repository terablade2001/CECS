#include "src/include/CECS.h"



/*	Trying to read errors without having initialize CECS_ before, will 
autoinitialize CECS_ and will insert at in a relative error, thus the 
client to be informed.
	Notice also that a auto-initialized CECS_ is taking the name 'CECS_UnNamed'.

Expected Result:
Test Error #0: CECS_getErrorStr(): CECS was Not Initialized!
Name of CECS before Initialization: CECS-UnNamed
*/
int test00(void) {
	printf("Test Error #0: %s\n", CECS_getErrorStr(0));
	printf("Name of CECS before Initialization: %s\n", CECS_getName());
	CECS_clear();
	CECS_Shutdown(NULL);
	return 0;
}





/* Trying to read an Error after Initialization, while no errors exist, will 
produce a new error: "CECS_getErrorStr(): CECS had no errors recorded!".

Expected Result:
Name of CECS after Initialization: MainCECS
Recorded Errors after Initialization: CECS_getErrorStr(): CECS had no errors recorded!
*/
int test01(void) {
	CECS_Initialize("MainCECS",NULL);
	printf("Name of CECS after Initialization: %s\n", CECS_getName());
	printf("Recorded Errors after Initialization: %s\n", CECS_getErrorStr(0));
	CECS_clear();
	CECS_Shutdown(NULL);
	return 0;
}





/* Testing error recording procedure of different types.

Expected Result:
        Record: 0: Error during checking: 7 > 5!
        Record: 1: Error  : y is 2 !
        Record: 2: Warning: y is 2 !
        Record 0 of type Error: Error during checking: 7 > 5!
        Record 1 of type Error: Error  : y is 2 !
        Record 0 of type Warning: Warning: y is 2 !
*/
int test02(void) {
	int i;
	int* IndxE; int NE;
	CECS_Initialize("MainCECS",NULL);

	int x = 7, y = 2;
	// This should produce an error (with errid value = -99) as x=7 > 5
	CECS_IERR(x > 5 , -99, "Error during checking: %i > 5!", x);
	// This should Not produce error as y=2 < 5.
	CECS_IERR(y > 5 , -99, "Error during checking: %i > 5!", y);

	// The following checks should procude Errors of types: Error And Warning
	// Notice that No error-id is used (internal default value is used instead!)
	CECS_ERR (y == 2, "Error  : y is 2 !");
	CECS_WARN(y == 2, "Warning: y is 2 !");


	// Reviewing all records in the Error Table:
	for (i = 0; i < CECS_GetNumberOfAllErrors(); i++)
		printf("\tRecord: %i: %s\n", i, CECS_getErrorStr(i));

	// Reviewing only the records of type Error:
	IndxE = CECS_GetErrorsIDsByType(_CECS_ERRTYPE_ERROR, &NE);
	for (i = 0; i < NE; i++)
		printf("\tRecord %i of type Error: %s\n", i, CECS_getErrorStr(IndxE[i]));

	// Reviewing only the records of type WARNING:
	IndxE = CECS_GetErrorsIDsByType(_CECS_ERRTYPE_WARNING, &NE);
	for (i = 0; i < NE; i++)
		printf("\tRecord %i of type Warning: %s\n", i, CECS_getErrorStr(IndxE[i]));

	CECS_clear();
	CECS_Shutdown(NULL);
	return 0;
}



// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
// --------------- ADVANCED MACRO ERROR RECORDING ----------------------
// ---------------------------------------------------------------------
// ---------------------------------------------------------------------


/* Testing Display Macros (without Initialization) >> Display ALL errors

Expected Result:
------- CECS-UnNamed:: 5 Errors of ALL Types recorded! -------
[ET:0]-> #Mod-Default: -10000 | [CECS.c], 201:: CECS_RecError/Mod(): CECS was Not Initialized!
[ET:0]-> #Mod-Default: -9999 | [t00_test.c], 116:: CECS_RecError/Mod(): CECS was Not Initialized!
[ET:1]-> #Mod-Default: -9998 | [t00_test.c], 117:: Warn #2
[ET:2]-> #Mod-Default: 0 | [t00_test.c], 118:: Info #3
[ET:4]-> #Mod-Default: 0 | [t00_test.c], 119:: Debug #4
-------------------------------------------------------

Display Again:
------- CECS-UnNamed:: 0 Errors of ALL Types recorded! -------
*/
int test03(void) {
	CECS_ERR(1,"Error #1");
	CECS_WARN(1,"Warn #2");
	CECS_INFO(1,"Info #3");
	CECS_DEBUG(1,"Debug #4");

	printf("%s\n", CECS_str(_CECS_ERRTYPE_ALL));

	printf("Display Again:\n%s", CECS_str(_CECS_ERRTYPE_ALL));

	CECS_Shutdown(NULL);
	return 0;
}






/* Testing Display Macros >> Display Info Errors only!

Expected Result:
------- MainCECS:: 1 Errors of Type {2} recorded! -------
-> #Mod-Default: 0 | [t00_test.c], 149:: Info #3
-------------------------------------------------------

Display Again:
------- MainCECS:: 0 Errors of Type {2} recorded! -------
*/
int test04(void) {
	CECS_Initialize("MainCECS",NULL);
	
	CECS_ERR(1,"Error #1");
	CECS_WARN(1,"Warn #2");
	CECS_INFO(1,"Info #3");
	CECS_DEBUG(1,"Debug #4");

	printf("%s\n", CECS_str(_CECS_ERRTYPE_INFO));

	printf("Display Again:\n%s", CECS_str(_CECS_ERRTYPE_INFO));

	CECS_Shutdown(NULL);
	return 0;
}






/* Testing Modules. Keep tracking modules information using CECS_ModName[] 
variable in combination with the CECS_M___() macros. In this way client is 
able to use the same CECS object over multiple functions/files/libraries and 
distinguish the errors based on a Module-Name. This make's it easier for him 
to track-back the error inside the specific modules.

	In this example the initialization at test05_ModB is using the same CECS 
object as the test05_ModA. However CECS_M___() macros are using 
different Module-Name identifier, thus the errors contains different 
module-names information.

Expected Result:
------- MainCECS-A:: 4 Errors of ALL Types recorded! -------
[ET:0]-> #Module A: -9999 | [t00_test.c], 189:: Test Error in test05_ModA.
[ET:0]-> #Module B: -9999 | [t00_test.c], 203:: Test error in test05_ModB.
[ET:1]-> #Module B: -9998 | [t00_test.c], 204:: Test warning in test05_ModB.
[ET:1]-> #Module A: -9998 | [t00_test.c], 191:: Test Warning in test05_ModA.
-------------------------------------------------------
*/
int test05_ModB(sCECS* cecs);
int test05_ModA(void) {
	// Use CECS-Internal
	sCECS* CECS = CECS_Initialize("MainCECS-A", NULL);
	const char CECS_ModName[] = "Module A";
	CECS_MERR(1,"Test Error in test05_ModA.");
	test05_ModB(CECS);
	CECS_MWARN(1,"Test Warning in test05_ModA.");

	printf("%s\n", CECS_str(_CECS_ERRTYPE_ALL));

	// Shutdown once to free the used CECS-Internal
	CECS_Shutdown(NULL);
	return 0;
}
int test05_ModB(sCECS* cecs) {
	// Initialize using common CECS-Internal.
	CECS_Initialize("MainCECS-B", cecs);
	const char CECS_ModName[] = "Module B";
	CECS_MERR(1,"Test error in test05_ModB.");
	CECS_MWARN(1,"Test warning in test05_ModB.");
	return 0;
}


int main(int argc, char **argv) {
	// return test00();
	// return test01();
	// return test02();
	// return test03();
	// return test04();
	return test05_ModA();
}
