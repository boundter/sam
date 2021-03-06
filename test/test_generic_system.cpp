// Copyright 2019 Erik Teichmann <kontakt.teichmann@gmail.com>

#include <stdexcept>
#include <utility>
#include <vector>

#include "test/catch.hpp"
#include "test/harmonic_oscillator_ode.hpp"
#include "include/sam/system/generic_system.hpp"

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

  SECTION("can resize the system") {
    system.Resize(2);
    // check if state has correct size
    REQUIRE(system.GetPosition().size() == 4);
    // check if new state can be set
    std::vector<double> new_state = {1., 2., 3., 4.};
    REQUIRE_NOTHROW(system.SetPosition(new_state));
    std::vector<double> position = system.GetPosition();
    REQUIRE(position.size() == new_state.size());
    CHECK(position[0] == Approx(new_state[0]).margin(0.01));
    CHECK(position[1] == Approx(new_state[1]).margin(0.01));
    CHECK(position[2] == Approx(new_state[2]).margin(0.01));
    CHECK(position[3] == Approx(new_state[3]).margin(0.01));
    // check false lengths
    std::vector<double> too_short(3);
    std::vector<double> too_long(5);
    CHECK_THROWS_AS(system.SetPosition(too_short), std::length_error);
    CHECK_THROWS_AS(system.SetPosition(too_long), std::length_error);
  }

  SECTION("change parameters") {
    double new_omega = 8.3;
    system.SetParameters(new_omega);
    // indirect check using the derivative
      std::vector<double> initial {0.5, 0.1};
    system.SetPosition(initial);
    std::vector<double> derivative = system.GetDerivative();
    std::vector<double> position = system.GetPosition();
    REQUIRE(derivative.size() == dimension);
    CHECK(derivative[0] == Approx(0.1).margin(0.01));
    CHECK(derivative[1] == Approx(-34.445).margin(0.001));
    REQUIRE(position.size() == initial.size());
    CHECK(position[0] == Approx(initial[0]).margin(0.01));
    CHECK(position[1] == Approx(initial[1]).margin(0.01));
  }

  SECTION("get and set time") {
    REQUIRE(system.GetTime() == Approx(0).margin(0.001));
    double new_t = 1.5;
    system.SetTime(new_t);
    REQUIRE(system.GetTime() == Approx(new_t).margin(0.001));
  }

  SECTION("get dimensionality of the system") {
    std::pair<unsigned int, unsigned int> dim = system.GetDimension();
    REQUIRE(dim.first == N);
    REQUIRE(dim.second == dimension);
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
    sam::GenericSystem<CoupledHarmonicOscillatorODE> system(
        N, dimension, omega_1, omega_2, coupling);
    std::vector<double> position = {1., 2., 4., 5.};
    system.SetPosition(position);
    std::vector<double> derivative = system.GetDerivative();
    REQUIRE(derivative.size() == 4);
    CHECK(derivative[0] == Approx(position[1]).margin(0.01));
    CHECK(derivative[1] == Approx(-2.5).margin(0.01));
    CHECK(derivative[2] == Approx(position[3]).margin(0.01));
    CHECK(derivative[3] == Approx(-37.5).margin(0.01));
  }

  SECTION("multiple parameters of different types") {
    sam::GenericSystem<CoupledHarmonicOscillatorODE> system(
        N, dimension, omega, coupling);
    std::vector<double> position = {1., 2., 4., 5.};
    system.SetPosition(position);
    std::vector<double> derivative = system.GetDerivative();
    REQUIRE(derivative.size() == 4);
    CHECK(derivative[0] == Approx(position[1]).margin(0.01));
    CHECK(derivative[1] == Approx(-2.5).margin(0.01));
    CHECK(derivative[2] == Approx(position[3]).margin(0.01));
    CHECK(derivative[3] == Approx(-37.5).margin(0.01));
  }

  SECTION("change parameters") {
    std::vector<double> new_omega({4., 6.});
    double new_coupling = 0.;
    sam::GenericSystem<CoupledHarmonicOscillatorODE> system(
        N, dimension, omega, coupling);
    REQUIRE_NOTHROW(system.SetParameters(new_omega, new_coupling));
    std::vector<double> position = {1., 2., 4., 5.};
    system.SetPosition(position);
    std::vector<double> derivative = system.GetDerivative();
    REQUIRE(derivative.size() == 4);
    CHECK(derivative[0] == Approx(position[1]).margin(0.01));
    CHECK(derivative[1] == Approx(-16).margin(0.01));
    CHECK(derivative[2] == Approx(position[3]).margin(0.01));
    CHECK(derivative[3] == Approx(-144).margin(0.01));
  }
}

