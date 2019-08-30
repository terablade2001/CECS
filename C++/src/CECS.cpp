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

#include "include/CECS.hpp"

using namespace std;

CECSBase::CECSBase():
	EcsName(NULL),
	ModName(NULL),
	pCECS(NULL)
{ }

CECS::CECS() { Initialize(NULL, NULL, NULL); }
CECS::~CECS() { Shutdown(); }

CECS::CECS(const char* modName, const char* ecsName, void* cecs) {
	Initialize(const_cast<char*>(ecsName), const_cast<char*>(modName), cecs);
}
CECS::CECS(char* modName, char* ecsName, void* cecs) {
	Initialize(ecsName, modName, cecs);
}

void CECS::Initialize(char* ecsName, char* modName, void* cecs) {
	if (pCECS != NULL) return;

	EcsName = (char*)malloc(CECS__ECSNAMELENGTH * sizeof(char));
	if (ecsName != NULL) snprintf(EcsName, CECS__ECSNAMELENGTH, "%s", ecsName);
	else snprintf(EcsName, CECS__ECSNAMELENGTH, "CECS-Default");

	ModName = (char*)malloc(CECS__MODNAMELENGTH * sizeof(char));
	if (modName != NULL) snprintf(ModName, CECS__MODNAMELENGTH, "%s", modName);
	else snprintf(ModName, CECS__MODNAMELENGTH, "ModDefault");
	const int replaceName = (ecsName != NULL) ? 1 : 0;

	pCECS = CECS_Initialize(EcsName, (sCECS*)cecs, replaceName);
}

void CECS::ConnectTo(void* cecs) {
	if (cecs == NULL) return;
	pCECS = CECS_Initialize(EcsName, (sCECS*)cecs, 0);
}

void CECS::ReInitIfDead(void) {
	// Class has been initialized, but CECS global object has been shutdown..
	if ((pCECS != NULL) && (CECS_CheckIfInitNoMsg(pCECS) != 0)) {
		Initialize(EcsName, ModName, NULL); // Then Initialize and grab mainObj.
	}
}

void CECS::Shutdown(void) {
	if (pCECS != NULL) { CECS_Shutdown(pCECS); pCECS = NULL; }
	if (EcsName != NULL) { free(EcsName); EcsName = NULL; }
	if (ModName != NULL) { free(ModName); ModName = NULL; }
}

void CECS::RecError(
	int errid,
	int type,
	const char* fname,
	const unsigned int line,
	const char* msg,
	...
) {
	#ifdef ENABLE_PTHREAD_SUPPORT
		char vaStr[CECS__FERRORL]={0};
	#else
		static char vaStr[CECS__FERRORL]={0};
	#endif
	va_list(vargs);
	va_start(vargs, msg);
	vsnprintf(vaStr, CECS__FERRORL, msg, vargs);
	va_end(vargs);

	CECS_RecErrorMod(pCECS,
		const_cast<char*>(ModName),
		errid, type, fname, line, const_cast<char*>(vaStr)
	);
}


const char* CECS::str(void) {
	return CECS_str(pCECS, _CECS_ERRTYPE_ALL);
}

const char* CECS::str(int typeId) {
	return CECS_str(pCECS, typeId);
}

const char* CECS::name(void) {
	return CECS_getName(pCECS);
}

const char* CECS::modname(void) {
	return ModName;
}

void CECS::throwErrors(int type) {
	std::stringstream ErrStr;
	const char* ErrString = str(type);
	if (ErrString != NULL) {
		ErrStr << std::string(ErrString) << endl;
		throw std::runtime_error(ErrStr.str().c_str());
	}
}

void CECS::clear(void) {
	CECS_clear(pCECS);
}

int CECS::GetNumberOfErrors(int type){
	return CECS_GetNumberOfErrorsByType(pCECS, type);
}

void* CECS::cecs(void) {
	if (pCECS == NULL) Initialize(NULL, NULL, NULL);
	return (void*)pCECS;
}

void CECS::SetSignal(int signalId) {
	CECS_HandleSignal(signalId, pCECS);
}