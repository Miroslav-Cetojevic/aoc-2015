#include <array>
#include <fstream>
#include <iostream>
#include <numeric>
#include <string>
#include <unordered_map>

template<typename T>
struct Position { T row, col; };

template<typename T>
auto& operator>>(std::istream& in, Position<T>& pos) {
	return in >> pos.row >> pos.col;
}

template<typename T>
struct EndPoints { Position<T> first, last; };

template<typename T>
auto& operator>>(std::istream& in, EndPoints<T>& pos) {
	return in >> pos.first >> pos.last;
}

template<typename G, typename T>
auto on(G& grid, T pos) { ++grid[pos]; }

template<typename G, typename T>
auto off(G& grid, T pos) { if(grid[pos] > 0) { --grid[pos]; } }

template<typename G, typename T>
auto toggle(G& grid, T pos) { grid[pos] += 2; }

int main() {
	std::ios_base::sync_with_stdio(false);

	auto filename = std::string{"instructions.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {
		constexpr auto gridlen = 1000UL;
		constexpr auto numlights = gridlen*gridlen;
		using Grid = std::array<std::uint64_t, numlights>;
		auto grid = Grid{};

		using cmd = void (*)(Grid&, std::size_t);
		auto cmd_map = std::unordered_map<std::string, cmd>{
			{"on", on},
			{"off", off},
			{"toggle", toggle}
		};

		std::string instruction;
		EndPoints<std::size_t> endpoints;

		while(file >> instruction >> endpoints) {

			auto command = cmd_map[instruction];

			for(auto row = endpoints.first.row; row <= endpoints.last.row; ++row) {
				for(auto col = endpoints.first.col; col <= endpoints.last.col; ++col) {
					command(grid, row*gridlen+col);
				}
			}
		}

		auto sum = std::accumulate(grid.begin(), grid.end(), 0UL);

		std::cout << sum << std::endl;
	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}
	return 0;
}
