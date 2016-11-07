# CECS

## What is CECS ?

CECS (C Error Control System) is a pure C libary for textual informative error control. Errors, Warnings, Info  (or other types) that are recording during runtime can be traced back to the file and line where the error occured. Error Logging include information like:
* Error Type: You can define your own error types usign a number. Default types are: Error, Warning, Info, Debug.
* Module-Name (Each Function/File/Library can be a Module)
* Error ID number (To describe an error in a typical way)
* File-Name where the error occured.
* Line-Number where the error occured.
* Analytical Textual Information for further understanding of the error.

## Why should I use CECS ?

If you need a bit more advanced system of Error Control / Logging, you can try CECS. All Errors are being stored with information tags in internal memory and can be printed-out in any format when they are requested. Usefull to track-down errors, on complicated projects.

# Example

An example taken from the test code "t00_test.c":
```C
#include "CECS.h"

// Creating Error and Warning records in a function which represents Module B.
int test05_ModB(sCECS* cecs) {
	CECS_Initialize("MainCECS", cecs);
	const char CECS_ModName[] = "Module B";
	CECS_MERR(1,"Test error in test05_ModB.");
	CECS_MWARN(1,"Test warning in test05_ModB.");
	return 0;
}

// Creating Error and Warning records in another function which represents Module A.
// Both test05_ModA and test05_ModB are sharing the same CECS object, thus they write their records in the same Error Control System.
int test05_ModA(void) {
	sCECS* CECS = CECS_Initialize("MainCECS", NULL);
	const char CECS_ModName[] = "Module A";
	CECS_MERR(1,"Test Error in test05_ModA.");
	test05_ModB(CECS);
	CECS_MWARN(1,"Test Warning in test05_ModA.");

	printf("%s\n", CECS_str(_CECS_ERRTYPE_ALL));

	CECS_Shutdown(NULL);
	return 0;
}

// The results after Calling they main() function will be: 
// ------- CECS:: 4 Errors of ALL Types recorded! -------
// [ET:0]-> #Module A: -9999 | [t00_test.c], 122:: Test Error in test05_ModA.
// [ET:0]-> #Module B: -9999 | [t00_test.c], 134:: Test error in test05_ModB.
// [ET:1]-> #Module B: -9998 | [t00_test.c], 135:: Test warning in test05_ModB.
// [ET:1]-> #Module A: -9998 | [t00_test.c], 124:: Test Warning in test05_ModA.
// -------------------------------------------------------
int main(int argc, char **argv) {
	return test05_ModA();
}
```
