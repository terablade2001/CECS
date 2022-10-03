// MIT License

// Copyright (c) 2016-2020 Vasileios Kon. Pothos (terablade2001)
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
#include <cstdint>

// #define dbg_SHOW_INFO 2
#define dbg_SHOW_ID (uint64_t(0b0001) | (uint64_t(1)<<63))
#define info_(level,str) if (info_kVerboseLevel_ >= level) { std::cout<<str<<std::endl; }

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
#define CECS_MODULE(moduleName) static CECS __ECSOBJ__(moduleName);
#define CECS_MAIN_MODULE(moduleName, cecsName) static CECS __ECSOBJ__(moduleName, cecsName);
#define CECS_EXT_MODULE(moduleName, cecsPtr) static CECS __ECSOBJ__(moduleName,nullptr,cecsPtr);
#define CECS_EXT_MAIN_MODULE(moduleName, cecsName, cecsPtr) static CECS __ECSOBJ__(moduleName, cecsName,cecsPtr);
#define _ECSCLS_ { __ECSOBJ__.clear(); }
#define _ECSCLS(numberOfLatestRecords) { __ECSOBJ__.clear((numberOfLatestRecords)); }
#define _ECSFORMAT(display, errId, srcFile, srcLine, msg, module, cecsInfo, trackErrors) {\
	__ECSOBJ__.FormatReport((display),(errId),(srcFile),(srcLine),(msg),(module),(cecsInfo),(trackErrors));\
}
#define CECS_SETFUNC_LOCK(func) __ECSOBJ__.SetFunc_Lock((func))
#define CECS_SETFUNC_UNLOCK(func) __ECSOBJ__.SetFunc_Unlock((func))
#define CECS_MUTEXPTR __ECSOBJ__.getMutexPtr()
#define _NERR_ (__ECSOBJ__.GetNumberOfErrors())
#define _NERR(x) (__ECSOBJ__.GetNumberOfErrors(x))

#define __CECS_IRETURN__(ErrID) return (ErrID);
#define __CECS_RETURN__ return;
#define __CECS_RETURN_NULL_ return NULL;
#define __CECS_RETURN_BOOL_ return false;
#define __CECS_THROW__(Obj) (Obj).throwErrors();
#define _CECS_SS_ ss

