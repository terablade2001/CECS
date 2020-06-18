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

#include "include/CECS.h"

static sCECS* pCECS = NULL;

static sCECS CECS = {
	_ECS_SFINIT(Name, NULL),
	_ECS_SFINIT(SetupFlag, 0xFE),
	_ECS_SFINIT(NErrors  , 0),
	_ECS_SFINIT(SErrors  ,NULL),
	_ECS_SFINIT(SErrorsL ,NULL),
	_ECS_SFINIT(IErrors  ,NULL),
	_ECS_SFINIT(TErrors  ,NULL),
	_ECS_SFINIT(FErrors  ,NULL),
	_ECS_SFINIT(LErrors  ,NULL),
	_ECS_SFINIT(SErrIDs  ,NULL),
	_ECS_SFINIT(DispStr  ,NULL),
	_ECS_SFINIT(MErrors  ,NULL),
	_ECS_SFINIT(ErrorLength, CECS__FERRORL),
	_ECS_SFINIT(MaxErrors, CECS__MAXERRORS),
	_ECS_SFINIT(RefCounter, 0),
	_ECS_SFINIT(cecs_lock, NULL),
	_ECS_SFINIT(cecs_unlock, NULL),
	_ECS_SFINIT(cecs_mutex, NULL),
#ifdef CECSDEBUG
	_ECS_SFINIT(sigArray, {0}),
	_ECS_SFINIT(nsigArray, 0),
#endif
	_ECS_SFINIT(unused, 0)
};

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CECSDEBUG
static void CECS_SigHandler( int signum ) {
	static char SigErrStr[8] = {0};
	static char SigErrCECSStr[(CECS__MAXERRORS+7) * CECS__FERRORL] = {0};
	const int MAXL = CECS__MAXERRORS * CECS__FERRORL;
	write(2,"(*) Exception occurred: System: SIGNAL [",40);
	switch( signum )
	{
		case SIGABRT: write(2,"SIGABRT]\r\n",9);  break;
		case SIGILL : write(2,"SIGILL ]\r\n",9);  break;
		case SIGFPE : write(2,"SIGFPE ]\r\n",9);  break;
		case SIGSEGV: write(2,"SIGSEGV]\r\n",9);  break;
		default:
			snprintf(SigErrStr,7,"%2.1i]\r\n",signum);
			write(2,SigErrStr,4);
			break;
	}

	if (pCECS != nullptr) {
		char* outPtr = SigErrCECSStr;
		unsigned char FS = pCECS->SetupFlag;
		int l=0;

		l = snprintf(outPtr,MAXL,
			"-------------------------------------------------------\r\n"
			"::    CECS (C/C++ Error Control System) v[%5.3f]     ::\r\n"
			"::        www.github.com/terablade2001/CECS          ::\r\n"
			"-------------------------------------------------------\r\n"
			"======= (%s):: [%i] Record(s) for CECSDEBUG Signal =======\r\n",
			CECS__VERSION, pCECS->Name, pCECS->NErrors
		);
		if (l >= 0) {
			int lc = 0;
			outPtr += l; lc+=l;
			for (int i = 0; i < pCECS->NErrors; i++) {
				int errtype = pCECS->TErrors[i];
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
				l = snprintf(outPtr, MAXL-lc,"= [%s]> ",serrtype);
				if (l < 0) break; outPtr += l; lc+=l;

				if (FS & 0x20) {
					l = snprintf(outPtr, MAXL-lc, "#%s: ",pCECS->MErrors[i]);
					if (l < 0) break; outPtr += l; lc+=l;
				}
				if (FS & 0x01) {
					l = snprintf(outPtr, MAXL-lc, "%i | ",pCECS->IErrors[i]);
					if (l < 0) break; outPtr += l; lc+=l;
				}
				if (FS & 0x04) {
					l = snprintf(outPtr, MAXL-lc, "[%s], ",pCECS->FErrors[i]);
					if (l < 0) break; outPtr += l; lc+=l;
				}
				if (FS & 0x08) {
					l = snprintf(outPtr, MAXL-lc, "%i |>  ",pCECS->LErrors[i]);
					if (l < 0) break; outPtr += l; lc+=l;
				}
				if (FS & 0x10) {
					if (pCECS->SErrors[i] == nullptr) {
						l = snprintf(outPtr, MAXL-lc, "[CECS NULL]");
					} else {
						l = snprintf(outPtr, MAXL-lc, "%s", pCECS->SErrors[i]);
					}
					if (l < 0) break; outPtr += l; lc+=l;
				}
				l = snprintf(outPtr, MAXL-lc, "\r\n");
				if (l < 0) break; outPtr += l; lc+=l;
			}
			l = snprintf(outPtr, MAXL-lc, "=======================================================\r\n");
			if (l > 0) { outPtr += l; lc+=l; }
			write(2,SigErrCECSStr,lc);
		}
	}

	// const char* str = CECS_str(pCECS, _CECS_ERRTYPE_ALL);
	// int n = 0;
	// while (str[n] != 0) n++;
	// write(2,str,n);

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
	pcecs->cecs_lock   = NULL;
	pcecs->cecs_unlock = NULL;
	pcecs->cecs_mutex  = NULL;
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
			CECS__ERRORID, _CECS_ERRTYPE_ERROR, __FNAME__, __LINE__,
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
		char vaStr[CECS__FERRORL]={0};
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
	if (NULL!=pCECS->cecs_lock) pCECS->cecs_lock();
	if (pCECS->NErrors < pCECS->MaxErrors-1) {
		idx = pCECS->NErrors++;
		FS = pCECS->SetupFlag;

		if (FS & 0x01) // <<0: errid
			pCECS->IErrors[idx] = errid;

		if (FS & 0x02) // <<1: err-type
			pCECS->TErrors[idx] = type;

		if (FS & 0x04) // <<2: file-name
			strncpy(pCECS->FErrors[idx], fname, CECS__FERRORL);

		if (FS & 0x08) // <<3: src-line
			pCECS->LErrors[idx] = line;

		if (FS & 0x10) { // <<4: msg
			if (pCECS->SErrors[idx] != NULL) free(pCECS->SErrors[idx]);
			pCECS->SErrorsL[idx] = 0;
			if ((msgSize == 0) || (msg == NULL))
				pCECS->SErrors[idx] = NULL;
			else
			{
				pCECS->SErrorsL[idx] = sizeof(char)*(msgSize+2);
				pCECS->SErrors[idx] = (char*)calloc(pCECS->SErrorsL[idx], 1);
				if (pCECS->SErrors[idx] != NULL) {
					memcpy(pCECS->SErrors[idx], msg, sizeof(char)*msgSize);
				} else {
					pCECS->SErrorsL[idx] = 0;
				}
			}
		}

		if (FS & 0x20) // <<5: module
			strncpy(pCECS->MErrors[idx], modName, CECS__MODNAMELENGTH);
	}
	if (NULL!=pCECS->cecs_unlock) pCECS->cecs_unlock();
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
	char vaStr[CECS__FERRORL]={0};
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
				CECS__ERRORID, _CECS_ERRTYPE_ERROR, __FNAME__, __LINE__,
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
				CECS__ERRORID, _CECS_ERRTYPE_ERROR, __FNAME__, __LINE__,
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
				CECS__ERRORID, _CECS_ERRTYPE_ERROR, __FNAME__, __LINE__,
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
				CECS__ERRORID, _CECS_ERRTYPE_ERROR, __FNAME__, __LINE__,
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
				CECS__ERRORID, _CECS_ERRTYPE_ERROR, __FNAME__, __LINE__,
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
				CECS__ERRORID, _CECS_ERRTYPE_ERROR, __FNAME__, __LINE__,
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
	unsigned int maxstrprint = MINIMUM_ALLOC_FOR_STR_CALL+4;
	unsigned int maxLineLength = 0;
	for (int i=0; i < NE; i++) {
		unsigned int lineLength = pCECS->SErrorsL[IndxE[i]];
		if (lineLength > maxLineLength) { maxLineLength = lineLength; }
		maxstrprint += lineLength+64;
	}

	char* lstr = (char*)calloc(maxLineLength+2, sizeof(char));
	if (lstr == NULL) return "CECS-Error:: CECS_str(): Failed to allocate line memory!";

	if (pCECS->DispStr != NULL) free(pCECS->DispStr);
	pCECS->DispStr = (char*)calloc(maxstrprint+2, sizeof(char));
	char* str = pCECS->DispStr;
	const int devBuffLimitSize = (MINIMUM_ALLOC_FOR_STR_CALL)/2;
	static char devBuff[devBuffLimitSize] = {0};
	if (pCECS->SetupFlag & 0x80) { // <<7: Show CECS info
		snprintf(str, devBuffLimitSize,
			"-------------------------------------------------------\n"
			"::    CECS (C/C++ Error Control System) v[%5.3f]     ::\n"
			"::        www.github.com/terablade2001/CECS          ::\n"
			"-------------------------------------------------------\n",
			CECS__VERSION
		);
	} else { str[0]=0; str[1]=0; }

	if (!(pCECS->SetupFlag & 0x40)) { // <<6: Show track errors
		snprintf(devBuff, devBuffLimitSize,
		"=======================================================\n"
		"= CECS:: Errors occurred! (Errors tracking is [OFF])  =\n"
		"=======================================================\n"
		);
		strncat(str, devBuff, devBuffLimitSize);
		return str;
	}


	if (typeId == _CECS_ERRTYPE_ALL) {
		snprintf(devBuff, devBuffLimitSize,
			"======= (%s):: [%i] Record(s) of ALL Types recorded! =======\n", pCECS->Name, NE
		);
	} else {
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
		snprintf(devBuff, devBuffLimitSize,
			"======= (%s):: [%i] Record(s) of Type [%s] recorded! =======\n", pCECS->Name, NE, serrtype
		);
	}
	strncat(str, devBuff, devBuffLimitSize);

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
				snprintf(lstr, maxLineLength, "= [%s]> ",serrtype);
				strncat(str,lstr, maxLineLength);
			}

			if (FS & 0x20) {
				snprintf(lstr, maxLineLength, "#%s: ",CECS_getErrorMod(pCECS, IDX));
				strncat(str,lstr, maxLineLength);
			}
			if (FS & 0x01) {
				snprintf(lstr, maxLineLength, "%i | ",CECS_getErrorId(pCECS, IDX));
				strncat(str,lstr, maxLineLength);
			}
			if (FS & 0x04) {
				snprintf(lstr, maxLineLength, "[%s], ",CECS_getErrorFile(pCECS, IDX));
				strncat(str,lstr, maxLineLength);
			}
			if (FS & 0x08){
				snprintf(lstr, maxLineLength, "%i |> ",CECS_getErrorLine(pCECS, IDX));
				strncat(str,lstr, maxLineLength);
			}
			if (FS & 0x10){
				snprintf(lstr, maxLineLength, "%s",CECS_getErrorStr(pCECS, IDX));
				strncat(str,lstr, maxLineLength);
			}
			strncat(str,"\n",maxLineLength);
		}
		strncat(str,"=======================================================\n",maxLineLength);
	}

	if (lstr != NULL) free(lstr);
	// CECS_clear();
	return str;
}

