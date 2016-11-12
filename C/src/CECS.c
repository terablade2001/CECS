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

#include "include/CECS.h"

static sCECS* pCECS = NULL;

static sCECS CECS = {
	.Name = NULL,
	.SetupFlag = 0xFF,
	.NErrors = 0,
	.SErrors = NULL,
	.IErrors = NULL,
	.TErrors = NULL,
	.FErrors = NULL,
	.LErrors = NULL,
	.SErrIDs = NULL,
	.DispStr = NULL,
	.MErrors = NULL,
	.ErrorLength = CECS__FERRORL,
	.MaxErrors = CECS__MAXERRORS,
	.MaxDisplayStringSize = CECS__MAXDISPSTRSIZE,
	.RefCounter = 0
};


sCECS* CECS_Initialize(const char* name, sCECS* pcecs) {
	int i = 0;
	int MaxErrors = 0;

	if (pcecs != NULL) { // Initialize using an external CECS object.
		if ((pCECS != NULL) && (pCECS != pcecs)) CECS_Shutdown(pCECS);
		pCECS = pcecs;
	} else { // Initialize using the internal CECS object.
		pCECS = &CECS;
	}

/*	C Version: Any call to Shutdown should be set RefCounter to 0, and thus 
clean up the memory. For this reason RefCounter should take values 0 and 1. 
In case value 1 is taken, then no reinitialization is need. This is due to 
the fact that there are no destructors in C, thus client should have to call 
Shutdown for any Initialize() (Like in C++ version). However this make it 
more complex for the client. In C Version client should use Shutdown only when
CECS-Internal is not going to be used anymore, where in C++ Version Shutdown 
is called for every Initialize call.
	C++ Version: As each CECS class destructor calls Shutdown(), the 
RefCounter must be increased by the number of times a Constructor initialize 
the CECS-Internal. In this way, when all classes that use CECS-Internal are 
destructed then the CECS-Internal is free.
*/
	#ifndef __cplusplus
		if (pCECS->RefCounter == 1) return pCECS;
		pCECS->RefCounter++;
	#else
		pCECS->RefCounter++;
		if (pCECS->RefCounter > 1) return pCECS;
	#endif

	MaxErrors = pCECS->MaxErrors;

	//@#| ############### Allocate Memory ###############

	if (pCECS->SErrors == NULL) {
		pCECS->NErrors = 0;
		pCECS->SErrors = (char**) malloc(sizeof(char*) * MaxErrors);
		for (i = 0; i < MaxErrors; i++) {
			pCECS->SErrors[i] = (char*) calloc(pCECS->ErrorLength, sizeof(char));
		}
	}

	if (pCECS->Name == NULL) {
		pCECS->Name = (char*) calloc(CECS__ECSNAMELENGTH, sizeof(char));
		if (name != NULL) snprintf(pCECS->Name, CECS__ECSNAMELENGTH, name);
		else snprintf(pCECS->Name, CECS__ECSNAMELENGTH, "CECS-UnNamed");
	}

	if (pCECS->IErrors == NULL)
		pCECS->IErrors = (int*) calloc(MaxErrors, sizeof(int));
	
	if (pCECS->TErrors == NULL)
		pCECS->TErrors = (int*) calloc(MaxErrors, sizeof(int));

	if(pCECS->FErrors == NULL) {
		pCECS->FErrors = (char**)malloc(sizeof(char*) * MaxErrors);
		for (int i = 0; i < MaxErrors; i++)
			pCECS->FErrors[i] = (char*)calloc(CECS__FERRORL, sizeof(char));
	}

	if (pCECS->LErrors == NULL)
		pCECS->LErrors = (unsigned int*) calloc(MaxErrors, sizeof(unsigned int));

	if (pCECS->SErrIDs == NULL)
		pCECS->SErrIDs = (int*) calloc(MaxErrors, sizeof(int));

	if (pCECS->DispStr == NULL)
		pCECS->DispStr = (char*) calloc(pCECS->MaxDisplayStringSize, sizeof(char));

	if(pCECS->MErrors == NULL) {
		pCECS->MErrors = (char**)malloc(sizeof(char*) * MaxErrors);
		for (int i = 0; i < MaxErrors; i++)
			pCECS->MErrors[i] = (char*)calloc(CECS__MODNAMELENGTH, sizeof(char));
	}

	return pCECS;
}



