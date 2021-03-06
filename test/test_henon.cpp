// Copyright 2020 Erik Teichmann <kontakt.teichmann@gmail.com>

#include <cmath>
#include <utility>
#include <vector>

#include "test/catch.hpp"
#include "test/harmonic_oscillator_ode.hpp"
#include "include/sam/analysis/henon.hpp"
#include "include/sam/system/rk4_system.hpp"

TEST_CASE("single harmonic oscillator") {
  // omega = 1 -> T = 2*pi and crossing y-axis happens at pi/4
  sam::CrossingParameters params;
  double omega = 1;
  unsigned int N = 1;
  unsigned int dimension = 2;
  double dt = 0.01;
  std::vector<double> initial({1., 0.});
  sam::RK4System<HarmonicOscillatorODE> system(N, dimension, omega);
  system.SetPosition(initial);

  SECTION("first dimension") {
    unsigned int n = static_cast<unsigned int>(M_PI/2./dt);
    system.Integrate(dt, n);
    std::pair<double, std::vector<double>> crossing = HenonTrick(system,
                                                                 params);
    REQUIRE(crossing.first == Approx(M_PI/2.).margin(0.0001));
    REQUIRE(crossing.second[0] == Approx(0).margin(0.001));
    REQUIRE(crossing.second[1] == Approx(-1).margin(0.001));
  }

  SECTION("second dimension") {
    unsigned int n = static_cast<unsigned int>(M_PI/dt);
    system.Integrate(dt, n);
    params.dimension = 1;
    std::pair<double, std::vector<double>> crossing = HenonTrick(system,
                                                                 params);
    REQUIRE(crossing.first == Approx(M_PI).margin(0.0001));
    REQUIRE(crossing.second[0] == Approx(-1).margin(0.001));
    REQUIRE(crossing.second[1] == Approx(0).margin(0.001));
  }

  SECTION("full integration first dimension") {
    std::pair<double, std::vector<double>> crossing =
      IntegrateToCrossing(system, dt, params);
    REQUIRE(crossing.first == Approx(M_PI/2.).margin(0.0001));
    REQUIRE(crossing.second[0] == Approx(0).margin(0.001));
    REQUIRE(crossing.second[1] == Approx(-1).margin(0.001));
  }

  SECTION("full integration first dimension with condition") {
    std::pair<double, std::vector<double>> crossing =
      IntegrateToCrossingConditional(system, dt,
          [](std::vector<double> x) { return x[1] > 0; }, params);
    REQUIRE(crossing.first == Approx(3.*M_PI/2.).margin(0.0001));
    REQUIRE(crossing.second[0] == Approx(0).margin(0.001));
    REQUIRE(crossing.second[1] == Approx(1).margin(0.001));
  }

  SECTION("full integration second dimension") {
    // get a short way away from the crossing
    params.dimension = 1;
    system.Integrate(dt, 1);
    std::pair<double, std::vector<double>> crossing =
      IntegrateToCrossing(system, dt, params);
    REQUIRE(crossing.first == Approx(M_PI).margin(0.0001));
    REQUIRE(crossing.second[0] == Approx(-1).margin(0.001));
    REQUIRE(crossing.second[1] == Approx(0).margin(0.001));
  }

  SECTION("full integration second dimension with condition") {
    // get a short way away from the crossing
    params.dimension = 1;
    system.Integrate(dt, 1);
    std::pair<double, std::vector<double>> crossing =
      IntegrateToCrossingConditional(system, dt,
          [](std::vector<double> x) { return x[0] > 0; }, params);
    REQUIRE(crossing.first == Approx(2.*M_PI).margin(0.0001));
    REQUIRE(crossing.second[0] == Approx(1).margin(0.001));
    REQUIRE(crossing.second[1] == Approx(0).margin(0.001));
  }
}

