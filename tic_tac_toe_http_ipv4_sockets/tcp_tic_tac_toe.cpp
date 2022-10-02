#include "sockets.h"

int main(int argc, char *argv[]) {
  if (argc != 3 && argc != 1) {
	  printf("\rtic_tac_toe.out <PEER ADDRESS IPv4> <PEER PORT>\n");
	  return EXIT_SUCCESS;
  }
  Machine machine(argc == 1 ? new Socket() : new Socket(argv[1], argv[2]));
  machine.execute();
  return EXIT_SUCCESS;
}
