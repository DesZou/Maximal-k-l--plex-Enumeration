#include "base_on_server.h"

namespace BaseOnServer {
    std::istream& operator>>(std::istream& in, Graph& g) {
        Vec<Tup<u32, u32>> e;
        Str s;
        u32 u, v;
        u32 n = 0, m = 0;

        while (std::getline(in, s, '\n')) {
            if (s[0] == '#' || std::sscanf(s.c_str(), "%u%u", &u, &v) != 2) {
                continue;
            }

            ++n;
            m = std::max(m, std::max(u, v));
            e.emplace_back(u, v);
        }

        g.edg_num = n;
        g.vrt_num = m + 1;

        g.edges.resize(g.vrt_num);
        g.edges_r.resize(g.vrt_num);

        for (auto& t: e) {
            std::tie(u, v) = t;
            g.edges[u].push_back(v);
            g.edges_r[v].push_back(u);
        }

        return in;
    }

    std::ostream& operator<<(std::ostream& out, Graph& g) {
        // TODO

        return out;
    }

    Set::Set() {}

    Set::Set(Vec<u32> const& vrt) {
        data = vrt;
        std::sort(data.begin(), data.end());
    }

    bool Set::operator==(Set const& rhs) const {
        return data == rhs.data;
    }

    Set Set::operator-(Set const& rhs) const {
        Set ret;

        std::set_difference(data.begin(), data.end(),
                            rhs.data.begin(), rhs.data.end(),
                            std::back_inserter(ret.data));

        return ret;
    }

    Set Set::operator+(Set const& rhs) const {
        Set ret;

        std::set_union(data.begin(), data.end(),
                        rhs.data.begin(), rhs.data.end(),
                        std::back_inserter(ret.data));

        return ret;
    }

    Set Set::operator+(u32 val) const {
        Set ret(*this);

        auto pos = std::lower_bound(ret.data.begin(), ret.data.end(), val);
        ret.data.insert(pos, val);

        return ret;
    }

    bool Set::operator<(Set const& rhs) const {
        if (data.size() != rhs.data.size()) {
            return (data.size() < rhs.data.size());
        }

        auto iter = rhs.data.begin();

        for (auto& v : data) {
            if (v == *iter) {
                ++iter;
                continue;
            }
            return (v < *iter);
        }

        return false;
    }

    bool Set::operator>(Set const& rhs) const {
        return (rhs < *this);
    }

    bool Set::contain(Set const& rhs) const {
        return std::includes(data.begin(), data.end(),
                                rhs.data.begin(), rhs.data.end());
    }

    bool Set::exist(u32 val) const {
        return std::binary_search(data.begin(), data.end(), val);
    }

    bool Set::picker(u32 n, u32 i, Vec<u32>& pos, Vec<u32>& pick) const {
        if (i >= n) return false;

        if (!picker(n, i + 1, pos, pick)) {
            if (++pos[i] >= data.size()) {
                return false;
            }

            pick[i] = data[pos[i]];
            
            for (u32 j = i + 1; j < n; ++j) {
                pos[j] = pos[j - 1] + 1;
                if (pos[j] >= data.size()) return false;
                pick[j] = data[pos[j]];
            }

            return true;
        }

        return true;
    }

    bool Set::next_pick(u32 n, Vec<u32>& pos, Vec<u32>& pick) const {
        if (n == 0) {
            pos.resize(1);
            pick.resize(0);
            if (pos[0] == 1) return false;
            pos[0] = 1;
            return true;
        }
        
        pos.resize(n);
        pick.resize(n);

        if (n > data.size()) return false;

        if (n == 1) {
            if (pos[0] >= data.size()) return false;
            pick[0] = data[pos[0]++];
            return true;
        }

        if (pos[n - 1] == 0) {
            for (u32 i = 0; i < n; ++i) {
                pos[i] = i;
                pick[i] = data[i];
            }

            return true;
        }

        return picker(n, 0, pos, pick);
    }

    void Set::add(u32 val) {
        auto pos = std::lower_bound(data.begin(), data.end(), val);
        data.insert(pos, val);
    }

    void Set::del(u32 val) {
        auto pos = std::lower_bound(data.begin(), data.end(), val);
        if (pos != data.end() && *pos == val) {
            data.erase(pos);
        }
    }

    std::ostream& operator<<(std::ostream& out, Set const& s) {
        for (auto& v : s.data) {
            out << v << " ";
        }
        out << "\n";
        return out;
    }
}
