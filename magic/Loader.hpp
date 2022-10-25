#pragma once

#include "detail/ClassMember.hpp"
#include "detail/traits.hpp"
#include "detail/error.hpp"
#include "PropertyChecks.h"

#include <functional>
#include <map>
#include <memory>
#include <sstream>
#include <type_traits>
#include <vector>

namespace magic_config { namespace magic {

template <typename Derived, typename CTraits>
class Loader : detail::AggregateConfigTag
{
public:
    using Traits = CTraits;
    using Config = typename Traits::Config;

    // Load the contents of the Config into the class
    static Derived load(const Config& cfg);

protected:

    using Member         = detail::ClassMemberBase<Traits, Derived>;
    using MemberPtr      = std::unique_ptr< Member >;
    using PropertyCheck  = std::unique_ptr< IPropertyCheck<Derived> >;
    using PropertyChecks = std::vector<PropertyCheck>;

    struct MemberInfo {
        MemberPtr      member;
        bool           required = false;
        PropertyChecks propertyChecks;
    };

    using ClassMemberMap = std::map<std::string, MemberInfo>;

    static ClassMemberMap s_classMembers;

    // PropertyProxy makes it possible to set multiple properties on members
    template <typename MemberType>
    struct PropertyProxy;

    // ... and is a friend, so it can access protected member definitions
    template <typename MemberType>
    friend struct PropertyProxy;

    // Assign a name from the config to a 'member' of the class
    // MemberType deduces the type of the 'Class' member variable
    //
    // NOTE: Returns a PropertyProxy which enables setting additional properties

    template <typename MemberType>
    static PropertyProxy<MemberType> assign(const char* cfgName, MemberType Derived::*member);

    // NOTE: We need to properly handle pointers to base class member variables
    template <typename BaseClass, typename MemberType>
    static PropertyProxy<MemberType> assign(const char* cfgName, MemberType BaseClass::*member);
};


// =========================================================================
// IMPLEMENTATION:

template <typename Derived, typename CTraits>
typename Loader<Derived, CTraits>::ClassMemberMap Loader<Derived, CTraits>::s_classMembers;

template <typename Derived, typename CTraits>
template <typename MemberType>
struct Loader<Derived, CTraits>::PropertyProxy
{
    using iterator_t = typename ClassMemberMap::iterator;

    PropertyProxy(iterator_t it) : m_it(it) {}

    // Make member is required
    PropertyProxy& required() {
        m_it->second.required = true;
        return *this;
    }

    // Set a range of acceptable values for this member
    // Range is inclusive by default, i.e. [min, max]
    // Setting inclusive to false makes the range exclusive, i.e. (min, max)
    PropertyProxy& range(MemberType min, MemberType max, bool inclusive = true);

    // Set the expected cardinality for this member
    PropertyProxy& cardinality(size_t expectSize);

    // Attach a user-define PropertyCheck
    template <template <typename C, typename M> class UserDefinedCheck, typename... Args>
    PropertyProxy& attach(Args&&... args);

