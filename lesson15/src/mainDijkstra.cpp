#include <vector>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <set>
#define x first
#define y second
#define int long long
using namespace std;
int debugPoint(int line) {
    if (line < 0)
        return 0;

    // You can put breakpoint at the following line to catch any rassert failure:
    return line;
}

#define rassert(condition, message) if (!(condition)) { std::stringstream ss; (ss << "Assertion \"" << message << "\" failed at line " << debugPoint(__LINE__) << "!"); throw std::runtime_error(ss.str()); }


struct Edge {
    signed u, v; // номера вершин которые это ребро соединяет
    int w; // длина ребра (т.е. насколько длинный путь предстоит преодолеть переходя по этому ребру между вершинами)

    Edge(signed u, signed v, int w) : u(u), v(v), w(w)
    {}
};

signed * previ;
int * mind;

vector<Edge>* e;
void run() {
    // https://codeforces.com/problemset/problem/20/C?locale=ru
    // Не требуется сделать оптимально быструю версию, поэтому если вы получили:
    //
    // Превышено ограничение времени на тесте 31
    //
    // То все замечательно и вы молодец.

    int n, m;
    cin >> n;
    cin >> m;

    e = new vector<Edge>[n];
    for (int i = 0; i < m; ++i) {
        int ai, bi, w;
        std::cin >> ai >> bi >> w;
        rassert(ai >= 1 && ai <= n, 23472894792020);
        rassert(bi >= 1 && bi <= n, 23472894792021);

        ai -= 1;
        bi -= 1;
        rassert(ai >= 0 && ai < n, 3472897424024);
        rassert(bi >= 0 && bi < n, 3472897424025);

        e[ai].push_back(Edge(ai, bi, w));

        e[bi].push_back(Edge(bi, ai, w));
    }

    const int s = 0;
    const int t = n - 1;

    int* mind = new int[n];
    int * previ = new int[n];

    set<pair<int, int>> now;
    mind[s] = 0;
    previ[s] = -1;
    now.insert({mind[s], s});
    for(int i = 0; i<n; i++){
        if(i!=s){
            mind[i] = 10000000000000ll;
            now.insert({mind[i], i});
            previ[i] = -1;
        }
    }

    while(!now.empty()){
        int v = (*now.begin()).y;
        int len = (*now.begin()).x;
        now.erase(now.begin());
        for(auto z:e[v]){
            if(len+z.w<mind[z.v]){
                now.erase({mind[z.v], z.v});
                mind[z.v] = len+z.w;
                previ[z.v] = v;
                now.insert({mind[z.v], z.v});
            }

        }
    }

    if (previ[t]!=-1) {
        int now = t;
        vector<int> ans;
        while(now!=-1) {
            ans.push_back(now+1);
            now = previ[now];
        }
        reverse(ans.begin(), ans.end());
        for(auto z:ans){
            cout << z << " ";
        }
        std::cout << std::endl;
    } else {
        std::cout << -1 << std::endl;
    }
}

signed main() {
    try {
        run();

        return 0;
    } catch (const std::exception &e) {
        std::cout << "Exception! " << e.what() << std::endl;
        return 1;
    }
}
