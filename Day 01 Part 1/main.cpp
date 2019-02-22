#include <fstream>
#include <iostream>
#include <string>

int main() {
	std::ios::sync_with_stdio(false);

	auto filename = std::string{"directions.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {
		auto floor = 0L;

		char direction;
		while(file >> direction) {
			floor += ((direction == '(') ? 1 : -1);
		}

		std::cout << floor << std::endl;
	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"" << std::endl;
	}

	return 0;
}
