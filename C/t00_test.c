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
    
	CECS_RecError(-0, 0, __FNAME__, __LINE__, "%s", "First error occurred!");
	printf("Test Error #2: %s\n", CECS_getErrorStr(0));
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
