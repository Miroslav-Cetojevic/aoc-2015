#include <algorithm>
#include <array>
#include <iostream>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <boost/range/irange.hpp>

#include "input.hpp"

struct ParseStats {
  int num_lines,
      med_size;
};

constexpr auto NEWLINE = '\n';
constexpr auto UPPERCASE_ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

constexpr auto parse_stats(std::string_view input) {

  auto stats = ParseStats{};

  auto& num_lines = stats.num_lines;

  for(auto c : input) {
    num_lines += (c == NEWLINE);
  }
  ++num_lines;

  auto& med_size = stats.med_size;
  auto pos = input.rfind(NEWLINE) + 1;

  while(pos != input.npos) {
      auto next = input.find_first_of(UPPERCASE_ALPHABET, (pos + 1));
      ++med_size;
      pos = next;
  }
  ++med_size;

  return stats;
}

constexpr auto PARSE_STATS = parse_stats(puzzle_input);

constexpr auto NUM_LINES = PARSE_STATS.num_lines;
constexpr auto MED_SIZE = PARSE_STATS.med_size;

auto split_input(std::string_view input) {

  auto lines = std::array<decltype(input), NUM_LINES>{};

  auto index = 0;
  auto pos = decltype(input.size()){};

  while(pos != input.npos) {
    pos = input.find(NEWLINE);
    lines[index++] = input.substr(0, pos);
    input.remove_prefix(pos+1);
  }

  return lines;
}

using Rules = std::unordered_map<std::string_view, std::vector<std::string_view>>;
using Medicine = std::array<std::string_view, MED_SIZE>;

struct Machine {
  Rules rules;
  Medicine medicine;
};

/*
 * Molecules always start with an uppercase letter. If we represent the medicine
 * as a sequence of separate molecules, we can detect duplicates very easily
 * later on.
 */
auto parse_medicine(std::string_view input) {

  auto medicine = Medicine{};

  auto index = 0;
  auto pos = decltype(input.size()){};

  while(pos != input.npos) {
    const auto next = input.find_first_of(UPPERCASE_ALPHABET, (pos + 1));
    medicine[index++] = input.substr(pos, (next - pos));
    pos = next;
  }

  return medicine;
}


auto parse(std::string_view input) {

  auto rules = Rules{};

  const auto& lines = split_input(input);

  const auto& medicine = parse_medicine(lines.back());

  constexpr auto SPACE = ' ';

  for(const auto i : boost::irange(NUM_LINES - 2)) {

    auto line = lines[i];

    auto key = line.substr(0, line.find(SPACE));
    auto value = line.substr((line.rfind(SPACE) + 1), line.npos);

    rules[key].push_back(value);
  }

  return Machine{rules, medicine};
}

auto num_unique_molecules(const Machine& machine) {

  const auto& rules = machine.rules;

  /*
   * The duplicate lists contains the rules for which the input is part of the
   * output.
   *
   * Front duplicates are the rules where a molecule is both input and first part
   * of the output.
   *
   * Back duplicates are the rules where a molecule is both input and last part of
   * the output.
   *
   * Example:
   *
   * H => HCa   (front duplicate)
   * P => CaP   (back duplicate)
   * Ti => TiTi (front & back duplicate)
   *
   * For rule "H => HCa", "Ca" will be stored in the list of front duplicates
   * for "H", so "H" will map to a list that contains "Ca".
   */
  auto front_duplicates = std::unordered_map<std::string_view, std::unordered_set<std::string_view>>{};
  auto back_duplicates = decltype(front_duplicates){};

  for(const auto& rule : rules) {

    auto input = rule.first;
    const auto& outputs = rule.second;

    const auto in_size = input.size();

    for(auto output : outputs) {

      const auto out_size = output.size();
      const auto output_is_big_enough = (out_size >= in_size);

      const auto output_is_equal_to = [&] (auto&&... args) {
        return (output.compare(std::forward<decltype(args)>(args)...) == 0);
      };

      if(output_is_big_enough and output_is_equal_to(0, in_size, input)) {
        front_duplicates[input].insert(output.substr(in_size));
      }

      if(output_is_big_enough and output_is_equal_to((out_size - in_size), output.npos, input)) {
        output.remove_suffix(in_size);
        back_duplicates[input].insert(output);
      }
    }
  }

  const auto& medicine = machine.medicine;

  const auto pairwise_iteration = [] (const auto& container, auto&& callback) {
    auto first1 = container.begin();
    auto first2 = (first1 + 1);
    const auto last = container.end();
    while(first2 != last) {
      callback(*first1++,*first2++);
    }
  };

  /*
   * We do a pairwise iteration through the medicine molecules and discard the
   * rules for the second element of the pair that have already been generated
   * by the rule for the first element.
   *
   * Example Rules (general | duplicate):
   *
   * P => PTi | P => Ti (front duplicate)
   * C => TiC | C => Ti (back duplicate)
   *
   * If our pair consists of molecules P and C (PC, in short), then the combination
   * PTiC will have been generated by the rules for P.
   *
   * At that point we can count all the duplicates found in both the front and back
   * lists and subtract it from the number of rules found for C and add it to the
   * number of unique molecules we have already found.
   */
  auto count_unique_molecules = [&, acc = rules.at(medicine.front()).size()] () mutable {

    pairwise_iteration(medicine, [&] (auto a, auto b) {

      if(rules.find(b) != rules.end()) {

        const auto& fda = front_duplicates[a];
        const auto& bdb = back_duplicates[b];

        const auto num_duplicates = std::count_if(bdb.begin(), bdb.end(), [&] (auto duplicate) {
          return fda.find(duplicate) != fda.end();
        });

        acc += (rules.at(b).size() - num_duplicates);
      }
    });

    return acc;
  };

  return count_unique_molecules();
}

auto solution(std::string_view input) {

  auto machine = parse(input);

  return num_unique_molecules(machine);
}

int main() {
  std::cout << solution(puzzle_input) << std::endl;
}