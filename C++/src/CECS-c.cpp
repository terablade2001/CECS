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

#include "include/CECS.h"

static sCECS* pCECS = NULL;

static sCECS CECS = {
	.Name = NULL,
	// .SetupFlag = 0xFF,
	.SetupFlag = 0xFE,
	.NErrors = 0,
	.SErrors = NULL,
	.SErrorsL = NULL,
	.IErrors = NULL,
	.TErrors = NULL,
	.FErrors = NULL,
	.LErrors = NULL,
	.SErrIDs = NULL,
	.DispStr = NULL,
	.MErrors = NULL,
	.ErrorLength = CECS__FERRORL,
	.MaxErrors = CECS__MAXERRORS,
	.RefCounter = 0,
#ifdef ENABLE_PTHREAD_SUPPORT
	.q_mtx = PTHREAD_MUTEX_INITIALIZER,
#endif
#ifdef CECSDEBUG
	.sigArray = {0},
	.nsigArray = 0,
#endif
	.unused = 0
};

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CECSDEBUG
static void CECS_SigHandler( int signum ) {
	static char SigErrStr[8] = {0};
	write(2,"(*) Exception occurred: System: SIGNAL [",40);
	switch( signum )
	{
		case SIGABRT: write(2,"SIGABRT]\n",9);  break;
		case SIGILL : write(2,"SIGILL ]\n",9);  break;
		case SIGFPE : write(2,"SIGFPE ]\n",9);  break;
		case SIGSEGV: write(2,"SIGSEGV]\n",9);  break;
		default:
			snprintf(SigErrStr,7,"%2.1i]\n",signum);
			write(2,SigErrStr,4);
			break;
	}
	const char* str = CECS_str(pCECS, _CECS_ERRTYPE_ALL);
	int n = 0;
	while (str[n] != 0) n++;
	write(2,str,n);

  _exit( signum );
}
#endif