#ifndef _MSC_VER
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
	#define CECS_ERRB(Obj, ExpR, args...) \
		if ((ExpR))  { (Obj).RecError(_CECS_DEFAULT_ERRID, _CECS_ERRTYPE_ERROR, __FNAME__, __LINE__, args); __CECS_RETURN_BOOL_ }
	#define CECS_ERRL(Obj, ExpR, args...) \
		if ((ExpR))  { (Obj).RecError(_CECS_DEFAULT_ERRID, _CECS_ERRTYPE_ERRLOG, __FNAME__, __LINE__, args); }
	#define CECS_ERRO(Obj, ExpR, __UserReturn__, args...) \
		if ((ExpR))  { (Obj).RecError(_CECS_DEFAULT_ERRID, _CECS_ERRTYPE_ERROR, __FNAME__, __LINE__, args); __UserReturn__ }
	#define CECS_WARN(Obj, ExpR, args...) \
		if ((ExpR))  { (Obj).RecError(_CECS_DEFAULT_WARNID, _CECS_ERRTYPE_WARNING, __FNAME__, __LINE__, args); }
	#define CECS_INFO(Obj, ExpR, args...) \
		if ((ExpR))  { (Obj).RecError(_CECS_DEFAULT_INFOID, _CECS_ERRTYPE_INFO, __FNAME__, __LINE__, args); }
	#define CECS_DEBUG(Obj, ExpR, args...) \
		if ((ExpR))  { (Obj).RecError(_CECS_DEFAULT_DEBUGID, _CECS_ERRTYPE_DEBUG, __FNAME__, __LINE__, args); }
	#define CECS_SIGDEBUG(Obj, ExpR, args...) \
		if ((ExpR))  { (Obj).RecError(_CECS_DEFAULT_DEBUGID, _CECS_ERRTYPE_SIGDBG, __FNAME__, __LINE__, args); }
	#define CECS_ERRINF(Obj, ExpR, args...) \
		if ((ExpR))  { (Obj).RecError(_CECS_DEFAULT_WARNID, _CECS_ERRTYPE_ERRINFO, __FNAME__, __LINE__, args); }
	#define CECS_ERRSTR(Obj, ExpR, __UserSS__) \
		if ((ExpR))  { std::stringstream _CECS_SS_; __UserSS__; \
			(Obj).RecError_NoList(_CECS_DEFAULT_WARNID, _CECS_ERRTYPE_ERRSTR, __FNAME__, __LINE__, _CECS_SS_.str().c_str(), _CECS_SS_.str().size()); }
	#define CECS_WARNO(Obj, ExpR, __UserReturn__, args...) \
		if ((ExpR))  { (Obj).RecError(_CECS_DEFAULT_WARNID, _CECS_ERRTYPE_WARNING, __FNAME__, __LINE__, args); __UserReturn__ }
	#define CECS_INFOO(Obj, ExpR, __UserReturn__, args...) \
		if ((ExpR))  { (Obj).RecError(_CECS_DEFAULT_INFOID, _CECS_ERRTYPE_INFO, __FNAME__, __LINE__, args); __UserReturn__ }
	#define CECS_DEBUGO(Obj, ExpR, __UserReturn__, args...) \
		if ((ExpR))  { (Obj).RecError(_CECS_DEFAULT_DEBUGID, _CECS_ERRTYPE_DEBUG, __FNAME__, __LINE__, args); __UserReturn__ }

	#define CECS_CHECKERRT(Obj) \
		CECS_ERRT((Obj), (Obj).GetNumberOfErrors() != 0, "CECS_CHECKERROR captured: Function 'throw' executed.")
	#define CECS_CHECKERR(Obj) \
		CECS_ERR((Obj), (Obj).GetNumberOfErrors() != 0, "CECS_CHECKERROR captured: Function 'return' executed.")
	#define CECS_CHECKERRI(Obj) \
		CECS_ERRI((Obj), (Obj).GetNumberOfErrors() != 0, "CECS_CHECKERROR captured: Function 'return  _CECS_DEFAULT_ERRID(=%i)' executed.",_CECS_DEFAULT_ERRID)
	#define CECS_CHECKERRN(Obj) \
		CECS_ERRN((Obj), (Obj).GetNumberOfErrors() != 0, "CECS_CHECKERROR captured: Function 'return NULL' executed.")
	#define CECS_CHECKERRB(Obj) \
		CECS_ERRB((Obj), (Obj).GetNumberOfErrors() != 0, "CECS_CHECKERROR captured: Function 'return false' executed.")
	#define CECS_CHECKERRL(Obj) \
		CECS_ERRL((Obj), (Obj).GetNumberOfErrors() != 0, "CECS_CHECKERROR captured: Error Logged.")
	#define CECS_CHECKERRO(Obj, __UserReturn__) \
		CECS_ERRO((Obj), (Obj).GetNumberOfErrors() != 0, __UserReturn__, "CECS_CHECKERROR captured: __UserReturn__ code executed!.")

	#ifdef CECSDEBUG
		#define _SIGDBG(args...) CECS_SIGDEBUG(__ECSOBJ__, 1, args)
		#define _ERRT(ExpR, args...) { _SIGDBG(args) CECS_ERRT(__ECSOBJ__, ExpR, args) }
		#define _ERR(ExpR, args...) { _SIGDBG(args) CECS_ERR(__ECSOBJ__, ExpR, args) }
		#define _ERRI(ExpR, args...) { _SIGDBG(args) CECS_ERRI(__ECSOBJ__, ExpR, args) }
		#define _ERRN(ExpR, args...) { _SIGDBG(args) CECS_ERRN(__ECSOBJ__, ExpR, args) }
		#define _ERRB(ExpR, args...) { _SIGDBG(args) CECS_ERRB(__ECSOBJ__, ExpR, args) }
		#define _ERRL(ExpR, args...) { _SIGDBG(args) CECS_ERRL(__ECSOBJ__, ExpR, args) }
		#define _ERRO(ExpR, __UserReturn__, args...) { _SIGDBG(args) CECS_ERRO(__ECSOBJ__, ExpR, __UserReturn__, args) }
		#define _WARN(ExpR, args...) { _SIGDBG(args) CECS_WARN(__ECSOBJ__, ExpR, args) }
		#define _INFO(ExpR, args...) { _SIGDBG(args) CECS_INFO(__ECSOBJ__, ExpR, args) }
		#define _DEBUG(ExpR, args...) { _SIGDBG(args) CECS_DEBUG(__ECSOBJ__, ExpR, args) }
		#define _ERRINF(ExpR, args...) { _SIGDBG(args) CECS_ERRINF(__ECSOBJ__, ExpR, args) }
		#define _ERRSTR(ExpR, __UserSS__) { _SIGDBG("_ERRSTR() call") CECS_ERRSTR(__ECSOBJ__, ExpR, __UserSS__) }
		#define _WARNO(ExpR, __UserReturn__, args...) { _SIGDBG(args) CECS_WARNO(__ECSOBJ__, ExpR, __UserReturn__, args) }
		#define _INFOO(ExpR, __UserReturn__, args...) { _SIGDBG(args) CECS_INFOO(__ECSOBJ__, ExpR, __UserReturn__, args) }
		#define _DEBUGO(ExpR, __UserReturn__, args...) { _SIGDBG(args) CECS_DEBUGO(__ECSOBJ__, ExpR, __UserReturn__, args) }
		#define _CHECKRT_ { _SIGDBG("CECS_CHECKERROR: _CHECKRT_") CECS_CHECKERRT(__ECSOBJ__) }
		#define _CHECKR_  { _SIGDBG("CECS_CHECKERROR: _CHECKR_")  CECS_CHECKERR(__ECSOBJ__) }
		#define _CHECKRI_ { _SIGDBG("CECS_CHECKERROR: _CHECKRI_") CECS_CHECKERRI(__ECSOBJ__) }
		#define _CHECKRN_ { _SIGDBG("CECS_CHECKERROR: _CHECKRN_") CECS_CHECKERRN(__ECSOBJ__) }
		#define _CHECKRB_ { _SIGDBG("CECS_CHECKERROR: _CHECKRB_") CECS_CHECKERRB(__ECSOBJ__) }
		#define _CHECKRL_ { _SIGDBG("CECS_CHECKERROR: _CHECKRL_") CECS_CHECKERRL(__ECSOBJ__) }
		#define _CHECKRO_(__UserReturn__) { _SIGDBG("CECS_CHECKERROR: _CHECKRO_")  CECS_CHECKERRO(__ECSOBJ__, __UserReturn__) }
		#define _SETSIGNAL(Signal) __ECSOBJ__.SetSignal(Signal);
		#define _CERRT(args...) { _SIGDBG(args) _ERRT(0!=_NERR_, args) }
		#define _CERR(args...)  { _SIGDBG(args) _ERR (0!=_NERR_, args) }
		#define _CERRI(args...) { _SIGDBG(args) _ERRI(0!=_NERR_, args) }
		#define _CERRN(args...) { _SIGDBG(args) _ERRN(0!=_NERR_, args) }
		#define _CERRB(args...) { _SIGDBG(args) _ERRB(0!=_NERR_, args) }
		#define _CERRL(args...) { _SIGDBG(args) _ERRL(0!=_NERR_, args) }
		#define _CERRO(__UserReturn__, args...) { _SIGDBG(args) _ERRO(0!=_NERR_, __UserReturn__, args) }
	#else
		#define _SIGDBG(args...) { /* dummy */ }
		#define _ERRT(ExpR, args...) CECS_ERRT(__ECSOBJ__, ExpR, args)
		#define _ERR(ExpR, args...) CECS_ERR(__ECSOBJ__, ExpR, args)
		#define _ERRI(ExpR, args...) CECS_ERRI(__ECSOBJ__, ExpR, args)
		#define _ERRN(ExpR, args...) CECS_ERRN(__ECSOBJ__, ExpR, args)
		#define _ERRB(ExpR, args...) CECS_ERRB(__ECSOBJ__, ExpR, args)
		#define _ERRL(ExpR, args...) CECS_ERRL(__ECSOBJ__, ExpR, args)
		#define _ERRO(ExpR, __UserReturn__, args...) CECS_ERRO(__ECSOBJ__, ExpR, __UserReturn__, args)
		#define _WARN(ExpR, args...) CECS_WARN(__ECSOBJ__, ExpR, args)
		#define _INFO(ExpR, args...) CECS_INFO(__ECSOBJ__, ExpR, args)
		#define _ERRINF(ExpR, args...) CECS_ERRINF(__ECSOBJ__, ExpR, args)
		#define _ERRSTR(ExpR, __UserSS__) CECS_ERRSTR(__ECSOBJ__, ExpR, __UserSS__)
		#define _WARNO(ExpR, __UserReturn__, args...) CECS_WARNO(__ECSOBJ__, ExpR, __UserReturn__, args)
		#define _INFOO(ExpR, __UserReturn__, args...) CECS_INFOO(__ECSOBJ__, ExpR, __UserReturn__, args)
		#define _DEBUGO(ExpR, __UserReturn__, args...) CECS_DEBUGO(__ECSOBJ__, ExpR, __UserReturn__, args)
		#define _CHECKRT_ CECS_CHECKERRT(__ECSOBJ__)
		#define _CHECKR_ CECS_CHECKERR(__ECSOBJ__)
		#define _CHECKRI_ CECS_CHECKERRI(__ECSOBJ__)
		#define _CHECKRN_ CECS_CHECKERRN(__ECSOBJ__)
		#define _CHECKRB_ CECS_CHECKERRB(__ECSOBJ__)
		#define _CHECKRL_ CECS_CHECKERRL(__ECSOBJ__)
		#define _CHECKRO_(__UserReturn__) CECS_CHECKERRO(__ECSOBJ__, __UserReturn__)
		#define _SETSIGNAL(Signal) { /* dummy */ }
		#define _CERRT(args...) _ERRT(0!=_NERR_, args)
		#define _CERR(args...) _ERR (0!=_NERR_, args)
		#define _CERRI(args...) _ERRI(0!=_NERR_, args)
		#define _CERRN(args...) _ERRN(0!=_NERR_, args)
		#define _CERRB(args...) _ERRB(0!=_NERR_, args)
		#define _CERRL(args...) _ERRL(0!=_NERR_, args)
		#define _CERRO(__UserReturn__, args...) _ERRO(0!=_NERR_, __UserReturn__, args)
	#endif
