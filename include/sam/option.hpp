// Copyright 2019 Erik Teichmann <kontakt.teichmann@gmail.com>

#ifndef INCLUDE_SAM_OPTION_HPP_
#define INCLUDE_SAM_OPTION_HPP_

#include <fstream>
#include <iostream>
#include <memory>
#include <vector>
#include <string>

#include <boost/program_options.hpp>

namespace std {

template<typename T>
  std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec) {
  for (auto item : vec) {
    os << item << " ";
  }
  return os;
}

std::string to_string(const std::string& value) {
  return value;
}

}  // namespace std

namespace sam {

/*!
 *  \brief Base struct for the definition of a vector holding all the arguments.
 */
struct ArgumentBase {
  std::string short_name_;
  std::string long_name_;
  std::string name_;
  std::string description_;

  /*!
   *  @param long_name Long name of the command line arguments (with --).
   *  @param short_name Short name of the command line arguments (with -).
   *  @param description Description of the command line argument.
   */
  ArgumentBase(std::string long_name, std::string short_name,
               std::string description)
      : short_name_(short_name), long_name_(long_name),
        description_(description) {name_ = long_name + "," + short_name;}

  /*!
   *  @param long_name Long name of the command line arguments (with --).
   *  @param description Description of the command line argument.
   */
  ArgumentBase(std::string long_name, std::string description)
      : long_name_(long_name), name_(long_name), description_(description) {}

  /*!
   *  Adds the arguments to the description.
   */
  virtual void AddArgument(boost::program_options::options_description& desc) {}

  /*!
   *  Parse the arguments and save the value to the variable. If no value was
   *  passed it will set the default value.
   */
  virtual void ParseArgument(boost::program_options::variables_map& vmap) {}

  /*!
   *  Convert the value to a string for printing.
   */
  virtual std::string GetValueAsString() {return std::string();}

  /*!
   *  Get the long name of the argument.
   */
  std::string GetName() {
    return long_name_;
  }
};


/*!
 * \brief Struct to hold the command line arguments.
 *
 *  The value of the argument will be saved to a given variable of the
 *  appropriate type. If the argument is not specified on the command line the
 *  value will be filled by some given default.
 */
template<typename T>
struct Argument: public ArgumentBase {
  T& value_;
  T default_value_;

  /*!
   *  @param long_name Long name of the command line arguments.
   *  @param description Description of the command line argument.
   *  @param value Variable holding the argument.
   *  @param default_value Default value of the argument.
   */
  Argument(std::string long_name, std::string description, T& value,
           T default_value)
      : ArgumentBase(long_name, description), value_(value),
        default_value_(default_value) {}

  /*!
   *  @param long_name Long name of the command line arguments.
   *  @param short_name Short name of the command line arguments.
   *  @param description Description of the command line argument.
   *  @param value Variable holding the argument.
   *  @param default_value Default value of the argument.
   */
  Argument(std::string long_name, std::string short_name,
           std::string description, T& value, T default_value)
      : ArgumentBase(long_name, short_name, description), value_(value),
        default_value_(default_value) {}

  /*!
   *  Adds the arguments to the description.
   */
  void AddArgument(boost::program_options::options_description& desc) {
    // Helper function to differentiate between the handling of values and
    // vectors.
    AddArgumentHelper(default_value_, desc);
  }

  /*!
   *  Parse the arguments and save the value to the variable. If no value was
   *  passed it will set the default value.
   */
  void ParseArgument(boost::program_options::variables_map& vmap) {
    if (vmap.count(long_name_)) {
      value_ = vmap[long_name_].as<T>();
    }
  }

  /*!
   *  Convert the value to a string for printing.
   */
  std::string GetValueAsString() {
    // Helper function to differentiate between the handling of values and
    // vectors.
    return ValueAsStringHelper(value_);
  }

 private:
  template<typename M>
  void AddArgumentHelper(M default_value,
                         boost::program_options::options_description& desc) {
    desc.add_options()(name_.c_str(),
                       boost::program_options::value<M>()->
                       default_value(default_value), description_.c_str());
  }

  template<typename M>
  void AddArgumentHelper(std::vector<M> default_value,
                         boost::program_options::options_description& desc) {
    desc.add_options()(name_.c_str(),
                       boost::program_options::value<std::vector<M>>()
                       ->multitoken()-> default_value(default_value),
                       description_.c_str());
    }

  template<typename M>
  std::string ValueAsStringHelper(M value) {
    return std::to_string(value);
  }

  template<typename M>
  std::string ValueAsStringHelper(std::vector<M> value) {
    std::stringstream ss;
    for (auto it = value.begin(); it != value.end(); ++it) {
      ss << (*it);
      if (it + 1 != value.end()) {
        ss << ",";
      }
    }
    return ss.str();
  }
};

/*!
  *  Parse the values from the command line arguments to the variables given as
  *  Arguments. This also build the interface of the help message. Should
  *  options be given that not exist in the arguments list an error will be
  *  thrown.
  *
  *  @param argc The total number of command line arguments.
  *  @param argv The given command line arguments.
  *  @param arguments The whole list of possible arguments.
  */
void ParseArguments(int argc, char* argv[],
                    std::vector<std::unique_ptr<ArgumentBase>>& arguments) {
  boost::program_options::options_description desc("Allowed options");
  desc.add_options()("help,h", "show this help message");
  for (auto it = arguments.begin(); it != arguments.end(); ++it) {
    (*it)->AddArgument(desc);
  }
  boost::program_options::variables_map vmap;
  boost::program_options::store(
    boost::program_options::parse_command_line(argc, argv, desc), vmap);
  boost::program_options::notify(vmap);
  if (vmap.count("help")) {
    std::cout << desc;
    exit(0);
  }
  for (auto it = arguments.begin(); it != arguments.end(); ++it) {
    (*it)->ParseArgument(vmap);
  }
}

/*!
 *  Write the arguments to the given file. The arguments will be written on a
 *  single line starting with a #. They are written in the form long_name=value
 *  and are separated by a space. Single values of a vector are separated by a
 *  comma.
 */
void WriteArgumentsToFile(std::vector<std::unique_ptr<ArgumentBase>>& arguments,
                          std::fstream& file) {
  file << "#";
  for (auto it = arguments.begin(); it != arguments.end(); ++it) {
    file << " " << (*it)->GetName() << "=" << (*it)->GetValueAsString();
  }
  file << "\n";
  file.flush();
}

}  // namespace sam

#endif  // INCLUDE_SAM_OPTIONS_HPP_