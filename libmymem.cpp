#include "libmymem.hpp"

Slab * slabmeta(int bsize, Slab *head, int bid){
	Slab *temp;
	if ((temp = (Slab *)mmap(NULL, SLABSIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0)) == MAP_FAILED) {
		perror("mmap");
		exit(1);
	}
	
	temp->nextslab = NULL;
	if(head == NULL)
		temp->head=temp;
	else
		temp->head=head;
	temp->bid=bid;
	temp->size = bsize;
	temp->totobj = (SLABSIZE-sizeof(Slab))/(bsize+8);
	temp->rem = temp->totobj-1;
	temp->bitmap[0]=1;

	return temp;
}

Bucket b[14];												//14 sizes of buckets

void * mymalloc(int size){
	int i=1, bsize, iter=-1;									//first find the required bucket
	while(size > i){
		i*=2;
		iter++;
	}
	sem_wait(&(b[iter].sem1));									//semaphore to ensure thread safety

	bsize=i;													//variable to store the required bucket index
	if(b[iter].head == NULL){									// Initially bucket points to nothing

		Slab *temp=slabmeta(bsize,NULL,iter), **base;			//create a slab by calling the function which creates a bucket of required bucket size
		b[iter].head = temp;									//slab has been created, adjust pointer to point head of allocated slab
		base=(Slab **)((bool *)temp+sizeof(Slab));				//base stores the starting address of object
		*base=temp->head;										
		sem_post(&(b[iter].sem1));	
		return (void *)((bool*)base+8);							
	}
	else {														//else clause
		Slab *temp=b[iter].head, *prev=NULL;
		while(temp != NULL && temp->rem == 0 ){					//find the required empty object
			prev=temp;
			temp=temp->nextslab;
		}

		if(temp == NULL){										//if slab is full
			temp=slabmeta(bsize,b[iter].head,iter);				//create new slab
			prev->nextslab = temp;
			Slab **base=(Slab **)((bool *)temp+sizeof(Slab));
			*base=temp->head;
			sem_post(&(b[iter].sem1));
			return (void *)((bool*)base+8);

		}
		else{												
			i=0;
			while(temp->bitmap[i] == 1 && i<(int)temp->totobj)	//iterate until we find a empty object
				i++;
			temp->bitmap[i]=true;							//set bitmap to true
			temp->rem-=1;
			Slab **base=(Slab **)((bool *)temp+sizeof(Slab)+i*(8+bsize));
			*base=temp->head;
			sem_post(&(b[iter].sem1));
			return (void *)((bool*)base+8);
		}
	}
	sem_post(&(b[iter].sem1));
}

void myfree(void *ptr){
	int diff,pos;
	bool *x=(bool *)ptr;												
	Slab **temp=(Slab **)(x-8), *head;
	head=*temp;
	pos=head->bid;
	sem_wait(&(b[pos].sem1));											 			//semaphore to ensure thread safety
	diff=(abs((bool*)temp-((bool*)(*temp)+sizeof(Slab))))/(8+head->size);			//find the difference 
	head->rem+=1;														
	head->bitmap[diff]=0;
	if(head->bitmap.count() == 0){													//if all objects are empty, we need to deallocate the whole slab
		Slab *base=head->head, *prev=NULL;

		while(base != head){											
			prev=base;
			base=base->nextslab;
		}
		if(base == head->head){														
			b[base->bid].head=NULL;
			munmap(base,SLABSIZE);													
		}
		else if(base->nextslab == NULL){
			prev->nextslab=NULL;
			munmap(base,SLABSIZE);//unmap
		}
		else{																	
			prev->nextslab=base->nextslab;
			munmap(base,SLABSIZE);//unmap
		}
	}
	sem_post(&(b[pos].sem1));										
}