sCECS* CECS_Shutdown(sCECS* pcecs) {
	int i = 0;
	int MaxErrors = 0;
	if (pcecs == NULL) pcecs = pCECS;
	
	if (pcecs->RefCounter > 0) pcecs->RefCounter--;

	if (pcecs->RefCounter > 0) return pCECS; 

	pcecs->RefCounter = 0;

	MaxErrors = pcecs->MaxErrors;

	// If the CECS is not initialized yet, then return.
	if (pcecs->SErrors == NULL) return pCECS;

	if (pcecs->Name != NULL) free(pcecs->Name);

	if (pcecs->SErrors != NULL) {
		for (i = 0; i < MaxErrors; i++)
			free(pcecs->SErrors[i]);
		free(pcecs->SErrors);
	}

	if (pcecs->IErrors != NULL) free(pcecs->IErrors);
	if (pcecs->TErrors != NULL) free(pcecs->TErrors);

	if (pcecs->FErrors != NULL) {
		for (i = 0; i < MaxErrors; i++)
			free(pcecs->FErrors[i]);
		free(pcecs->FErrors);
	}

	if (pcecs->LErrors != NULL) free(pcecs->LErrors);
	if (pcecs->SErrIDs != NULL) free(pcecs->SErrIDs);
	if (pcecs->DispStr != NULL) free(pcecs->DispStr);

	if (pcecs->MErrors != NULL) {
		for (i = 0; i < MaxErrors; i++)
			free(pcecs->MErrors[i]);
		free(pcecs->MErrors);
	}

	pcecs->Name    = NULL;
	pcecs->NErrors = 0;
	pcecs->SErrors = NULL;
	pcecs->IErrors = NULL;
	pcecs->TErrors = NULL;
	pcecs->FErrors = NULL;
	pcecs->LErrors = NULL;
	pcecs->SErrIDs = NULL;
	pcecs->DispStr = NULL;
	pcecs->MErrors = NULL;

	return pCECS;
}

int CECS_CheckIfInitNoMsg(void) {
	int isInit = 0;
	if (pCECS == NULL) { pCECS = &CECS; isInit = -1; }
	if (pCECS->SErrors == NULL) { CECS_Initialize(NULL, pCECS); isInit = -1; }
	return isInit;
}

sCECS* CECS_getCecs(void) { return pCECS; }

int CECS_CheckIfInit(const char* msg) {
	int isInit = 0;

	if (pCECS == NULL) { pCECS = &CECS; isInit = -1; }
	if (pCECS->SErrors == NULL) { CECS_Initialize(NULL, pCECS); isInit = -1; }
	if (isInit == -1) {
		CECS_RecError(
			CECS__ERRORID, 0, __FNAME__, __LINE__,
			"%s: CECS was Not Initialized!", msg
		);
	}
	return isInit;
}

sCECS* CECS_RecErrorMod(
	const char* modName,
	int errid,
	int type,
	const char* fname,
	const unsigned int line,
	const char* msg,
	...
) {
	int idx = 0;
	unsigned char FS = 0;
	CECS_CheckIfInit("CECS_RecError/Mod()");
	if (pCECS->NErrors < pCECS->MaxErrors-1) {
		idx = pCECS->NErrors++;
		FS = pCECS->SetupFlag;

		if (FS & 0x01)
			pCECS->IErrors[idx] = errid;

		if (FS & 0x02)
			pCECS->TErrors[idx] = type;

		if (FS & 0x04)
			strncpy(pCECS->FErrors[idx], fname, CECS__FERRORL);

		if (FS & 0x08)
			pCECS->LErrors[idx] = line;

		if (FS & 0x10) {
			va_list(vargs);
			va_start(vargs, msg);
			vsnprintf(pCECS->SErrors[idx], pCECS->ErrorLength, msg, vargs);
			va_end(vargs);
		}
		
		if (FS & 0x20)
			strncpy(pCECS->MErrors[idx], modName, CECS__MODNAMELENGTH);
	}
    return pCECS;
}

sCECS* CECS_RecError(
	int errid,
	int type,
	const char* fname,
	const unsigned int line,
	const char* msg,
	...
) {
	sCECS* ret;
	static char vaStr[CECS__FERRORL]={0};
	va_list(vargs);
	va_start(vargs, msg);
	vsnprintf(vaStr, CECS__FERRORL, msg, vargs);
	va_end(vargs);
	ret = CECS_RecErrorMod("Mod-Default", errid, type, fname, line, vaStr);
	return ret; 
}

int CECS_GetNumberOfAllErrors(void) {
	CECS_CheckIfInit("CECS_GetNumberOfAllErrors()");
	return pCECS->NErrors;
}

int CECS_GetNumberOfErrorsByType(int typeId) {
	int i, ret = 0;
	CECS_CheckIfInit("CECS_GetNumberOfErrorsByType()");
	const int NErrors = pCECS->NErrors;
	if (typeId != _CECS_ERRTYPE_ALL) {
		for (i = 0; i < NErrors; i++)
			if (pCECS->TErrors[i] == typeId) pCECS->SErrIDs[ret++] = i;
	} else {
		ret = NErrors;
		for (int i = 0; i < NErrors; i++) pCECS->SErrIDs[i]=i;
	}
	return ret;
}

int* CECS_GetErrorsIDsByType(int typeId, int* NErr) {
	(*NErr) = CECS_GetNumberOfErrorsByType(typeId);
	return pCECS->SErrIDs;
}

