#include <sys/ipc.h> 
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "string.h"
#include "sys/stat.h"
#include "errno.h"
#include "dirent.h"
#include <sys/types.h>
#include <time.h>
#include <fcntl.h>
#include "sys/wait.h"

int main()
{

	int qid;
	struct msqid_ds qstat;
	qid1=msgget((key_t)131,IPC_CREAT);
	qid=msgget((key_t)132,IPC_CREAT);

	
	
	if(qid==-1)
	{
		perror("msg failed\n");
		exit(1);
	}
	if(msgctl(qid,IPC_STAT,&qstat)<0)
	{
		perror("msgctl failed");
		exit(1);
	}
	printf("\n %d msg in q \n",qstat.msg_qnum);
	printf("last msg send by process % d\n",qstat.msg_lspid);
	printf("last msg receved by process %d\n",qstat.msg_lrpid);
	printf("current number of bytes on queue %d\n",qstat.msg_cbytes);
	printf("max number of bytes %d\n",qstat.msg_qbytes);

	qstat.msg_qbytes=5120
	qstat.msg_perm.mode=0644
	if(sgctl(qid,IPC_SET,&qstat)<0)
	{
		perror("msgctl failed");
		exit(1);
	}
	printf("\n %d msg in q \n",qstat.msg_qnum);
	printf("last msg send by process % d\n",qstat.msg_lspid);
	printf("last msg receved by process %d\n",qstat.msg_lrpid);
	printf("current number of bytes on queue %d\n",qstat.msg_cbytes);
	printf("max number of bytes %d\n",qstat.msg_qbytes);
}