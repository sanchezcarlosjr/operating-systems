#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h> 
#include <unistd.h>
#include <signal.h>


#define MESSAGE_EOF "fin de la transmision\n"
#define MESSAGE_ME ">"
#define FTOK_FILE_NAME "progfile"
#define MAX_MESSAGE_STR 100

struct Message {
	long type;
	char text[MAX_MESSAGE_STR];
} message;


int main(int argc, char* argv[]) {
	if(argc != 3) {
		printf("CHAT 1.0\n");
		printf("chat name_client_1 name_client_2\n");
		return 0;
	}
	key_t key = ftok(FTOK_FILE_NAME, 65);
	int msgid = msgget(key, 0666 | IPC_CREAT);
	if(msgid < 0) {
		perror("error: ");
		return 1;
	}
	pid_t c_pid = -1;
	if ((c_pid = fork()) == 0) {
		// Child consumer.
		do {
			msgrcv(msgid, &message, sizeof(message), atoi(argv[2]), 0);
			printf("\r%s: %s\n",  argv[2], message.text);
			printf(MESSAGE_ME);
			fflush(stdout);
		} while(strcmp(message.text, MESSAGE_EOF));
		kill(getppid(), SIGKILL);
		exit(0);
	} else {
		// Parent producer.
		message.type = atoi(argv[1]);
		do {
			printf(MESSAGE_ME);
			scanf(" %[^\n]s", message.text);
			msgsnd(msgid, &message, sizeof(message), 0);
		} while(strcmp(message.text,  MESSAGE_EOF));
		kill(c_pid, SIGKILL);
		msgctl(msgid, IPC_RMID, NULL);
	}
	return 0;
}

