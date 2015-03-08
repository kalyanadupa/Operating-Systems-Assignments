/*
** kirk.c -- writes to a message queue
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define UP 1234
#define DOWN 4321

struct my_msgbuf {
	long mtype;
	char mtext[200];
};


struct my_users
{
    long int pid;
    char chatID[20];
};

void removeSubstring(char *s,const char *toremove){
  while( s=strstr(s,toremove) )
    memmove(s,s+strlen(toremove),1+strlen(s+strlen(toremove)));
}

int main(void)
{
	struct my_msgbuf upBuf;
	struct my_msgbuf downBuf;
	struct msqid_ds qstat;
	int midUp,midDown;
	key_t upKey,downKey;
	upKey = UP;
	downKey = DOWN;
	int flag;
	struct my_users users[200];
	flag = 0;
	int index,i;
	char list[200] = "LIST ";
	for(index = 0; index< 200; index++){
		users[index].pid = -100;
	}
	index = 0;
	if(flag){
		if ((midUp = msgget(upKey, 0644 | IPC_CREAT)) == -1) {
			perror("msgget1");
			exit(1);
		}
		//writing Up buf
		printf("Enter lines of text, ^D to quit:\n");

		upBuf.mtype = 1; 

		while(fgets(upBuf.mtext, sizeof upBuf.mtext, stdin) != NULL) {
			int len = strlen(upBuf.mtext);

			
			if (upBuf.mtext[len-1] == '\n') upBuf.mtext[len-1] = '\0';

			if (msgsnd(midUp, &upBuf, len+1, 0) == -1) /* +1 for '\0' */
				perror("msgsnd");
		}
		if (msgctl(midUp, IPC_RMID, NULL) == -1) {
			perror("msgctl");
			exit(1);
		}

	}

	// Reading down Buf

	else{
		if ((midDown = msgget(downKey, 0644)) == -1) { 
			perror("msgget2");
			exit(1);
		}
		if ((midUp = msgget(upKey, 0644 | IPC_CREAT)) == -1) {
			perror("msgget1");
			exit(1);
		}



		printf("captain: ready to receive messages\n");
		for(;;) {
			if (msgrcv(midDown, &downBuf, sizeof(downBuf.mtext), 0, 0) == -1) {
				perror("msgrcv");
				exit(1);
			}
			if(strstr(downBuf.mtext,"NEW") != NULL){
				if(msgctl(midDown,IPC_STAT,&qstat)<0){
					perror("msgctl failed");
					exit(1);
				}
				removeSubstring(downBuf.mtext,"NEW ");
				users[index].pid = qstat.msg_lspid;
				strcpy(users[index].chatID,downBuf.mtext);
				printf("process %d chatID %s added\n",qstat.msg_lspid, users[index].chatID);
				index ++;
			}
			printf("captain: \"%s\"\n", downBuf.mtext);


			memset(upBuf.mtext, 0, sizeof upBuf.mtext);
			upBuf.mtype = 1; 
			strcat(list,upBuf.mtext);
			strcpy(upBuf.mtext,list);
			for(i = 0; i < index ; i++){
				strcat(upBuf.mtext, users[i].chatID);
				strcat(upBuf.mtext, " ");
			}

		
			int len = strlen(upBuf.mtext);

			
			if (upBuf.mtext[len-1] == '\n') upBuf.mtext[len-1] = '\0';
			printf("Sending %s\n",upBuf.mtext );
			if (msgsnd(midUp, &upBuf, len+1, 0) == -1) /* +1 for '\0' */
				perror("msgsnd");
			
			if (msgctl(midUp, IPC_RMID, NULL) == -1) {
				perror("msgctl");
				exit(1);
			}

		}
	}


	
	return 0;
}