const char* CECS_getErrorStr(int id) {
	CECS_CheckIfInit("CECS_getErrorStr()");

	if (id >= pCECS->NErrors) {
		id = pCECS->NErrors-1;
		if (id < 0) {
			id = 0;
			CECS_RecError(
				CECS__ERRORID, 0, __FNAME__, __LINE__,
				"CECS_getErrorStr(): CECS had no errors recorded!"
			);
		}
	}
	return (const char*) pCECS->SErrors[id];
}


int CECS_getErrorId(int id) {
	CECS_CheckIfInit("CECS_getErrorId()");

	if (id >= pCECS->NErrors) {
		id = pCECS->NErrors-1;
		if (id < 0) {
			id = 0;
			CECS_RecError(
				CECS__ERRORID, 0, __FNAME__, __LINE__,
				"CECS_getErrorId(): CECS had no errors recorded!"
			);
		}
	}
	return pCECS->IErrors[id];
}

int CECS_getErrorType(int id) {
	CECS_CheckIfInit("CECS_getErrorType()");

	if (id >= pCECS->NErrors) {
		id = pCECS->NErrors-1;
		if (id < 0) {
			id = 0;
			CECS_RecError(
				CECS__ERRORID, 0, __FNAME__, __LINE__,
				"CECS_getErrorType(): CECS had no errors recorded!"
			);
		}
	}
	return pCECS->TErrors[id];
}

const char* CECS_getErrorFile(int id) {
	CECS_CheckIfInit("CECS_getErrorFile()");

	if (id >= pCECS->NErrors) {
		id = pCECS->NErrors-1;
		if (id < 0) {
			id = 0;
			CECS_RecError(
				CECS__ERRORID, 0, __FNAME__, __LINE__,
				"CECS_getErrorFile(): CECS had no errors recorded!"
			);
		}
	}
	return (const char*) pCECS->FErrors[id];
}

const char* CECS_getErrorMod(int id) {
	CECS_CheckIfInit("CECS_getErrorMod()");

	if (id >= pCECS->NErrors) {
		id = pCECS->NErrors-1;
		if (id < 0) {
			id = 0;
			CECS_RecError(
				CECS__ERRORID, 0, __FNAME__, __LINE__,
				"CECS_getErrorMod(): CECS had no errors recorded!"
			);
		}
	}
	return (const char*) pCECS->MErrors[id];
}

unsigned int CECS_getErrorLine(int id) {
	CECS_CheckIfInit("CECS_getErrorLine()");

	if (id >= pCECS->NErrors) {
		id = pCECS->NErrors-1;
		if (id < 0) {
			id = 0;
			CECS_RecError(
				CECS__ERRORID, 0, __FNAME__, __LINE__,
				"CECS_getErrorLine(): CECS had no errors recorded!"
			);
		}
	}
	return pCECS->LErrors[id];
}



const char* CECS_getName(void) {
	if (pCECS == NULL) pCECS = &CECS;
	if (pCECS->Name == NULL) CECS_Initialize(NULL, NULL);
	return (const char*)pCECS->Name;
}

const char* CECS_str(int typeId) {
	int i; int NE; int* IndxE;
	unsigned char FS;
	CECS_CheckIfInit("CECS_str()");
	IndxE = CECS_GetErrorsIDsByType(typeId, &NE);
	char* str = pCECS->DispStr;
	int maxstrprint = pCECS->MaxDisplayStringSize;

	if (typeId == _CECS_ERRTYPE_ALL) 
		snprintf(str, maxstrprint,
			"------- %s:: %i Errors of ALL Types recorded! -------\n", pCECS->Name, NE
		);
	else
		snprintf(str, maxstrprint,
			"------- %s:: %i Errors of Type {%i} recorded! -------\n", pCECS->Name, NE, typeId
		);

	if (NE > 0) {
		FS = pCECS->SetupFlag;
		for (i = 0; i < NE; i++) {
			const int IDX = IndxE[i];
			if (typeId != _CECS_ERRTYPE_ALL)
				snprintf(str, maxstrprint, "%s-> ", str);
			else
				snprintf(str, maxstrprint, "%s[ET:%i]-> ", str, CECS_getErrorType(IDX));

			if (FS & 0x20)
				snprintf(str, maxstrprint, "%s#%s: ", str, CECS_getErrorMod(IDX));
			if (FS & 0x01)
				snprintf(str, maxstrprint, "%s%i |", str, CECS_getErrorId(IDX));
			if (FS & 0x04)
				snprintf(str, maxstrprint, "%s [%s],", str, CECS_getErrorFile(IDX));
			if (FS & 0x08)
				snprintf(str, maxstrprint, "%s %i::", str, CECS_getErrorLine(IDX));
			if (FS & 0x10)
				snprintf(str, maxstrprint, "%s %s", str, CECS_getErrorStr(IDX));
			snprintf(str, maxstrprint, "%s\n",str);
		}
		snprintf(str, maxstrprint,
			"%s-------------------------------------------------------\n",str);
	}
	CECS_clear();
	return str;
}

void CECS_clear(void) {
	if (pCECS == NULL) return;
	pCECS->NErrors = 0;
}