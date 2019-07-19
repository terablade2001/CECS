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

#ifndef __CECS__HEADER__
#define __CECS__HEADER__

#define CECS__VERSION (0.013)

#define CECS__MAXERRORS (1024)
#define CECS__ERRORID (-10000)
#define CECS__ECSNAMELENGTH (32)
#define CECS__MODNAMELENGTH (32)

#define CECS__FERRORL (512)
#define CECS__MAXDISPSTRSIZE (CECS__FERRORL * CECS__MAXERRORS + 2)



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#ifndef __FNAME__
	#define __FNAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif


// Default Error Types ids
#define _CECS_ERRTYPE_ALL (-1)
#define _CECS_ERRTYPE_ERROR (0)
#define _CECS_ERRTYPE_WARNING (1)
#define _CECS_ERRTYPE_INFO (2)
#define _CECS_ERRTYPE_DEBUG (4)
#define _CECS_ERRTYPE_ERRINFO (5)

// Default Macro-Numbers used in CECS_ERR/WARN/INFO/DEBUG macros.
#define _CECS_DEFAULT_ERRID (CECS__ERRORID+1)
#define _CECS_DEFAULT_WARNID (CECS__ERRORID+2)
#define _CECS_DEFAULT_INFOID (0)
#define _CECS_DEFAULT_DEBUGID (0)

// User short-code macros
#define CECS_IERR(ExpR, ErrID, args...) \
	if ((ExpR)) CECS_RecError((ErrID), _CECS_ERRTYPE_ERROR, __FNAME__, __LINE__, args);
#define CECS_IWARN(ExpR, ErrID, args...) \
	if ((ExpR)) CECS_RecError((ErrID), _CECS_ERRTYPE_WARNING, __FNAME__, __LINE__, args);

#define CECS_ERR(ExpR, args...) \
	if ((ExpR)) CECS_RecError(_CECS_DEFAULT_ERRID, _CECS_ERRTYPE_ERROR, __FNAME__, __LINE__, args);
#define CECS_WARN(ExpR, args...) \
	if ((ExpR)) CECS_RecError(_CECS_DEFAULT_WARNID, _CECS_ERRTYPE_WARNING, __FNAME__, __LINE__, args);
#define CECS_INFO(ExpR, args...) \
	if ((ExpR)) CECS_RecError(_CECS_DEFAULT_INFOID, _CECS_ERRTYPE_INFO, __FNAME__, __LINE__, args);
#define CECS_DEBUG(ExpR, args...) \
	if ((ExpR)) CECS_RecError(_CECS_DEFAULT_DEBUGID, _CECS_ERRTYPE_DEBUG, __FNAME__, __LINE__, args);
#define CECS_ERRINF(ExpR, args...) \
	if ((ExpR)) CECS_RecError(_CECS_DEFAULT_DEBUGID, _CECS_ERRTYPE_ERRINFO, __FNAME__, __LINE__, args);

// Those macros also record Module-Name (which must exist in CECS_ModName[] table).
#define CECS_MIERR(ExpR, ErrID, args...) \
	if ((ExpR)) CECS_RecErrorMod(CECS_ModName, (ErrID), _CECS_ERRTYPE_ERROR, __FNAME__, __LINE__, args);
#define CECS_MIWARN(ExpR, ErrID, args...) \
	if ((ExpR)) CECS_RecErrorMod(CECS_ModName, (ErrID), _CECS_ERRTYPE_WARNING, __FNAME__, __LINE__, args);

#define CECS_MERR(ExpR, args...) \
	if ((ExpR)) CECS_RecErrorMod(CECS_ModName, _CECS_DEFAULT_ERRID, _CECS_ERRTYPE_ERROR, __FNAME__, __LINE__, args);
#define CECS_MWARN(ExpR, args...) \
	if ((ExpR)) CECS_RecErrorMod(CECS_ModName, _CECS_DEFAULT_WARNID, _CECS_ERRTYPE_WARNING, __FNAME__, __LINE__, args);
#define CECS_MINFO(ExpR, args...) \
	if ((ExpR)) CECS_RecErrorMod(CECS_ModName, _CECS_DEFAULT_INFOID, _CECS_ERRTYPE_INFO, __FNAME__, __LINE__, args);