#endif
#ifdef _MSC_VER
	// Using client error-id
	#define CECS_IERR(Obj, ExpR, ErrID, ...) \
		if ((ExpR))  { (Obj).RecError((ErrID), _CECS_ERRTYPE_ERROR, __FNAME__, __LINE__, __VA_ARGS__); __CECS_IRETURN__(ErrID) }
	#define CECS_IWARN(Obj, ExpR, ErrID, ...) \
		if ((ExpR))  { (Obj).RecError((ErrID), _CECS_ERRTYPE_WARNING, __FNAME__, __LINE__, __VA_ARGS__); __CECS_IRETURN__(ErrID) }

	// Using internal error-id
	#define CECS_ERRT(Obj, ExpR, ...) \
		if ((ExpR))  { (Obj).RecError(_CECS_DEFAULT_ERRID, _CECS_ERRTYPE_ERROR, __FNAME__, __LINE__, __VA_ARGS__); __CECS_THROW__(Obj) }
	#define CECS_ERR(Obj, ExpR, ...) \
		if ((ExpR))  { (Obj).RecError(_CECS_DEFAULT_ERRID, _CECS_ERRTYPE_ERROR, __FNAME__, __LINE__, __VA_ARGS__); __CECS_RETURN__ }
	#define CECS_ERRI(Obj, ExpR, ...) \
		if ((ExpR))  { (Obj).RecError(_CECS_DEFAULT_ERRID, _CECS_ERRTYPE_ERROR, __FNAME__, __LINE__, __VA_ARGS__); __CECS_IRETURN__(_CECS_DEFAULT_ERRID) }
	#define CECS_ERRN(Obj, ExpR, ...) \
		if ((ExpR))  { (Obj).RecError(_CECS_DEFAULT_ERRID, _CECS_ERRTYPE_ERROR, __FNAME__, __LINE__, __VA_ARGS__); __CECS_RETURN_NULL_ }
	#define CECS_ERRB(Obj, ExpR, ...) \
		if ((ExpR))  { (Obj).RecError(_CECS_DEFAULT_ERRID, _CECS_ERRTYPE_ERROR, __FNAME__, __LINE__, __VA_ARGS__); __CECS_RETURN_BOOL_ }
	#define CECS_ERRL(Obj, ExpR, ...) \
		if ((ExpR))  { (Obj).RecError(_CECS_DEFAULT_ERRID, _CECS_ERRTYPE_ERRLOG, __FNAME__, __LINE__, __VA_ARGS__); }
	#define CECS_ERRO(Obj, ExpR, __UserReturn__, ...) \
		if ((ExpR))  { (Obj).RecError(_CECS_DEFAULT_ERRID, _CECS_ERRTYPE_ERROR, __FNAME__, __LINE__, __VA_ARGS__); __UserReturn__ }
	#define CECS_WARN(Obj, ExpR, ...) \
		if ((ExpR))  { (Obj).RecError(_CECS_DEFAULT_WARNID, _CECS_ERRTYPE_WARNING, __FNAME__, __LINE__, __VA_ARGS__); }
	#define CECS_INFO(Obj, ExpR, ...) \
		if ((ExpR))  { (Obj).RecError(_CECS_DEFAULT_INFOID, _CECS_ERRTYPE_INFO, __FNAME__, __LINE__, __VA_ARGS__); }
	#define CECS_DEBUG(Obj, ExpR, ...) \
		if ((ExpR))  { (Obj).RecError(_CECS_DEFAULT_DEBUGID, _CECS_ERRTYPE_DEBUG, __FNAME__, __LINE__, __VA_ARGS__); }
	#define CECS_SIGDEBUG(Obj, ExpR, ...) \
		if ((ExpR))  { (Obj).RecError(_CECS_DEFAULT_DEBUGID, _CECS_ERRTYPE_SIGDBG, __FNAME__, __LINE__, __VA_ARGS__); }
	#define CECS_ERRINF(Obj, ExpR, ...) \
		if ((ExpR))  { (Obj).RecError(_CECS_DEFAULT_WARNID, _CECS_ERRTYPE_ERRINFO, __FNAME__, __LINE__, __VA_ARGS__); }
	#define CECS_ERRSTR(Obj, ExpR, __UserSS__) \
		if ((ExpR))  { std::stringstream _CECS_SS_; __UserSS__; \
			(Obj).RecError_NoList(_CECS_DEFAULT_WARNID, _CECS_ERRTYPE_ERRSTR, __FNAME__, __LINE__, _CECS_SS_.str().c_str(), _CECS_SS_.str().size()); }
	#define CECS_WARNO(Obj, ExpR, __UserReturn__, ...) \
		if ((ExpR))  { (Obj).RecError(_CECS_DEFAULT_WARNID, _CECS_ERRTYPE_WARNING, __FNAME__, __LINE__, __VA_ARGS__); __UserReturn__ }
	#define CECS_INFOO(Obj, ExpR, __UserReturn__, ...) \
		if ((ExpR))  { (Obj).RecError(_CECS_DEFAULT_INFOID, _CECS_ERRTYPE_INFO, __FNAME__, __LINE__, __VA_ARGS__); __UserReturn__ }
	#define CECS_DEBUGO(Obj, ExpR, __UserReturn__, ...) \
		if ((ExpR))  { (Obj).RecError(_CECS_DEFAULT_DEBUGID, _CECS_ERRTYPE_DEBUG, __FNAME__, __LINE__, __VA_ARGS__); __UserReturn__ }

	#define CECS_CHECKERRT(Obj) \
		CECS_ERRT((Obj), (Obj).GetNumberOfErrors() != 0, "CECS_CHECKERROR captured: Function 'throw' executed.")
	#define CECS_CHECKERR(Obj) \
		CECS_ERR((Obj), (Obj).GetNumberOfErrors() != 0, "CECS_CHECKERROR captured: Function 'return' executed.")
	#define CECS_CHECKERRI(Obj) \
		CECS_ERRI((Obj), (Obj).GetNumberOfErrors() != 0, "CECS_CHECKERROR captured: Function 'return  _CECS_DEFAULT_ERRID(=%i)' executed.",_CECS_DEFAULT_ERRID)
	#define CECS_CHECKERRN(Obj) \
		CECS_ERRN((Obj), (Obj).GetNumberOfErrors() != 0, "CECS_CHECKERROR captured: Function 'return NULL' executed.")
	#define CECS_CHECKERRB(Obj) \
		CECS_ERRB((Obj), (Obj).GetNumberOfErrors() != 0, "CECS_CHECKERROR captured: Function 'return false' executed.")
	#define CECS_CHECKERRL(Obj) \
		CECS_ERRL((Obj), (Obj).GetNumberOfErrors() != 0, "CECS_CHECKERROR captured: Error Logged.")
	#define CECS_CHECKERRO(Obj, __UserReturn__) \
		CECS_ERRO((Obj), (Obj).GetNumberOfErrors() != 0, __UserReturn__, "CECS_CHECKERROR captured: __UserReturn__ code executed!.")

	#ifdef CECSDEBUG
		#define _SIGDBG(...) CECS_SIGDEBUG(__ECSOBJ__, 1, __VA_ARGS__)
		#define _ERRT(ExpR, ...) { _SIGDBG(__VA_ARGS__) CECS_ERRT(__ECSOBJ__, ExpR, __VA_ARGS__) }
		#define _ERR(ExpR, ...) { _SIGDBG(__VA_ARGS__) CECS_ERR(__ECSOBJ__, ExpR, __VA_ARGS__) }
		#define _ERRI(ExpR, ...) { _SIGDBG(__VA_ARGS__) CECS_ERRI(__ECSOBJ__, ExpR, __VA_ARGS__) }
		#define _ERRN(ExpR, ...) { _SIGDBG(__VA_ARGS__) CECS_ERRN(__ECSOBJ__, ExpR, __VA_ARGS__) }
		#define _ERRB(ExpR, ...) { _SIGDBG(__VA_ARGS__) CECS_ERRB(__ECSOBJ__, ExpR, __VA_ARGS__) }
		#define _ERRL(ExpR, ...) { _SIGDBG(__VA_ARGS__) CECS_ERRL(__ECSOBJ__, ExpR, __VA_ARGS__) }
		#define _ERRO(ExpR, __UserReturn__, ...) { _SIGDBG(__VA_ARGS__) CECS_ERRO(__ECSOBJ__, ExpR, __UserReturn__, __VA_ARGS__) }
		#define _WARN(ExpR, ...) { _SIGDBG(__VA_ARGS__) CECS_WARN(__ECSOBJ__, ExpR, __VA_ARGS__) }
		#define _INFO(ExpR, ...) { _SIGDBG(__VA_ARGS__) CECS_INFO(__ECSOBJ__, ExpR, __VA_ARGS__) }
		#define _DEBUG(ExpR, ...) { _SIGDBG(__VA_ARGS__) CECS_DEBUG(__ECSOBJ__, ExpR, __VA_ARGS__) }
		#define _ERRINF(ExpR, ...) { _SIGDBG(__VA_ARGS__) CECS_ERRINF(__ECSOBJ__, ExpR, __VA_ARGS__) }
		#define _ERRSTR(ExpR, __UserSS__) { _SIGDBG("_ERRSTR() call") CECS_ERRSTR(__ECSOBJ__, ExpR, __UserSS__) }
		#define _WARNO(ExpR, __UserReturn__, ...) { _SIGDBG(__VA_ARGS__) CECS_WARNO(__ECSOBJ__, ExpR, __UserReturn__, __VA_ARGS__) }
		#define _INFOO(ExpR, __UserReturn__, ...) { _SIGDBG(__VA_ARGS__) CECS_INFOO(__ECSOBJ__, ExpR, __UserReturn__, __VA_ARGS__) }
		#define _DEBUGO(ExpR, __UserReturn__, ...) { _SIGDBG(__VA_ARGS__) CECS_DEBUGO(__ECSOBJ__, ExpR, __UserReturn__, __VA_ARGS__) }
		#define _CHECKRT_ { _SIGDBG("CECS_CHECKERROR: _CHECKRT_") CECS_CHECKERRT(__ECSOBJ__) }
		#define _CHECKR_  { _SIGDBG("CECS_CHECKERROR: _CHECKR_") CECS_CHECKERR(__ECSOBJ__) }
		#define _CHECKRI_ { _SIGDBG("CECS_CHECKERROR: _CHECKRI_") CECS_CHECKERRI(__ECSOBJ__) }
		#define _CHECKRN_ { _SIGDBG("CECS_CHECKERROR: _CHECKRN_") CECS_CHECKERRN(__ECSOBJ__) }
		#define _CHECKRB_ { _SIGDBG("CECS_CHECKERROR: _CHECKRB_") CECS_CHECKERRB(__ECSOBJ__) }
		#define _CHECKRL_ { _SIGDBG("CECS_CHECKERROR: _CHECKRL_") CECS_CHECKERRL(__ECSOBJ__) }
		#define _CHECKRO_(__UserReturn__) { _SIGDBG("CECS_CHECKERROR: _CHECKRO_")  CECS_CHECKERRO(__ECSOBJ__, __UserReturn__) }
		#define _SETSIGNAL(Signal) __ECSOBJ__.SetSignal(Signal);
		#define _CERRT(...) { _SIGDBG(__VA_ARGS__) _ERRT(0!=_NERR_, __VA_ARGS__) }
		#define _CERR(...)  { _SIGDBG(__VA_ARGS__) _ERR (0!=_NERR_, __VA_ARGS__) }
		#define _CERRI(...) { _SIGDBG(__VA_ARGS__) _ERRI(0!=_NERR_, __VA_ARGS__) }
		#define _CERRN(...) { _SIGDBG(__VA_ARGS__) _ERRN(0!=_NERR_, __VA_ARGS__) }
		#define _CERRB(...) { _SIGDBG(__VA_ARGS__) _ERRB(0!=_NERR_, __VA_ARGS__) }
		#define _CERRL(...) { _SIGDBG(__VA_ARGS__) _ERRL(0!=_NERR_, __VA_ARGS__) }
		#define _CERRO(__UserReturn__, ...) { _SIGDBG(__VA_ARGS__) _ERRO(0!=_NERR_, __UserReturn__, __VA_ARGS__) }
	#else
		#define _SIGDBG(...) { /* dummy */ }
		#define _ERRT(ExpR, ...) CECS_ERRT(__ECSOBJ__, ExpR, __VA_ARGS__)
		#define _ERR(ExpR, ...) CECS_ERR(__ECSOBJ__, ExpR, __VA_ARGS__)
		#define _ERRI(ExpR, ...) CECS_ERRI(__ECSOBJ__, ExpR, __VA_ARGS__)
		#define _ERRN(ExpR, ...) CECS_ERRN(__ECSOBJ__, ExpR, __VA_ARGS__)
		#define _ERRB(ExpR, ...) CECS_ERRB(__ECSOBJ__, ExpR, __VA_ARGS__)
		#define _ERRL(ExpR, ...) CECS_ERRL(__ECSOBJ__, ExpR, __VA_ARGS__)
		#define _ERRO(ExpR, __UserReturn__, ...) CECS_ERRO(__ECSOBJ__, ExpR, __UserReturn__, __VA_ARGS__)
		#define _WARN(ExpR, ...) CECS_WARN(__ECSOBJ__, ExpR, __VA_ARGS__)
		#define _INFO(ExpR, ...) CECS_INFO(__ECSOBJ__, ExpR, __VA_ARGS__)
		#define _ERRINF(ExpR, ...) CECS_ERRINF(__ECSOBJ__, ExpR, __VA_ARGS__)
		#define _ERRSTR(ExpR, __UserSS__) CECS_ERRSTR(__ECSOBJ__, ExpR, __UserSS__)
		#define _WARNO(ExpR, __UserReturn__, ...) CECS_WARNO(__ECSOBJ__, ExpR, __UserReturn__, __VA_ARGS__)
		#define _INFOO(ExpR, __UserReturn__, ...) CECS_INFOO(__ECSOBJ__, ExpR, __UserReturn__, __VA_ARGS__)
		#define _DEBUGO(ExpR, __UserReturn__, ...) CECS_DEBUGO(__ECSOBJ__, ExpR, __UserReturn__, __VA_ARGS__)
		#define _CHECKRT_ CECS_CHECKERRT(__ECSOBJ__)
		#define _CHECKR_ CECS_CHECKERR(__ECSOBJ__)
		#define _CHECKRI_ CECS_CHECKERRI(__ECSOBJ__)
		#define _CHECKRN_ CECS_CHECKERRN(__ECSOBJ__)
		#define _CHECKRB_ CECS_CHECKERRB(__ECSOBJ__)
		#define _CHECKRL_ CECS_CHECKERRL(__ECSOBJ__)
		#define _CHECKRO_(__UserReturn__) CECS_CHECKERRO(__ECSOBJ__, __UserReturn__)
		#define _SETSIGNAL(Signal) { /* dummy */ }
		#define _CERRT(...) _ERRT(0!=_NERR_, __VA_ARGS__)
		#define _CERR(...) _ERR (0!=_NERR_, __VA_ARGS__)
		#define _CERRI(...) _ERRI(0!=_NERR_, __VA_ARGS__)
		#define _CERRN(...) _ERRN(0!=_NERR_, __VA_ARGS__)
		#define _CERRB(...) _ERRB(0!=_NERR_, __VA_ARGS__)
		#define _CERRL(...) _ERRL(0!=_NERR_, __VA_ARGS__)
		#define _CERRO(__UserReturn__, ...) _ERRO(0!=_NERR_, __UserReturn__, __VA_ARGS__)
	#endif

