#pragma once

// http://stackoverflow.com/questions/281818/unmangling-the-result-of-stdtype-infoname

#include <iostream>
#include <string>
#include <typeinfo>

#ifdef __GNUG__
#include <cstdlib>
#include <memory>
#include <cxxabi.h>
#endif

namespace magic_config { namespace detail {

std::string demangle(const char* name);

template <typename... Ts>
struct resolve_type;

template <class T>
inline std::string type_to_str(const T& t) {
   return demangle(typeid(t).name());
}

template <class T>
inline std::string type_to_str() {
   return demangle(typeid(T).name());
}

template <class T>
inline void print_type(const T& t) {
   std::cout << type_to_str(t) << std::endl;
}

template <class T>
inline void print_type() {
   std::cout << type_to_str<T>() << std::endl;
}

#ifdef __GNUG__

inline std::string demangle(const char* name) {
   int status = -4; // some arbitrary value to eliminate the compiler warning

   // enable c++11 by passing the flag -std=c++11 to g++
   std::unique_ptr<char, void(*)(void*)> res {
       abi::__cxa_demangle(name, NULL, NULL, &status),
       std::free
   };

   return (status == 0) ? res.get() : name;
}

#else

// does nothing if not g++
std::string demangle(const char* name) {
   return name;
}

#endif

}}  // namespace magic_config::detail