TEST_CASE("two uncoupled harmonic oscillators") {
  // omega = 1 -> T = 2*pi and crossing y-axis happens at pi/2
  sam::CrossingParameters params;
  std::vector<double> omega({1, 2});
  double coupling = 0;
  unsigned int N = 2;
  unsigned int dimension = 2;
  double dt = 0.01;
  std::vector<double> initial({1., 0., 1., 0.});
  sam::RK4System<CoupledHarmonicOscillatorODE> system(N, dimension, omega[0],
                                                      omega[1], coupling);
  system.SetPosition(initial);

  SECTION("first dimension first oscillator") {
    unsigned int n = static_cast<unsigned int>(M_PI/2./dt);
    system.Integrate(dt, n);
    std::pair<double, std::vector<double>> crossing = HenonTrick(system,
                                                                 params);
    REQUIRE(crossing.first == Approx(M_PI/2.).margin(0.0001));
    REQUIRE(crossing.second[0] == Approx(0).margin(0.001));
    REQUIRE(crossing.second[1] == Approx(-1).margin(0.001));
    REQUIRE(crossing.second[2] == Approx(-1).margin(0.001));
    REQUIRE(crossing.second[3] == Approx(0).margin(0.001));
  }

  SECTION("second dimension first oscillator") {
    params.dimension = 1;
    unsigned int n = static_cast<unsigned int>(M_PI/dt);
    system.Integrate(dt, n);
    std::pair<double, std::vector<double>> crossing = HenonTrick(system,
                                                                 params);
    REQUIRE(crossing.first == Approx(M_PI).margin(0.0001));
    REQUIRE(crossing.second[0] == Approx(-1).margin(0.001));
    REQUIRE(crossing.second[1] == Approx(0).margin(0.001));
    REQUIRE(crossing.second[2] == Approx(1).margin(0.001));
    REQUIRE(crossing.second[3] == Approx(0).margin(0.001));
  }

  SECTION("first dimension second oscillator") {
    params.n_osc = 1;
    unsigned int n = static_cast<unsigned int>(M_PI/4./dt);
    system.Integrate(dt, n);
    std::pair<double, std::vector<double>> crossing = HenonTrick(system,
                                                                 params);
    REQUIRE(crossing.first == Approx(M_PI/4.).margin(0.0001));
    REQUIRE(crossing.second[0] == Approx(0.707).margin(0.01));
    REQUIRE(crossing.second[1] == Approx(-0.707).margin(0.01));
    REQUIRE(crossing.second[2] == Approx(0).margin(0.001));
    REQUIRE(crossing.second[3] == Approx(-2).margin(0.001));
  }

  SECTION("second dimension second oscillator") {
    params.n_osc = 1;
    params.dimension = 1;
    unsigned int n = static_cast<unsigned int>(M_PI/2./dt);
    system.Integrate(dt, n);
    std::pair<double, std::vector<double>> crossing = HenonTrick(system,
                                                                 params);
    REQUIRE(crossing.first == Approx(M_PI/2.).margin(0.0001));
    REQUIRE(crossing.second[0] == Approx(0).margin(0.001));
    REQUIRE(crossing.second[1] == Approx(-1).margin(0.001));
    REQUIRE(crossing.second[2] == Approx(-1).margin(0.001));
    REQUIRE(crossing.second[3] == Approx(0).margin(0.001));
  }

  SECTION("full integration first dimension first oscillator") {
    std::pair<double, std::vector<double>> crossing =
      IntegrateToCrossing(system, dt, params);
    REQUIRE(crossing.first == Approx(M_PI/2.).margin(0.0001));
    REQUIRE(crossing.second[0] == Approx(0).margin(0.001));
    REQUIRE(crossing.second[1] == Approx(-1).margin(0.001));
    REQUIRE(crossing.second[2] == Approx(-1).margin(0.001));
    REQUIRE(crossing.second[3] == Approx(0).margin(0.001));
  }

  SECTION("full integration first dimension first oscillator") {
    std::pair<double, std::vector<double>> crossing =
      IntegrateToCrossingConditional(system, dt,
          [](std::vector<double> x) { return x[1] > 0; }, params);
    REQUIRE(crossing.first == Approx(3.*M_PI/2.).margin(0.0001));
    REQUIRE(crossing.second[0] == Approx(0).margin(0.001));
    REQUIRE(crossing.second[1] == Approx(1).margin(0.001));
    REQUIRE(crossing.second[2] == Approx(-1).margin(0.001));
    REQUIRE(crossing.second[3] == Approx(0).margin(0.001));
  }

  SECTION("full integration second dimension first oscillator") {
    params.dimension = 1;
    system.Integrate(dt, 1);
    std::pair<double, std::vector<double>> crossing =
      IntegrateToCrossing(system, dt, params);
    REQUIRE(crossing.first == Approx(M_PI).margin(0.0001));
    REQUIRE(crossing.second[0] == Approx(-1).margin(0.001));
    REQUIRE(crossing.second[1] == Approx(0).margin(0.001));
    REQUIRE(crossing.second[2] == Approx(1).margin(0.001));
    REQUIRE(crossing.second[3] == Approx(0).margin(0.001));
  }

  SECTION("full integration second dimension first oscillator") {
    params.dimension = 1;
    system.Integrate(dt, 1);
    std::pair<double, std::vector<double>> crossing =
      IntegrateToCrossingConditional(system, dt,
          [](std::vector<double> x) { return x[0] > 0; }, params);
    REQUIRE(crossing.first == Approx(2.*M_PI).margin(0.0001));
    REQUIRE(crossing.second[0] == Approx(1).margin(0.001));
    REQUIRE(crossing.second[1] == Approx(0).margin(0.001));
    REQUIRE(crossing.second[2] == Approx(1).margin(0.001));
    REQUIRE(crossing.second[3] == Approx(0).margin(0.001));
  }

  SECTION("full integration first dimension second oscillator") {
    params.n_osc = 1;
    std::pair<double, std::vector<double>> crossing =
      IntegrateToCrossing(system, dt, params);
    REQUIRE(crossing.first == Approx(M_PI/4.).margin(0.0001));
    REQUIRE(crossing.second[0] == Approx(0.707).margin(0.01));
    REQUIRE(crossing.second[1] == Approx(-0.707).margin(0.01));
    REQUIRE(crossing.second[2] == Approx(0).margin(0.001));
    REQUIRE(crossing.second[3] == Approx(-2).margin(0.001));
  }

  SECTION("full integration first dimension second oscillator") {
    params.n_osc = 1;
    std::pair<double, std::vector<double>> crossing =
      IntegrateToCrossingConditional(system, dt,
          [](std::vector<double> x) { return x[3] > 0; }, params);
    REQUIRE(crossing.first == Approx(3*M_PI/4.).margin(0.0001));
    REQUIRE(crossing.second[0] == Approx(-0.707).margin(0.001));
    REQUIRE(crossing.second[1] == Approx(-0.707).margin(0.001));
    REQUIRE(crossing.second[2] == Approx(0).margin(0.001));
    REQUIRE(crossing.second[3] == Approx(2).margin(0.001));
  }

  SECTION("full integration second dimension second oscillator") {
    params.n_osc = 1;
    params.dimension = 1;
    system.Integrate(dt, 1);
    std::pair<double, std::vector<double>> crossing =
      IntegrateToCrossing(system, dt, params);
    REQUIRE(crossing.first == Approx(M_PI/2.).margin(0.0001));
    REQUIRE(crossing.second[0] == Approx(0).margin(0.001));
    REQUIRE(crossing.second[1] == Approx(-1).margin(0.001));
    REQUIRE(crossing.second[2] == Approx(-1).margin(0.001));
    REQUIRE(crossing.second[3] == Approx(0).margin(0.001));
  }

  SECTION("full integration second dimension second oscillator") {
    params.n_osc = 1;
    params.dimension = 1;
    system.Integrate(dt, 1);
    std::pair<double, std::vector<double>> crossing =
      IntegrateToCrossingConditional(system, dt,
          [](std::vector<double> x) { return x[2] > 0; }, params);
    REQUIRE(crossing.first == Approx(M_PI).margin(0.0001));
    REQUIRE(crossing.second[0] == Approx(-1).margin(0.001));
    REQUIRE(crossing.second[1] == Approx(0).margin(0.001));
    REQUIRE(crossing.second[2] == Approx(1).margin(0.001));
    REQUIRE(crossing.second[3] == Approx(0).margin(0.001));
  }
}
