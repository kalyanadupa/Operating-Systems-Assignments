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

	while(1){
		printf("Barber is sleeping as there are no customers\n");
		down(custID,0);

		down(mutexID,0);
		up(barbID,0);
		printf("Barber is ready to cut har\n");
		up(mutexID,0);
		printf("Barber is cutting hair\n");
		randwait(4);

	}

}