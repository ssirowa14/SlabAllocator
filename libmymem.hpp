typedef unsigned int u32;
#include <iostream>
#include <string>
#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>
#include <mutex>
#include <semaphore.h>
#include <bits/stdc++.h>

#define SLABSIZE 65536


using namespace std;


class Slab {																		//Class Slab
	public:
		Slab* nextslab;
		Slab *head;
		u32 size;
		u32 totobj;
		u32 rem;
		u32 bid;
		bitset <8192> bitmap;
};


class Bucket{															//Class Bucket
	public:
		Slab* head;
		sem_t sem1;

		Bucket(){
			sem_init(&sem1, 0, 1);
			head=NULL;
		}

};

void * mymalloc(int size);
void myfree(void *ptr);
