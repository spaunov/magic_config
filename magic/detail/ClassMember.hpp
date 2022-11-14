#pragma once

#include "traits.hpp"
#include "error.hpp"

#include "DispatchConverter.hpp"

namespace magic_config { namespace magic { namespace detail {

template <typename Traits, typename Class>
struct ClassMemberBase {
    using Config = typename Traits::Config;

    virtual ~ClassMemberBase() {}

    // load the value from the dynamic to a member of the class instance
    virtual void set(Class& obj, const Config& cfg) = 0;
};

template <typename Traits, typename Class, typename MemberType>
struct ClassMember : ClassMemberBase<Traits, Class>
{
    using Config = typename Traits::Config;

    ClassMember(MemberType Class::*member) : m_member(member) {}

    void set(Class& obj, const Config& cfg) override
    {
        if constexpr (magic_config::traits::is_optional_v<MemberType>) {
            using value_type = typename MemberType::value_type;

            obj.*m_member = convert<Traits, value_type>(cfg);
        }
        else {
            obj.*m_member = convert<Traits, MemberType>(cfg);
        }

        if constexpr (magic_config::traits::has_verify_config<Class, Config>::value) {
            bool ok = obj.verify_config(cfg);

            VERIFY_CONFIG<Traits>(ok, "Verification failed!", &obj, &cfg);
        }
    }

    MemberType Class::*m_member;
};

}}} // magic_config::magic::detail
