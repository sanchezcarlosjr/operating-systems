#include <iostream>
#include <stdio.h>

int main(int argc, const char* argv[]) {
	if (argc != 3) {
		std::cout << argv[0] << ": no enough arguments" << std::endl;
		return 1;
	}
	FILE *source = fopen(argv[1], "r");
	FILE *dest = fopen(argv[2], "w");
	
	int data = 0;
	while ((data = fgetc(source)) != EOF)
		fputc(data, dest);
	fclose(source);
	fclose(dest);
	
	return 0;
}