void CECS_clear(sCECS* pcecs, int numberOfLatestRecords) {
	CECS_CheckIfInit(pcecs, "CECS_str()");
	if (numberOfLatestRecords < 0) pCECS->NErrors = 0;
	else {
		pCECS->NErrors -= numberOfLatestRecords;
		if (pCECS->NErrors < 0) pCECS->NErrors = 0;
	}
}

void CECS_HandleSignal(int SignalId, sCECS* pcecs) {
	pCECS = CECS_CheckIfInit(pcecs, "CECS_HandleSignal/Mod()");
#ifdef CECSDEBUG
	if (pCECS->nsigArray >= 32) {
		CECS_RecError(pCECS,
			CECS__ERRORID, _CECS_ERRTYPE_ERROR, __FNAME__, __LINE__,
			"CECS_HandleSignal(): 32 Signals has been registered. Can't register more!"
		);
	}
	if (NULL!=pCECS->cecs_lock) pCECS->cecs_lock();
	if (SIG_ERR != signal( SignalId, CECS_SigHandler)) {
		pCECS->sigArray[pCECS->nsigArray++] = SignalId;
		if (NULL!=pCECS->cecs_unlock) pCECS->cecs_unlock();
	} else {
		if (NULL!=pCECS->cecs_unlock) pCECS->cecs_unlock();
		CECS_RecError(pCECS,
			CECS__ERRORID, _CECS_ERRTYPE_ERROR, __FNAME__, __LINE__,
			"CECS_HandleSignal(): Signal [%i] failed to be registered!", SignalId
		);
	}
#endif
}

