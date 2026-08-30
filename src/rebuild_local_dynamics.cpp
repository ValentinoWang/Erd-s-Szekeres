#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <functional>
#include <iostream>
#include <numeric>
#include <queue>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using u64 = std::uint64_t;
using u32 = std::uint32_t;

namespace {

std::array<std::array<std::array<int, 9>, 9>, 9> tidx{};
std::array<std::array<int, 9>, 9> pidx{};
std::vector<std::array<int,3>> triples8;
std::vector<std::array<int,3>> triples7;
std::vector<std::array<int,2>> pairs7;

void init_indices() {
    for (auto &a : tidx) for (auto &b : a) b.fill(-1);
    int k = 0;
    for (int a=0;a<8;++a) for (int b=a+1;b<8;++b) for (int c=b+1;c<8;++c) {
        tidx[a][b][c]=k++;
        triples8.push_back({a,b,c});
    }
    assert(k==56);
    triples7.clear();
    for (int a=0;a<7;++a) for (int b=a+1;b<7;++b) for (int c=b+1;c<7;++c)
        triples7.push_back({a,b,c});
    int p=0;
    for (int i=0;i<9;++i) pidx[i].fill(-1);
    for (int i=0;i<8;++i) for (int j=i+1;j<8;++j) pidx[i][j]=p++;
}

inline int bit(u64 m, int a, int b, int c) {
    return int((m >> tidx[a][b][c]) & 1ULL);
}

inline int changes4(int a,int b,int c,int d) {
    return (a!=b)+(b!=c)+(c!=d);
}

inline bool convex4_bits(int a, int b, int c, int d) {
    int code = a | (b<<1) | (c<<2) | (d<<3);
    return code==0 || code==6 || code==9 || code==15;
}

inline bool convex4(u64 m, int a,int b,int c,int d) {
    return convex4_bits(bit(m,a,b,c), bit(m,b,c,d), bit(m,a,c,d), bit(m,a,b,d));
}

bool subset_convex(u64 m, const std::vector<int>& s) {
    const int n=s.size();
    for (int i=0;i<n;++i) for (int j=i+1;j<n;++j)
    for (int k=j+1;k<n;++k) for (int l=k+1;l<n;++l)
        if (!convex4(m,s[i],s[j],s[k],s[l])) return false;
    return true;
}

bool has_convex7(u64 m, int n) {
    if (n<7) return false;
    std::vector<int> s;
    if (n==7) {
        s={0,1,2,3,4,5,6};
        return subset_convex(m,s);
    }
    assert(n==8);
    for (int omit=0; omit<8; ++omit) {
        s.clear();
        for (int i=0;i<8;++i) if (i!=omit) s.push_back(i);
        if (subset_convex(m,s)) return true;
    }
    return false;
}

struct PairIndex {
    int m;
    std::vector<std::pair<int,int>> pair_of;
    std::vector<std::vector<int>> idx;
    explicit PairIndex(int m_):m(m_),idx(m_,std::vector<int>(m_,-1)) {
        int q=0;
        for(int i=0;i<m;++i) for(int j=i+1;j<m;++j){ idx[i][j]=q++; pair_of.push_back({i,j}); }
    }
};

void enumerate_implication_assignments(const std::vector<u64>& reach,
                                       const std::vector<u64>& pred,
                                       int m,
                                       u64 T,
                                       u64 F,
                                       std::vector<u64>& out) {
    const u64 all = (m==64 ? ~0ULL : ((1ULL<<m)-1));
    if (T & F) return;
    const u64 U = all & ~(T|F);
    if (!U) { out.push_back(T); return; }
    int best=-1, score=-1;
    u64 tmp=U;
    while(tmp){ int v=__builtin_ctzll(tmp); tmp&=tmp-1; int sc=__builtin_popcountll(reach[v]|pred[v]); if(sc>score){score=sc;best=v;} }
    {
        u64 nt = T | reach[best];
        if (!(nt & F)) enumerate_implication_assignments(reach,pred,m,nt,F,out);
    }
    {
        u64 nf = F | pred[best];
        if (!(nf & T)) enumerate_implication_assignments(reach,pred,m,T,nf,out);
    }
}

std::vector<u64> extend_models(const std::vector<u64>& prev, int n) {
    const int oldn=n-1;
    PairIndex pi(oldn);
    const int m=pi.pair_of.size();
    std::vector<u64> result;
    if(n==4) result.reserve(8);
    else if(n==5) result.reserve(62);
    else if(n==6) result.reserve(908);
    else if(n==7) result.reserve(24698);
    else if(n==8) result.reserve(1232944);
    for (u64 oldmask : prev) {
        std::vector<u64> reach(m);
        for(int v=0;v<m;++v) reach[v]=(1ULL<<v);
        auto add_imp=[&](int u,int v){reach[u]|=(1ULL<<v);};
        for(int a=0;a<oldn;++a) for(int b=a+1;b<oldn;++b) for(int c=b+1;c<oldn;++c){
            int xab=pi.idx[a][b], xac=pi.idx[a][c], xbc=pi.idx[b][c];
            int o=bit(oldmask,a,b,c);
            if(o==0){ add_imp(xab,xac); add_imp(xac,xbc); }
            else { add_imp(xbc,xac); add_imp(xac,xab); }
        }
        for(int k=0;k<m;++k) for(int i=0;i<m;++i) if((reach[i]>>k)&1ULL) reach[i]|=reach[k];
        std::vector<u64> pred(m,0);
        for(int i=0;i<m;++i) for(int j=0;j<m;++j) if((reach[i]>>j)&1ULL) pred[j]|=(1ULL<<i);
        std::vector<u64> assigns;
        enumerate_implication_assignments(reach,pred,m,0,0,assigns);
        for(u64 A:assigns){
            u64 nm=oldmask;
            for(int q=0;q<m;++q) if((A>>q)&1ULL){
                auto [i,j]=pi.pair_of[q];
                int idx=tidx[i][j][n-1];
                nm |= (1ULL<<idx);
            }
            result.push_back(nm);
        }
    }
    std::sort(result.begin(), result.end());
    result.erase(std::unique(result.begin(), result.end()), result.end());
    return result;
}

u64 restriction_key_left(u64 m) {
    u64 key=0; int q=0;
    for(int a=1;a<8;++a) for(int b=a+1;b<8;++b) for(int c=b+1;c<8;++c)
        key |= (u64(bit(m,a,b,c))<<q++);
    assert(q==35); return key;
}
u64 restriction_key_right(u64 m) {
    u64 key=0; int q=0;
    for(int a=0;a<7;++a) for(int b=a+1;b<7;++b) for(int c=b+1;c<7;++c)
        key |= (u64(bit(m,a,b,c))<<q++);
    assert(q==35); return key;
}

struct Mask128 { u64 lo,hi; };
inline Mask128 mand(Mask128 a, Mask128 b){return {a.lo&b.lo,a.hi&b.hi};}
inline bool empty(Mask128 a){return (a.lo|a.hi)==0;}
inline int popcnt(Mask128 a){return __builtin_popcountll(a.lo)+__builtin_popcountll(a.hi);}

Mask128 assignments_for_predicate(const std::function<bool(int)>& pred){
    Mask128 m{0,0};
    for(int y=0;y<128;++y) if(pred(y)){ if(y<64)m.lo|=1ULL<<y; else m.hi|=1ULL<<(y-64); }
    return m;
}

std::vector<std::array<int,5>> middle5sets(){
    std::vector<std::array<int,5>> v;
    for(int a=1;a<=7;++a) for(int b=a+1;b<=7;++b) for(int c=b+1;c<=7;++c)
    for(int d=c+1;d<=7;++d) for(int e=d+1;e<=7;++e) v.push_back({a,b,c,d,e});
    assert(v.size()==21); return v;
}

u32 convex6_masks_left(u64 m, const std::vector<std::array<int,5>>& s5){
    u32 mask=0;
    for(int q=0;q<21;++q){ std::vector<int>s{0}; for(int x:s5[q])s.push_back(x); if(subset_convex(m,s))mask|=1u<<q; }
    return mask;
}
u32 convex6_masks_right(u64 m, const std::vector<std::array<int,5>>& s5){
    u32 mask=0;
    for(int q=0;q<21;++q){ std::vector<int>s; for(int x:s5[q])s.push_back(x-1); s.push_back(7); std::sort(s.begin(),s.end()); if(subset_convex(m,s))mask|=1u<<q; }
    return mask;
}

} // namespace

