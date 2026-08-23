#include <concepts>
#include <memory>

import grace.memory;

using namespace grace::memory;

// ============================================================================
// esft_common_base: simple (single) inheritance
// ============================================================================

struct simple_esft : public esft_common_base
{};

void esft_simple_inheritance() {
    auto sp = std::make_shared<simple_esft>();

    auto sft = sp->shared_from_this();
    static_assert(std::same_as<decltype(sft), std::shared_ptr<simple_esft>>);
    if (sft.get() != sp.get()) {
        throw esft_simple_inheritance;
    }
    if (sp.use_count() != 2) {
        throw esft_simple_inheritance;
    }

    auto wft = sp->weak_from_this();
    static_assert(std::same_as<decltype(wft), std::weak_ptr<simple_esft>>);
    if (wft.lock().get() != sp.get()) {
        throw esft_simple_inheritance;
    }
}

void esft_weak_from_this_on_non_owned_object() {
    simple_esft obj;

    // an object that is not managed by a `shared_ptr` cannot produce a valid weak reference
    auto wft = obj.weak_from_this();
    if (!wft.expired()) {
        throw esft_weak_from_this_on_non_owned_object;
    }
}

// ============================================================================
// esft_common_base: virtual (diamond) inheritance
// ============================================================================

struct esft_virtual_base1 : public virtual esft_common_base
{};

struct esft_virtual_base2 : public virtual esft_common_base
{};

struct esft_diamond : public esft_virtual_base1
                    , public esft_virtual_base2
{};

void esft_virtual_inheritance() {
    auto sp = std::make_shared<esft_diamond>();

    auto sft = sp->shared_from_this();
    static_assert(std::same_as<decltype(sft), std::shared_ptr<esft_diamond>>);
    if (sft.get() != sp.get()) {
        throw esft_virtual_inheritance;
    }
    if (sp.use_count() != 2) {
        throw esft_virtual_inheritance;
    }

    auto wft = sp->weak_from_this();
    static_assert(std::same_as<decltype(wft), std::weak_ptr<esft_diamond>>);
    if (wft.lock().get() != sp.get()) {
        throw esft_virtual_inheritance;
    }

    std::shared_ptr<esft_virtual_base1> sp1 = sp;
    auto sft1 = sp1->shared_from_this();
    static_assert(std::same_as<decltype(sft1), std::shared_ptr<esft_virtual_base1>>);
    if (sft1.get() != sp1.get()) {
        throw esft_virtual_inheritance;
    }

    std::shared_ptr<esft_virtual_base2> sp2 = sp;
    auto sft2 = sp2->shared_from_this();
    static_assert(std::same_as<decltype(sft2), std::shared_ptr<esft_virtual_base2>>);
    if (sft2.get() != sp2.get()) {
        throw esft_virtual_inheritance;
    }
}

// ============================================================================
// shared_from_this_base: simple (single) inheritance
// ============================================================================

struct simple_sftb : public shared_from_this_base
{
    explicit simple_sftb(sftb_tag tag) : shared_from_this_base(tag) {}
};

void sftb_simple_inheritance() {
    auto sp = make_shared<simple_sftb>();
    static_assert(std::same_as<decltype(sp), std::shared_ptr<simple_sftb>>);

    auto sft = sp->shared_from_this();
    static_assert(std::same_as<decltype(sft), std::shared_ptr<simple_sftb>>);
    if (sft.get() != sp.get()) {
        throw sftb_simple_inheritance;
    }
    if (sp.use_count() != 2) {
        throw sftb_simple_inheritance;
    }

    auto wft = sp->weak_from_this();
    static_assert(std::same_as<decltype(wft), std::weak_ptr<simple_sftb>>);
    if (wft.lock().get() != sp.get()) {
        throw sftb_simple_inheritance;
    }
}

// a further-derived class must forward the tag to its base
struct sftb_grandchild : public simple_sftb
{
    explicit sftb_grandchild(sftb_tag tag, int, int) : simple_sftb(tag) {}
};

void sftb_multilevel_inheritance() {
    auto sp = make_shared<sftb_grandchild>(0, 0);
    static_assert(std::same_as<decltype(sp), std::shared_ptr<sftb_grandchild>>);

    auto sft = sp->shared_from_this();
    static_assert(std::same_as<decltype(sft), std::shared_ptr<sftb_grandchild>>);
    if (sft.get() != sp.get()) {
        throw sftb_multilevel_inheritance;
    }

    std::shared_ptr<simple_sftb> base_sp = sp;
    auto base_sft = base_sp->shared_from_this();
    static_assert(std::same_as<decltype(base_sft), std::shared_ptr<simple_sftb>>);
    if (base_sft.get() != base_sp.get()) {
        throw sftb_multilevel_inheritance;
    }
}

