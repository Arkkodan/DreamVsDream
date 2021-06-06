#include <iostream>

#include <nlohmann/json.hpp>

int main(int argc, char* argv[]) {
    auto j_ex = nlohmann::json::parse(R"(
{
  "pi": 3.141,
  "happy": true,
  "name": "Niels",
  "nothing": null,
  "answer": {
    "everything": 42
  },
  "list": [1, 0, 2],
  "object": {
    "currency": "USD",
    "value": 42.99
  }
}
)");

    std::cout << j_ex.dump(4) << '\n';
}
