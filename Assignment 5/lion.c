#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <strings.h>
#include <time.h>


#define entryKey = 111
#define meatMKey = 222
#define stateKey = 333
#define lionCKey = 444
#define jackalCKey = 555
#define rangerCKey = 666
#define meatCKey = 777
#define commonWKey = 888
 


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
	// key_t entryKey = 111;
	// key_t meatMKey = 222;
	// key_t stateKey = 333;
	// key_t lionCKey = 444;
	// key_t jackalCKey = 555;
	// key_t rangerCKey = 666;
	// key_t meatCKey = 777;
	// key_t commonWKey = 888;

	int nL,nJ,i,j,k,l,pid;
	int lionNo,iterNo,pit;
	int randNumber, pitNumber;



	int entryID = semget(entryKey,3,IPC_CREAT | 0666);
	int meatMID = semget(meatMKey,3,IPC_CREAT | 0666);
	int stateID = semget(stateKey,3,IPC_CREAT | 0666);
	int lionCID = semget(lionCKey,3,IPC_CREAT | 0666);
	int jackalCID = semget(jackalCKey,3,IPC_CREAT | 0666);
	int rangerCID = semget(rangerCKey,3,IPC_CREAT | 0666);
	int meatCID = semget(meatCKey,3,IPC_CREAT | 0666);
	int commonWID = semget(commonWKey,2,IPC_CREAT | 0666);

	//Initialize the semaphores

	for(i  = 0; i < 3; i ++){
		semctl(entryID,i,SETVAL,1);
		semctl(meatMID,i,SETVAL,1);
		semctl(stateID,i,SETVAL,1);
		semctl(lionCID,i,SETVAL,0);
		semctl(jackalCID,i,SETVAL,0);
		semctl(rangerCID,i,SETVAL,0);
		semctl(meatCID,i,SETVAL,0);
		
	}
	semctl(commonWID,0,SETVAL,0);
	semctl(commonWID,1,SETVAL,0);

	printf("Enter the Number of Lions ?\n");
	scanf("%d",&nL);
	pid_t lionPid[nL];

	srand(time(NULL));
	for(lionNo = 0; lionNo < nL; lionNo++){
		if((lionPid[lionNo] = fork())< 0 ){
			perror("fork");
			exit(1);
		}
		if(pid == 0){
			
			randwait(lionNo);
			
			for(iterNo = 0; iterNo < 3; iterNo++){
				randNumber = rand()%3;
				for(pit = 0; pit < 3; pit ++){
					pitNumber = (randNumber + pit)%3;
					printf("Lion %d checking for food in pit %d\n",lionNo, pitNumber );
					// Capturing the entry of pit
					down(entryID,pitNumber);
					// Checking if meat count is greater than 0 and if there's lion or no one in the pit 
					if((semctl(meatCID,pitNumber,GETVAL,0) > 0) && ((semctl(stateID,pitNumber,GETVAL,0) == 0)||(semctl(stateID,pitNumber,GETVAL,0) == 1))){
						if(semctl(stateID,pitNumber,GETVAL,0) == 0)
							semctl(stateID,pitNumber,SETVAL,1);
						printf("Lion %d entered the pit %d\n",lionNo, pitNumber );
						up(lionCID,pitNumber); // Increasing lion count in that pit
						up(entryID,pitNumber);
						// Locking the meat mutex
						down(meatMID,pitNumber);
						if(semctl(meatCID,randNumber,GETVAL,0) > 0){
							down(meatCID,pitNumber); // Lion ate the meat
							up(meatMID,pitNumber);
							if(semctl(lionCID,pitNumber,GETVAL,0) == 0){
								// This is the last lion in the pit
								semctl(stateID,pitNumber,SETVAL,0);
								semctl(commonWID,1,SETVAL,pitNumber);
								sb.sem_num = 0;
								sb.sem_op = semctl(commonWID,0,GETNCNT,0);
								sb.sem_flg = 0;
								semop(commonWID,&sb,1);
								// Waking up all the other jackals waiting on other pits 
							}
							up(entryID,pitNumber);
						}
						else{
							down(lionCID,pitNumber);
							semctl(stateID,pitNumber,SETVAL,0);
							up(meatMID,pitNumber);
							printf("Lion %d waiting at pit %d because it was empty \n", lionNo,pitNumber);
							down(commonWID,0);
							i = 0;
						}
					}
					else{
						if(pit >2){
							printf("Lion %d is waiting at pit %d (Last Pit)\n", lionNo, pitNumber);
							up(entryID,pitNumber);
							down(commonWID, 0);
							pit  = 0;
						}
						else{
							printf("Lion %d is waiting at pit %d (Moving for next pit)\n", lionNo, pitNumber);
							up(entryID,pitNumber);
						}

					}
				}
				randwait(lionNo + 1);
				printf("Lion %d Loop %d  Done \n",lionNo,iterNo );

			}
			exit(0);
		}

	}

	for(i = 0; i < 3; i++){
		wait(&lionPid[i]);
	}
	return;
}