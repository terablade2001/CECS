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

#define CECS__VERSION (0.003)
#define CECS__MAXERRORS (64)
#define CECS__ERRORID (-1000000000)

#define CECS__FERRORL (256)



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#ifndef __FNAME__
	#define __FNAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif


typedef struct sCECS {
	char* Name;
	unsigned char SetupFlag;
	int NErrors;
	char** SErrors;
	int* IErrors;
	int* TErrors;
	char** FErrors;
	unsigned int* LErrors;
	int ErrorLength;
	int MaxErrors;
} sCECS;

/**
 *  \brief Initialize the CECS object. Multiple CECS objects can exist in a 
 * project (i.e. when mixing third party libaries which in use a static 
 * existance of the CECS library), thus for proper usage, the client can select 
 * which CECS object will be used using a pointer to it.
 *  \param [in] name Is a string (up to 64 characters) representing in a
 * textual way the CECS object that is used. If the CECS it has been initialied 
 * before (with another name) then, the 'name' argument is ignored.
 *  \param [in] Pointer to a CECS object that is going to be used for error 
 * recording. If that pointer is NULL then the default CECS object is going to 
 * be used.
 *  \return The pointer to the CECS object that is ussed (Linked object)
 *  \details When 'pcecs' argument is NULL this means that the CECS object 
 * that is going to be initialized is a brand new object; and therefore it 
 * needs and a name. The 'name' argument is then used. When the 'pcecs' 
 * argument is not NULL, the 'name' argument is used on if the 'pcecs' argument 
 * is pointing to a non-initialized CECS object.
 */
sCECS* CECS_Initialize(char* name, sCECS* pcecs);

/**
 *  \brief Cleanup and Resets an CECS object by pointer. If the input pointer 
 * is NULL then it Shutdown the linked object (if any).
 *  \param [in] pcecs Pointer to the CECS object that is going to be Cleanup.
 *  \return The pointer to the Linked CECS object.
 */
sCECS* CECS_Shutdown(sCECS* pcecs);

/**
 *  \brief Check if the linked CECS object is initialized. If not initialize 
 * it and add an error to it.
 *  \param [in] msg A string identifier to used in case of error.
 *  \return 0-It is already initialized, -1 it was not initialized.
 */
int CECS_CheckIfInit(const char* msg);

/**
 *  \brief Recording an Error in a CECS object. If not Linked object exist, 
 * then it uses the internal CECS object. If the used CECS object is not 
 * initialized, then it also initialize it.
 *  \param [in] errid An error id number.
 *  \param [in] type A number indicating the type of the error (i.e. 0-Error, 1-Warning, 2-Info etc)
 *  \param [in] fname Filename where error occurred (i.e. use __FNAME__ macro)
 *  \param [in] line Line where error occurred (i.e. use __LINE__ macro)
 *  \param [in] msg Text formating of the error.
 *  \param [in] ... The client-defined error message parameters
 *   \return The pointer to the Linked CECS object.
 */
sCECS* CECS_RecError(
	int errid,
	int type,
	const char* fname,
	const unsigned int line,
	char* msg,
	...
);

/**
 *  \brief Returns an error message based on it's id (First error occuring is 
 * recorded with id 0)
 *  \param [in] id index for the error message to be returned.
 *  \return The error's message string.
 */
const char* CECS_getErrorStr(int id);
/**
 *  \brief Returns an ErrorID based on it's id (First error occuring is 
 * recorded with id 0)
 *  \param [in] id index for the error ErrorID to be returned.
 *  \return The ErrorId
 */
int CECS_getErrorId(int id);
/**
 *  \brief Returns an error Type based on it's id (First error occuring is 
 * recorded with id 0)
 *  \param [in] id index for the error Type to be returned.
 *  \return The Type
 */
int CECS_getErrorType(int id);
/**
 *  \brief Returns an error Filename (where the error was occured) based on 
 * it's id (First error occuring is recorded with id 0)
 *  \param [in] id index for the error Filename to be returned.
 *  \return The filename
 */
const char* CECS_getErrorFile(int id);
/**
 *  \brief Returns an error source-code line (where the error was occured) 
 * based on it's id (First error occuring is recorded with id 0)
 *  \param [in] id index for the error Line to be returned.
 *  \return The line
 */
unsigned int CECS_getErrorLine(int id);

/**
 *  \brief Returns the name of the linked CECS object (for identification 
 * reasons)
 *  \return The name of the CECS linked object.
 */
const char* CECS_getName(void);

/**
 *  \brief Clears the recorded errors from the error table.
 */
void CECS_clear(void);



#endif