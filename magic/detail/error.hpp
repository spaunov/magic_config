#pragma once

#include "type2str.hpp"

#include <sstream>
#include <stdexcept>
#include <utility>

namespace magic_config { namespace detail {

// TODO(spaunov): Add ability to print out stack trace
template <typename Msg, typename Config, typename Exception = std::runtime_error>
void throw_error(Msg&& what, const Config* cfg)
{
    std::stringstream ss;

    ss << "ERROR: " << std::forward<Msg>(what);

    if (cfg) {
        ss << " while processing cfg: [\n" << *cfg << "\n]";
    }

    throw Exception(ss.str());
}

} // namespace detail

template <typename Traits, typename Msg>
inline void VERIFY_CONFIG(bool                           predicate,
                          Msg&&                          msg,
                          const typename Traits::Config* cfg = nullptr)
{
    if (!predicate) {
        detail::throw_error<>(std::forward<Msg>(msg), cfg);
    }
}

template <typename Traits, typename Msg, typename Class>
inline void VERIFY_CONFIG(bool                           predicate,
                          Msg&&                          msg,
                          const Class*                   obj,
                          const typename Traits::Config* cfg = nullptr)
{
    if (!predicate) {
        std::stringstream ss;

        ss << "[" << detail::type_to_str(*obj) << "] " << msg << std::endl;

        detail::throw_error<>(ss.str(), cfg);
    }
}

// Create and error msg
// NOTE: This is quite expensive and should only be used as part of initialization
template <typename... Args>
std::string make_error_msg(Args&&... args) {
    std::stringstream ss;

    (ss << ... << std::forward<Args>(args));

    return ss.str();
}

} // magic_config
