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
	.ErrorLength = 256,
	.MaxErrors = CECS__MAXERRORS
};


sCECS* CECS_Initialize(char* name, sCECS* pcecs) {
	int i = 0;
	int MaxErrors = 0;

	// Clean up current CECS object
	sCECS* tCECS = &CECS;
	CECS_Shutdown(tCECS);

	if (pcecs != NULL) { // Initialize using an external CECS object.
		pCECS = pcecs;
	} else { // Initialize using the internal CECS object.
		pCECS = &CECS;
	}

	MaxErrors = pCECS->MaxErrors;

	//@#| ############### Allocate Memory ###############

	if (pCECS->SErrors == NULL) {
		pCECS->NErrors = 0;
		pCECS->SErrors = (char**) malloc(sizeof(char**) * MaxErrors);
		for (i = 0; i < MaxErrors; i++) {
			pCECS->SErrors[i] = (char*) calloc(pCECS->ErrorLength, sizeof(char*));
		}
	}

	if (pCECS->Name == NULL) {
		pCECS->Name = (char*) malloc(sizeof(char*) * 64);
		if (name != NULL) snprintf(pCECS->Name, 64, name);
		else snprintf(pCECS->Name, 64, "CECS-UnNamed");
	}

	if (pCECS->IErrors == NULL)
		pCECS->IErrors = (int*) calloc(MaxErrors, sizeof(int*));
	
	if (pCECS->TErrors == NULL)
		pCECS->TErrors = (int*) calloc(MaxErrors, sizeof(int*));

	if(pCECS->FErrors == NULL) {
		pCECS->FErrors = (char**)malloc(sizeof(char**) * MaxErrors);
		for (int i = 0; i < MaxErrors; i++)
			pCECS->FErrors[i] = (char*)calloc(CECS__FERRORL, sizeof(char*));
	}

	if (pCECS->LErrors == NULL)
		pCECS->LErrors = (unsigned int*) calloc(MaxErrors, sizeof(unsigned int*));

	if (pCECS->SErrIDs == NULL)
		pCECS->SErrIDs = (int*) calloc(MaxErrors, sizeof(int*));

	return pCECS;
}



sCECS* CECS_Shutdown(sCECS* pcecs) {
	int i = 0;
	int MaxErrors = 0;

	if (pcecs == NULL) pcecs = pCECS;

	MaxErrors = pcecs->MaxErrors;

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

	pcecs->Name    = NULL;
	pcecs->NErrors = 0;
	pcecs->SErrors = NULL;
	pcecs->IErrors = NULL;
	pcecs->TErrors = NULL;
	pcecs->FErrors = NULL;
	pcecs->LErrors = NULL;
	pcecs->SErrIDs = NULL;

	return pCECS;
}

int CECS_CheckIfInit(const char* msg) {
	int isInit = 0;

	if (pCECS == NULL) { pCECS = &CECS; isInit = -1; }
	if (pCECS->SErrors == NULL) { CECS_Initialize(NULL, NULL); isInit = -1; }
	if (isInit == -1) CECS_RecError(
		CECS__ERRORID, 0, __FNAME__, __LINE__,
		"%s: CECS was Not Initialized!", msg
	);
	return isInit;
}

sCECS* CECS_RecError(
	int errid,
	int type,
	const char* fname,
	const unsigned int line,
	char* msg,
	...
) {
	int idx = 0;
	unsigned char FS = 0;
	CECS_CheckIfInit("CECS_RecError()");
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

	}
    return pCECS;
}

int CECS_GetNumberOfAllErrors(void) {
	CECS_CheckIfInit("CECS_getErrorStr()");
	return pCECS->NErrors;
}

int CECS_GetNumberOfErrorsByType(int typeId) {
	int i, ret = 0;
	CECS_CheckIfInit("CECS_getErrorStr()");
	const int NErrors = pCECS->NErrors;
	for (i = 0; i < NErrors; i++)
		if (pCECS->TErrors[i] == typeId) pCECS->SErrIDs[ret++] = i;
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

void CECS_clear(void) {
	if (pCECS == NULL) return;
	pCECS->NErrors = 0;
}