/**
 *  \brief Set a function to be called when threads-locking is required.
 *  \param [in] pcecs Pointer to Linked CECS object.
 *  \param [in] Pointer to a function of type void f(void)
 *  \return The pointer to the Linked CECS object.
 */
sCECS* CECS_SetFunc_Lock(
	sCECS* pcecs,
	void (*func)(void)
) {
	pCECS = CECS_CheckIfInit(pcecs, "CECS_SetFunc_Lock()");
	if (NULL!=pCECS->cecs_lock) {
		CECS_RecError(pCECS,
			CECS__ERRORID, _CECS_ERRTYPE_ERROR, __FNAME__, __LINE__,
			"CECS_SetFunc_Lock(): CECS has already a threads Lock function!"
		);
	}
	pCECS->cecs_lock = func;
	return pCECS;
}

/**
 *  \brief Set a function to be called when threads-unlocking is required.
 *  \param [in] Pointer to a function of type void f(void)
 *  \return The pointer to the Linked CECS object.
 */
sCECS* CECS_SetFunc_Unlock(
	sCECS* pcecs,
	void (*func)(void)
) {
	pCECS = CECS_CheckIfInit(pcecs, "CECS_SetFunc_Unlock()");
	if (NULL!=pCECS->cecs_unlock) {
		CECS_RecError(pCECS,
			CECS__ERRORID, _CECS_ERRTYPE_ERROR, __FNAME__, __LINE__,
			"CECS_SetFunc_Unlock(): CECS has already a threads Unlock function!"
		);
	}
	pCECS->cecs_unlock = func;
	return pCECS;
}

#ifdef __cplusplus
} // extern "C" {
#endif
