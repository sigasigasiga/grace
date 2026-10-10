import grace.bit;

int main()
{
    // the last one in the pack is the first one in the bitset, as in `bitset`'s constructor
    constexpr auto b = grace::bit::make_bitset(1, 1, 0, 0, 1, 0, 0);
    static_assert(b[6] == 1);
    static_assert(b[5] == 1);
    static_assert(b[4] == 0);
    static_assert(b[3] == 0);
    static_assert(b[2] == 1);
    static_assert(b[1] == 0);
    static_assert(b[0] == 0);
}
