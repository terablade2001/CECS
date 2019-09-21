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

#define CECS__VERSION (0.110)

#define ENABLE_PTHREAD_SUPPORT

#define CECS__MAXERRORS (1024)
#define CECS__ERRORID (-10000)
#define CECS__ECSNAMELENGTH (32)
#define CECS__MODNAMELENGTH (32)

#define CECS__FERRORL (512)


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#ifdef ENABLE_PTHREAD_SUPPORT
	#include <pthread.h>
#endif

#ifdef CECSDEBUG
	#include <csignal>
	#ifdef _WINDOWS_
		#define _exit _Exit
	#else
		#include <unistd.h>
	#endif
#endif


#ifndef __FNAME__
	#define __FNAMEBSL__ (strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)
	#define __FNAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FNAMEBSL__ )
#endif


// Default Error Types ids
#define _CECS_ERRTYPE_ALL (-1)
#define _CECS_ERRTYPE_ERROR (1)
#define _CECS_ERRTYPE_WARNING (2)
#define _CECS_ERRTYPE_INFO (3)
#define _CECS_ERRTYPE_DEBUG (4)
#define _CECS_ERRTYPE_ERRINFO (5)
#define _CECS_ERRTYPE_ERRSTR (6)

// Default Macro-Numbers used in CECS_ERR/WARN/INFO/DEBUG macros.
#define _CECS_DEFAULT_ERRID (CECS__ERRORID+1)
#define _CECS_DEFAULT_WARNID (CECS__ERRORID+2)
#define _CECS_DEFAULT_INFOID (0)
#define _CECS_DEFAULT_DEBUGID (0)

// User short-code macros
#define CECS_IERR(ecsptr, ExpR, ErrID, args...) \
	if ((ExpR)) CECS_RecError((ecsptr), (ErrID), _CECS_ERRTYPE_ERROR, __FNAME__, __LINE__, args);
#define CECS_IWARN(ecsptr, ExpR, ErrID, args...) \
	if ((ExpR)) CECS_RecError((ecsptr), (ErrID), _CECS_ERRTYPE_WARNING, __FNAME__, __LINE__, args);

#define CECS_ERR(ecsptr, ExpR, args...) \
	if ((ExpR)) CECS_RecError((ecsptr), _CECS_DEFAULT_ERRID, _CECS_ERRTYPE_ERROR, __FNAME__, __LINE__, args);
#define CECS_WARN(ecsptr, ExpR, args...) \
	if ((ExpR)) CECS_RecError((ecsptr), _CECS_DEFAULT_WARNID, _CECS_ERRTYPE_WARNING, __FNAME__, __LINE__, args);
#define CECS_INFO(ecsptr, ExpR, args...) \
	if ((ExpR)) CECS_RecError((ecsptr), _CECS_DEFAULT_INFOID, _CECS_ERRTYPE_INFO, __FNAME__, __LINE__, args);
#define CECS_DEBUG(ecsptr, ExpR, args...) \
	if ((ExpR)) CECS_RecError((ecsptr), _CECS_DEFAULT_DEBUGID, _CECS_ERRTYPE_DEBUG, __FNAME__, __LINE__, args);
#define CECS_ERRINF(ecsptr, ExpR, args...) \
	if ((ExpR)) CECS_RecError((ecsptr), _CECS_DEFAULT_DEBUGID, _CECS_ERRTYPE_ERRINFO, __FNAME__, __LINE__, args);

// Those macros also record Module-Name (which must exist in CECS_ModName[] table).
#define CECS_MIERR(ecsptr, ExpR, ErrID, args...) \
	if ((ExpR)) CECS_RecErrorMod((ecsptr), CECS_ModName, (ErrID), _CECS_ERRTYPE_ERROR, __FNAME__, __LINE__, args);
#define CECS_MIWARN(ecsptr, ExpR, ErrID, args...) \
	if ((ExpR)) CECS_RecErrorMod((ecsptr), CECS_ModName, (ErrID), _CECS_ERRTYPE_WARNING, __FNAME__, __LINE__, args);

#define CECS_MERR(ecsptr, ExpR, args...) \
	if ((ExpR)) CECS_RecErrorMod((ecsptr), CECS_ModName, _CECS_DEFAULT_ERRID, _CECS_ERRTYPE_ERROR, __FNAME__, __LINE__, args);
