// Copyright 2019 Erik Teichmann <kontakt.teichmann@gmail.com>

#include <string>
#include <vector>

#include "test/catch.hpp"
#include "sam/options.hpp"

TEST_CASE("seting name of option", "[options]") {
  std::string name = "aa";
  std::string description = "Whatever";
  SECTION("ArgumentBase") {
    sam::ArgumentBase base(name, description);
    CHECK(base.GetName() == name);
  }
  SECTION("Argument") {
    double dummy_value;
    sam::Argument<double> argument(name, description, dummy_value, 0.0);
    CHECK(argument.GetName() == name);
  }
}

TEST_CASE("get value of double as string", "[options]") {
  double value = 1.5;
  sam::Argument<double> argument("aa", "foo", value, 0.);
  std::string value_string = std::to_string(value);
  CHECK(argument.GetValueAsString() == value_string);
}

TEST_CASE("get value of int as string", "[options]") {
  int value = 1;
  sam::Argument<int> argument("aa", "foo", value, 0);
  std::string value_string = std::to_string(value);
  CHECK(argument.GetValueAsString() == value_string);
}

TEST_CASE("get value of string as string", "[options]") {
  std::string value = "Test";
  sam::Argument<std::string> argument("aa", "foo", value, "A");
  CHECK(argument.GetValueAsString() == value);
}

TEST_CASE("get value of int vector as string", "[options]") {
  std::vector<int> value({1, 2, 3});
  sam::Argument<std::vector<int>> argument("aa", "foo", value,
    std::vector<int>({1}));
    std::string value_string = "1,2,3";
    CHECK(argument.GetValueAsString() == value_string);
}

TEST_CASE("get value of double vector as string", "[options]") {
  std::vector<double> value({1.3, 2.5});
  sam::Argument<std::vector<double>> argument("aa", "foo", value,
    std::vector<double>({1.}));
  std::string value_string = "1.3,2.5";
  CHECK(argument.GetValueAsString() == value_string);
}

TEST_CASE("parsing arguments", "[options]") {
  unsigned int N, N_0 = 10;
  double eps, eps_0 = 0.5;
  std::string filename, filename_0 = "a.csv";
  std::vector<std::unique_ptr<sam::ArgumentBase>> args;
  args.emplace_back(new sam::Argument<unsigned int>("oscillators", "N",
                    "number oscillators", N, N_0));
  args.emplace_back(new sam::Argument<double>("epsilon", "coupling", eps,
                                              eps_0));
  args.emplace_back(new sam::Argument<std::string>("filename", "output",
                                                   filename, filename_0));

    SECTION("set default values") {
      int argc = 1;
      char* argv[] = {"test"};
      sam::ParseArguments(argc, argv, args);
      CHECK(N == N_0);
      CHECK(eps == Approx(eps_0).margin(0.01));
      CHECK(filename == filename_0);
    }

    SECTION("set only one argument") {
      int argc = 3;
      char* argv[] = {"test", "-N", "50"};
      sam::ParseArguments(argc, argv, args);
      CHECK(N == 50);
      CHECK(eps == Approx(eps_0).margin(0.01));
      CHECK(filename == filename_0);
    }

    SECTION("set multiple arguments") {
      int argc = 7;
      char* argv[] = {"test", "-N", "50", "--filename", "b.csv",
                      "--epsilon", "0.5"};
      sam::ParseArguments(argc, argv, args);
      CHECK(N == 50);
      CHECK(eps == Approx(0.5).margin(0.01));
      CHECK(filename == "b.csv");
    }
}

TEST_CASE("double vector as argument", "[options]") {
  std::vector<double> test_vector;
  int argc = 4;
  char* argv[] = {"test", "--N", "2.5", "32"};
  std::vector<std::unique_ptr<sam::ArgumentBase>> args;
  args.emplace_back(new sam::Argument<std::vector<double>>(
      "N", "number oscillators", test_vector, std::vector<double>()));
  sam::ParseArguments(argc, argv, args);
  REQUIRE(test_vector.size() == 2);
  CHECK(test_vector[0] == Approx(2.5).margin(0.01));
  CHECK(test_vector[1] == Approx(32).margin(0.01));
}

TEST_CASE("double vector as argument default", "[options]") {
  std::vector<double> test_vector;
  int argc = 1;
  char* argv[] = {"test"};
  std::vector<std::unique_ptr<sam::ArgumentBase>> args;
  args.emplace_back(new sam::Argument<std::vector<double>>(
      "N", "number oscillators", test_vector, {1., 2.}));
  sam::ParseArguments(argc, argv, args);
  REQUIRE(test_vector.size() == 2);
  CHECK(test_vector[0] == Approx(1).margin(0.01));
  CHECK(test_vector[1] == Approx(2).margin(0.01));
}

TEST_CASE("unsigned vector as argument", "[options]") {
  std::vector<unsigned int> test_vector;
  int argc = 4;
  char* argv[] = {"test", "--N", "2", "32"};
  std::vector<std::unique_ptr<sam::ArgumentBase>> args;
  args.emplace_back(new sam::Argument<std::vector<unsigned int>>(
      "N", "number oscillators", test_vector, std::vector<unsigned int>()));
  sam::ParseArguments(argc, argv, args);
  REQUIRE(test_vector.size() == 2);
  CHECK(test_vector[0] == 2);
  CHECK(test_vector[1] == 32);
}