    // Attach a free function to perform a desived action of this member
    PropertyProxy& attach(std::function<void (const MemberType&)> action);

private:
    iterator_t m_it;
};


template <typename Derived, typename CTraits>
template <typename MemberType>
typename Loader<Derived, CTraits>::template PropertyProxy<MemberType>
Loader<Derived, CTraits>::assign(const char* cfgName, MemberType Derived::*member)
{
    using wrap_member = detail::ClassMember<Traits, Derived, MemberType>;

    MemberInfo info { std::make_unique<wrap_member>(member), false, PropertyChecks{} };

    auto result = s_classMembers.emplace(cfgName, std::move(info));

    VERIFY_CONFIG<Traits>(result.second, "Could not insert class memeber.");

    return PropertyProxy<MemberType>(result.first);
}

// NOTE: Needed to appropriately handle pointers to base class members
template <typename Derived, typename CTraits>
template <typename BaseClass, typename MemberType>
typename Loader<Derived, CTraits>::template PropertyProxy<MemberType>
Loader<Derived, CTraits>::assign(const char* cfgName, MemberType BaseClass::*member)
{
    VERIFY_CONFIG<Traits>((std::is_base_of<BaseClass, Derived>::value),
                          "Pointer to member does not belong to a base class");

    // Cast to member to derived and forward the call
    auto asDerived = static_cast<MemberType Derived::*>(member);
    return Loader<Derived, CTraits>::assign(cfgName, asDerived);
}

template <typename Derived, typename CTraits>
Derived Loader<Derived, CTraits>::load(const Config& cfg)
{
    static_assert(magic_config::traits::has_defineConfigMapping<Derived>::value,
                  "Derived must provide a static defineConfigMapping() method!");

    // Call defineConfigMapping() on the class to load config mapping
    if (s_classMembers.empty()) Derived::defineConfigMapping();

    VERIFY_CONFIG<Traits>(Traits::isComplex(cfg),
                          "Type mismatch: object expected!", &cfg);

    Derived result;

    for (auto& classMember : s_classMembers)
    {
        const std::string& name = classMember.first;

        auto& memberInfo = classMember.second;

        // Check if key is in the object dictionary:
        if (!Traits::isPresent(cfg, name)) {
            if (memberInfo.required) {
                std::stringstream ss;
                ss << "magic::Loader::load(): Missing key: " << name;
                VERIFY_CONFIG<Traits>(false, ss.str(), &cfg);
            }
            else {
                continue;
            }
        }

        memberInfo.member->set(result, Traits::get(cfg, name));

        for (auto& property : memberInfo.propertyChecks) {
            property->check(result);
        }
    }

    return result;
}


template <typename Derived, typename CTraits>
template <typename MemberType>
typename Loader<Derived, CTraits>::template PropertyProxy<MemberType>&
Loader<Derived, CTraits>::PropertyProxy<MemberType>::range(MemberType min, MemberType max, bool inclusive)
{
    auto& memberInfo = m_it->second;

    using Check       = RangeCheck<Derived, MemberType>;
    using wrap_member = detail::ClassMember<Traits, Derived, MemberType>;

    auto* wrapper = dynamic_cast<wrap_member*>(memberInfo.member.get());

    memberInfo.propertyChecks.emplace_back(
        std::make_unique<Check>(wrapper->m_member, min, max, inclusive));
    return *this;
}

template <typename Derived, typename CTraits>
template <typename MemberType>
typename Loader<Derived, CTraits>::template PropertyProxy<MemberType>&
Loader<Derived, CTraits>::PropertyProxy<MemberType>::cardinality(size_t expectSize)
{
    auto& memberInfo = m_it->second;

    using Check       = CardinalityCheck<Derived, MemberType>;
    using wrap_member = detail::ClassMember<Traits, Derived, MemberType>;

    auto* wrapper = dynamic_cast<wrap_member*>(memberInfo.member.get());

    memberInfo.propertyChecks.emplace_back(
                       std::make_unique<Check>(wrapper->m_member, expectSize));
    return *this;
}


template <typename Derived, typename CTraits>
template <typename MemberType>
template <template <typename C, typename M> class UserDefinedCheck, typename... Args>
typename Loader<Derived, CTraits>::template PropertyProxy<MemberType>&
Loader<Derived, CTraits>::PropertyProxy<MemberType>::attach(Args&&... args)
{
    auto& memberInfo = m_it->second;

    using UserPropertyCheck = UserDefinedCheck<Derived, MemberType>;
    using wrap_member       = detail::ClassMember<Traits, Derived, MemberType>;

    auto* wrapper = dynamic_cast<wrap_member*>(memberInfo.member.get());

    memberInfo.propertyChecks.emplace_back(
        new UserPropertyCheck(wrapper->m_member, std::forward<Args>(args)...));

    return *this;
}

template <typename Derived, typename CTraits>
template <typename MemberType>
typename Loader<Derived, CTraits>::template PropertyProxy<MemberType>&
Loader<Derived, CTraits>::PropertyProxy<MemberType>::attach(std::function<void (const MemberType&)> action)
{
    auto& memberInfo = m_it->second;

    using Check       = CheckAction<Derived, MemberType>;
    using wrap_member = detail::ClassMember<Traits, Derived, MemberType>;

    typename Check::CheckFunc checkFunc = std::move(action);

    auto* wrapper = dynamic_cast<wrap_member*>(memberInfo.member.get());

    memberInfo.propertyChecks.emplace_back(
        std::make_unique<Check>(wrapper->m_member, checkFunc));
    return *this;
}

}} // magic_config::magic
