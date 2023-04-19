#include "utils_copy.h"

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

        for (auto& e : g.from) {
            e.shrink_to_fit();
            std::sort(e.begin(), e.end());
        }
        for (auto& e : g.to) {
            e.shrink_to_fit();
            std::sort(e.begin(), e.end());
        }

        return in;
    }

    std::ostream& operator<<(std::ostream& out, Graph& g) {
        // TODO

        return out;
    }
    std::ostream& operator<<(std::ostream& out, Vec<u32> const& s) {
        for (auto& v : s) {
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

    void Logger::record_and_print(Vec<u32> const& s) {
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

    Combinations::Combinations(Vec<u32> const& elem, int n, int m) : elem(elem), n(n), m(m), indices(m), done(false) {
        if (n < m) {
            done = true;
        } else {
            for (int i = 0; i < m; i++) {
                indices[i] = i;
            }
        }
    }

    bool Combinations::next(Vec<u32> &combination) {
        if (done) {
            return false;
        }
        combination.resize(m);
        for (int i = 0; i < m; i++) {
            combination[i] = elem[indices[i]];
        }
        int i = m - 1;
        while (i >= 0 && indices[i] >= n - m + i) {
            i--;
        }
        if (i < 0) {
            done = true;
        } else {
            indices[i]++;
            for (int j = i + 1; j < m; j++) {
                indices[j] = indices[j - 1] + 1;
            }
        }
        return true;
    }
} // namespace Utils;