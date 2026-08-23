module;

#include <memory>

export module grace.memory:shared_from_this_base;

export namespace grace::memory {

// multiple inheritance from esft-enabled classes is a pain:
// 1. the class will have excess `weak_ptr`s in it
// 2. `.shared_from_this()` won't be callable directly
//
// `esft_common_base` solves these problems by letting its users inherit virtually and
// providing `shared_from_this` and `weak_from_this` that are implemented with deducing this
class esft_common_base : public std::enable_shared_from_this<esft_common_base>
{
protected:
    constexpr esft_common_base() = default;
    esft_common_base(const esft_common_base &) = default;
    esft_common_base(esft_common_base &&) = default;
    ~esft_common_base() = default;

public:
    [[nodiscard]] auto shared_from_this(this auto &&self) { return self.make_sft(); }
    [[nodiscard]] auto weak_from_this(this auto &&self) noexcept { return self.make_wft(); }

private:
    template<typename Self>
    auto raw_sft(this Self &self)
    {
        return self.enable_shared_from_this::shared_from_this();
    }

    template<typename Self>
    std::shared_ptr<Self> make_sft(this Self &self)
    {
        // NB: must use `dynamic_cast` because `static_cast` does not work with virtual inheritance
        return std::dynamic_pointer_cast<Self>(self.raw_sft());
    }

    template<typename Self>
    auto raw_wft(this Self &self) noexcept
    {
        return self.enable_shared_from_this::weak_from_this();
    }

    template<typename Self>
    std::weak_ptr<Self> make_wft(this Self &self) noexcept
    {
        // NB: must use `dynamic_cast` because `static_cast` does not work with virtual inheritance
        // NB: `weak_from_this` should never throw, thus it cannot be delegated to `make_sft`
        return std::dynamic_pointer_cast<Self>(self.raw_wft().lock());
    }
};

// `std::enable_shared_from_this` requires its derived classes (let's call one `A`)
// to have some sort of static method that would construct an instance of the class.
//
// this approach has some problems:
// 1. boilerplate
// 2. if we want to derive `B` from `A`, `A::make` will still be available by default
// 3. it makes `std::make_shared` usage tricky because of `A`'s private constructor
//
// `shared_from_this_base` solves these problems:
// 1. the only thing that sftb-derived classes need to do
//    is to put `sftb_tag` as the first constructor argument and init the base class with it
// 2. classes that are derived from `A` also need to put `sftb_tag` as the first constructor arg,
//    and then they should forward it to the derived class
// 3. `memory::make_shared` becomes the only way to construct it and it is optimal
//
// it is also derived from `esft_common_base` to solve the problem with multiple inheritance
class shared_from_this_base;

template<typename T>
concept sftb_makeable = std::convertible_to<T *, const volatile shared_from_this_base *>;

class shared_from_this_base : public esft_common_base
{
public:
    template<sftb_makeable T>
    class make_shared_with_sftb_tag;

    class sftb_tag
    {
    private:
        constexpr explicit sftb_tag() = default;

        template<sftb_makeable T>
        friend class make_shared_with_sftb_tag;
    };

    // TODO: should we mark `shared_from_this` as `noexcept`?

protected:
    constexpr explicit shared_from_this_base(sftb_tag) noexcept {}
    shared_from_this_base(const shared_from_this_base &) = default;
    shared_from_this_base(shared_from_this_base &&) = default;
    ~shared_from_this_base() = default;
};

template<sftb_makeable T>
class shared_from_this_base::make_shared_with_sftb_tag
{
private:
    using sftb_tag = shared_from_this_base::sftb_tag;

public:
    template<typename... Args>
    requires std::constructible_from<T, sftb_tag, Args &&...>
    [[nodiscard]] static auto operator()(Args &&...args)
    {
        return std::make_shared<T>(sftb_tag{}, std::forward<Args>(args)...);
    }
};

template<typename T>
class universal_make_shared
{
public:
    template<typename... Args>
    requires std::constructible_from<T, Args &&...>
    [[nodiscard]] static auto operator()(Args &&...args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
};

template<sftb_makeable T>
class universal_make_shared<T> : public shared_from_this_base::make_shared_with_sftb_tag<T>
{};

template<typename T>
inline constexpr universal_make_shared<T> make_shared;

} // namespace grace::memory