#endif


#ifndef dbg_SHOW_INFO
	#define dbg_SHOW_INFO 0
#endif
#if (dbg_SHOW_INFO==2)
  #define dbg_(id, str) if (((uint64_t(1) << id) & dbg_SHOW_ID) != 0) cout<<"<"<<id<<", "<<__FNAME__<<", L-"<<__LINE__<<">: "<<str<<endl;
  #define dbg_c(id, expr, str) if (((uint64_t(1) << id) & dbg_SHOW_ID) != 0) { if(expr) cout<<"<"<<id<<", ["<<#expr<<"], "<<__FNAME__<<", L-"<<__LINE__<<">: "<<str<<endl; }
  #define dbg__(id, str, info) if (((uint64_t(1) << id) & dbg_SHOW_ID) != 0) { if(info != 0) cout<<"<"<<id<<", "<<__FNAME__<<", L-"<<__LINE__<<">: "; cout<<str; }
#elif (dbg_SHOW_INFO==1)
  #define dbg_(id, str) if (((uint64_t(1) << id) & dbg_SHOW_ID) != 0) cout<<str<<endl;
  #define dbg_c(id, expr, str) if (((uint64_t(1) << id) & dbg_SHOW_ID) != 0) { if (expr) cout<<"["<<#expr<<"]: "<<str<<endl; }
  #define dbg__(id, str, info) if (((uint64_t(1) << id) & dbg_SHOW_ID) != 0) cout<<str;
#else
  #define dbg_(id, str) { /* */ }
  #define dbg_c(id, expr, str) { /* */ }
  #define dbg__(id, str, info) { /* */ }
#endif


class CECS {
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
	void RecError_NoList(
		int errid,
		int type,
		const char* fname,
		const unsigned int line,
		const char* msg,
		const unsigned int msgSize
	);
	const char* str(void);
	const char* str(int typeId);
	const char* name(void);
	const char* modname(void);
	void throwErrors(int type=_CECS_ERRTYPE_ALL);
	void clear(int numberOfLatestRecords = -1);
	int GetNumberOfErrors(int type=_CECS_ERRTYPE_ERROR);
	void* cecs(void);
	void SetSignal(int signalId);
	void FormatReport(
		bool display=true,
		bool errId=false,
		bool srcFile=true,
		bool srcLine=true,
		bool msg=true,
		bool module=true,
		bool cecsInfo=true,
		bool trackErrors=true
	);
	void SetFunc_Lock(void (*func)(void));
	void SetFunc_Unlock(void (*func)(void));
	void** getMutexPtr(void);
protected:
	char* EcsName;
	char* ModName;
	sCECS* pCECS;
};



#endif
#endif
