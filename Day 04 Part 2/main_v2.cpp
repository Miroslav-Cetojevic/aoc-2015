/*
 * NOTE:
 *
 * Linker flags required for openssl:
 *
 * -lssl -lcrypto
 */
#include <algorithm>
#include <array>
#include <charconv>
#include <iostream>
#include <iterator>
#include <limits>

#include <boost/range/irange.hpp>

#include <openssl/md5.h>

auto get_value() {

	// openssl requires unsigned char for its interfaces
	using uchar = unsigned char;

	const uchar key[] = "ckczppom";
	const auto keylen = (sizeof(key) - 1);

	auto hash = std::array<uchar, MD5_DIGEST_LENGTH>{};

	auto md5 = MD5_CTX{};

	MD5_Init(&md5);
	MD5_Update(&md5, key, keylen);

	using uint32 = std::uint32_t;
	using uint32_limits = std::numeric_limits<uint32>;

	// this is where the suffixes to the key will be stored -
	// size of array is equal to number of digits of maximum
	// value of an unsigned int
	auto output_stream = std::array<char, uint32_limits::digits>{};

	const auto begin = uint32{117946 + 1}; // avoid repeating computations from Part 1
	const auto end   = uint32_limits::max();

	const auto range = boost::irange(begin, end);

	auto result = std::find_if(range.begin(), range.end(), [&] (auto i) {

		const auto begin = output_stream.begin();
		const auto end   = output_stream.end();

		const auto ptr = std::to_chars(begin, end, i).ptr;

		const auto outlen = std::distance(begin, ptr);
		const auto data = reinterpret_cast<uchar*>(begin);

		auto md5_copy = md5;

		MD5_Update(&md5_copy, data, outlen);
		MD5_Final(hash.data(), &md5_copy);

		const auto prefix = boost::irange(uint32{3});

		// each element in hash is one-byte-sized (char) and represents two hexadecimal characters -
		// in other words, an all-zero hash[0-2] (3 elements) is equal to "000000"
		const auto has_prefix = std::all_of(prefix.begin(), prefix.end(), [&hash] (auto i) {
			return (hash[i] == 0);
		});

		return has_prefix;
	});

	return *result;
}

int main() {

	std::cout << get_value() << std::endl;

	return 0;
}
