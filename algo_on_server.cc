#include "base_on_server.h"

using namespace BaseOnServer;

u32 const K = 2;
u32 const L = 2;

u64 sol_cnt = 0;
std::time_t T;

bool sat(Graph const& g, Set const& s) {
    u32 n = s.data.size();
    auto f = [&s](u32 x) { return s.exist(x); };

    for (auto &v : s.data) {
        auto &es = g.edges[v];
        auto &es_r = g.edges_r[v];
        u32 out = std::count_if(es.begin(), es.end(), f);
        u32 in = std::count_if(es_r.begin(), es_r.end(), f);

        if (((out + K) < n) || ((in + L) < n)) return false;
    }

    return true;
}

Set extend(Graph const& g, Set const& s) {
    Set ret = s;
    u32 n = g.vrt_num;

    for (u32 i = 0; i < n; ++i) {
        if (!ret.exist(i)) {
            ret.add(i);
            if (sat(g, ret)) continue;
            ret.del(i);
        }
    }
    
    return ret;
}

// EnumAlmostCoro 枚举局部解协程
struct EnumAlmostCoro {
    enum State { Start, Resume, Finish };

    State cur;      // 记录协程运行状态
    /*--- 参数 ---*/
    Graph const& g; // 枚举所在的图
    Set const& s;   // 枚举的点集
    u32 v;          // 新加入的点
    /*--- 内部变量 ---*/
    std::set<Set> save;     // 保存解集
    Set value;              // 实际上的返回值
    Set e1, e2, e;          // 计算s中多少点未与v相连
    u32 n1, n2, n, m;       // 计算s中需要删除/保留的点数目
    Set p1, q1, p2, q2;     // 用于记录从点集中选择若干个点的选法

    // 构造函数，初始化各个变量的值，将cur设为Start
    EnumAlmostCoro(Graph const& g_, Set const& s_, u32 v_):
        cur(Start), g(g_), s(s_), v(v_), m(0), n1(0), n2(0), n(0) {
        e1 = s - Set(g.edges[v]);
        e2 = s - Set(g.edges_r[v]);
        n1 = std::min(K - 1, u32(e1.data.size()));
        n2 = std::min(L - 1, u32(e2.data.size()));
        n = n1 + n2;
        e = e1 + e2;
        n = std::min(n, u32(e.data.size()));
    }
    
    // 实际运行部分
    bool operator()() {
        if (cur == Finish) return false;    // 若已经结束
        if (cur == Resume) goto CoroResume; // 若之前已经运行过，则直接跳至返回处
        cur = Resume;
         
        while (e.next_pick(n, p1.data, q1.data)) {  // 从不与v相连的点集e中选择n个保留，存至q1
            m = -1;
            while (++m <= n) {  // 从n个保留中决定fix数目，存至m
                p2.data.clear();
                while (q1.next_pick(m, p2.data, q2.data)) { // 从q1中选m个点进行fix操作，存至q2
                    value = s - (e - (q1 - q2)) + v;
                    if (sat(g, value) && save.find(value) == save.end()) {
                        save.insert(value);
                        return true;        // 从协程退出
                    CoroResume:;            // 从外部返回
                    }
                }
            }
        }
        cur = Finish;
        return false;
    }
};

void print(Set const& c) {
    ++sol_cnt;
    std::cout << "[" << (std::time(nullptr) - T) << "s] No." << sol_cnt << " solution: " << c;
    T = std::time(nullptr);
}

// enumAll 枚举主程序； g：枚举所在的图；s：当前枚举的点集；sol：解集；output_flag：决定输出先后
void enumAll(Graph& g, Set const& s, std::set<Set>& sol, bool output_flag = true) {
    for (u32 i = 0; i < g.vrt_num; ++i) {   // 向点集加入新点
        if (s.exist(i)) continue;           // 若已经在点集中则跳过

        auto enumAlmost = EnumAlmostCoro(g, s, i);  // 创建enumAlmost协程
        Set& t = enumAlmost.value;                  // t为协程返回值

        while (enumAlmost()) {                      // 若找到一个local解
            Set c = extend(g, t);                   // 将local解扩充为g上的极大解

            if (sol.find(c) == sol.end()) {
                sol.insert(c);
                if (output_flag) print(c);
                enumAll(g, c, sol, !output_flag);
                if (!output_flag) print(c);
            }
        }
    }
}

i32 main() {
    Str input_path = "../data/small_graph.txt";
    std::ifstream input_stream(input_path);
    Graph g;
    std::set<Set> sol;

    input_stream >> g;
    input_stream.close();

    auto tik = T = std::time(nullptr);

    enumAll(g, Vec<u32>{}, sol);

    auto tok = std::time(nullptr);

    std::cout << "---------------------\n"
        << "Total (" << K << ", " << L << ")-plex(es): " << sol_cnt << "\n"
        << "Total runtime: " << (tok - tik) << "s\n"
        << "---------------------";

    return 0;
}