#define CECS_MDEBUG(ExpR, args...) \
	if ((ExpR)) CECS_RecErrorMod(CECS_ModName, _CECS_DEFAULT_DEBUGID, _CECS_ERRTYPE_DEBUG, __FNAME__, __LINE__, args);

typedef struct sCECS {
	char* Name;
	unsigned char SetupFlag;
	int NErrors;
	char** SErrors;
	int* IErrors;
	int* TErrors;
	char** FErrors;
	unsigned int* LErrors;
	int* SErrIDs;
	char* DispStr;
	char** MErrors;
	int ErrorLength;
	int MaxErrors;
	int MaxDisplayStringSize;
	int RefCounter;
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
 *  \param [in] It it's != 0, it will enforce name update of the ECS.
 *  \return The pointer to the CECS object that is ussed (Linked object)
 *  \details When 'pcecs' argument is NULL this means that the CECS object 
 * that is going to be initialized is a brand new object; and therefore it 
 * needs and a name. The 'name' argument is then used. When the 'pcecs' 
 * argument is not NULL, the 'name' argument is used on if the 'pcecs' argument 
 * is pointing to a non-initialized CECS object.
 */
sCECS* CECS_Initialize(const char* name, sCECS* pcecs, int replaceName);

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
 *  \brief Check only if the linked CECS object is initialized.
 *  \return 0-It is already initialized, -1 it was not initialized.
 */
int CECS_CheckIfInitNoMsg(void);

/**
 *  \brief Retrive the pCECS pointer, for sharing purposes.
 *  \return Return the pCECS pointer.
 */
sCECS* CECS_getCecs(void);

/**
 *  \brief Recording an Error in a CECS object using an extra identification 
 *  flag called "modName". If not Linked object exist, then it uses the 
 *  internal CECS object. If the used CECS object is not initialized, then it 
 *  also initialize it.
 *  \param [in] ModName Module Name (for further identification)
 *  \param [in] errid An error id number.
 *  \param [in] type A number indicating the type of the error (i.e. 0-Error, 1-Warning, 2-Info etc)
 *  \param [in] fname Filename where error occurred (i.e. use __FNAME__ macro)
 *  \param [in] line Line where error occurred (i.e. use __LINE__ macro)
 *  \param [in] msg Text formating of the error.
 *  \param [in] ... The client-defined error message parameters
 *   \return The pointer to the Linked CECS object.
 */
sCECS* CECS_RecErrorMod(
	const char* modName,
	int errid,
	int type,
	const char* fname,
	const unsigned int line,
	const char* msg,
	...
);

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
	const char* msg,
	...
);

/**
 *  \brief Returns number of recorded errors/events in the table.
 *  \return The number of records.
 */
int CECS_GetNumberOfAllErrors(void);

/**
 *  \brief Returns number of recorded errors/events in the table of specific 
 *  type.
 *  \param [in] typeId A number representing the Type of the error.
 *  \return The number of records of specific type.
 */
int CECS_GetNumberOfErrorsByType(int typeId);

/**
 *  \brief Returns the IDs of recorded errors, for a specific type, thus the 
 *  client to be able to read only the errors of the specific type by using 
 *  the corresponding functions CECS_getErrorStr/Id/Type/File/Line/Name().
 *  \param [in] typeId A number representing the Type of the error.
 *  \param [out] NErr Number of recorded errors of specific type.
 *  \return Pointer to table with error IDs of the specific type.
 */
int* CECS_GetErrorsIDsByType(int typeId, int* NErr);

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
 *  \brief Returns an error Module-name (where the error was occured) based on 
 * it's id (First error occuring is recorded with id 0)
 *  \param [in] id index for the error Module name to be returned.
 *  \return The Module name
 */
const char* CECS_getErrorMod(int id);

/**
 *  \brief Returns the name of the linked CECS object (for identification 
 * reasons)
 *  \return The name of the CECS linked object.
 */
const char* CECS_getName(void);

/**
 *  \brief Formatted display of all recorded errors
 *  \param [in] typeId A number representing the Type of the error.
 *  \return A pointer to character sequence with the formated errors.
 */
const char* CECS_str(int typeId);

/**
 *  \brief Clears the recorded errors from the error table.
 */
void CECS_clear(void);



#endif