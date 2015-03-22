#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <strings.h>
#include <time.h>


#define entryKey 111
#define meatMKey 222
#define stateKey 333
#define lionCKey 444
#define jackalCKey 555
#define rangerCKey 666
#define meatCKey 777
#define commonWKey 888
 

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

		for(iterNo = 0; iterNo < 3; iterNo++){
			randNumber = rand()%3;
			for(pit = 0; pit < 3; pit ++){
				pitNumber = (randNumber +pit) % 3;
				down(entryID, pitNumber);
				if((semctl(meatCID,pitNumber,GETVAL,0) <= 40) && ((semctl(stateID,pitNumber,GETVAL,0) == 0)||(semctl(stateID,pitNumber,GETVAL,0) == 4))){
					if(semctl(stateID,pitNumber,GETVAL,0) == 0)
						semctl(stateID,pitNumber,SETVAL,4);
					up(rangerCID,pitNumber);
					up(entryID,pitNumber);
					
					down(meatMID,pitNumber);
					printf("Ranger Adding Meat to pit %d\n",pitNumber );
					sb.sem_num = pitNumber;
					sb.sem_op = 10;
					sb.sem_flg = 0;
					semop(meatCID,&sb,1);
					printf("Meat in pit %d is %d \n",pitNumber,semctl(meatCID,pitNumber,GETVAL,0)  );
					up(meatMID,pitNumber);

					down(entryID,pitNumber);
					printf("Ranger leaving pit %d\n",pitNumber );
					semctl(stateID,pitNumber,SETVAL,0);
					down(rangerCID,pitNumber);
					semctl(commonWID,1,SETVAL,pitNumber);
					sb.sem_num = 0;
					sb.sem_op = semctl(commonWID,0,GETNCNT,0);
					sb.sem_flg = 0;
					semop(commonWID,&sb,1);
					up(entryID,pitNumber);
			}
			else{
				if(pit >2){
					printf("Ranger %d is waiting at pit %d (Last Pit)\n", lionNo, pitNumber);
					up(entryID,pitNumber);
					down(commonWID, 0);
					randNumber = semctl(commonWID,1,GETVAL,0);
					pit  = 0;
				}
				else{
					printf("Ranger %d is waiting at pit %d (Moving for next pit)\n", lionNo, pitNumber);
					up(entryID,pitNumber);
				}

			}

		}
		randwait(iterNo + 1);
	}
	return;
} 