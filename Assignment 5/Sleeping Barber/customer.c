#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <strings.h>
#include <time.h>


#define custKey 111
#define barbKey 222
#define mutexKey 333

struct sembuf sb;

int up(int semID, int subID){
	sb.sem_num = subID;
	sb.sem_op = 1;
	sb.sem_flg = 0;
	semop(semID, &sb,1);
}

int down(int semID, int subID){
	sb.sem_num = subID;
	sb.sem_op = -1;
	sb.sem_flg = 0;
	semop(semID, &sb,1);
}

void randwait(int secs) { 
	int len; // Generate a random number... 
	len = (int) ((drand48() * secs) + 1); 
	sleep(len); 
}


void main(){
	int custID = semget(custKey,1,IPC_CREAT | 0666);
	int barbID = semget(barbKey,1,IPC_CREAT | 0666);
	int mutexID = semget(mutexKey,1,IPC_CREAT | 0666);


	semctl(custID,0,SETVAL,0);
	semctl(barbID,0,SETVAL,0);
	semctl(mutexID,0,SETVAL,1);

	int nChairs,nCust,i;
	printf("Enter number of customers\n");
	scanf("%d",&nCust);
	printf("Enter number of chairs\n");
	scanf("%d",&nChairs);

	pid_t cPid[nCust];	
	for(i = 0; i < nCust ; i ++){
		
		cPid[i] = fork();
		if(cPid[i] == 0){
			printf("Customer %d enters Shop\n",i);
			down(mutexID,0);
			if(semctl(custID,0,GETVAL,0) < nChairs){
				printf("Customer %d waits in waiting room\n",i );
				up(custID,0);
				up(mutexID,0);
				down(barbID,0);
				printf("Customer %d is getting harcut\n",i );
				randwait(5);
				printf("Customer %d leaves\n",i );
			}
			else{
				printf("Shop is full Customer %d Leaves\n",i );
				up(mutexID,0);
			}
			return;
		}

	}

	for(i = 0; i < nCust; i++){
		wait(&cPid[i]);
	}
	return;
}