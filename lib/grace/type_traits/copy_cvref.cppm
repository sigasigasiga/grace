module;

#include <type_traits>

export module grace.type_traits:copy_cvref;

export namespace grace::type_traits {

template<typename From, typename To>
class copy_const
{
public:
    using type = To;
};

template<typename From, typename To>
class copy_const<From const, To>
{
public:
    using type = To const;
};

template<typename From, typename To>
using copy_const_t = copy_const<From, To>::type;

// -------------------------------------------------------------------------------------------------

template<typename From, typename To>
class copy_volatile
{
public:
    using type = To;
};

template<typename From, typename To>
class copy_volatile<From volatile, To>
{
public:
    using type = To volatile;
};

template<typename From, typename To>
using copy_volatile_t = copy_volatile<From, To>::type;

// -------------------------------------------------------------------------------------------------

template<typename From, typename To>
class copy_cv
{
public:
    using type = copy_const_t<From, copy_volatile_t<From, To>>;
};

template<typename From, typename To>
using copy_cv_t = copy_cv<From, To>::type;

// -------------------------------------------------------------------------------------------------

template<typename From, typename To>
class copy_lvalue_ref
{
public:
    using type = To;
};

template<typename From, typename To>
class copy_lvalue_ref<From &, To>
{
public:
    using type = To &;
};

template<typename From, typename To>
using copy_lvalue_ref_t = copy_lvalue_ref<From, To>::type;

// -------------------------------------------------------------------------------------------------

template<typename From, typename To>
class copy_rvalue_ref
{
public:
    using type = To;
};

template<typename From, typename To>
class copy_rvalue_ref<From &&, To>
{
public:
    using type = To &&;
};

template<typename From, typename To>
using copy_rvalue_ref_t = copy_rvalue_ref<From, To>::type;

// -------------------------------------------------------------------------------------------------

template<typename From, typename To>
class copy_ref
{
public:
    using type = copy_lvalue_ref_t<From, copy_rvalue_ref_t<From, To>>;
};

template<typename From, typename To>
using copy_ref_t = copy_ref<From, To>::type;

// -------------------------------------------------------------------------------------------------

template<typename From, typename To>
class copy_cvref
{
private:
    using from_unref_t = std::remove_reference_t<From>;
    using to_unref_t = std::remove_reference_t<To>;
    using to_cv_t = copy_cv_t<from_unref_t, to_unref_t>;
    using to_cv_ref_t = copy_ref_t<From, to_cv_t>;

public:
    using type = to_cv_ref_t;
};

template<typename From, typename To>
using copy_cvref_t = copy_cvref<From, To>::type;

} // namespace grace::type_traits
