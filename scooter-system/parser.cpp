#include <iostream>
#include <string>
#include <vector>

std::vector<std::string> parse_comands(const std::string& s) {
  size_t j = 0;
  std::vector<std::string> parsed_comands;
  std::cout << "Parsing str: " << s << '\n';
  for (int i = 0; i < s.size(); ++i) {
    if (s[i] == ':') {
      parsed_comands.push_back(s.substr(j, i - j));
      j = i + 1;
    }
  }
  parsed_comands.push_back(s.substr(j));
  std::cout << "Into: ";
  for (auto com: parsed_comands) {
    std::cout << com << '|';
  }
  std::cout << '\n';
  return parsed_comands;
}