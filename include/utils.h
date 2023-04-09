#ifndef _ZZC_UTILS_H_
#define _ZZC_UTILS_H_

#include <algorithm>
#include <set>
#include <map>
#include <vector>
#include <string>
#include <tuple>
#include <iostream>
#include <fstream>
#include <cassert>
#include <cstdio>
#include <ctime>
#include <cstdint>
#include <bitset>
#include <chrono>

namespace Utils {
    using i8  = std::int8_t;
    using u8  = std::uint8_t;
    using i16 = std::int16_t;
    using u16 = std::uint16_t;
    using i32 = std::int32_t;
    using u32 = std::uint32_t;
    using i64 = std::int64_t;
    using u64 = std::uint64_t;
    using f32 = std::float_t;
    using f64 = std::double_t;

    template<class T> using Vec = std::vector<T>;
    template<class... T> using Tup = std::tuple<T...>;
    using Str = std::string;

    struct Graph {
        Vec<Vec<u32>> from;
        Vec<Vec<u32>> to;
        u32 vrt_size;
        u32 edg_size;
    };

    std::istream& operator>>(std::istream&, Graph&);
    std::ostream& operator<<(std::ostream&, Graph&);

    struct Set {
        Vec<u32> data;

        Set();
        Set(Vec<u32> const&);
        Set(Vec<u32>&&);

        operator Vec<u32> const&();

        bool operator==(Set const&) const;
        Set operator-(Set const&) const;
        Set operator+(Set const&) const;
        Set operator+(u32) const;
        bool operator<(Set const&) const;

        bool contain(Set const&) const;
        bool exist(u32) const;

        void add(u32);
        void del(u32);
        void push(u32);
        void pop();
    };

    std::ostream& operator<<(std::ostream&, Set const&);

    struct Logger {
        using clk = std::chrono::steady_clock;
        using sec = std::chrono::duration<f64>;

        std::chrono::time_point<clk> beg, cur;
        Vec<f64> rec;
        f64 total, max_delay, min_delay;

        Logger();
        void start_timer();
        void record_and_print(Set const&);
        u32 record_count() const;
        f64 time_count() const;
        void finish_and_print() const;
    };

    bool pick_next(Vec<u32> const&, u32, u32, Vec<u32>&, Vec<u32>&);
} // namespace Utils

#endif // _ZZC_UTILS_H_