TEST_CASE("position in spherical coordinates 1d", "[generic_system]") {
  // use HarmonicOscillator as dummy ODE but with dimension 1
  double omega = 1;
  unsigned int N = 3;
  unsigned int d = 1;
  sam::GenericSystem<HarmonicOscillatorODE> system(N, d, omega);
  std::vector<double> x = {1., 2., 3.};
  system.SetPosition(x);
  std::vector<double> spherical = system.GetPositionSpherical();
  REQUIRE(spherical.size() == x.size());
  CHECK(spherical[0] == Approx(x[0]).margin(0.01));
  CHECK(spherical[1] == Approx(x[1]).margin(0.01));
  CHECK(spherical[2] == Approx(x[2]).margin(0.01));
}

TEST_CASE("position in spherical coordinates 2d", "[generic_system]") {
  // use HarmonicOscillatorODE as a dummy ODE with dimensionality 2
  double omega = 1;
  unsigned int N = 3;
  unsigned int d = 2;
  sam::GenericSystem<HarmonicOscillatorODE> system(N, d, omega);
  std::vector<double> x = {1., 2., 3., 4., 5., -6.};
  std::vector<double> analytical = {2.236, 1.107, 5, 0.927, 7.81, 5.407};
  system.SetPosition(x);
  std::vector<double> spherical = system.GetPositionSpherical();
  REQUIRE(spherical.size() == analytical.size());
  CHECK(spherical[0] == Approx(analytical[0]).margin(0.1));
  CHECK(spherical[1] == Approx(analytical[1]).margin(0.1));
  CHECK(spherical[2] == Approx(analytical[2]).margin(0.1));
  CHECK(spherical[3] == Approx(analytical[3]).margin(0.1));
  CHECK(spherical[4] == Approx(analytical[4]).margin(0.1));
  CHECK(spherical[5] == Approx(analytical[5]).margin(0.1));
}

TEST_CASE("mean field in 2d with 2 oscillators", "[generic_system]") {
  // use HarmonicOscillatorODE as a dummy ODE with dimensionality 2
  double omega = 1;
  unsigned int N = 2;
  unsigned int d = 2;
  sam::GenericSystem<HarmonicOscillatorODE> system(N, d, omega);
  std::vector<double> initial_condition = {0., 5., 1., -2.5};
  std::vector<double> analytical =  {0.5, 1.25};
  system.SetPosition(initial_condition);
  std::vector<double> numerical = system.CalculateMeanField();
  REQUIRE(numerical.size() == analytical.size());
  CHECK(numerical[0] == Approx(analytical[0]).margin(0.01));
  CHECK(numerical[1] == Approx(analytical[1]).margin(0.01));
}

TEST_CASE("spherical mean field in 1d", "[generic_system]") {
  // ODE will not be used, HarmonicOscillator as dummy
  double omega = 1.;
  sam::GenericSystem<HarmonicOscillatorODE> system(5, 1, omega);
  std::vector<double> x = {0., 0., 3*M_PI, 3*M_PI, M_PI/2.};
  std::vector<double> analytical = {0.2, M_PI/2.};
  system.SetPosition(x);
  std::vector<double> spherical = system.CalculateMeanFieldSpherical();
  REQUIRE(spherical.size() == analytical.size());
  CHECK(spherical[0] == Approx(analytical[0]).margin(0.01));
  CHECK(spherical[1] == Approx(analytical[1]).margin(0.01));
}

TEST_CASE("spherical mean field in 2d", "[generic_system]") {
  // ODE will not be used, HarmonicOscillator as dummy
  double omega = 1.;
  sam::GenericSystem<HarmonicOscillatorODE> system(4, 2, omega);
  std::vector<double> x = {0., 5., 3., 2., 1., 3., 7., 8.};
  std::vector<double> analytical = {5.27, 1.0222};
  system.SetPosition(x);
  std::vector<double> spherical = system.CalculateMeanFieldSpherical();
  REQUIRE(spherical.size() == analytical.size());
  CHECK(spherical[0] == Approx(analytical[0]).margin(0.1));
  CHECK(spherical[1] == Approx(analytical[1]).margin(0.1));
}

TEST_CASE("spherical mean field in 3d", "[generic_system]") {
  // ODE will not be used, HarmonicOscillator as dummy
  double omega = 1.;
  sam::GenericSystem<HarmonicOscillatorODE> system(3, 3, omega);
  std::vector<double> x = {0., 1., 5., 4., 2., 7., 5., 2., 4.};
  std::vector<double> analytical = {6.342, 1.078, 1.2679};
  system.SetPosition(x);
  std::vector<double> spherical = system.CalculateMeanFieldSpherical();
  REQUIRE(spherical.size() == analytical.size());
  CHECK(spherical[0] == Approx(analytical[0]).margin(0.1));
  CHECK(spherical[1] == Approx(analytical[1]).margin(0.1));
  CHECK(spherical[2] == Approx(analytical[2]).margin(0.1));
}

