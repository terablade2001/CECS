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
#include "../../src/include/CECS.hpp"
#include "PThreadPool/include/PThreadPool.hpp"

CECS_MAIN_MODULE("Main()","Main-ECS")

#include <stdint.h>
#include <unistd.h>
#define MAX_THREADS (4)
#define MAX_TASKS (12)
static bool CancelThreads = false;
static pthread_mutex_t q_mtx = PTHREAD_MUTEX_INITIALIZER;
using namespace vkp;

typedef struct SThread_data {
	int task_id;
} SThread_data;

static SThread_data Thread_Data[MAX_TASKS];

void* Thread_Execute(void* data) {
	SThread_data* ithread_data = (SThread_data*)data;
	int task_id = ithread_data->task_id;
	pthread_mutex_lock(&q_mtx);
		_WARNO(CancelThreads,
			{pthread_mutex_unlock(&q_mtx); return NULL;},
			"CANCELLED:: Task [%i] didn't run!",task_id
		)
	pthread_mutex_unlock(&q_mtx);
	
	_ERRO(task_id==(MAX_TASKS>>2),
		{ pthread_mutex_lock(&q_mtx);
		_ERRSTR(1, {ss << "Something went"; ss << " wrong! - as expected!"; })
		CancelThreads = true; pthread_mutex_unlock(&q_mtx); return NULL; },
		"ERROR: Task [%i] failed {== (%i/4)}",task_id,MAX_TASKS
	)
	
	pthread_mutex_lock(&q_mtx);
	cout << "(working on Task ["<<task_id<<"] ...)" << endl;
	pthread_mutex_unlock(&q_mtx);
	return NULL;
}

int TestThreading() {
	CancelThreads = false;
	PThreadPool TPool;
	TPool.Initialize(Thread_Execute, MAX_THREADS-1, 92);

	for (int tid = 0; tid < MAX_TASKS; tid++) {
		Thread_Data[tid].task_id = tid;
	}

	
	for (int tid = MAX_TASKS-1; tid >= 0; tid--)
		TPool.AddTask(&Thread_Data[tid]);

	TPool.Wait();

	TPool.Shutdown();
	_CHECKRI_
	return 0;
}


int SIGTest_Sum(float* x, int n, float& sum) {
	sum = 0.0f;
	_ERRI(nullptr==x,"SIGTest_Sum():: x == nullptr")
	_ERRI(n < 0,"SIGTest_Sum():: n < 0")
	for (int i = 0 ; i < n; i++)
		sum+=x[i];
	_ERRI(sum < 0.0f,"SIGTest_Sum():: sum < 0!")
	return 0;
}

int SIGTest(int elms, float& sum) {
	_ECSCLS_
	float* x = new float[20];
	for (int i = 0 ; i < 20; i++) x[i] = 1.0f;
	_ERRI(0!=SIGTest_Sum(x, elms, sum),"SIGTest_Sum(elms=%i) Failed!", elms)
	delete[] x;
	return 0;
}


int main(int argc, char** argv) {
	_ECSFORMAT(1,0,1,1,1,1,1,1)
	_SETSIGNAL(SIGSEGV)
	try {
		#ifdef CECSDEBUG
			float sum0, sum1;
			_ERRT(0!=SIGTest(20, sum0),"Main():: SIGTest(20) crashed!")
			_CHECKRT_
			_ERRT(0!=SIGTest(200, sum1),"Main():: SIGTest(2000) crashed!")
			_CHECKRT_
			cout << (sum0+sum1) << endl;
		#else
			_ERRT(0!=TestThreading(),"Main(): TestThreading() function produced an error!")
		#endif
	} catch(std::exception &e) {
		 std::cout<< std::endl<<"(*) Exception occured: "<< std::endl << e.what() << std::endl;
	}
}