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

#include "src/include/CECS.h"


static sCECS* pcecs = NULL;

/*	Trying to read errors without having initialize CECS_ before, will 
autoinitialize CECS_ and will insert at in a relative error, thus the 
client to be informed.
	Notice also that a auto-initialized CECS_ is taking the name 'CECS_UnNamed'.

Expected Result:
Test Error #0: CECS_getErrorStr(): CECS was Not Initialized!
Name of CECS before Initialization: CECS-UnNamed
*/
int test00(void) {
	printf("Test Error #0: %s\n", CECS_getErrorStr(pcecs, 0));
	printf("Name of CECS before Initialization: %s\n", CECS_getName(pcecs));
	CECS_clear(pcecs);
	CECS_Shutdown(pcecs);
	return 0;
}





/* Trying to read an Error after Initialization, while no errors exist, will 
produce a new error: "CECS_getErrorStr(): CECS had no errors recorded!".

Expected Result:
Name of CECS after Initialization: MainCECS
Recorded Errors after Initialization: CECS_getErrorStr(): CECS had no errors recorded!
*/
int test01(void) {
	pcecs = CECS_Initialize("MainCECS",NULL,0);
	printf("Name of CECS after Initialization: %s\n", CECS_getName(pcecs));
	printf("Recorded Errors after Initialization: %s\n", CECS_getErrorStr(pcecs, 0));
	CECS_clear(pcecs);
	CECS_Shutdown(pcecs);
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
	pcecs = CECS_Initialize("MainCECS",NULL,0);

	int x = 7, y = 2;
	// This should produce an error (with errid value = -99) as x=7 > 5
	CECS_IERR(pcecs, x > 5 , -99, "Error during checking: %i > 5!", x);
	// This should Not produce error as y=2 < 5.
	CECS_IERR(pcecs, y > 5 , -99, "Error during checking: %i > 5!", y);

	// The following checks should procude Errors of types: Error And Warning
	// Notice that No error-id is used (internal default value is used instead!)
	CECS_ERR (pcecs, y == 2, "Error  : y is 2 !");
	CECS_WARN(pcecs, y == 2, "Warning: y is 2 !");


	// Reviewing all records in the Error Table:
	for (i = 0; i < CECS_GetNumberOfAllErrors(pcecs); i++)
		printf("\tRecord: %i: %s\n", i, CECS_getErrorStr(pcecs, i));

	// Reviewing only the records of type Error:
	IndxE = CECS_GetErrorsIDsByType(pcecs, _CECS_ERRTYPE_ERROR, &NE);
	for (i = 0; i < NE; i++)
		printf("\tRecord %i of type Error: %s\n", i, CECS_getErrorStr(pcecs, IndxE[i]));

	// Reviewing only the records of type WARNING:
	IndxE = CECS_GetErrorsIDsByType(pcecs, _CECS_ERRTYPE_WARNING, &NE);
	for (i = 0; i < NE; i++)
		printf("\tRecord %i of type Warning: %s\n", i, CECS_getErrorStr(pcecs, IndxE[i]));

	CECS_clear(pcecs);
	CECS_Shutdown(pcecs);
	return 0;
}



// ---------------------------------------------------------------------
// ---------------------------------------------------------------------
// --------------- ADVANCED MACRO ERROR RECORDING ----------------------
// ---------------------------------------------------------------------
// ---------------------------------------------------------------------


/* Testing Display Macros (without Initialization) >> Display ALL errors

Expected Result:
------- CECS-UnNamed:: 5 Record(s) of ALL Types recorded! -------
[ERROR  ]> #Mod-Default: [CECS.c], 211 |> CECS ERROR:: CECS was Not Initialized!
[ERROR  ]> #Mod-Default: [t00_test.c], 117 |> Error #1
[WARNING]> #Mod-Default: [t00_test.c], 118 |> Warn #2
[INFO   ]> #Mod-Default: [t00_test.c], 119 |> Info #3
[DEBUG  ]> #Mod-Default: [t00_test.c], 120 |> Debug #4
-------------------------------------------------------

Display Again:
------- CECS-UnNamed:: 0 Record(s) of ALL Types recorded! -------
*/
int test03(void) {
	pcecs = CECS_CheckIfInit(pcecs, "CECS ERROR:");
	CECS_ERR(pcecs, 1,"Error #1");
	CECS_WARN(pcecs, 1,"Warn #2");
	CECS_INFO(pcecs, 1,"Info #3");
	CECS_DEBUG(pcecs, 1,"Debug #4");

	printf("%s\n", CECS_str(pcecs, _CECS_ERRTYPE_ALL));
	CECS_clear(pcecs);

	printf("Display Again:\n%s", CECS_str(pcecs, _CECS_ERRTYPE_ALL));

	CECS_Shutdown(pcecs);
	return 0;
}






/* Testing Display Macros >> Display Info Errors only!

Expected Result:
------- MainCECS:: 1 Record(s) of Type [INFO   ] recorded! -------
#Mod-Default: [t00_test.c], 151 |> Info #3
-------------------------------------------------------

Display Again:
------- MainCECS:: 1 Record(s) of Type [INFO   ] recorded! -------
#Mod-Default: [t00_test.c], 151 |> Info #3
--------------------------------------------------------
*/
int test04(void) {
	pcecs = CECS_Initialize("MainCECS",NULL,0);
	
	CECS_ERR(pcecs, 1,"Error #1");
	CECS_WARN(pcecs, 1,"Warn #2");
	CECS_INFO(pcecs, 1,"Info #3");
	CECS_DEBUG(pcecs, 1,"Debug #4");

	printf("%s\n", CECS_str(pcecs, _CECS_ERRTYPE_INFO));

	printf("Display Again:\n%s", CECS_str(pcecs, _CECS_ERRTYPE_INFO));

	CECS_Shutdown(pcecs);
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
------- MainCECS-A:: 4 Record(s) of ALL Types recorded! -------
[ERROR  ]> #Module A: [t00_test.c], 193 |> Test Error in test05_ModA.
[ERROR  ]> #Module B: [t00_test.c], 207 |> Test error in test05_ModB.
[WARNING]> #Module B: [t00_test.c], 208 |> Test warning in test05_ModB.
[WARNING]> #Module A: [t00_test.c], 195 |> Test Warning in test05_ModA.
-------------------------------------------------------
*/
int test05_ModB(sCECS* cecs);
int test05_ModA(void) {
	// Use CECS-Internal
	sCECS* CECS = CECS_Initialize("MainCECS-A", NULL,0);
	const char CECS_ModName[] = "Module A";
	CECS_MERR(CECS, 1,"Test Error in test05_ModA.");
	test05_ModB(CECS);
	CECS_MWARN(CECS, 1,"Test Warning in test05_ModA.");

	printf("%s\n", CECS_str(CECS, _CECS_ERRTYPE_ALL));

	// Shutdown once to free the used CECS-Internal
	CECS_Shutdown(CECS);
	return 0;
}
int test05_ModB(sCECS* cecs) {
	// Initialize using common CECS-Internal.
	cecs = CECS_Initialize("MainCECS-B", cecs,0);
	const char CECS_ModName[] = "Module B";
	CECS_MERR(cecs, 1,"Test error in test05_ModB.");
	CECS_MWARN(cecs, 1,"Test warning in test05_ModB.");
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