sCECS* CECS_Initialize(const char* name, sCECS* pcecs, int replaceName) {
	int i = 0;
	int MaxErrors = 0;

	if (pcecs != NULL) { // Initialize using an external CECS object.
		if ((pCECS != NULL) && (pCECS != pcecs)) CECS_Shutdown(pCECS);
		pCECS = pcecs;
	} else { // Initialize using the internal CECS object.
		pCECS = &CECS;
	}

	// CECS name should be able to change if replaceName is set.
	if (pCECS->Name == NULL) {
		pCECS->Name = (char*) calloc(CECS__ECSNAMELENGTH, sizeof(char));
		replaceName = 1;
	}
	if (replaceName != 0) {
		if (name != NULL) snprintf(pCECS->Name, CECS__ECSNAMELENGTH, "%s", name);
		else snprintf(pCECS->Name, CECS__ECSNAMELENGTH, "CECS-UnNamed");
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
#ifdef CECSDEBUG
		pCECS->nsigArray = 0;
#endif
		pCECS->SErrors = (char**) malloc(sizeof(char*) * MaxErrors);
		for (i = 0; i < MaxErrors; i++) pCECS->SErrors[i] = NULL;
	}
	if (pCECS->SErrorsL == NULL) {
		pCECS->SErrorsL = (unsigned int*) calloc(MaxErrors, sizeof(unsigned int));
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

	if (pCECS->DispStr != NULL) { free(pCECS->DispStr); pCECS->DispStr = NULL; }

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

	if (pcecs->Name != NULL)
		free(pcecs->Name);

	if (pcecs->SErrors != NULL) {
		for (i = 0; i < MaxErrors; i++)
			if (pcecs->SErrors[i] != NULL) free(pcecs->SErrors[i]);
		free(pcecs->SErrors);
	}
	if (pcecs->SErrorsL != NULL) free(pcecs->SErrorsL);

	if (pcecs->IErrors != NULL) free(pcecs->IErrors);
	if (pcecs->TErrors != NULL) free(pcecs->TErrors);

	if (pcecs->FErrors != NULL) {
		for (i = 0; i < MaxErrors; i++)
			if (pcecs->FErrors[i] != NULL) free(pcecs->FErrors[i]);
		free(pcecs->FErrors);
	}

	if (pcecs->LErrors != NULL) free(pcecs->LErrors);
	if (pcecs->SErrIDs != NULL) free(pcecs->SErrIDs);
	if (pcecs->DispStr != NULL) free(pcecs->DispStr);

	if (pcecs->MErrors != NULL) {
		for (i = 0; i < MaxErrors; i++)
			if (pcecs->MErrors[i]!= NULL) free(pcecs->MErrors[i]);
		free(pcecs->MErrors);
	}

	pcecs->Name     = NULL;
	pcecs->NErrors  = 0;
	pcecs->SErrors  = NULL;
	pcecs->SErrorsL = NULL;
	pcecs->IErrors  = NULL;
	pcecs->TErrors  = NULL;
	pcecs->FErrors  = NULL;
	pcecs->LErrors  = NULL;
	pcecs->SErrIDs  = NULL;
	pcecs->DispStr  = NULL;
	pcecs->MErrors  = NULL;
#ifdef CECSDEBUG
		pCECS->nsigArray = 0;
#endif
	return pCECS;
}

int CECS_CheckIfInitNoMsg(sCECS* pcecs) {
	int isInit = 0;
	if (pcecs != NULL) { pCECS = pcecs; }
	else { pCECS = &CECS; isInit = -1; }
	if (pCECS->SErrors == NULL) { CECS_Initialize(NULL, pCECS, 0); isInit = -1; }
	return isInit;
}

sCECS* CECS_getCecs(sCECS* pcecs) {
	CECS_CheckIfInitNoMsg(pcecs);
	return pCECS;
}

sCECS* CECS_CheckIfInit(sCECS* pcecs, const char* msg) {
	int isInit = 0;
	if (pcecs != NULL) { pCECS = pcecs; }
	else { pCECS = &CECS; isInit = -1; }
	if (pCECS->SErrors == NULL) { CECS_Initialize(NULL, pCECS, 0); isInit = -1; }
	if (isInit == -1) {
		CECS_RecError(pCECS,
			CECS__ERRORID, 0, __FNAME__, __LINE__,
			"%s: CECS was Not Initialized!", msg
		);
	}
	return pCECS;
}

sCECS* CECS_RecErrorMod(
	sCECS* pcecs,
	const char* modName,
	int errid,
	int type,
	const char* fname,
	const unsigned int line,
	const char* msg,
	...
) {
	pCECS = CECS_CheckIfInit(pcecs, "CECS_RecError/Mod()");
	unsigned char FS = pCECS->SetupFlag;
	if (FS & 0x10) {
		#ifdef ENABLE_PTHREAD_SUPPORT
			char vaStr[CECS__FERRORL]={0};
		#else
			static char vaStr[CECS__FERRORL]={0};
		#endif
		int len = 0;
		va_list(vargs);
		va_start(vargs, msg);
		len = vsnprintf(vaStr, CECS__FERRORL, msg, vargs);
		va_end(vargs);
		if (len <= 0) snprintf(vaStr,CECS__FERRORL,"CECS_RecErrorMod():: %i = vsnprintf() >> failed!", len);
		CECS_RecErrorMod_NoList(
			pcecs, modName, errid, type, fname, line, vaStr, len
		);
	} else {
		CECS_RecErrorMod_NoList(
			pcecs, modName, errid, type, fname, line, NULL, 0
		);
	}
  return pCECS;
}

sCECS* CECS_RecErrorMod_NoList(
	sCECS* pcecs,
	const char* modName,
	int errid,
	int type,
	const char* fname,
	const unsigned int line,
	const char* msg,
	const unsigned int msgSize
) {
	int idx = 0;
	unsigned char FS = 0;
	pCECS = CECS_CheckIfInit(pcecs, "CECS_RecError/Mod()");
	#ifdef ENABLE_PTHREAD_SUPPORT
		pthread_mutex_lock(&pCECS->q_mtx);
	#endif
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
			if (pCECS->SErrors[idx] != NULL) free(pCECS->SErrors[idx]);
			pCECS->SErrorsL[idx] = 0;
			if ((msgSize == 0) || (msg == NULL))
				pCECS->SErrors[idx] = NULL;
			else
			{
				pCECS->SErrorsL[idx] = sizeof(char)*(msgSize+2); 
				pCECS->SErrors[idx] = (char*)calloc(pCECS->SErrorsL[idx], 1);
				if (pCECS->SErrors[idx] != NULL)
					memcpy(pCECS->SErrors[idx], msg, sizeof(char)*msgSize);
				else
					pCECS->SErrorsL[idx] = 0;
			}
		}
		
		if (FS & 0x20)
			strncpy(pCECS->MErrors[idx], modName, CECS__MODNAMELENGTH);
	}
	#ifdef ENABLE_PTHREAD_SUPPORT
		pthread_mutex_unlock(&pCECS->q_mtx);
	#endif
  return pCECS;
}

