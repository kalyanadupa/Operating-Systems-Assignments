#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
 

main(){
	key_t entryKey = 111;
	key_t meatMKey = 222;
	key_t stateKey = 333;
	key_t lionCKey = 444;
	key_t jackalCKey = 555;
	key_t rangerCKey = 666;
	key_t meatCKey = 777;
	key_t commonWKey = 888;

	int nL,nJ,i,j,k,l,pid;
	int randNumber, pitNumber;


	int entryID = semget(entryKey,3,IPC_CREAT | 0666);
	int meatMID = semget(meatMKey,3,IPC_CREAT | 0666);
	int stateID = semget(stateKey,3,IPC_CREAT | 0666);
	int lionCID = semget(lionCKey,3,IPC_CREAT | 0666);
	int jackalCID = semget(jackalCKey,3,IPC_CREAT | 0666);
	int rangerCID = semget(rangerCKey,3,IPC_CREAT | 0666);
	int meatCID = semget(meatCKey,3,IPC_CREAT | 0666);
	int commonWID = semget(commonWKey,2,IPC_CREAT | 0666);

	printf("Enter the Number of Lions ?\n");
	scanf("%d",&nL);

	for(i = 0; i < nL; i++){
		if((pid = fork())< 0 ){
			perror("fork");
			exit(1);
		}
		if(pid == 0){
			randNumber = rand()%3;
			for(i = 0; i < 3; i++){
				
			}
		}

	}

}