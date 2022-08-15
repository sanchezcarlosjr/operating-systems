#include <iostream>
#include <cstdio>

int main(int argc, const char* argv[]) {
	if (argc != 3) {
		std::cout << argv[0] << ": not enough arguments" << std::endl;
		return 1;
	}
	if(std::rename(argv[1], argv[2]) == 0)
		return 0;
	std::cout << "Error renaming file";
	return 1;
}
