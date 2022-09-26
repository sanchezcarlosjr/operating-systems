#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MY_SOCK_PATH "/tmp/test_path2"
#define LISTEN_BACKLOG 50
#define NSTRS       3

#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while (0)


char *strs[NSTRS] = {
    "This is the first string from the server.\n",
    "This is the second string from the server.\n",
    "This is the third string from the server.\n"
};


	int
main(int argc, char *argv[])
{
	int sfd, cfd;
	struct sockaddr_un my_addr, peer_addr;
	socklen_t peer_addr_size;

	sfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sfd == -1)
		handle_error("socket");

	memset(&my_addr, 0, sizeof(my_addr));
	my_addr.sun_family = AF_UNIX;
	strncpy(my_addr.sun_path, MY_SOCK_PATH,
			sizeof(my_addr.sun_path) - 1);

	if (bind(sfd, (struct sockaddr *) &my_addr,
				sizeof(my_addr)) == -1)
		handle_error("bind");

	if (listen(sfd, LISTEN_BACKLOG) == -1)
		handle_error("listen");

	/* Now we can accept incoming connections one
	   at a time using accept(2). */

	peer_addr_size = sizeof(peer_addr);
	cfd = accept(sfd, (struct sockaddr *) &peer_addr,
			&peer_addr_size);
	if (cfd == -1)
		handle_error("accept");

	/* Code to deal with incoming connection(s)... */
	FILE* fp = fdopen(cfd, "r");

	for (int i = 0; i < NSTRS; i++)
            send(cfd, strs[i], strlen(strs[i]), 0);

	char c;
	for (int i = 0; i < NSTRS; i++) {
		while ((c = fgetc(fp)) != EOF) {
			putchar(c);

			if (c == '\n')
				break;
		}
	}

	close(cfd);

	/* When no longer required, the socket pathname, MY_SOCK_PATH
	   should be deleted using unlink(2) or remove(3). */
	remove(MY_SOCK_PATH);
	return 0;
}
