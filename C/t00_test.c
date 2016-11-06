#include "src/include/CECS.h"


int main(int argc, char **argv) {
	sCECS* CECS;

	printf("Test Error #0: %s\n", CECS_getErrorStr(0));
	CECS_clear();

    printf("Name of CECS before Initialization: %s\n", CECS_getName());

	CECS = CECS_Initialize("MainCECS",NULL);
	printf("Test Error #1: %s\n", CECS_getErrorStr(0));
	CECS_clear();

    printf("Name of CECS after Initialization: %s\n", CECS_getName());
    
	int x = 7;
	CECS_IERR(x > 5, 0, "Error during checking: x = %i > 5!", x);
	x = 2;
	CECS_IERR(x > 5, 0, "Error during checking: x = %i > 5!", x);
	CECS_WARN(x == 2, "Warning: x is 2 !");
	
	int NE; int* IndxE;
	NE = CECS_GetNumberOfAllErrors();
	printf("\n*** Number of ALL Errors: %i\n", NE);
	
	IndxE = CECS_GetErrorsIDsByType(_CECS_ERRTYPE_ERROR, &NE);
	printf("*** Number of Errors: %i\n", NE);
	for (int i = 0; i < NE; i++) {
		printf("\tTest Error #2-%i: %s\n", i, CECS_getErrorStr(IndxE[i]));
	}
	IndxE = CECS_GetErrorsIDsByType(_CECS_ERRTYPE_WARNING, &NE);
	printf("*** Number of Warnings: %i\n", NE);
	for (int i = 0; i < NE; i++) {
		printf("\tTest Warning #2-%i: %s\n", i, CECS_getErrorStr(IndxE[i]));
	}
	printf("*****\n");
	CECS_clear();

	CECS_RecError(-1, 0, __FNAME__, __LINE__, "Error %i %f", 10, 2.5f);
	CECS_RecError(-2, 1, __FNAME__, __LINE__, "OMG! Thats a huge error!");
	printf("Test Error #3: %i, %i, %s, %u, %s\n",
		CECS_getErrorId(0),
		CECS_getErrorType(0),
		CECS_getErrorFile(0),
		CECS_getErrorLine(0),
		CECS_getErrorStr(0)
	);
	printf("Test Error #4: %i, %i, %s, %u, %s\n",
		CECS_getErrorId(1),
		CECS_getErrorType(1),
		CECS_getErrorFile(1),
		CECS_getErrorLine(1),
		CECS_getErrorStr(1)
	);
	CECS_clear();

	CECS_Shutdown(CECS);
	return 0;
}
