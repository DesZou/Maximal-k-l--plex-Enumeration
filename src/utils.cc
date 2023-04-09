#include "utils.h"

namespace Utils {
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

        g.edg_size = n;
        g.vrt_size = m + 1;

        g.from.resize(g.vrt_size);
        g.to.resize(g.vrt_size);

        for (auto& t : e) {
            std::tie(u, v) = t;
            g.from[u].push_back(v);
            g.to[v].push_back(u);
        }

        for (auto& e : g.from) e.shrink_to_fit();
        for (auto& e : g.to) e.shrink_to_fit();

        return in;
    }

    std::ostream& operator<<(std::ostream& out, Graph& g) {
        // TODO

        return out;
    }

    Set::Set() {}

    Set::operator Vec<u32> const&() {
        return data;
    }

    Set::Set(Vec<u32> const& vrt) {
        data = vrt;
        std::sort(data.begin(), data.end());
    }

    Set::Set(Vec<u32>&& vrt) {
        data = std::move(vrt);
        std::sort(data.begin(), data.end());
    }

    bool Set::operator==(Set const& rhs) const {
        return data == rhs.data;
    }

    Set Set::operator-(Set const& rhs) const {
        Set ret;
        ret.data.reserve(data.size());

        std::set_difference(data.begin(), data.end(),
                            rhs.data.begin(), rhs.data.end(),
                            std::back_inserter(ret.data));

        return ret;
    }

    Set Set::operator+(Set const& rhs) const {
        Set ret;
        ret.data.reserve(data.size() + rhs.data.size());

        std::set_union(data.begin(), data.end(),
                        rhs.data.begin(), rhs.data.end(),
                        std::back_inserter(ret.data));

        return ret;
    }

    Set Set::operator+(u32 v) const {
        Set ret(*this);
        ret.add(v);
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

    bool Set::contain(Set const& rhs) const {
        return std::includes(data.begin(), data.end(),
                                rhs.data.begin(), rhs.data.end());
    }

    bool Set::exist(u32 val) const {
        return std::binary_search(data.begin(), data.end(), val);
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

    void Set::push(u32 val) {
        data.push_back(val);
    }

    void Set::pop() {
        data.pop_back();
    }

    std::ostream& operator<<(std::ostream& out, Set const& s) {
        for (auto& v : s.data) {
            out << v << " ";
        }
        out << "\n";
        return out;
    }

    Logger::Logger():
        total(0),
        max_delay(0),
        min_delay(1e9) {}

    void Logger::start_timer() {
        beg = cur = clk::now();
    }

    void Logger::record_and_print(Set const& s) {
        auto prev = cur;
        cur = clk::now();
        auto delay = sec(cur - prev).count();
        if (delay > max_delay) max_delay = delay;
        if (delay < min_delay) min_delay = delay;
        total += delay;
        rec.push_back(delay);

        std::cout << "(" << rec.size() << ")\t"
                << "[" << delay << "s]\t"
                << "{" << total << "s} \t"
                << s;
    }

    u32 Logger::record_count() const {
        return rec.size();
    }

    f64 Logger::time_count() const {
        return total;
    }

    void Logger::finish_and_print() const {
        f64 avg_delay = 0;
        if (!rec.empty()) {
            avg_delay = total / rec.size();
        }

        std::cout << "--------------------\n"
                << "Find plex(es): " << rec.size() << "\n"
                << "Total time: " << total << "s\n"
                << "Delay (avg/min/max): " << avg_delay << "s/" << min_delay << "s/" << max_delay << "s\n";
    }


    bool pick_picker(Vec<u32> const& e, u32 n, u32 m, u32 i, Vec<u32>& p, Vec<u32>& r) {
        if (i >= m) return false;

        if (!pick_picker(e, n, m, i + 1, p, r)) {
            if (++p[i] + m > n + i) {
                return false;
            }

            r[i] = e[p[i]];
            for (u32 j = i + 1; j < m; ++j) {
                p[j] = p[j - 1] + 1;
                r[j] = e[p[j]];
            }
        }
        return true;
    }

    bool pick_next(Vec<u32> const& e, u32 n, u32 m, Vec<u32>& p, Vec<u32>& r) {
        p.resize(m + 1);
        r.resize(m);

        if (p[m] == 0) {
            if (m <= n) {
                for (u32 i = 0; i < m; ++i) {
                    p[i] = i;
                    r[i] = e[i];
                }
                p[m] = 1;
                return true;
            } else {
                return false;
            }
        } else {
            auto result = pick_picker(e, n, m, 0, p, r);
            p[m] += result;
            return result;
        }
    }
} // namespace Utils;