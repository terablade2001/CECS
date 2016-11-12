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
#ifndef __CECS__HEADER_HPP__
#define __CECS__HEADER_HPP__
#ifndef __cplusplus
	static_assert(1,"C++ compiler (at least C++11) is required...");
#else

#ifndef __CECS__HEADER__
	#include "CECS.h"
#endif
#undef CECS_IERR
#undef CECS_IWARN
#undef CECS_ERR
#undef CECS_WARN
#undef CECS_INFO
#undef CECS_DEBUG
#undef CECS_MIERR
#undef CECS_MIWARN
#undef CECS_MERR
#undef CECS_MWARN
#undef CECS_MINFO
#undef CECS_MDEBUG

/* Modify these defines for the selected behaviour.
I.e. the macros that are using client-specific error-id, now are returning 
the id and returns from the function that the error-check is executed. In 
contrast the macros that are using internal error-id, does not return from 
the function. If need client-specific macros can also be created.
*/
#define __CECS_IRETURN__(ErrID) return (ErrID);
#define __CECS_RETURN__(ErrID) /* Do nothing */

// Using client error-id
#define CECS_IERR(Obj, ExpR, ErrID, args...) \
	if ((ExpR))  { (Obj).RecError((ErrID), _CECS_ERRTYPE_ERROR, __FNAME__, __LINE__, args); __CECS_IRETURN__(ErrID) }
#define CECS_IWARN(Obj, ExpR, ErrID, args...) \
	if ((ExpR))  { (Obj).RecError((ErrID), _CECS_ERRTYPE_WARNING, __FNAME__, __LINE__, args); __CECS_IRETURN__(ErrID) }

// Using internal error-id
#define CECS_ERR(Obj, ExpR, args...) \
	if ((ExpR))  { (Obj).RecError(_CECS_DEFAULT_ERRID, _CECS_ERRTYPE_ERROR, __FNAME__, __LINE__, args); __CECS_RETURN__(ErrID) }
#define CECS_WARN(Obj, ExpR, args...) \
	if ((ExpR))  { (Obj).RecError(_CECS_DEFAULT_WARNID, _CECS_ERRTYPE_WARNING, __FNAME__, __LINE__, args); __CECS_RETURN__(ErrID) }
#define CECS_INFO(Obj, ExpR, args...) \
	if ((ExpR))  { (Obj).RecError(_CECS_DEFAULT_INFOID, _CECS_ERRTYPE_INFO, __FNAME__, __LINE__, args); __CECS_RETURN__(ErrID) }
#define CECS_DEBUG(Obj, ExpR, args...) \
	if ((ExpR))  { (Obj).RecError(_CECS_DEFAULT_DEBUGID, _CECS_ERRTYPE_DEBUG, __FNAME__, __LINE__, args); __CECS_RETURN__(ErrID) }



class CECSBase {
public:
	virtual ~CECSBase() {}
	virtual void Initialize(char* ecsName, char* modName = NULL, sCECS* cecs = NULL) = 0;
	virtual void ReInitIfDead(void) = 0;
	virtual void Shutdown(void) = 0;
	virtual void RecError(int errid, int type, const char* fname,
				 const unsigned int line, const char* msg, ... ) = 0;
	virtual const char* str(void) = 0;
	virtual const char* str(int typeId) = 0;
	virtual const char* name(void) = 0;
	virtual const char* modname(void) = 0;
	virtual sCECS* cecs(void) = 0;
protected:
	CECSBase();
	char* EcsName;
	char* ModName;
	sCECS* pCECS;
};

class CECS : private CECSBase {
public:
	CECS();
	CECS(char* ecsName, char* modName = NULL, sCECS* cecs = NULL);
	CECS(const char* ecsName, const char* modName = NULL, sCECS* cecs = NULL);
	~CECS();

	void Initialize(char* ecsName, char* modName = NULL, sCECS* cecs = NULL);
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
	sCECS* cecs(void);
};



#endif
#endif