// constructor arguments other than the tag must be forwarded through `make_shared`
struct sftb_with_args : public shared_from_this_base
{
    int value;

    sftb_with_args(sftb_tag tag, int v) : shared_from_this_base(tag), value(v) {}

    explicit sftb_with_args(sftb_tag tag, int v, int, int)
        : shared_from_this_base(tag), value(v) {}
};

void sftb_extra_constructor_args() {
    auto sp = make_shared<sftb_with_args>(42);
    if (sp->value != 42) {
        throw sftb_extra_constructor_args;
    }

    auto sp2 = make_shared<sftb_with_args>(69, 420, 80085);
    if (sp2->value != 69) {
        throw sftb_extra_constructor_args;
    }
}

// ============================================================================
// shared_from_this_base: virtual (diamond) inheritance
// ============================================================================

struct sftb_virtual_base1 : public virtual shared_from_this_base
{
    explicit sftb_virtual_base1(sftb_tag tag) : shared_from_this_base(tag) {}
};

struct sftb_virtual_base2 : public virtual shared_from_this_base
{
    explicit sftb_virtual_base2(sftb_tag tag) : shared_from_this_base(tag) {}
};

struct sftb_diamond : public sftb_virtual_base1
                    , public sftb_virtual_base2
{
    explicit sftb_diamond(sftb_tag tag)
        : shared_from_this_base(tag)
        , sftb_virtual_base1(tag)
        , sftb_virtual_base2(tag)
    {}
};

void sftb_virtual_inheritance() {
    auto sp = make_shared<sftb_diamond>();
    static_assert(std::same_as<decltype(sp), std::shared_ptr<sftb_diamond>>);

    auto sft = sp->shared_from_this();
    static_assert(std::same_as<decltype(sft), std::shared_ptr<sftb_diamond>>);
    if (sft.get() != sp.get()) {
        throw sftb_virtual_inheritance;
    }
    if (sp.use_count() != 2) {
        throw sftb_virtual_inheritance;
    }

    auto wft = sp->weak_from_this();
    static_assert(std::same_as<decltype(wft), std::weak_ptr<sftb_diamond>>);
    if (wft.lock().get() != sp.get()) {
        throw sftb_virtual_inheritance;
    }

    std::shared_ptr<sftb_virtual_base1> sp1 = sp;
    auto sft1 = sp1->shared_from_this();
    static_assert(std::same_as<decltype(sft1), std::shared_ptr<sftb_virtual_base1>>);
    if (sft1.get() != sp1.get()) {
        throw sftb_virtual_inheritance;
    }

    auto wft1 = sp1->weak_from_this();
    static_assert(std::same_as<decltype(wft1), std::weak_ptr<sftb_virtual_base1>>);
    if (wft1.lock().get() != sp1.get()) {
        throw sftb_virtual_inheritance;
    }

    std::shared_ptr<sftb_virtual_base2> sp2 = sp;
    auto sft2 = sp2->shared_from_this();
    static_assert(std::same_as<decltype(sft2), std::shared_ptr<sftb_virtual_base2>>);
    if (sft2.get() != sp2.get()) {
        throw sftb_virtual_inheritance;
    }

    auto wft2 = sp2->weak_from_this();
    static_assert(std::same_as<decltype(wft2), std::weak_ptr<sftb_virtual_base2>>);
    if (wft2.lock().get() != sp2.get()) {
        throw sftb_virtual_inheritance;
    }
}

// ============================================================================
// universal_make_shared: plain (non-sftb) types must still work
// ============================================================================

struct plain_type
{
    int value;

    explicit plain_type(int v) : value(v) {}
};

void universal_make_shared_plain_type() {
    auto sp = make_shared<plain_type>(42);
    static_assert(std::same_as<decltype(sp), std::shared_ptr<plain_type>>);
    if (sp->value != 42) {
        throw universal_make_shared_plain_type;
    }
}

int main() {
    esft_simple_inheritance();
    esft_weak_from_this_on_non_owned_object();
    esft_virtual_inheritance();

    sftb_simple_inheritance();
    sftb_multilevel_inheritance();
    sftb_extra_constructor_args();
    sftb_virtual_inheritance();

    universal_make_shared_plain_type();
}
