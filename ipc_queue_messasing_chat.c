#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
// structure for message queue
struct Message {
    long mesg_type;
    char mesg_text[100];
} message;
 
int main()
{
    key_t key = ftok("progfile", 65);
    int msgid = msgget(key, 0666 | IPC_CREAT);
    
    do {
	    message.mesg_type = 1;
	    printf(">");
	    fgets(message.mesg_text, sizeof(message.mesg_text)/sizeof(char), stdin);
	    msgsnd(msgid, &message, sizeof(message), 0);
	    printf("You: %s", message.mesg_text);
    } while(strcmp(message.mesg_text, "fin de la transmision") != 0);
    msgctl(msgid, IPC_RMID, NULL);
    return 0;
}

