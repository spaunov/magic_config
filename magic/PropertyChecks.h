#pragma once

#include <stdexcept>
#include <sstream>

namespace magic_config {

template <typename Class>
struct IPropertyCheck {
    virtual ~IPropertyCheck() {}

    // Check property for correctness and throw exception on error
    virtual void check(Class& obj) = 0;
};

template <typename Class, typename Member>
struct RangeCheck : IPropertyCheck<Class>
{
    // Range is inclusive by default, i.e. [min, max]
    // Setting inclusive to false makes the range exclusive, i.e. (min, max)
    RangeCheck(Member Class::*member, Member min, Member max, bool inclusive = true)
        : m_member(member)
        , m_min(min)
        , m_max(max)
        , m_inclusive(inclusive) {
    }

    // Check if the member value is in range
    void check(Class& obj) override;

private:
    Member Class::*m_member;
    Member         m_min;
    Member         m_max;
    bool           m_inclusive = true;
};

template <typename Class, typename Member>
struct CardinalityCheck : IPropertyCheck<Class>
{
    CardinalityCheck(Member Class::*member, size_t size)
        : m_member(member)
        , m_size(size) {
    }

    // Check if the size of the member matches the assigned cardinality
    void check(Class& obj) override;

private:
    Member Class::*m_member;
    size_t         m_size;
};

template <typename Class, typename Member>
struct CheckAction : IPropertyCheck<Class>
{
    using CheckFunc = std::function<void (const Member&)>;

    CheckAction(Member Class::*member, CheckFunc func)
        : m_member(member)
        , m_checkFunction(std::move(func)) {
    }

    // Check if the member value passes the user-defined property check
    void check(Class& obj) override { m_checkFunction(obj.*m_member); }

private:
    Member Class::*m_member;
    CheckFunc      m_checkFunction;
};


// =========================================================================
// IMPLEMENTATION:

template <typename Class, typename Member>
void RangeCheck<Class, Member>::check(Class& obj)
{
    auto const& value = obj.*m_member;

    bool minFailed = m_inclusive ? value < m_min : value <= m_min;
    bool maxFailed = m_inclusive ? value > m_max : value >= m_max;

    if (minFailed) {
        std::stringstream ss;
        ss << "member value [" << value
           << "] failed min [" << m_min << "] check!";
        throw std::range_error(ss.str());
    }

    if (maxFailed) {
        std::stringstream ss;
        ss << "member value [" << value
           << "] failed max [" << m_max << "] check!";
        throw std::range_error(ss.str());
    }
}


template <typename Class, typename Member>
void CardinalityCheck<Class, Member>::check(Class& obj)
{
    auto const& member = obj.*m_member;

    if (member.size() != m_size) {
        std::stringstream ss;
        ss << "member size [" << member.size()
           << "] differs from expected [" << m_size << "]";
        throw std::range_error(ss.str());
    }
}

}  // namespace magic_config
