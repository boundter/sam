// Copyright 2019 Erik Teichmann <kontakt.teichmann@gmail.com>

#include <stdexcept>
#include <vector>

#include "test/catch.hpp"
#include "test/harmonic_oscillator_ode.hpp"
#include "include/sam/systems/generic_system.hpp"

TEST_CASE("simple system", "[generic_system]") {
  double omega = 2.;
  unsigned int N = 1;
  unsigned int dimension = 2;
  sam::GenericSystem<HarmonicOscillatorODE> system(N, dimension, omega);

  SECTION("state initializes to 0") {
    std::vector<double> position = system.GetPosition();
    REQUIRE(position.size() == dimension);
    CHECK(position[0] == Approx(0.).margin(0.01));
    CHECK(position[1] == Approx(0.).margin(0.01));
  }

  SECTION("set position changes internal position") {
    std::vector<double> new_state({0.5, 0.1});
    system.SetPosition(new_state);
    std::vector<double> position = system.GetPosition();
    REQUIRE(position.size() == dimension);
    CHECK(position[0] == Approx(0.5).margin(0.01));
    CHECK(position[1] == Approx(0.1).margin(0.01));
  }

  SECTION("cannot set position with wrong size") {
    std::vector<double> too_long = {0.3, 0.1, 6.0};
    std::vector<double> too_short = {0.1};
    CHECK_THROWS_AS(system.SetPosition(too_long), std::length_error);
    CHECK_THROWS_AS(system.SetPosition(too_short), std::length_error);
  }

  SECTION("return the derivative without integrating") {
    std::vector<double> initial {0.5, 0.1};
    system.SetPosition(initial);
    std::vector<double> derivative = system.GetDerivative();
    std::vector<double> position = system.GetPosition();
    REQUIRE(derivative.size() == dimension);
    CHECK(derivative[0] == Approx(0.1).margin(0.01));
    CHECK(derivative[1] == Approx(-2.).margin(0.01));
    REQUIRE(position.size() == initial.size());
    CHECK(position[0] == Approx(initial[0]).margin(0.01));
    CHECK(position[1] == Approx(initial[1]).margin(0.01));
  }
}


TEST_CASE("ODE with multiple parameters", "[generic_system]") {
  double omega_1 = 2.;
  double omega_2 = 3.;
  std::vector<double> omega({omega_1, omega_2});
  double coupling = 0.5;
  unsigned int N = 2;
  unsigned int dimension = 2;

  SECTION("multiple parameters of same type") {
    // TODO(boundter): Check if correctly set
    REQUIRE_NOTHROW(sam::GenericSystem<CoupledHarmonicOscillatorODE>(
          N, dimension, omega_1, omega_2, coupling));
  }

  SECTION("multiple parameters of different types") {
  // TODO(boundter): Check if correctly set
  REQUIRE_NOTHROW(sam::GenericSystem<CoupledHarmonicOscillatorODE>(
        N, dimension, omega, coupling));
  }
}
