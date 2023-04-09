#include "utils.h"

using namespace Utils;

u32 K = 2;
u32 L = 2;

u64 sol_cnt = 0;
u64 sol_bnd = 1000;
f64 time_bnd = 1000;

Logger T;

bool sat(Graph const& g, Set const& s) {
    u32 n = s.data.size();
    auto f = [&s](u32 v) { return s.exist(v); };

    for (auto& x : s.data) {
        auto& fr = g.from[x];
        auto& to = g.to[x];
        u32 k = std::count_if(fr.begin(), fr.end(), f);
        u32 l = std::count_if(to.begin(), to.end(), f);
        if (k + K < n || l + L < n) return false;
    }
    return true;
}

Set extend(Graph const& g, Set const& s) {
    Set ret(s);
    u32 n = g.vrt_size;

    for (u32 i = 0; i < n; ++i) {
        if (!ret.exist(i)) {
            ret.add(i);
            if (sat(g, ret)) continue;
            ret.del(i);
        }
    }
    
    return ret;
}

bool maximal(Graph const& g, Set const& h, Set const& s) {
    Set s_cp(s);
    for (u32 i : h.data) if (!s_cp.exist(i)) {
        s_cp.add(i);
        if (sat(g, s_cp)) return false;
        s_cp.del(i);
    }
    // std::cerr << __LINE__ << std::endl;
    return true;
}

struct EnumAlmostCoro {
    enum State { Start, Resume, Finish };

    State cur;

    Graph const& g;
    Set const& s;
    u32 v;

    Set value;
    Set r;
    Set e, q;
    u32 n, m;
    Vec<u32> p;

    EnumAlmostCoro(Graph const& g_, Set const& s_, u32 v_):
        cur(Start), g(g_), s(s_), v(v_), m(0), n(0) {
        auto e1 = s - Set(g.from[v]);
        auto e2 = s - Set(g.to[v]);
        auto n1 = std::min(K - 1, u32(e1.data.size()));
        auto n2 = std::min(L - 1, u32(e2.data.size()));
        n = n1 + n2;
        e = e1 + e2;
        n = std::min(n, u32(e.data.size()));
        m = n + 1;
        value = s - e;
        value.add(v);
    }

    bool operator()() {
        if (cur == Finish) return false;
        if (cur == Resume) goto CoroResume;
        cur = Resume;

        while (m --> 0) {
            p.clear();
            while (pick_next(e.data, e.data.size(), m, p, q.data)) {
                if (!q.data.empty() && v < q.data[0]) break; // force making order increase
                value = value + q;
                if (sat(g, value)) {
                    return true;
                CoroResume:;
                }
                value = value - q;
            }
        }
 
        cur = Finish;
        return false;
    }
};

void record_and_check(Set const& s) {
    T.record_and_print(s);
    if (T.time_count() >= time_bnd || T.record_count() >= sol_bnd) {
        T.finish_and_print();
        std::exit(0);
    }
}

bool has_intersection(Set const& a, Set const& b) {
    auto x = a.data.begin();
    auto y = b.data.begin();

    while (x != a.data.end() && y != b.data.end()) {
        if (*x == *y) return true;
        if (*x < *y) x = std::next(x);
        else         y = std::next(y);
    }

    return false;
}

bool ord_less(Set const& a, Set const& b) {
    auto x = a.data.begin(), xe = a.data.end();
    auto y = b.data.begin(), ye = b.data.end();

    while (x != xe || y != ye) {
        if (x == xe) return true;
        if (y == ye) return false;
        if (*x < *y) return true;
        if (*x > *y) return false;
        x = std::next(x);
        y = std::next(y);
    }

    return false;
}

void enumAll(Graph& g, Set const& s, Set E, std::set<Set>& sol, bool output_flag = true) {
    u32 start = s.data.empty()? 0 : s.data[0] + 1;
    for (u32 i = start; i < g.vrt_size; ++i) {
        if (s.exist(i) || E.exist(i)) continue;

        auto enumAlmost = EnumAlmostCoro(g, s, i);
        Set& t = enumAlmost.value;

        while (enumAlmost()) {
            Set c = extend(g, t);

            if (sol.find(c) == sol.end()) {
                sol.insert(c);
                if (output_flag) record_and_check(c);
                enumAll(g, c, E, sol, !output_flag);
                if (!output_flag) record_and_check(c);
            }
        }

        E.add(i);
    }
}

i32 main(i32 argc, char* argv[]) {
    Str input_path = "../data/small_graph.txt";

    if (argc >= 2) input_path = Str(argv[1]);
    if (argc >= 3) {
        i32 tmp = sscanf(argv[2], "%llu", &sol_bnd);
        if (tmp != 1) {
            return -1;
        }
    }
    if (argc >= 4) {
        i32 tmp = sscanf(argv[3], "%lf", &time_bnd);
        if (tmp != 1) {
            return -1;
        }
    }
    if (argc >= 6) {
        i32 tmp1 = sscanf(argv[4], "%u", &K);
        i32 tmp2 = sscanf(argv[5], "%u", &L);
        if (tmp1 != 1 || tmp2 != 1) {
            return -1;
        }
    }

    std::ifstream input_stream(input_path);
    Graph g;
    std::set<Set> sol;

    input_stream >> g;
    input_stream.close();

    T.start_timer();

    enumAll(g, {}, {}, sol);

    T.finish_and_print();

    return 0;
}
