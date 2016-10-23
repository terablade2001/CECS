#include "src/include/CECS.h"


int main(int argc, char **argv) {
	sCECS* CECS;

	printf("Test Error #0: %s\n", CECS_getErrorStr(0));
	CECS_clear();

	CECS = CECS_Initialize(NULL);
	printf("Test Error #1: %s\n", CECS_getErrorStr(0));
	CECS_clear();

	CECS_RecError(-0,"%s", "First error occurred!");
	printf("Test Error #2: %s\n", CECS_getErrorStr(0));
	CECS_clear();

	CECS_RecError(-1,"Error %i %f", 10, 2.5f);
	CECS_RecError(-2,"OMG! Thats a huge error!");
	printf("Test Error #3: %s\n", CECS_getErrorStr(0));
	printf("Test Error #4: %s\n", CECS_getErrorStr(1));
	CECS_clear();

	CECS_Shutdown(CECS);
	return 0;
}
