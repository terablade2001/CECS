# CECS

## What is CECS ?

CECS (C Error Control System) is a C/C++ libary for textual informative error control. Errors, Warnings, Info  (or other types) that are recording during runtime can be traced back to the file and line where the error occurred. Error Logging include information like:
* Error Type: You can define your own error types usign a number. Default types are: Error, Warning, Info, Debug.
* Module-Name (Each Function/File/Library can be a Module)
* Error ID number (To describe an error in a typical way)
* File-Name where the error occured.
* Line-Number where the error occured.
* Analytical Textual Information for further understanding of the error.

## Why should I use CECS ?

If you need a bit more advanced system of Error Control / Logging, you can try CECS which is self-included as source code in each compilation. All Errors are being stored with information tags in internal memory and can be printed-out in any format when they are requested. Useful to track-down errors, on complicated projects.

## LinkedIn Articles

• [CECS: C/C++ Error Control System](https://www.linkedin.com/pulse/cecs-cc-error-control-system-vasileios-pothos/) \
• [Signals Tracking with CECS](https://www.linkedin.com/pulse/signals-tracking-cecs-vasileios-pothos/)

# C++ Target design

CECS was start as a C implementation, and over the C implementation a C++ class and relative MACROs have been implemented for C++ support. The C++ design which is proposed to be used, is aiming on four targets:

* **Full Error tracking**
  The user should be able to understand an error and track back to specific code of functions that called before that error occurs. This error tracking is providing a full understanding of the system behavior that led to the error.
* **Reduce required type-writing**
  An error should be checked almost like an "if" statement with a "then" report.
* **Support for basic exception handling**
  For each error checking condition, the user should be able to decide easily the stop actions; return something or throw an exception?
* **Common error reporting among different libraries.**
  In a program different libraries are called that may use functions from other libraries. If they've build using CECS, for each captured error there will be generated an error tracking report referring properly each executed library's function that contributed to the error generation.

## Design rules

For maximizing the CECS abilities there are some rules to be followed by the user:

* For each function user should do all possible error checks using *_ERR/I/N/T(expression, report)* macros.
* After each function call user should check possible error either via *_ERR/I/N/T(expression, report)* macros either via the *\_CHECKR/I/N/T\_* macros. The *_ERR/I/N/T(expression, report)* macros are preferred as they provide the generation of custom report of the error at that point, which means more semantic information.
* If there is a main() function in the program there should be a try-catch statement inside the main() and all errors in the main() function to be checked via the *_ERRT(expression, report)* macro or the *\_CHECKRT\_* macro. 
  * If there is not any main() function then the first level functions should return an integer or a pointer. When error should be returned the integer should be < 0 and the pointer to be NULL. In this case the *\_ERRI(expression, report)* and the *\_ERRN(expression, report)* macros should be used in those functions.



## Easy C++ CECS Setup

In order to use the C++ CECS copy the 4 files of the folder `C++/src/` as is (with the `include`folder) in your project. For example:

```shell
mkdir <myproject>/CECS
cp -rf C++/src/ <myproject>/CECS/
```

Then include the two .cpp files (`CECS.cpp` and `CECS-c.cpp`) in your project and also the `<myproject>/CECs/include/CECS.hpp` file in every C++ file that you are going to use CECS to.

For each C++ file create a static CECS instance as below, where XXX is a specific short-name that can be used to describe a group of files (i.e. a module or a library or even a single C++ file).
```c++
static CECS ECS("XXX")
// or if need to provide specific unique CECS Name:
static CES ECS("XXX","ECS-Name")
```

Enable the definition `#define __ECSOBJ__ ECS` in your project (i.e. `-D__ECSOBJ__=ECS` in compilation flags)

## C++ Quick Macros

### 1. Expression Errors with reports

Given an expression that validates to True and a custom error report:

* **_ERR** \
  Logs an Error and returns (void)
* **_ERRI** \
  Logs an Error and return _CECS_DEFAULT_ERRID=-9999
* **_ERRN** \
  Logs an Error and return NULL
* **_ERRT** \
  Logs an Error and throw exception
* **_ERRL** \
  Just logs an error and continue
* **_ERRO** \
  Logs an error and apply custom user-defined code.

### 2. Automated Error Checking from records.

If an error has been reported by the use of the previous macros, then the following macros automatically search for errors and if found they do the following:

* **\_CHECKR\_** \
  Logs a new error with standard report and returns (void)
* **\_CHECKRI\_** \
  Logs a new error with standard report and return _CECS_DEFAULT_ERRID=-9999
* **\_CHECKRN\_** \
  Logs a new error with standard report and return NULL
* **\_CHECKRT\_** \
  Logs a new error with standard report and throw exception
* **\_CHECKRL\_** \
  Logs a new error with standard report and apply custom user-defined code.
* **\_CHECKRO\_** \
  Logs a new error with standard report and return



## C++ Example

```c++
#include "../src/include/CECS.hpp"

#define __ECSOBJ__ ECS
static CECS ECS("Main-Module","Main-CECS");
using namespace std;

int TableAt = 0; // 1 for success.

int TableIsAt(int _tableat) {
	_ERRI((_tableat < 0) || (_tableat > 2),"TableIsAt():: Works only for input 0, 1 or 2. Got input = (%i)",_tableat)
	_ERRL(_tableat==0,"TableIsAt():: Zero value for table!")
	_ERRO(_tableat==2,  { cout<<"User Code Executed!"<<endl; return 8; }, "TableIsAt():: User code execution detected!")
	return 7;
}

int PersonIsInTable(const char* name) {
	string namestr(name);
	_ERRI(namestr.compare("Vasileios"),"Unknown name on tables!")
	int TableNo = TableIsAt(TableAt);
	_ERRI(TableNo < 0,"Wrong position of Table given the person [%s]",name)
	_WARN(TableNo==8, "Warning: TableId == 8!")
	_CHECKRI_
	_ERRI(TableAt==0,"error recorded as [ERR-LOG] above. Here it's captured!")
	return TableNo;
}

const char* BallIsInHandsOf(const string& nickname) {
	_ERRN(nickname.compare("terablade2001"),"Unknown nickname (%s)",nickname.c_str())
	static string name("Vasileios");
	
	int TableNo = PersonIsInTable(name.c_str());
	_ERRN(TableNo < 0,"Return of PersonIsInTable(%s) was negative",nickname.c_str())
	return name.c_str();
}

void FindTheBall() {
	const string nickname("terablade2001");
	const char* name = BallIsInHandsOf(nickname);
	_ERR(name == NULL,"Return of BallIsInHandsOf() is NULL!")
	cout << "The ball is at person ["<< name << "]" << endl;
}

int main(int argc, char **argv)
{
	_ECSFORMAT(1,0,0,1,1,1,1,1)
	try {
		FindTheBall();
		_CHECKRO_(_ERRSTR((TableAt!=0)&&(TableAt!=2),{
			ss << ">> ..." << endl << "+  This is a custom user message." << endl;
			ss << "+  This message will be printed with [ERRSTR] tag." << endl;
			ss << "+  In such custom messages, the develeper can add any kind of" << endl;
			ss << "information in the \"ss\" stringstream object that _ERRSTR()" << endl;
			ss << "macro provides." << endl;
			ss << "+  That means that he can export whole debug data if need in" << endl;
			ss << "the error log! Like this multiline text!" << endl;
			for (int i=0; i < 5; i++) {
				ss << "i: " << i << endl;
			}
			ss << "... debug data done!";
		}))
		_CHECKRT_
		cout << "Test Completed Succesfully!" << endl;
	} catch(std::exception &e) {
		 std::cout<< std::endl<<"(*) Exception occured: "<< std::endl << e.what() << std::endl;
	}
	return 0;
}
```

The above code produce the following results:

* If `TableAt == 1`

```shell
  The ball is at person [Vasileios]
  Test Completed Succesfully!
```

* If `TableAt == 0`

```shell
(*) Exception occured:
-------------------------------------------------------
::    CECS (C/C++ Error Control System) v[0.117]     ::
::        www.github.com/terablade2001/CECS          ::
-------------------------------------------------------
======= (Main-CECS):: [6] Record(s) of ALL Types recorded! =======
= [ERR-LOG]> #Main-Module: 11 |> TableIsAt():: Zero value for table!
= [ERROR  ]> #Main-Module: 23 |> error recorded as [ERR-LOG] above. Here it's captured!
= [ERROR  ]> #Main-Module: 32 |> Return of PersonIsInTable(terablade2001) was negative
= [ERROR  ]> #Main-Module: 39 |> Return of BallIsInHandsOf() is NULL!
= [ERROR  ]> #Main-Module: 60 |> CECS_CHECKERROR captured: __UserReturn__ code executed!.
= [ERROR  ]> #Main-Module: 61 |> CECS_CHECKERROR captured: Function throw executed.
=======================================================
```

* If `TableAt == 2`

```shell
User Code Executed!

(*) Exception occured:
-------------------------------------------------------
::    CECS (C/C++ Error Control System) v[0.117]     ::
::        www.github.com/terablade2001/CECS          ::
-------------------------------------------------------
======= (Main-CECS):: [7] Record(s) of ALL Types recorded! =======
= [ERROR  ]> #Main-Module: 12 |> TableIsAt():: User code execution detected!
= [WARNING]> #Main-Module: 21 |> Warning: TableId == 8!
= [ERROR  ]> #Main-Module: 22 |> CECS_CHECKERROR captured: Function return executed.
= [ERROR  ]> #Main-Module: 32 |> Return of PersonIsInTable(terablade2001) was negative
= [ERROR  ]> #Main-Module: 39 |> Return of BallIsInHandsOf() is NULL!
= [ERROR  ]> #Main-Module: 60 |> CECS_CHECKERROR captured: __UserReturn__ code executed!.
= [ERROR  ]> #Main-Module: 61 |> CECS_CHECKERROR captured: Function throw executed.
=======================================================
```

* For any other value of `TableAt`, i.e. `-1`: 

```shell
(*) Exception occured:
-------------------------------------------------------
::    CECS (C/C++ Error Control System) v[0.117]     ::
::        www.github.com/terablade2001/CECS          ::
-------------------------------------------------------
======= (Main-CECS):: [7] Record(s) of ALL Types recorded! =======
= [ERROR  ]> #Main-Module: 10 |> TableIsAt():: Works only for input 0, 1 or 2. Got input = (-1)
= [ERROR  ]> #Main-Module: 20 |> Wrong position of Table given the person [Vasileios]
= [ERROR  ]> #Main-Module: 32 |> Return of PersonIsInTable(terablade2001) was negative
= [ERROR  ]> #Main-Module: 39 |> Return of BallIsInHandsOf() is NULL!
= [ERROR  ]> #Main-Module: 60 |> CECS_CHECKERROR captured: __UserReturn__ code executed!.
= [ERRSTR ]> #Main-Module: 60 |> >> ...
+  This is a custom user message.
+  This message will be printed with [ERRSTR] tag.
+  In such custom messages, the develeper can add any kind of
information in the "ss" stringstream object that _ERRSTR()
macro provides.
+  That means that he can export whole debug data if need in
the error log! Like this multiline text!
i: 0
i: 1
i: 2
i: 3
i: 4
... debug data done!
= [ERROR  ]> #Main-Module: 61 |> CECS_CHECKERROR captured: Function throw executed.
=======================================================
```






# C Example (*outdated...*)

An example taken from the test code "t00_test.c":

```C
#include "CECS.h"

// Creating Error and Warning records in a function which represents Module B.
int test05_ModB(sCECS* cecs) {
	CECS_Initialize("MainCECS", cecs);
	const char CECS_ModName[] = "Module B";
	CECS_MERR(1,"Test error in test05_ModB.");
	CECS_MWARN(1,"Test warning in test05_ModB.");
	return 0;
}

// Creating Error and Warning records in another function which represents Module A.
// Both test05_ModA and test05_ModB are sharing the same CECS object, thus they write their records in the same Error Control System.
int test05_ModA(void) {
	sCECS* CECS = CECS_Initialize("MainCECS", NULL);
	const char CECS_ModName[] = "Module A";
	CECS_MERR(1,"Test Error in test05_ModA.");
	test05_ModB(CECS);
	CECS_MWARN(1,"Test Warning in test05_ModA.");

	printf("%s\n", CECS_str(_CECS_ERRTYPE_ALL));
	
	CECS_Shutdown(NULL);
	return 0;
}

// The results after Calling they main() function will be: 
// ------- CECS:: 4 Errors of ALL Types recorded! -------
// [ET:0]-> #Module A: -9999 | [t00_test.c], 122:: Test Error in test05_ModA.
// [ET:0]-> #Module B: -9999 | [t00_test.c], 134:: Test error in test05_ModB.
// [ET:1]-> #Module B: -9998 | [t00_test.c], 135:: Test warning in test05_ModB.
// [ET:1]-> #Module A: -9998 | [t00_test.c], 124:: Test Warning in test05_ModA.
// -------------------------------------------------------
int main(int argc, char **argv) {
	return test05_ModA();
}
```



# Significant updates

## Signals support ([0.106]+)

In version [0.106] CECS can be used for signal tracking & debugging like SIGSEGV. This feature is enabled by defining the flag `CECSDEBUG` in the project, and modifies the CECS mechanism to do pre-logging with the DEBUG flag in every `_ERR{X}()` and `_CHECKR{X}_` macro call. 

During the start of the program (main() or initialization function()) the developer can call the `_SETSIGNAL(X)` macro which will bind X signal (e.g. X = SIGSEGV) to CECS handler. If such a signal occurs, then CECS will write() at stderr the contents of CECS which will correspond to pre-logged `DEBUG` records. From top to bottom developer can then track the last position of CECS calls that took place before the signal.

To demonstrate this feature the C++ example has been modified based on the `CECSDEBUG` flag. Running this example the user can take the following result:

```shell
ThreadTest.exe
(*) Exception occurred: System: SIGNAL [SIGSEGV]
------- Main-ECS:: 10 Record(s) of ALL Types recorded! -------
[DEBUG  ]> #Main(): [main.cpp], 110 |> Main():: SIGTest(20) crashed!
[DEBUG  ]> #Main(): [main.cpp], 99 |> SIGTest_Sum(elms=20) Failed!
[DEBUG  ]> #Main(): [main.cpp], 87 |> SIGTest_Sum():: x == nullptr
[DEBUG  ]> #Main(): [main.cpp], 88 |> SIGTest_Sum():: n < 0
[DEBUG  ]> #Main(): [main.cpp], 91 |> SIGTest_Sum():: sum < 0!
[DEBUG  ]> #Main(): [main.cpp], 111 |> CECS_CHECKERROR: _CHECKRT_
[DEBUG  ]> #Main(): [main.cpp], 112 |> Main():: SIGTest(2000) crashed!
[DEBUG  ]> #Main(): [main.cpp], 99 |> SIGTest_Sum(elms=200) Failed!
[DEBUG  ]> #Main(): [main.cpp], 87 |> SIGTest_Sum():: x == nullptr
[DEBUG  ]> #Main(): [main.cpp], 88 |> SIGTest_Sum():: n < 0
-------------------------------------------------------
```

From top to bottom the code went from line 110, to 99, to 87 and then to 88 and 91 and 111. SIGTest(20) was successful. Then the code wen from 112, to 99, to 87, to 88 ... and didn't reach the line 91. Thus the SIGSEGV was happened between lines 88 to 91 for the call SIGTest(200). And this is true! By correcting the tracked issue, the developer can then unset the CECSDEBUG definition and continue development.

**Note**: In order not to fill CECS logging buffer with DEBUG information, it's proposed to use the `_ECSCLS_` macro call to key-points functions (e.g. API functions) thus to reset logging buffer in each successful cycle. For example if the `_ECSCLS_` is placed in the start of the SIGTest() function the result will be:

```shell
ThreadTest.exe
(*) Exception occurred: System: SIGNAL [SIGSEGV]
------- Main-ECS:: 3 Record(s) of ALL Types recorded! -------
[DEBUG  ]> #Main(): [main.cpp], 99 |> SIGTest_Sum(elms=200) Failed!
[DEBUG  ]> #Main(): [main.cpp], 87 |> SIGTest_Sum():: x == nullptr
[DEBUG  ]> #Main(): [main.cpp], 88 |> SIGTest_Sum():: n < 0
-------------------------------------------------------
```

It's obvious that the needless DEBUG report of the successful SIGTest(20) is now ignored, and report is more focused in the problematic call.

## Dumbing a lot of info ([0.113]+)

Using CECS there might be cases were the developer may capture an error, but in order to investigate it he may need to dump a lot of info in the error log. Supposing that the developer has a function `const char* get_printable_info()` that returns a large string of formatted information about the current status, it's possible to include the result in the error log using the `_ERRSTR()` macro. This macro can be used before an error check `_ERR{X}()` with the same condition, or inside an `_ERRO()` or `_CHECKRO_` command. For example a typical usage inside an void-return function is:
```c++
int result = test_function();
_ERRO(result!=0,{
    _ERRSTR(1,{ ss << get_printable_info(); })
    return;
	},
	"Error: result of test_function() != 0 (=%i)", result
)
```
The above code will yield an [ERROR] tag displaying the "`Error: result of test_function() != 0 (=%i)`" result, and then an [ERRSTR] tag containing everything (single line or multiline dumb) that will return the `get_printable_info()` function. The `main-test00.cpp` has been also modified to show how extensive debug info can be added. The result is:

```shell
(*) Exception occured:
  --> ------- Main-CECS:: 7 Record(s) of ALL Types recorded! -------
[ERROR  ]> #Main-Module: [main-test00.cpp], 37 |> TableIsAt():: Zero value for table!
[ERROR  ]> #Main-Module: [main-test00.cpp], 50 |> CECS_CHECKERROR captured: Function return executed.
[ERROR  ]> #Main-Module: [main-test00.cpp], 61 |> Return of PersonIsInTable(terablade2001) was negative
[ERROR  ]> #Main-Module: [main-test00.cpp], 70 |> Return of BallIsInHandsOf() is NULL!
[ERROR  ]> #Main-Module: [main-test00.cpp], 92 |> CECS_CHECKERROR captured: __UserReturn__ code executed!.
[ERRSTR ]> #Main-Module: [main-test00.cpp], 92 |> >> ...
+  This is a custom user message.
+  This message will be printed with [ERRSTR] tag.
+  In such custom messages, the develeper can add any kind of
information in the "ss" stringstream object that _ERRSTR()
macro provides.
+  That means that he can export whole debug data if need in
the error log! Like this multiline text!
i: 0
i: 1
i: 2
i: 3
i: 4
... debug data done!
[ERROR  ]> #Main-Module: [main-test00.cpp], 93 |> CECS_CHECKERROR captured: Function throw executed.
-------------------------------------------------------
```

