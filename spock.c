/*
** spock.c -- reads from a message queue
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#define UP 1234
#define DOWN 4321


struct my_msgbuf {
	long mtype;
	char mtext[200];
};

void removeSubstring(char *s,const char *toremove){
  while( s=strstr(s,toremove) )
    memmove(s,s+strlen(toremove),1+strlen(s+strlen(toremove)));
}
int parseList(char l[20][200], char *list) { 
	int cnt = 0; 
	char * token; 
	token = strtok(list, " "); 
	while((token = strtok(NULL, " ")) != NULL) { 
		strcpy(l[cnt], token);
		cnt++;
	} 
	return cnt;
}

int main(void)
{
	struct my_msgbuf upBuf;
	struct my_msgbuf downBuf;
	int midUp,midDown;
	key_t upKey,downKey;
	upKey = UP;
	downKey = DOWN;
	int flag;
	char ChatID[20] ;
	char a[20] = "NEW ";
	const char s[2] = "-";
	char strings[20][200];
	char input[200];
	char *token;
	int i = 0,index  = 0;



	flag = 0;
	if(flag){
		if ((midUp = msgget(upKey, 0644)) == -1) { 
			perror("msgget1");
			exit(1);
		}		
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
		if ((midDown = msgget(downKey, 0644 | IPC_CREAT)) == -1) {
			perror("msgget2");
			exit(1);
		}
		
		//Asking and sending the ChatID 
		printf("Enter the ChatID\n");
		downBuf.mtype = 1; /* we don't really care in this case */

		
		gets(ChatID);
		strcat(a,ChatID);
		strcpy(downBuf.mtext,a);
		int len = strlen(downBuf.mtext);

		if (downBuf.mtext[len-1] == '\n') 
			downBuf.mtext[len-1] = '\0';

		if (msgsnd(midDown, &downBuf, len+1, 0) == -1) /* +1 for '\0' */
			perror("msgsnd");
		
		//Geting the online user list
		if ((midUp = msgget(upKey, 0644)) == -1) { 
			perror("msgget1");
			exit(1);
		}	

		if (msgrcv(midUp, &upBuf, sizeof(upBuf.mtext), getpid(), 0) == -1) {
			perror("msgrcv");
			exit(1);
		}
		printf("spock: \"%s\"\n", upBuf.mtext);
		

		if(strstr(upBuf.mtext,"LIST") != NULL){
			index = parseList(strings,upBuf.mtext);

		}
		printf("List of Clients: \nidNo. ChatID\n");
		for(i = 0; i < index;i++){
			printf("%6d %s\n",i,strings[i]);
		}
		printf("Enter the id of the client\n" );

		while(fgets(input, sizeof input, stdin) != NULL){
			i = atoi(input);
			
			printf("Enter the Message to %d\n", i);
			downBuf.mtype = 1;

			if(fgets(downBuf.mtext, sizeof downBuf.mtext, stdin) != NULL) {
				strcat(downBuf.mtext," MSG ");
				strcat(downBuf.mtext,strings[i]);
				int len = strlen(downBuf.mtext);

				removeSubstring(downBuf.mtext,"\n");
				if (downBuf.mtext[len-1] == '\n') 
					downBuf.mtext[len-1] = '\0';
				printf("sending.. \"%s\" \n",downBuf.mtext );
				if (msgsnd(midDown, &downBuf, len+1, 0) == -1) /* +1 for '\0' */
					perror("msgsnd");
			}

			if (msgrcv(midUp, &upBuf, sizeof(upBuf.mtext), getpid(), 0) == -1) {
				perror("msgrcv");
				exit(1);
			}

			if(strstr(upBuf.mtext,"LIST") != NULL){
				index = parseList(strings,upBuf.mtext);

			}
			else{
				printf("%s\n",upBuf.mtext );
			}


			printf("List of Clients: \nidNo. ChatID\n");
			for(i = 0; i < index;i++){
				printf("%6d. %s\n",i,strings[i]);
			}
			printf("Enter the id of the client\n" );			
		}

		if (msgctl(midDown, IPC_RMID, NULL) == -1) {
				perror("msgctl");
				exit(1);
		}

	}
	

	return 0;
}