#define CECS_MWARN(ecsptr, ExpR, args...) \
	if ((ExpR)) CECS_RecErrorMod((ecsptr), CECS_ModName, _CECS_DEFAULT_WARNID, _CECS_ERRTYPE_WARNING, __FNAME__, __LINE__, args);
#define CECS_MINFO(ecsptr, ExpR, args...) \
	if ((ExpR)) CECS_RecErrorMod((ecsptr), CECS_ModName, _CECS_DEFAULT_INFOID, _CECS_ERRTYPE_INFO, __FNAME__, __LINE__, args);
#define CECS_MDEBUG(ecsptr, ExpR, args...) \
	if ((ExpR)) CECS_RecErrorMod((ecsptr), CECS_ModName, _CECS_DEFAULT_DEBUGID, _CECS_ERRTYPE_DEBUG, __FNAME__, __LINE__, args);

typedef struct sCECS {
	char* Name;
	unsigned char SetupFlag;
	int NErrors;
	char** SErrors;
	unsigned int* SErrorsL;
	int* IErrors;
	int* TErrors;
	char** FErrors;
	unsigned int* LErrors;
	int* SErrIDs;
	char* DispStr;
	char** MErrors;
	int ErrorLength;
	int MaxErrors;
	int RefCounter;
#ifdef ENABLE_PTHREAD_SUPPORT
	pthread_mutex_t q_mtx;
#endif
#ifdef CECSDEBUG
	int sigArray[32];
	int nsigArray;
#endif
	char unused;
} sCECS;

