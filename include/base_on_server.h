#ifndef _ZZC_BASE_ON_SERVER_
#define _ZZC_BASE_ON_SERVER_

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

namespace BaseOnServer {
    using i32 = std::int32_t;
    using u32 = std::uint32_t;
    using i64 = std::int64_t;
    using u64 = std::uint64_t;

    template<class T> using Vec = std::vector<T>;
    template<class... T> using Tup = std::tuple<T...>;
    using Str = std::string;

    struct Graph {
        Vec<Vec<u32>> edges;
        Vec<Vec<u32>> edges_r;
        u32 vrt_num;
        u32 edg_num;
    };

    std::istream& operator>>(std::istream&, Graph&);
    std::ostream& operator<<(std::ostream&, Graph&);

    struct Set {
        Vec<u32> data;

        Set();
        Set(Vec<u32> const&);

        bool operator==(Set const&) const;
        Set operator-(Set const&) const;
        Set operator+(Set const&) const;
        Set operator+(u32) const;
        bool operator<(Set const&) const;
        bool operator>(Set const&) const;

        bool contain(Set const&) const;
        bool exist(u32) const;

        void add(u32);
        void del(u32);
        
        bool next_pick(u32, Vec<u32>&, Vec<u32>&) const;
        bool picker(u32, u32, Vec<u32>&, Vec<u32>&) const;
    };

    std::ostream& operator<<(std::ostream&, Set const&);
} // BaseOnServer

#endif // _ZZC_BASE_ON_SERVER_