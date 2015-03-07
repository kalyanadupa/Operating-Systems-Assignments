/*
** spock.c -- reads from a message queue
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
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
	int flag


	if ((midUp = msgget(upKey, 0644)) == -1) { 
		perror("msgget1");
		exit(1);
	}

	if ((midDown = msgget(downKey, 0644 | IPC_CREAT)) == -1) {
		perror("msgget2");
		exit(1);
	}
	printf("To recieve message press 1 or press 0\n");
	scanf("%d",&flag);
	if(flag){		
		//Reading Up buf 
		printf("spock: ready to receive messages\n");


		for(;;) { 
			if (msgrcv(midUp, &upBuf, sizeof(upBuf.mtext), 0, 0) == -1) {
				perror("msgrcv");
				exit(1);
			}
			printf("spock: \"%s\"\n", upBuf.mtext);
		}
	}

		
	else{
		//Writing down buf
		downBuf.mtype = 1; /* we don't really care in this case */

		while(fgets(downBuf.mtext, sizeof downBuf.mtext, stdin) != NULL) {
			int len = strlen(downBuf.mtext);

			/* ditch newline at end, if it exists */
			if (downBuf.mtext[len-1] == '\n') downBuf.mtext[len-1] = '\0';

			if (msgsnd(midDown, &downBuf, len+1, 0) == -1) /* +1 for '\0' */
				perror("msgsnd");
		}
	}
	




	if (msgctl(midDown, IPC_RMID, NULL) == -1) {
		perror("msgctl");
		exit(1);
	}
	return 0;
}

