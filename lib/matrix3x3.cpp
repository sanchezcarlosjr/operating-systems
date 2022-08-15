#include <iostream>
#include<stdlib.h>

int main(int argv, const char* argc[]) {
	if (argv != 10) {
		std::cout << argc[0] << ": no enough arguments" << std::endl;
		return 1;
	}
	float sums[6] = {0, 0, 0, 0, 0, 0};
	for(int i=1; i<4; i++) {
		for(int j=0; j<3; j++) {
			float cell = atof(argc[i+j]);
			std::cout << cell << " ";
			sums[i-1] += cell;
			sums[j+2] += cell;
		}
		std::cout << std::endl;
	}
	std::cout << "Row 1: " << sums[0] << std::endl;
	std::cout << "Row 2: " << sums[1] << std::endl;
	std::cout << "Row 3: " << sums[2] << std::endl;


	std::cout << "Column 1: " << sums[3] << std::endl;
	std::cout << "Column 2: " << sums[4] << std::endl;
	std::cout << "Column 3: " << sums[5] << std::endl;

}
