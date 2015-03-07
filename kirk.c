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

int main(void)
{
	struct my_msgbuf upBuf;
	struct my_msgbuf downBuf;
	int midUp,midDown;
	key_t upKey,downKey;
	upKey = UP;
	downKey = DOWN;
	int flag;

	if ((midUp = msgget(upKey, 0644 | IPC_CREAT)) == -1) {
		perror("msgget1");
		exit(1);
	}
	
	if ((midDown = msgget(downKey, 0644)) == -1) { 
		perror("msgget2");
		exit(1);
	}

	printf("To send message press 1 or press 0\n");
	scanf("%d",&flag);

	//writing Up buf

	if(flag){
		printf("Enter lines of text, ^D to quit:\n");

		upBuf.mtype = 1; 

		while(fgets(upBuf.mtext, sizeof upBuf.mtext, stdin) != NULL) {
			int len = strlen(upBuf.mtext);

			
			if (upBuf.mtext[len-1] == '\n') upBuf.mtext[len-1] = '\0';

			if (msgsnd(midUp, &upBuf, len+1, 0) == -1) /* +1 for '\0' */
				perror("msgsnd");
		}
	}

	// Reading down Buf

	else{
		printf("captain: ready to receive messages\n");
		for(;;) {
			if (msgrcv(midDown, &downBuf, sizeof(downBuf.mtext), 0, 0) == -1) {
				perror("msgrcv");
				exit(1);
			}
			printf("captain: \"%s\"\n", downBuf.mtext);
		}
	}


	if (msgctl(midUp, IPC_RMID, NULL) == -1) {
		perror("msgctl");
		exit(1);
	}

	return 0;
}

