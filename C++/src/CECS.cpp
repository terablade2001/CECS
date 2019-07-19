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

CECS::CECS(const char* modName, const char* ecsName, sCECS* cecs) {
	Initialize(const_cast<char*>(ecsName), const_cast<char*>(modName), cecs);
}
CECS::CECS(char* modName, char* ecsName, sCECS* cecs) {
	Initialize(ecsName, modName, cecs);
}

void CECS::Initialize(char* ecsName, char* modName, sCECS* cecs) {
	Shutdown();

	EcsName = (char*)malloc(CECS__ECSNAMELENGTH * sizeof(char));
	if (ecsName != NULL) snprintf(EcsName, CECS__ECSNAMELENGTH, "%s", ecsName);
	else snprintf(EcsName, CECS__ECSNAMELENGTH, "CECS-Default");

	ModName = (char*)malloc(CECS__MODNAMELENGTH * sizeof(char));
	if (modName != NULL) snprintf(ModName, CECS__MODNAMELENGTH, "%s", modName);
	else snprintf(ModName, CECS__MODNAMELENGTH, "ModDefault");
	const int replaceName = (ecsName != NULL) ? 1 : 0;

	pCECS = CECS_Initialize(EcsName, cecs, replaceName);
}

void CECS::ReInitIfDead(void) {
	// Class has been initialized, but CECS global object has been shutdown..
	if ((pCECS != NULL) && (CECS_CheckIfInitNoMsg() != 0)) {
		Initialize(EcsName, ModName, NULL); // Then Initialize and grab mainObj.
	}
}

void CECS::Shutdown(void) {
	if (pCECS != NULL) { CECS_Shutdown(pCECS); pCECS = NULL; }
	if (EcsName != NULL) { free(EcsName); EcsName = NULL; }
	if (ModName != NULL) { free(ModName); ModName = NULL; }
	lastErrorMsg.clear();
}

void CECS::RecError(
	int errid,
	int type,
	const char* fname,
	const unsigned int line,
	const char* msg,
	...
) {
	static char vaStr[CECS__FERRORL]={0};
	va_list(vargs);
	va_start(vargs, msg);
	vsnprintf(vaStr, CECS__FERRORL, msg, vargs);
	va_end(vargs);

	CECS_RecErrorMod(
		const_cast<char*>(ModName),
		errid, type, fname, line, vaStr
	);
}


const char* CECS::str(void) {
	lastErrorMsg.clear();
	lastErrorMsg.append(CECS_str(_CECS_ERRTYPE_ALL));
	return lastErrorMsg.c_str();
}

const char* CECS::str(int typeId) {
	lastErrorMsg.clear();
	lastErrorMsg.append(CECS_str(typeId));
	return lastErrorMsg.c_str();
}

const char* CECS::name(void) {
	return CECS_getName();
}

const char* CECS::modname(void) {
	return ModName;
}

void CECS::throwErrors(void) {
	std::stringstream ErrStr;
	const char* ErrString = str();
	if (ErrString != NULL) {
		ErrStr << std::string(ErrString) << endl;
		throw std::runtime_error(ErrStr.str().c_str());
	}
}

void CECS::clearLastErrorMsg(void) {
	lastErrorMsg.clear();
}

void CECS::clear(void) {
	CECS_clear();
	clearLastErrorMsg();
}

const char* CECS::getLastErrorMsg(void) {
	return lastErrorMsg.c_str();
}

int CECS::GetNumberOfErrors(void){
	return CECS_GetNumberOfErrorsByType(_CECS_ERRTYPE_ERROR);
}

sCECS* CECS::cecs(void) {
	return pCECS;
}