int main(int argc,char**argv){
    init_indices();
    auto t0=std::chrono::steady_clock::now();
    std::vector<u64> models={0ULL,1ULL<<tidx[0][1][2]};
    std::cerr<<"n=3 "<<models.size()<<"\n";
    const std::array<std::size_t,9> expected={0,0,0,2,8,62,908,24698,1232944};
    for(int n=4;n<=8;++n){
        models=extend_models(models,n);
        std::cerr<<"n="<<n<<" "<<models.size()<<"\n";
        if(models.size()!=expected[n]){std::cerr<<"unexpected count\n";return 2;}
    }
    const std::vector<u64> all8=models;
    std::vector<u64> free8; free8.reserve(all8.size());
    std::size_t excluded=0;
    for(u64 m:all8){ if(has_convex7(m,8))++excluded; else free8.push_back(m); }
    std::cerr<<"n8 free="<<free8.size()<<" excluded="<<excluded<<"\n";
    if(free8.size()!=1221504 || excluded!=11440) return 3;

    const auto s5=middle5sets();
    std::vector<u32> lconv(free8.size()), rconv(free8.size());
    for(std::size_t i=0;i<free8.size();++i){lconv[i]=convex6_masks_left(free8[i],s5);rconv[i]=convex6_masks_right(free8[i],s5);}

    std::unordered_map<u64,std::vector<u32>> left_groups,right_groups;
    left_groups.reserve(30000); right_groups.reserve(30000);
    for(u32 i=0;i<free8.size();++i){left_groups[restriction_key_left(free8[i])].push_back(i);right_groups[restriction_key_right(free8[i])].push_back(i);}
    std::cerr<<"overlap keys L="<<left_groups.size()<<" R="<<right_groups.size()<<"\n";
    if(left_groups.size()!=24666 || right_groups.size()!=24666) return 4;

    struct MidPair{int i,j;};
    std::vector<MidPair> mpairs;
    for(int i=1;i<=7;++i)for(int j=i+1;j<=7;++j)mpairs.push_back({i,j});
    assert(mpairs.size()==21);
    Mask128 signoMask[21][2][2];
    Mask128 convexMask[21][2][2];
    for(int q=0;q<21;++q){int i=mpairs[q].i,j=mpairs[q].j;
        for(int a=0;a<2;++a)for(int b=0;b<2;++b){
            signoMask[q][a][b]=assignments_for_predicate([&](int y){int yi=(y>>(i-1))&1,yj=(y>>(j-1))&1;return changes4(b,yj,yi,a)<=1;});
            convexMask[q][a][b]=assignments_for_predicate([&](int y){int yi=(y>>(i-1))&1,yj=(y>>(j-1))&1;return convex4_bits(a,b,yj,yi);});
        }
    }
    int midPairIndex[8][8]; for(auto &r:midPairIndex)for(int &x:r)x=-1;
    for(int q=0;q<21;++q)midPairIndex[mpairs[q].i][mpairs[q].j]=q;

    u64 candidate_pairs=0, signo_transitions=0, signo_models=0, removed_by_convex=0, final_transitions=0, final_models=0;
    std::vector<u32> outdeg(free8.size(),0), indeg(free8.size(),0);
    std::vector<std::pair<u32,u32>> edges; edges.reserve(59441346ULL);
    std::array<u64,129> hist{};

    for(const auto &kv:left_groups){
        auto it=right_groups.find(kv.first); if(it==right_groups.end()) continue;
        const auto &Ls=kv.second; const auto &Rs=it->second;
        candidate_pairs += u64(Ls.size())*u64(Rs.size());
        for(u32 li:Ls){u64 L=free8[li];
            for(u32 ri:Rs){u64 R=free8[ri];
                Mask128 allowed{~0ULL,~0ULL};
                for(int q=0;q<21;++q){int i=mpairs[q].i,j=mpairs[q].j; int a=bit(L,0,i,j); int b=bit(R,i-1,j-1,7); allowed=mand(allowed,signoMask[q][a][b]); if(empty(allowed))break;}
                if(empty(allowed)) continue;
                ++signo_transitions; int before=popcnt(allowed); signo_models+=before;
                u32 cs = lconv[li] & rconv[ri];
                while(cs){int sq=__builtin_ctz(cs); cs&=cs-1; Mask128 bad{~0ULL,~0ULL}; auto S=s5[sq];
                    for(int x=0;x<5 && !empty(bad);++x)for(int z=x+1;z<5 && !empty(bad);++z){int i=S[x],j=S[z],q=midPairIndex[i][j];int a=bit(L,0,i,j),b=bit(R,i-1,j-1,7);bad=mand(bad,convexMask[q][a][b]);}
                    allowed.lo &= ~bad.lo; allowed.hi &= ~bad.hi;
                    if(empty(allowed)) break;
                }
                int after=popcnt(allowed); removed_by_convex += before-after;
                if(after){++final_transitions; final_models+=after; edges.emplace_back(li,ri); ++outdeg[li]; ++indeg[ri]; if(after<129)hist[after]++;}
            }
        }
    }
    std::cerr<<"candidate_pairs="<<candidate_pairs<<"\n";
    std::cerr<<"signo transitions="<<signo_transitions<<" models="<<signo_models<<"\n";
    std::cerr<<"removed="<<removed_by_convex<<" final transitions="<<final_transitions<<" models="<<final_models<<"\n";
    if(candidate_pairs!=64460192ULL || signo_transitions!=60063878ULL || signo_models!=109184802ULL || removed_by_convex!=1116672ULL || final_transitions!=59441346ULL || final_models!=108068130ULL){std::cerr<<"count mismatch\n";return 5;}

    const u32 N=free8.size();
    std::vector<u64> off(N+1,0), roff(N+1,0);
    for(u32 i=0;i<N;++i){off[i+1]=off[i]+outdeg[i];roff[i+1]=roff[i]+indeg[i];}
    std::vector<u32> adj(edges.size()), radj(edges.size());
    std::vector<u64> cur=off, rcur=roff;
    for(auto [u,v]:edges){adj[cur[u]++]=v;radj[rcur[v]++]=u;}
    auto bfs=[&](const std::vector<u64>&O,const std::vector<u32>&A){std::vector<char>seen(N,0);std::queue<u32>q;q.push(0);seen[0]=1;u64 cnt=1;while(!q.empty()){u32 u=q.front();q.pop();for(u64 p=O[u];p<O[u+1];++p){u32 v=A[p];if(!seen[v]){seen[v]=1;++cnt;q.push(v);}}}return cnt;};
    u64 fwd=bfs(off,adj),rev=bfs(roff,radj);
    auto [minout,maxout]=std::minmax_element(outdeg.begin(),outdeg.end());
    auto [minin,maxin]=std::minmax_element(indeg.begin(),indeg.end());
    std::cerr<<"reach="<<fwd<<","<<rev<<" degree in="<<*minin<<".."<<*maxin<<" out="<<*minout<<".."<<*maxout<<"\n";
    if(fwd!=N||rev!=N||*minin!=15||*maxin!=87||*minout!=15||*maxout!=87)return 6;

    for(u32 u=0;u<N;++u)std::sort(adj.begin()+off[u],adj.begin()+off[u+1]);
    auto hasedge=[&](u32 u,u32 v){return std::binary_search(adj.begin()+off[u],adj.begin()+off[u+1],v);};
    bool twoc= hasedge(181110,752273)&&hasedge(752273,181110);
    std::cerr<<"two_cycle="<<twoc<<" masks="<<std::hex<<free8[181110]<<","<<free8[752273]<<std::dec<<"\n";
    if(!twoc || free8[181110]!=0x12001004000001ULL || free8[752273]!=0xc0410000200040ULL)return 7;

    auto t1=std::chrono::steady_clock::now();
    double sec=std::chrono::duration<double>(t1-t0).count();
    std::string outpath=(argc>1?argv[1]:"rebuild_local_dynamics_result.json");
    std::ofstream out(outpath);
    out<<"{\n"
       <<"  \"status\": \"PASS\",\n"
       <<"  \"n7_signotopes\": 24698,\n"
       <<"  \"n8_signotopes\": 1232944,\n"
       <<"  \"n8_no_convex7\": "<<free8.size()<<",\n"
       <<"  \"n8_excluded\": "<<excluded<<",\n"
       <<"  \"candidate_pairs\": "<<candidate_pairs<<",\n"
       <<"  \"signotope_compatible_transitions_before_convex_filter\": "<<signo_transitions<<",\n"
       <<"  \"n9_models_before_cross_window_filter\": "<<signo_models<<",\n"
       <<"  \"bridge_assignments_removed_by_convex_filter\": "<<removed_by_convex<<",\n"
       <<"  \"compatible_transitions\": "<<final_transitions<<",\n"
       <<"  \"n9_no_convex7_models\": "<<final_models<<",\n"
       <<"  \"forward_reachable\": "<<fwd<<",\n"
       <<"  \"reverse_reachable\": "<<rev<<",\n"
       <<"  \"strongly_connected\": true,\n"
       <<"  \"min_indegree\": "<<*minin<<",\n"
       <<"  \"max_indegree\": "<<*maxin<<",\n"
       <<"  \"min_outdegree\": "<<*minout<<",\n"
       <<"  \"max_outdegree\": "<<*maxout<<",\n"
       <<"  \"two_cycle\": [181110, 752273],\n"
       <<"  \"seconds\": "<<sec<<"\n"
       <<"}\n";
    std::cerr<<"wrote "<<outpath<<" in "<<sec<<" s\n";
    return 0;
}
