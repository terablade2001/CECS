// MIT License

// Copyright (c) 2016 Vasileios Kon. Pothos (terablade2001)

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


#ifndef __CECS__HEADER__
#define __CECS__HEADER__

#define CECS__VERSION (0.001)
#define CECS__MAXERRORS (64)
#define CECS__ERRORID (-1000000000)



#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#ifndef __FNAME__
	#define __FNAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif


typedef struct sCECS {
	int NErrors;
	char** SErrors;
	int ErrorLength;
	int MaxErrors;
} sCECS;

/**
 *  \brief Initialize the CECS object. Multiple CECS objects can exist in a 
project (i.e. when mixing third party libaries which in use a static 
existance of the CECS library), thus for proper usage, the client can select 
which CECS object will be used using a pointer to it.
 *  \param [in] Pointer to a CECS object that is going to be used for error 
recording. If that pointer is NULL then the default CECS object is going to 
be used.
 *  \return The pointer to the CECS object that is used (Linked object)
 */
sCECS* CECS_Initialize(sCECS* pcecs);

/**
 *  \brief Cleanup and Resets an CECS object by pointer. If the input pointer 
is NULL then it Shutdown the linked object (if any).
 *  \param [in] pcecs Pointer to the CECS object that is going to be Cleanup.
 *  \return The pointer to the Linked CECS object.
 */
sCECS* CECS_Shutdown(sCECS* pcecs);

/**
 *  \brief Recording an Error in a CECS object. If not Linked object exist, 
then it uses the internal CECS object. If the used CECS object is not 
initialized, then it also initialize it.
 *  \param [in] errid An error id number.
 *  \param [in] msg Text formating of the error.
 *  \param [in] ... The client-defined error message parameters
 *   \return The pointer to the Linked CECS object.
 */
sCECS* CECS_RecError(int errid, char* msg, ...);

/**
 *  \brief Returns an error message based on it's id (First error occuring is 
recorded with id 0)
 *  \param [in] id Error id to be returned.
 *  \return The error's message string.
 */
const char* CECS_getErrorStr(int id);

/**
 *  \brief Clears the recorded errors from the error table.
 */
void CECS_clear(void);


// TODO:: Keep ErrorId
// TODO:: Keep File information and Line information in tables too.
// TODO:: Keep Type information too (error, warning, info, etc)


#endif