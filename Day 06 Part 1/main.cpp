#include <bitset>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <unordered_map>

#include <boost/range/irange.hpp>
#include <boost/range/istream_range.hpp>

enum class Cmd { on, off, toggle };

using uint64 = std::uint64_t;

struct Position {
	uint64 row, col;
};

struct EndPoints {
	std::string state;
	Position first, last;
};

auto& operator>>(std::istream& in, Position& pos) {
	return in >> pos.row >> pos.col;
}

auto& operator>>(std::istream& in, EndPoints& pos) {
	return in >> pos.state >> pos.first >> pos.last;
}

auto count_lights_on(std::fstream& file) {

	constexpr auto gridlen = uint64{1000};
	constexpr auto numlights = (gridlen * gridlen);

	auto grid = std::bitset<numlights>{};

	const auto commands = std::unordered_map<std::string_view, Cmd>{
		{"on", Cmd::on},
		{"off", Cmd::off},
		{"toggle", Cmd::toggle}
	};

	const auto stream = boost::istream_range<EndPoints>(file);

	for(const auto& endpoints : stream) {

		const auto command = commands.find(endpoints.state)->second;

		const auto& first = endpoints.first;
		const auto& last  = endpoints.last;

		const auto begin_row = first.row;
		const auto end_row   = (last.row + 1);

		for(const auto row : boost::irange(begin_row, end_row)) {

			const auto begin_col = first.col;
			const auto end_col   = (last.col + 1);

			for(const auto col : boost::irange(begin_col, end_col)) {

				const auto pos = (row * gridlen + col);

				switch(command) {
					case Cmd::on:	  grid.set(pos);   break;
					case Cmd::off:	  grid.reset(pos); break;
					case Cmd::toggle: grid.flip(pos);  break;
					default: std::cerr << "WTF?!? Something went really wrong!" << std::endl;
				}
			}
		}
	}

	return grid.count();
}

int main() {

	const auto filename = std::string{"instructions.txt"};
	auto file = std::fstream{filename};

	if(file.is_open()) {

		std::cout << count_lights_on(file) << std::endl;

	} else {
		std::cerr << "Error! Could not open \"" << filename << "\"!" << std::endl;
	}

	return 0;
}
