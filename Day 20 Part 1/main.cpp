#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

//https://www.reddit.com/r/adventofcode/comments/3xjpp2/day_20_solutions/
int main() {

	auto total_presents = 33100000UL;
	auto n_presents = 10UL;
	auto n_houses = (total_presents / n_presents);

	// array with 3 mil elements will smash the stack, so vector is used instead
	auto container_size = (n_houses + 1);
	auto houses = std::vector<std::size_t>(container_size, n_presents);

	// all the houses are initialized with 10 presents, so we don't need the first elf
	// elf_id is one-based, and house_id is zero_based
	for(auto elf_id = 2UL; elf_id <= n_houses; ++elf_id) {

		for(auto house_id = (elf_id - 1); house_id < n_houses; house_id += elf_id) {
			houses[house_id] += (elf_id * n_presents);
		}
	}

	auto lowest_house = std::next(std::find_if(houses.begin(), houses.end(), [total_presents] (auto house) {
		return (house >= total_presents);
	}));

	auto min_house_number = std::distance(houses.begin(), lowest_house);

	std::cout << min_house_number << std::endl;

	return 0;
}