#ifdef __cplusplus
extern "C" {
#endif

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
 *  \param [in] pcecs Pointer to Linked CECS object.
 *  \param [in] msg A string identifier to used in case of error.
 *  \return The pointer to the Linked CECS object.
 */
sCECS* CECS_CheckIfInit(sCECS* pcecs, const char* msg);

/**
 *  \brief Check only if the linked CECS object is initialized.
 *  \param [in] pcecs Pointer to Linked CECS object.
 *  \return 0-It is already initialized, -1 it was not initialized.
 */
int CECS_CheckIfInitNoMsg(sCECS* pcecs);

/**
 *  \brief Retrive the pCECS pointer, for sharing purposes.
 *  \param [in] pcecs Pointer to Linked CECS object.
 *  \return Return the pCECS pointer.
 */
sCECS* CECS_getCecs(sCECS* pcecs);

/**
 *  \brief Recording an Error in a CECS object using an extra identification 
 *  flag called "modName". If not Linked object exist, then it uses the 
 *  internal CECS object. If the used CECS object is not initialized, then it 
 *  also initialize it.
 *  \param [in] pcecs Pointer to Linked CECS object.
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
	sCECS* pcecs,
	const char* modName,
	int errid,
	int type,
	const char* fname,
	const unsigned int line,
	const char* msg,
	...
);

/**
 *  \brief Similar to "CECS_RecErrorMod" but not taking valist arguments. The message must have been formated already!
 *  \param [in] pcecs Pointer to Linked CECS object.
 *  \param [in] ModName Module Name (for further identification)
 *  \param [in] errid An error id number.
 *  \param [in] type A number indicating the type of the error (i.e. 0-Error, 1-Warning, 2-Info etc)
 *  \param [in] fname Filename where error occurred (i.e. use __FNAME__ macro)
 *  \param [in] line Line where error occurred (i.e. use __LINE__ macro)
 *  \param [in] msg Text formating of the error.
 *  \param [in] msgSize Number of characters in the msg text.
 *   \return The pointer to the Linked CECS object.
 */
sCECS* CECS_RecErrorMod_NoList(
	sCECS* pcecs,
	const char* modName,
	int errid,
	int type,
	const char* fname,
	const unsigned int line,
	const char* msg,
	const unsigned int msgSize
);

/**
 *  \brief Recording an Error in a CECS object. If not Linked object exist, 
 * then it uses the internal CECS object. If the used CECS object is not 
 * initialized, then it also initialize it.
 *  \param [in] pcecs Pointer to Linked CECS object.
 *  \param [in] errid An error id number.
 *  \param [in] type A number indicating the type of the error (i.e. 0-Error, 1-Warning, 2-Info etc)
 *  \param [in] fname Filename where error occurred (i.e. use __FNAME__ macro)
 *  \param [in] line Line where error occurred (i.e. use __LINE__ macro)
 *  \param [in] msg Text formating of the error.
 *  \param [in] ... The client-defined error message parameters
 *   \return The pointer to the Linked CECS object.
 */
sCECS* CECS_RecError(
	sCECS* pcecs,
	int errid,
	int type,
	const char* fname,
	const unsigned int line,
	const char* msg,
	...
);

/**
 *  \brief Returns number of recorded errors/events in the table.
 *  \param [in] pcecs Pointer to Linked CECS object.
 *  \return The number of records.
 */
int CECS_GetNumberOfAllErrors(sCECS* pcecs);

/**
 *  \brief Returns number of recorded errors/events in the table of specific 
 *  type.
 *  \param [in] pcecs Pointer to Linked CECS object.
 *  \param [in] typeId A number representing the Type of the error.
 *  \return The number of records of specific type.
 */
int CECS_GetNumberOfErrorsByType(sCECS* pcecs, int typeId);

/**
 *  \brief Returns the IDs of recorded errors, for a specific type, thus the 
 *  client to be able to read only the errors of the specific type by using 
 *  the corresponding functions CECS_getErrorStr/Id/Type/File/Line/Name().
 *  \param [in] pcecs Pointer to Linked CECS object.
 *  \param [in] typeId A number representing the Type of the error.
 *  \param [out] NErr Number of recorded errors of specific type.
 *  \return Pointer to table with error IDs of the specific type.
 */
int* CECS_GetErrorsIDsByType(sCECS* pcecs, int typeId, int* NErr);

/**
 *  \brief Returns an error message based on it's id (First error occuring is 
 * recorded with id 0)
 *  \param [in] pcecs Pointer to Linked CECS object.
 *  \param [in] id index for the error message to be returned.
 *  \return The error's message string.
 */
const char* CECS_getErrorStr(sCECS* pcecs, int id);
/**
 *  \brief Returns an ErrorID based on it's id (First error occuring is 
 * recorded with id 0)
 *  \param [in] pcecs Pointer to Linked CECS object.
 *  \param [in] id index for the error ErrorID to be returned.
 *  \return The ErrorId
 */
int CECS_getErrorId(sCECS* pcecs, int id);
/**
 *  \brief Returns an error Type based on it's id (First error occuring is 
 * recorded with id 0)
 *  \param [in] pcecs Pointer to Linked CECS object.
 *  \param [in] id index for the error Type to be returned.
 *  \return The Type
 */
int CECS_getErrorType(sCECS* pcecs, int id);
/**
 *  \brief Returns an error Filename (where the error was occured) based on 
 * it's id (First error occuring is recorded with id 0)
 *  \param [in] pcecs Pointer to Linked CECS object.
 *  \param [in] id index for the error Filename to be returned.
 *  \return The filename
 */
const char* CECS_getErrorFile(sCECS* pcecs, int id);
/**
 *  \brief Returns an error source-code line (where the error was occured) 
 * based on it's id (First error occuring is recorded with id 0)
 *  \param [in] pcecs Pointer to Linked CECS object.
 *  \param [in] id index for the error Line to be returned.
 *  \return The line
 */
unsigned int CECS_getErrorLine(sCECS* pcecs, int id);
/**
 *  \brief Returns an error Module-name (where the error was occured) based on 
 * it's id (First error occuring is recorded with id 0)
 *  \param [in] pcecs Pointer to Linked CECS object.
 *  \param [in] id index for the error Module name to be returned.
 *  \return The Module name
 */
const char* CECS_getErrorMod(sCECS* pcecs, int id);

/**
 *  \brief Returns the name of the linked CECS object (for identification 
 * reasons)
 *  \param [in] pcecs Pointer to Linked CECS object.
 *  \return The name of the CECS linked object.
 */
const char* CECS_getName(sCECS* pcecs);

/**
 *  \brief Formatted display of all recorded errors
 *  \param [in] pcecs Pointer to Linked CECS object.
 *  \param [in] typeId A number representing the Type of the error.
 *  \return A pointer to character sequence with the formated errors.
 */
const char* CECS_str(sCECS* pcecs, int typeId);

/**
 *  \brief Clears the recorded errors from the error table.
 *  \param [in] pcecs Pointer to Linked CECS object.
 */
void CECS_clear(sCECS* pcecs);

/**
 *  \brief Clears the recorded errors from the error table.
 *  \param [in] SignalId The signal id to be captured (e.g. SIGSEGV)
 *  \param [in] pcecs Pointer to Linked CECS object.
 */
void CECS_HandleSignal(int SignalId, sCECS* pcecs);

#ifdef __cplusplus
} // extern "C" { 
#endif

#endif