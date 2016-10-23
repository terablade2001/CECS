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
	.NErrors = 0,
	.SErrors = NULL,
	.ErrorLength = 256,
	.MaxErrors = CECS__MAXERRORS
};


sCECS* CECS_Initialize(char* name, sCECS* pcecs) {
	int i = 0;

	// Clean up current CECS object
	sCECS* tCECS = &CECS;
	CECS_Shutdown(tCECS);

	if (pcecs != NULL) { // Initialize using an external CECS object.
		pCECS = pcecs;
	} else { // Initialize using the internal CECS object.
		pCECS = &CECS;
	}
	
	// Setup the memory for errors
	if (pCECS->SErrors == NULL) {
		pCECS->NErrors = 0;
		pCECS->SErrors = (char**) malloc(sizeof(char**) * pCECS->MaxErrors);
		for (i = 0; i < pCECS->MaxErrors; i++) {
			pCECS->SErrors[i] = (char*) malloc(sizeof(char*) * pCECS->ErrorLength);
		}
	}
	
	if (pCECS->Name == NULL) {
		pCECS->Name = (char*) malloc(sizeof(char*) * 64);
		if (name != NULL) snprintf(pCECS->Name, 64, name);
		else snprintf(pCECS->Name, 64, "CECS-UnNamed");
	}

	return pCECS;
}



sCECS* CECS_Shutdown(sCECS* pcecs) {
	int i = 0;
	if (pcecs == NULL) pcecs = pCECS;

	if (pcecs->SErrors != NULL) {
		for (i = 0; i < pcecs->NErrors; i++)
			free(pcecs->SErrors[i]);
		free(pcecs->SErrors);
	}
	if (pcecs->Name != NULL) free(pcecs->Name);

	pcecs->Name    = NULL;
	pcecs->SErrors = NULL;
	pcecs->NErrors = 0;

	return pCECS;
}


sCECS* CECS_RecError(int errid, char* msg, ...) {
	// If pCECS or CECS are not initialized, initialize the internal CECS object.
	if (pCECS == NULL) pCECS = &CECS;
	if (pCECS->SErrors == NULL) CECS_Initialize(NULL, NULL);
	if (pCECS->NErrors < pCECS->MaxErrors-1) {
		va_list(vargs);
		va_start(vargs, msg);
		vsnprintf(pCECS->SErrors[pCECS->NErrors], pCECS->ErrorLength, msg, vargs);
		va_end(vargs);
		pCECS->NErrors++;
	}
    return pCECS;
}


const char* CECS_getErrorStr(int id) {
	int isInit = 1;

	if (pCECS == NULL) { pCECS = &CECS; isInit = 0; }
	if (pCECS->SErrors == NULL) { CECS_Initialize(NULL, NULL); isInit = 0; }
	if (isInit == 0) CECS_RecError(CECS__ERRORID, "CECS_getError(): CECS was Not Initialized!");
    
	
	if (id >= pCECS->NErrors) {
		
		id = pCECS->NErrors-1;
		if (id < 0) {
			id = 0;
			CECS_RecError(CECS__ERRORID, "CECS_getError(): There where no recorded errors!");
		}
	}
    
	return (const char*) pCECS->SErrors[id];
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