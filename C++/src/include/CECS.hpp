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

#ifndef __CECS__HEADER_HPP__
#define __CECS__HEADER_HPP__
#ifndef __cplusplus
	static_assert(1,"C++ compiler (at least C++11) is required...");
#else

#include <cstdio>
#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>
#include <sstream>
#include <stdexcept>
#include <cstring>


#ifndef __CECS__HEADER__
extern "C" {
	#include "CECS.h"
}
#endif
#undef CECS_IERR
#undef CECS_IWARN
#undef CECS_ERR
#undef CECS_ERRINF
#undef CECS_WARN
#undef CECS_INFO
#undef CECS_DEBUG
#undef CECS_MIERR
#undef CECS_MIWARN
#undef CECS_MERR
#undef CECS_MWARN
#undef CECS_MINFO
#undef CECS_MDEBUG

using namespace std;

/* Modify these defines for the selected behaviour.
I.e. the macros that are using client-specific error-id, now are returning 
the id and returns from the function that the error-check is executed. In 
contrast the macros that are using internal error-id, does not return from 
the function. If need client-specific macros can also be created.
*/
#define __CECS_IRETURN__(ErrID) return (ErrID);
#define __CECS_RETURN__ return;
#define __CECS_RETURN_NULL_ return NULL;
#define __CECS_THROW__(Obj) (Obj).throwErrors();

// Using client error-id
#define CECS_IERR(Obj, ExpR, ErrID, args...) \
	if ((ExpR))  { (Obj).RecError((ErrID), _CECS_ERRTYPE_ERROR, __FNAME__, __LINE__, args); __CECS_IRETURN__(ErrID) }
#define CECS_IWARN(Obj, ExpR, ErrID, args...) \
	if ((ExpR))  { (Obj).RecError((ErrID), _CECS_ERRTYPE_WARNING, __FNAME__, __LINE__, args); __CECS_IRETURN__(ErrID) }

// Using internal error-id
#define CECS_ERRT(Obj, ExpR, args...) \
	if ((ExpR))  { (Obj).RecError(_CECS_DEFAULT_ERRID, _CECS_ERRTYPE_ERROR, __FNAME__, __LINE__, args); __CECS_THROW__(Obj) }
#define CECS_ERR(Obj, ExpR, args...) \
	if ((ExpR))  { (Obj).RecError(_CECS_DEFAULT_ERRID, _CECS_ERRTYPE_ERROR, __FNAME__, __LINE__, args); __CECS_RETURN__ }
#define CECS_ERRI(Obj, ExpR, args...) \
	if ((ExpR))  { (Obj).RecError(_CECS_DEFAULT_ERRID, _CECS_ERRTYPE_ERROR, __FNAME__, __LINE__, args); __CECS_IRETURN__(_CECS_DEFAULT_ERRID) }
#define CECS_ERRN(Obj, ExpR, args...) \
	if ((ExpR))  { (Obj).RecError(_CECS_DEFAULT_ERRID, _CECS_ERRTYPE_ERROR, __FNAME__, __LINE__, args); __CECS_RETURN_NULL_ }
#define CECS_ERRL(Obj, ExpR, args...) \
	if ((ExpR))  { (Obj).RecError(_CECS_DEFAULT_ERRID, _CECS_ERRTYPE_ERROR, __FNAME__, __LINE__, args); }
#define CECS_ERRO(Obj, ExpR, __UserReturn__, args...) \
	if ((ExpR))  { (Obj).RecError(_CECS_DEFAULT_ERRID, _CECS_ERRTYPE_ERROR, __FNAME__, __LINE__, args); __UserReturn__ }
#define CECS_WARN(Obj, ExpR, args...) \
	if ((ExpR))  { (Obj).RecError(_CECS_DEFAULT_WARNID, _CECS_ERRTYPE_WARNING, __FNAME__, __LINE__, args); }
#define CECS_INFO(Obj, ExpR, args...) \
	if ((ExpR))  { (Obj).RecError(_CECS_DEFAULT_INFOID, _CECS_ERRTYPE_INFO, __FNAME__, __LINE__, args); }
#define CECS_DEBUG(Obj, ExpR, args...) \
	if ((ExpR))  { (Obj).RecError(_CECS_DEFAULT_DEBUGID, _CECS_ERRTYPE_DEBUG, __FNAME__, __LINE__, args); }
#define CECS_ERRINF(Obj, ExpR, args...) \
	if ((ExpR))  { (Obj).RecError(_CECS_DEFAULT_WARNID, _CECS_ERRTYPE_ERRINFO, __FNAME__, __LINE__, args); }
#define CECS_WARNO(Obj, ExpR, __UserReturn__, args...) \
	if ((ExpR))  { (Obj).RecError(_CECS_DEFAULT_WARNID, _CECS_ERRTYPE_WARNING, __FNAME__, __LINE__, args); __UserReturn__ }
#define CECS_INFOO(Obj, ExpR, __UserReturn__, args...) \
	if ((ExpR))  { (Obj).RecError(_CECS_DEFAULT_INFOID, _CECS_ERRTYPE_INFO, __FNAME__, __LINE__, args); __UserReturn__ }
#define CECS_DEBUGO(Obj, ExpR, __UserReturn__, args...) \
	if ((ExpR))  { (Obj).RecError(_CECS_DEFAULT_DEBUGID, _CECS_ERRTYPE_DEBUG, __FNAME__, __LINE__, args); __UserReturn__ }

