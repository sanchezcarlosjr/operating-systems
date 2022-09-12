#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h> 
#include <unistd.h>
#include <signal.h>


#define MESSAGE_EOF "fin de la transmision\n"
#define MAX_MESSAGE_STR 100

struct Message {
	long mesg_type;
	char mesg_text[MAX_MESSAGE_STR];
} message;


int main(int argc, char* argv[]) {
	key_t key = ftok("progfile", 65);
	int msgid = msgget(key, 0666 | IPC_CREAT);
	pid_t c_pid = -1;
	if ((c_pid = fork()) == 0) {
		msgrcv(msgid, &message, sizeof(message), atoi(argv[2]), IPC_NOWAIT);
		do {
			msgrcv(msgid, &message, sizeof(message), atoi(argv[2]), 0);
			printf("\n%s:%s>\n",  argv[2], message.mesg_text);
			fflush(stdin);
		} while(strcmp(message.mesg_text, MESSAGE_EOF));
		kill(getppid(), SIGKILL);
		exit(0);
	} else {
		message.mesg_type = atoi(argv[1]);
		do {
			printf(">");
			fgets(message.mesg_text, MAX_MESSAGE_STR, stdin);
			msgsnd(msgid, &message, sizeof(message), 0);
		} while(strcmp(message.mesg_text,  MESSAGE_EOF));
		kill(c_pid, SIGKILL);
		msgctl(msgid, IPC_RMID, NULL);
	}
	return 0;
}