TEST_CASE("copy system", "[generic_system]") {
  double omega = 2.;
  sam::GenericSystem<HarmonicOscillatorODE> system(1, 2, omega);
  double t = 5.;
  std::vector<double> x({1, 1});
  std::vector<double> derivative({1, -4});
  system.SetTime(t);
  system.SetPosition(x);
  double system_time = system.GetTime();
  std::vector<double> system_pos = system.GetPosition();
  std::vector<double> system_derivative = system.GetDerivative();
  CHECK(system_time == Approx(t).margin(0.0001));
  REQUIRE(system_pos.size() == 2);
  CHECK(system_pos[0] == Approx(x[0]).margin(0.0001));
  CHECK(system_pos[1] == Approx(x[1]).margin(0.0001));
  REQUIRE(system_derivative.size() == 2);
  CHECK(system_derivative[0] == Approx(derivative[0]).margin(0.0001));
  CHECK(system_derivative[1] == Approx(derivative[1]).margin(0.0001));

  SECTION("copy works") {
    sam::GenericSystem<HarmonicOscillatorODE> copy_system = system;
    double copy_time = copy_system.GetTime();
    std::vector<double> copy_pos = copy_system.GetPosition();
    std::vector<double> copy_derivative = copy_system.GetDerivative();
    CHECK(copy_time == Approx(t).margin(0.0001));
    REQUIRE(copy_pos.size() == 2);
    CHECK(copy_pos[0] == Approx(x[0]).margin(0.0001));
    CHECK(copy_pos[1] == Approx(x[1]).margin(0.0001));
    REQUIRE(copy_derivative.size() == 2);
    CHECK(copy_derivative[0] == Approx(derivative[0]).margin(0.0001));
    CHECK(copy_derivative[1] == Approx(derivative[1]).margin(0.0001));
  }

  SECTION("deep copy evovles independent of original") {
    sam::GenericSystem<HarmonicOscillatorODE> copy_system = system;
    double new_time = 6.;
    std::vector<double> new_pos({2, 2});
    std::vector<double> new_derivative({2, -8});
    copy_system.SetTime(new_time);
    copy_system.SetPosition(new_pos);

    double copy_time = copy_system.GetTime();
    std::vector<double> copy_pos = copy_system.GetPosition();
    std::vector<double> copy_derivative = copy_system.GetDerivative();
    CHECK(copy_time == Approx(new_time).margin(0.0001));
    REQUIRE(copy_pos.size() == 2);
    CHECK(copy_pos[0] == Approx(new_pos[0]).margin(0.0001));
    CHECK(copy_pos[1] == Approx(new_pos[1]).margin(0.0001));
    REQUIRE(copy_derivative.size() == 2);
    CHECK(copy_derivative[0] == Approx(new_derivative[0]).margin(0.0001));
    CHECK(copy_derivative[1] == Approx(new_derivative[1]).margin(0.0001));

    double system_time = system.GetTime();
    std::vector<double> system_pos = system.GetPosition();
    std::vector<double> system_derivative = system.GetDerivative();
    CHECK(system_time == Approx(t).margin(0.0001));
    REQUIRE(system_pos.size() == 2);
    CHECK(system_pos[0] == Approx(x[0]).margin(0.0001));
    CHECK(system_pos[1] == Approx(x[1]).margin(0.0001));
    REQUIRE(system_derivative.size() == 2);
    CHECK(system_derivative[0] == Approx(derivative[0]).margin(0.0001));
    CHECK(system_derivative[1] == Approx(derivative[1]).margin(0.0001));
  }

  SECTION("ODE independent of original system") {
    sam::GenericSystem<HarmonicOscillatorODE> copy_system = system;
    double new_omega = 3.;
    copy_system.SetParameters(new_omega);

    std::vector<double> new_derivative({1, -9});
    double copy_time = copy_system.GetTime();
    std::vector<double> copy_pos = copy_system.GetPosition();
    std::vector<double> copy_derivative = copy_system.GetDerivative();
    CHECK(copy_time == Approx(t).margin(0.0001));
    REQUIRE(copy_pos.size() == 2);
    CHECK(copy_pos[0] == Approx(x[0]).margin(0.0001));
    CHECK(copy_pos[1] == Approx(x[1]).margin(0.0001));
    REQUIRE(copy_derivative.size() == 2);
    CHECK(copy_derivative[0] == Approx(new_derivative[0]).margin(0.0001));
    CHECK(copy_derivative[1] == Approx(new_derivative[1]).margin(0.0001));

    double system_time = system.GetTime();
    std::vector<double> system_pos = system.GetPosition();
    std::vector<double> system_derivative = system.GetDerivative();
    CHECK(system_time == Approx(t).margin(0.0001));
    REQUIRE(system_pos.size() == 2);
    CHECK(system_pos[0] == Approx(x[0]).margin(0.0001));
    CHECK(system_pos[1] == Approx(x[1]).margin(0.0001));
    REQUIRE(system_derivative.size() == 2);
    CHECK(system_derivative[0] == Approx(derivative[0]).margin(0.0001));
    CHECK(system_derivative[1] == Approx(derivative[1]).margin(0.0001));
  }
}
