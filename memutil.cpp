//#include "mem1.cpp"
#include <bits/stdc++.h>
#include <thread>
#include <mutex>
#include <unistd.h>
#include "libmymem.hpp"
using namespace std;

void threadm(int objlist[], int niterations);
std::mutex mutex1;
std::mutex mutex2;


int main(int argc, char* argv[]){
	int n=atoi(argv[2]);
	int niterations=atoi(argv[4]);
	int objlist[13];
	objlist[0]=2;
	for (int i = 1; i < 13; ++i)
	{
		objlist[i]=2*objlist[i-1];
	}
	std::thread threads[n];
	for (int i = 0; i < n; ++i)
	{
			threads[i]=std::thread(threadm, objlist, niterations);
	}
	for (int i = 0; i < n; ++i)
	{
		threads[i].join();
	}
	
	return 0;
}

void threadm(int objlist[], int niterations){
	for (int i = 0; i < niterations; ++i)
	{
		int pickup=rand()%13;
		void* x = mymalloc(objlist[pickup]);
		memset(x,1,objlist[pickup]);
		//write to allocated memory
		int sleep=rand()%100;
		usleep(sleep);
		myfree(x);
	}

}