#define CECS_CHECKERRT(Obj) \
	CECS_ERRT((Obj), (Obj).GetNumberOfErrors() != 0, "CECS_CHECKERROR captured: Function throw executed.")
#define CECS_CHECKERR(Obj) \
	CECS_ERR((Obj), (Obj).GetNumberOfErrors() != 0, "CECS_CHECKERROR captured: Function return executed.")
#define CECS_CHECKERRI(Obj) \
	CECS_ERRI((Obj), (Obj).GetNumberOfErrors() != 0, "CECS_CHECKERROR captured: Function return executed.")
#define CECS_CHECKERRN(Obj) \
	CECS_ERRN((Obj), (Obj).GetNumberOfErrors() != 0, "CECS_CHECKERROR captured: Function return NULL executed.")
#define CECS_CHECKERRL(Obj) \
	CECS_ERRL((Obj), (Obj).GetNumberOfErrors() != 0, "CECS_CHECKERROR captured: Function return NULL executed.")
#define CECS_CHECKERRO(Obj, __UserReturn__) \
	CECS_ERRO((Obj), (Obj).GetNumberOfErrors() != 0, __UserReturn__, "CECS_CHECKERROR captured: __UserReturn__ code executed!.")
// Using internal error-id
#define _ERRT(ExpR, args...) CECS_ERRT(__ECSOBJ__, ExpR, args)
#define _ERR(ExpR, args...) CECS_ERR(__ECSOBJ__, ExpR, args)
#define _ERRI(ExpR, args...) CECS_ERRI(__ECSOBJ__, ExpR, args)
#define _ERRN(ExpR, args...) CECS_ERRN(__ECSOBJ__, ExpR, args)
#define _ERRL(ExpR, args...) CECS_ERRL(__ECSOBJ__, ExpR, args)
#define _ERRO(ExpR, __UserReturn__, args...) CECS_ERRO(__ECSOBJ__, ExpR, __UserReturn__, args)
#define _WARN(ExpR, args...) CECS_WARN(__ECSOBJ__, ExpR, args)
#define _INFO(ExpR, args...) CECS_INFO(__ECSOBJ__, ExpR, args)
#define _DEBUG(ExpR, args...) CECS_DEBUG(__ECSOBJ__, ExpR, args)
#define _ERRINF(ExpR, args...) CECS_ERRINF(__ECSOBJ__, ExpR, args)
#define _WARNO(ExpR, __UserReturn__, args...) CECS_WARNO(__ECSOBJ__, ExpR, __UserReturn__, args)
#define _INFOO(ExpR, __UserReturn__, args...) CECS_INFOO(__ECSOBJ__, ExpR, __UserReturn__, args)
#define _DEBUGO(ExpR, __UserReturn__, args...) CECS_DEBUGO(__ECSOBJ__, ExpR, __UserReturn__, args)

#define _CHECKRT_ CECS_CHECKERRT(__ECSOBJ__)
#define _CHECKR_ CECS_CHECKERR(__ECSOBJ__)
#define _CHECKRI_ CECS_CHECKERRI(__ECSOBJ__)
#define _CHECKRN_ CECS_CHECKERRN(__ECSOBJ__)
#define _CHECKRL_ CECS_CHECKERRL(__ECSOBJ__)
#define _CHECKRO_(__UserReturn__) CECS_CHECKERRO(__ECSOBJ__, __UserReturn__)


class CECSBase {
public:
	virtual ~CECSBase() {}
	virtual void Initialize(char* ecsName, char* modName = NULL, void* cecs = NULL) = 0;
	virtual void ConnectTo(void* cecs) = 0;
	virtual void ReInitIfDead(void) = 0;
	virtual void Shutdown(void) = 0;
	virtual void RecError(int errid, int type, const char* fname,
				 const unsigned int line, const char* msg, ... ) = 0;
	virtual const char* str(void) = 0;
	virtual const char* str(int typeId) = 0;
	virtual const char* name(void) = 0;
	virtual const char* modname(void) = 0;
	virtual void throwErrors(int type=_CECS_ERRTYPE_ALL) = 0;
	virtual void clear(void) = 0;
	virtual int GetNumberOfErrors(void) = 0;
	virtual void* cecs(void) = 0;
protected:
	CECSBase();
	char* EcsName;
	char* ModName;
	sCECS* pCECS;
};

class CECS : private CECSBase {
public:
	CECS();
	CECS(char* modName, char* ecsName = NULL, void* cecs = NULL);
	CECS(const char* modName, const char* ecsName = NULL, void* cecs = NULL);
	~CECS();

	void Initialize(char* ecsName, char* modName = NULL, void* cecs = NULL);
	void ConnectTo(void* cecs);
	void ReInitIfDead(void);
	void Shutdown(void);
	void RecError(
		int errid,
		int type,
		const char* fname,
		const unsigned int line,
		const char* msg,
		...
	);
	const char* str(void);
	const char* str(int typeId);
	const char* name(void);
	const char* modname(void);
	void throwErrors(int type=_CECS_ERRTYPE_ALL);
	void clear(void);
	int GetNumberOfErrors(void);
	void* cecs(void);
};



#endif
#endif