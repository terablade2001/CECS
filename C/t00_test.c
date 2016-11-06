#include "src/include/CECS.h"



/*	Trying to read errors without having initialize CECS_ before, will 
autoinitialize CECS_ and will insert at in a relative error, thus the 
client to be informed.
	Notice also that a auto-initialized CECS_ is taking the name 'CECS_UnNamed'.
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


/* Testing Display Macros (without Initialization) >> Display ALL errors
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


int main(int argc, char **argv) {
	// return test00();
	// return test01();
	// return test02();
	// return test03();
	return test04();
}