sCECS* CECS_RecError(
	sCECS* pcecs,
	int errid,
	int type,
	const char* fname,
	const unsigned int line,
	const char* msg,
	...
) {
	sCECS* ret;
	#ifdef ENABLE_PTHREAD_SUPPORT
		char vaStr[CECS__FERRORL]={0};
	#else
		static char vaStr[CECS__FERRORL]={0};
	#endif
	int len = 0;
	va_list(vargs);
	va_start(vargs, msg);
	len = vsnprintf(vaStr, CECS__FERRORL, msg, vargs);
	va_end(vargs);
	if (len <= 0) snprintf(vaStr,CECS__FERRORL,"CECS_RecError():: %i = vsnprintf() >> failed!", len);
	ret = CECS_RecErrorMod_NoList(
		pcecs, "Mod-Default", errid, type, fname, line, vaStr, len
	);
	return ret; 
}

int CECS_GetNumberOfAllErrors(sCECS* pcecs) {
	CECS_CheckIfInit(pcecs, "CECS_GetNumberOfAllErrors()");
	return pCECS->NErrors;
}

int CECS_GetNumberOfErrorsByType(sCECS* pcecs, int typeId) {
	int i, ret = 0;
	CECS_CheckIfInit(pcecs, "CECS_GetNumberOfErrorsByType()");
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

int* CECS_GetErrorsIDsByType(sCECS* pcecs, int typeId, int* NErr) {
	(*NErr) = CECS_GetNumberOfErrorsByType(pcecs, typeId);
	return pCECS->SErrIDs;
}

const char* CECS_getErrorStr(sCECS* pcecs, int id) {
	CECS_CheckIfInit(pcecs, "CECS_getErrorStr()");

	if (id >= pCECS->NErrors) {
		id = pCECS->NErrors-1;
		if (id < 0) {
			id = 0;
			CECS_RecError(pCECS,
				CECS__ERRORID, 0, __FNAME__, __LINE__,
				"CECS_getErrorStr(): CECS had no errors recorded!"
			);
		}
	}
	if (pCECS->SErrors[id]==NULL) {
		pCECS->SErrors[id] = (char*)calloc(CECS__FERRORL, sizeof(char));
		pCECS->SErrorsL[id] = sizeof(char)*CECS__FERRORL;
		snprintf(pCECS->SErrors[id], CECS__FERRORL-1, "[CECS NULL]");
	}
	return (const char*) pCECS->SErrors[id];
}


int CECS_getErrorId(sCECS* pcecs, int id) {
	CECS_CheckIfInit(pcecs, "CECS_getErrorId()");

	if (id >= pCECS->NErrors) {
		id = pCECS->NErrors-1;
		if (id < 0) {
			id = 0;
			CECS_RecError(pCECS,
				CECS__ERRORID, 0, __FNAME__, __LINE__,
				"CECS_getErrorId(): CECS had no errors recorded!"
			);
		}
	}
	return pCECS->IErrors[id];
}

int CECS_getErrorType(sCECS* pcecs, int id) {
	CECS_CheckIfInit(pcecs, "CECS_getErrorType()");

	if (id >= pCECS->NErrors) {
		id = pCECS->NErrors-1;
		if (id < 0) {
			id = 0;
			CECS_RecError(pCECS,
				CECS__ERRORID, 0, __FNAME__, __LINE__,
				"CECS_getErrorType(): CECS had no errors recorded!"
			);
		}
	}
	return pCECS->TErrors[id];
}

const char* CECS_getErrorFile(sCECS* pcecs, int id) {
	CECS_CheckIfInit(pcecs, "CECS_getErrorFile()");

	if (id >= pCECS->NErrors) {
		id = pCECS->NErrors-1;
		if (id < 0) {
			id = 0;
			CECS_RecError(pCECS,
				CECS__ERRORID, 0, __FNAME__, __LINE__,
				"CECS_getErrorFile(): CECS had no errors recorded!"
			);
		}
	}
	return (const char*) pCECS->FErrors[id];
}

const char* CECS_getErrorMod(sCECS* pcecs, int id) {
	CECS_CheckIfInit(pcecs, "CECS_getErrorMod()");

	if (id >= pCECS->NErrors) {
		id = pCECS->NErrors-1;
		if (id < 0) {
			id = 0;
			CECS_RecError(pCECS,
				CECS__ERRORID, 0, __FNAME__, __LINE__,
				"CECS_getErrorMod(): CECS had no errors recorded!"
			);
		}
	}
	return (const char*) pCECS->MErrors[id];
}

unsigned int CECS_getErrorLine(sCECS* pcecs, int id) {
	CECS_CheckIfInit(pcecs, "CECS_getErrorLine()");

	if (id >= pCECS->NErrors) {
		id = pCECS->NErrors-1;
		if (id < 0) {
			id = 0;
			CECS_RecError(pCECS,
				CECS__ERRORID, 0, __FNAME__, __LINE__,
				"CECS_getErrorLine(): CECS had no errors recorded!"
			);
		}
	}
	return pCECS->LErrors[id];
}



const char* CECS_getName(sCECS* pcecs) {
	CECS_CheckIfInit(pcecs, "CECS_getName()");
	return (const char*)pCECS->Name;
}

const char* CECS_str(sCECS* pcecs, int typeId) {
	int i; int NE; int* IndxE;
	unsigned char FS;
	CECS_CheckIfInit(pcecs, "CECS_str()");
	IndxE = CECS_GetErrorsIDsByType(pCECS, typeId, &NE);
	unsigned int maxstrprint = CECS__FERRORL*2;
	unsigned int maxLineLength = 0;
	for (int i=0; i < NE; i++) {
		unsigned int lineLength = pCECS->SErrorsL[IndxE[i]];
		if (lineLength > maxLineLength) { maxLineLength = lineLength; }
		maxstrprint += lineLength;
	}

	char* lstr = (char*)calloc(maxLineLength+2, sizeof(char));
	if (lstr == NULL) return "CECS-Error:: CECS_str(): Failed to allocate line memory!";

	if (pCECS->DispStr != NULL) free(pCECS->DispStr);
	pCECS->DispStr = (char*)calloc(maxstrprint+2, sizeof(char));
	char* str = pCECS->DispStr;
	if (!(pCECS->SetupFlag & 0x40)) {
		snprintf(str, maxstrprint, "CECS:: Errors occurred...");
		return str;
	}
	if (typeId == _CECS_ERRTYPE_ALL) 
		snprintf(str, maxstrprint,
			"------- %s:: %i Record(s) of ALL Types recorded! -------\n", pCECS->Name, NE
		);
	else {
		#define arsz (16)
		#define ars (arsz-1)
		static char serrtype [arsz]={0};
		switch (typeId) {
			case _CECS_ERRTYPE_ERROR   : snprintf(serrtype, ars ,"ERROR  "); break;
			case _CECS_ERRTYPE_WARNING : snprintf(serrtype, ars ,"WARNING"); break;
			case _CECS_ERRTYPE_INFO    : snprintf(serrtype, ars ,"INFO   "); break;
			case _CECS_ERRTYPE_DEBUG   : snprintf(serrtype, ars ,"DEBUG  "); break;
			case _CECS_ERRTYPE_ERRINFO : snprintf(serrtype, ars ,"ERRINF "); break;
			case _CECS_ERRTYPE_ERRSTR  : snprintf(serrtype, ars ,"ERRSTR "); break;
			case _CECS_ERRTYPE_SIGDBG  : snprintf(serrtype, ars ,"SIG-DBG"); break;
			case _CECS_ERRTYPE_ERRLOG  : snprintf(serrtype, ars ,"ERR-LOG"); break;
			default                    : snprintf(serrtype, ars ,"-OTHER-");
		}
		#undef ars
		#undef arsz
		snprintf(str, maxstrprint,
			"------- %s:: %i Record(s) of Type [%s] recorded! -------\n", pCECS->Name, NE, serrtype
		);
	}

	if (NE > 0) {
		FS = pCECS->SetupFlag;
		for (i = 0; i < NE; i++) {
			const int IDX = IndxE[i];
			if (typeId == _CECS_ERRTYPE_ALL) {
				int errtype = CECS_getErrorType(pCECS, IDX);
				#define arsz (16)
				#define ars (arsz-1)
				static char serrtype [arsz]={0};
				switch (errtype) {
					case _CECS_ERRTYPE_ERROR   : snprintf(serrtype, ars ,"ERROR  "); break;
					case _CECS_ERRTYPE_WARNING : snprintf(serrtype, ars ,"WARNING"); break;
					case _CECS_ERRTYPE_INFO    : snprintf(serrtype, ars ,"INFO   "); break;
					case _CECS_ERRTYPE_DEBUG   : snprintf(serrtype, ars ,"DEBUG  "); break;
					case _CECS_ERRTYPE_ERRINFO : snprintf(serrtype, ars ,"ERRINF "); break;
					case _CECS_ERRTYPE_ERRSTR  : snprintf(serrtype, ars ,"ERRSTR "); break;
					case _CECS_ERRTYPE_SIGDBG  : snprintf(serrtype, ars ,"SIG-DBG"); break;
					case _CECS_ERRTYPE_ERRLOG  : snprintf(serrtype, ars ,"ERR-LOG"); break;
					default                    : snprintf(serrtype, ars ,"-OTHER-");
				}
				#undef ars
				#undef arsz
				snprintf(lstr, maxLineLength, "[%s]> ",serrtype);
				strncat(str,lstr, maxstrprint);
			}

			if (FS & 0x20) {
				snprintf(lstr, maxLineLength, "#%s: ",CECS_getErrorMod(pCECS, IDX));
				strncat(str,lstr, maxstrprint);
			}
			if (FS & 0x01) {
				snprintf(lstr, maxLineLength, "%i | ",CECS_getErrorId(pCECS, IDX));
				strncat(str,lstr, maxstrprint);
			}
			if (FS & 0x04) {
				snprintf(lstr, maxLineLength, "[%s], ",CECS_getErrorFile(pCECS, IDX));
				strncat(str,lstr, maxstrprint);
			}
			if (FS & 0x08){
				snprintf(lstr, maxLineLength, "%i |> ",CECS_getErrorLine(pCECS, IDX));
				strncat(str,lstr, maxstrprint);
			}
			if (FS & 0x10){
				snprintf(lstr, maxLineLength, "%s",CECS_getErrorStr(pCECS, IDX));
				strncat(str,lstr, maxstrprint);
			}
			strncat(str,"\n",maxstrprint);
		}
		strncat(str,"-------------------------------------------------------\n",maxstrprint);
	}

	if (lstr != NULL) free(lstr);
	// CECS_clear();
	return str;
}

void CECS_clear(sCECS* pcecs) {
	CECS_CheckIfInit(pcecs, "CECS_str()");
	pCECS->NErrors = 0;
}

void CECS_HandleSignal(int SignalId, sCECS* pcecs) {
	pCECS = CECS_CheckIfInit(pcecs, "CECS_HandleSignal/Mod()");
#ifdef CECSDEBUG
	if (pCECS->nsigArray >= 32) {
		CECS_RecError(pCECS,
			CECS__ERRORID, 0, __FNAME__, __LINE__,
			"CECS_HandleSignal(): 32 Signals has been registered. Can't register more!"
		);
	}
	#ifdef ENABLE_PTHREAD_SUPPORT
		pthread_mutex_lock(&pCECS->q_mtx);
	#endif
	if (SIG_ERR != signal( SignalId, CECS_SigHandler)) {
		pCECS->sigArray[pCECS->nsigArray++] = SignalId;
		#ifdef ENABLE_PTHREAD_SUPPORT
			pthread_mutex_unlock(&pCECS->q_mtx);
		#endif
	} else {
		#ifdef ENABLE_PTHREAD_SUPPORT
			pthread_mutex_unlock(&pCECS->q_mtx);
		#endif
		CECS_RecError(pCECS,
			CECS__ERRORID, 0, __FNAME__, __LINE__,
			"CECS_HandleSignal(): Signal [%i] failed to be registered!", SignalId
		);
	}
#endif
}

#ifdef __cplusplus
} // extern "C" {
#endif
