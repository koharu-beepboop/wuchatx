// tinyc.h — a new language made of C. every primitive ≤3 chars.
// drop this in any project and write code that fits on one page.
#ifndef TH
#define TH
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <fcntl.h>
#include <ctype.h>

// ────── thread safety (opt-in via #define TC_THREADSAFE before include) ──────
#ifdef TC_THREADSAFE
#include <pthread.h>
static pthread_mutex_t _arena_mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t _gl_mtx    = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t _tk_mtx    = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t _hx_mtx    = PTHREAD_MUTEX_INITIALIZER;
#define lca()  pthread_mutex_lock(&_arena_mtx)
#define uca()  pthread_mutex_unlock(&_arena_mtx)
#define lcgl() pthread_mutex_lock(&_gl_mtx)
#define ucgl() pthread_mutex_unlock(&_gl_mtx)
#define lctk() pthread_mutex_lock(&_tk_mtx)
#define uctk() pthread_mutex_unlock(&_tk_mtx)
#define lchx() pthread_mutex_lock(&_hx_mtx)
#define uchx() pthread_mutex_unlock(&_hx_mtx)
#endif

// ────── types ──────
typedef int I; typedef char C; typedef void V; typedef unsigned char U;
typedef short S; typedef long L; typedef float F; typedef double D;
struct sockaddr_in A;           // sockaddr — t() writes here
struct winsize _w;              // set by s()
struct termios _t;              // set by r()
I Wi, He;                       // terminal size — set by s()

// ────── flow basics (needed by functions below) ──────
#define rt return
#define br break
#define ct continue
#define el else
#define fi for(;;)
#define wl while
#define fr(i,n) for(I i=0;i<(n);i++)

// dfr(expr) → runs expr when scope exits (FILO). dfr(cl(fd))
#define dfr(b) for(I _x=1;_x--;b)

// ────── network ──────
// t(host,port) → tcp socket fd. t("localhost",6667)
I t(C*h,I p){A.sin_family=2;
A.sin_port=htons(p);
A.sin_addr.s_addr=inet_addr(h);
I f=socket(2,1,0);
connect(f,(V*)&A,sizeof A);rt f;}

// l(port) → tcp listen fd. l(6667)
I l(I p){A.sin_family=2;
A.sin_port=htons(p);A.sin_addr.s_addr=0;
I f=socket(2,1,0);I o=1;
setsockopt(f,SOL_SOCKET,SO_REUSEADDR,&o,sizeof o);
bind(f,(V*)&A,16);listen(f,10);rt f;}

// ac(fd) → accept connection, returns client fd or -1
#define ac(f) accept(f,N,N)

// nb(fd) → set non-blocking on fd
#define nb(f) fcntl(f,F_SETFL,fcntl(f,F_GETFL)|O_NONBLOCK)

// ────── terminal ──────
// r(1)=raw on, r(0)=cooked
V r(I o){tcgetattr(0,&_t);
_t.c_lflag=o?0:(ICANON|ECHO);
_t.c_cc[VMIN]=o?1:0;
tcsetattr(0,0,&_t);}
// s() → Wi (cols), He (rows)
V s(){ioctl(1,TIOCGWINSZ,&_w);
Wi=_w.ws_col;He=_w.ws_row;}
// tt(s) → set terminal window title
#define tt(s) dprintf(1,"\x1b]0;%s\x1b\\",s)
// ky() → read one key in raw mode: regular keys return the byte,
// special keys return negative: UP=-1 DN=-2 RT=-3 LT=-4
// PGUP=-5 PGDN=-6 HOME=-7 END=-8 DEL=-9 INS=-10. 0=EOF, 27=unrecognized ESC
I ky(){U b[8];I r;
r=read(0,b,1);if(r<=0)rt 0;if(b[0]!=27)rt b[0];
r=read(0,b+1,1);if(r<=0)rt 27;if(b[1]!='[')rt 27;
r=read(0,b+2,1);if(r<=0)rt 27;
if(b[2]=='A')rt -1;if(b[2]=='B')rt -2;if(b[2]=='C')rt -3;if(b[2]=='D')rt -4;
if(b[2]=='H')rt -7;if(b[2]=='F')rt -8;
if(b[2]>='0'&&b[2]<='9'){I k=b[2]-48;
r=read(0,b+3,1);if(r<=0)rt 27;
if(b[3]>='0'&&b[3]<='9'){k=k*10+b[3]-48;r=read(0,b+4,1);if(r<=0)rt 27;}
if(b[3]=='~'){switch(k){case 1:rt-7;case 2:rt-10;case 3:rt-9;case 4:rt-8;case 5:rt-5;case 6:rt-6;default:rt 27;}}}
rt 27;}

// ────── string ──────
// f(h,n) → offset of n in h, or -1. f(buf,"\r\n")
I f(C*h,C*n){C*p=strstr(h,n);
rt p?(I)(p-h):-1;}
// fc(h,n) → case-insensitive find, returns offset or -1. fc("Hello","hEl")→0
I fc(C*h,C*n){I i=0,j;if(!*n)rt 0;wl(h[i]){j=0;wl(n[j]&&((h[i+j]|32)==(n[j]|32)))j++;if(!n[j])rt i;i++;}rt-1;}
#define ln(s) strlen(s)
#define eq(a,b) (strcmp((a),(b))==0)
#define cp(d,s) strcpy((d),(s))
#define cc(d,s) strcat((d),(s))
#define mv(d,s,n) memmove((d),(s),(n))
#define cz(b,n) memset((b),0,(n))

// a(h,d) → ptr after first d in h. a(l," :")
C* a(C*h,C*d){I i=f(h,d);rt i>=0?h+i+ln(d):h+ln(h);}

// us(s) → uppercase string in-place. us(cmd)
V us(C*s){wl(*s){*s=toupper(*s);s++;}}

// tk(s,n) → nth space-delimited token, or "". tk("a b c",1)→"b"
C* tk(C*s,I n){static C b[128];I w=0,p=0;
fr(i,ln(s)+1){if(s[i]==32||!s[i]){if(w==n&&p){b[p]=0;rt b;}
w++;p=0;}el if(w==n)b[p++]=s[i];}rt"";}

// ────── string views ──────
typedef struct{C*p;I n;}Sv;
#define sv(s) ((Sv){(s),ln(s)})
#define svn(s,n) ((Sv){(s),(n)})
#define sx(v,s,e) ((Sv){(v).p+(s),((e)-(s))<((v).n-(s))?(e)-(s):(v).n-(s)})
#define ss(v,c) ({Sv _r={N,0};I _i=0;\
  fr(_i,(v).n)if((v).p[_i]==(c)){_r=(Sv){(v).p,_i};\
  (v).p+=_i+1;(v).n-=_i+1;br;}\
  if(_i==(v).n&&(v).n){_r=(v);(v).p+=_i;(v).n=0;}_r;})
#define st(v,w) ({Sv _w=sv(w);(v).n>=(_w).n&&memcmp((v).p,_w.p,_w.n)==0;})
#define sc(a,b) ((a).n==(b).n&&memcmp((a).p,(b).p,(a).n)==0)

// ────── time ──────
// d(b,n) → 2-digit zero-padded. d(b,3)→"03"
V d(C*b,I n){b[0]=48+n/10%10;
b[1]=48+n%10;b[2]=0;}
// z(b) → HH:MM into b, returns b
C* z(C*b){I t=time(0)/60%1440;
d(b,t/60);b[2]=58;d(b+3,t%60);rt b;}

// ────── i/o ──────
#define wr(f,s) write((f),(s),ln(s))
#define w1(s) wr(1,s)             // write to stdout
#define pf(...) printf(__VA_ARGS__)
#define sf(b,...) sprintf((b),__VA_ARGS__)
#define dp(f,...) dprintf((f),__VA_ARGS__)
#define sl(n) usleep((n)*1000)

// gl(fd,buf,max) → read one line from fd into buf, returns length or ≤0
// handles \r\n and \n, partial reads via internal buffer. max is buf size.
I gl(I fd,C*b,I m){static C _b[4096];static I _n;
wl(1){I i=f(_b,"\n");if(i>=0){I l=i;if(i>0&&_b[i-1]==13)l--;
I c=l<m-1?l:m-1;if(c>0){mv(b,_b,c);}b[c]=0;_n-=i+1;mv(_b,_b+i+1,_n);rt l;}
if(_n>=(I)sizeof(_b)-1){_n=0;rt-1;}I n=read(fd,_b+_n,sizeof _b-_n-1);
if(n<=0)rt n;_n+=n;}}

// ────── os ──────
#define ex(n) exit(n)
#define sg(s,f) signal((s),(f))
#define cl(f) close(f)
#define op(p) open((p),0)
#define rn(b,n) do{I _f=op("/dev/urandom");\
read(_f,(b),(n));cl(_f);}while(0)

// ────── math ──────
#define mn(a,b) ((a)<(b)?(a):(b))
#define mx(a,b) ((a)>(b)?(a):(b))
#define ab(n) ((n)<0?-(n):(n))

// ────── memory ──────
#define ml(t,n) (t*)malloc((n)*sizeof(t))
#define fe(p) free(p)

// ────── string builder ──────
typedef struct{C*d;I c;I l;}Buf;
#define bk(n) ({Buf _b={ml(C,n),n,0};_b;})
#define bs(h,s) do{C*_s=(s);I _n=ln(_s);\
  if((h).l+_n>=(h).c){(h).c=mx((h).c*2,(h).l+_n+1);\
  (h).d=realloc((h).d,(h).c);}cp((h).d+(h).l,_s);(h).l+=_n;}while(0)
#define bw(h,...) ((h).l+=sf((h).d+(h).l,__VA_ARGS__))
#define bd(h) ((h).d)
#define bz(h) do{(h).l=0;(h).d[0]=0;}while(0)
#define bf(h) do{fe((h).d);(h).d=N;(h).c=(h).l=0;}while(0)

// ────── arena allocator ──────
// an(n) — create arena, aa(n) — alloc from arena, ar() — reset, ad() — destroy
static C*_ap;static I _as,_ai;
#define an(n) do{_ap=ml(C,n);_as=n;_ai=0;}while(0)
#define aa(n) (_ai+(n)<=_as?(_ai+=n,(V*)(_ap+_ai-(n))):(V*)0)
#define ar() (_ai=0)
#define ad() do{fe(_ap);_ap=N;_as=0;_ai=0;}while(0)

// ────── connection pools ──────
// cn(name,cap) — declare connection pool: cn(clients,64);
// Con: {I fd;C b[512];I bl;C*n;C*u;}  — fd, buffer, buflen, nick, user
typedef struct{I fd;C b[512];I bl;C*n;C*u;}Con;
#define cn(n,c) Con n##_p[c];I n##_n=0

// ca(pool,fd) — add new connection, returns index or -1
#define ca(p,f) ({I _i=-1;if(p##_n<(I)(sizeof(p##_p)/sizeof(*p##_p))){\
  _i=p##_n++;p##_p[_i].fd=f;p##_p[_i].bl=0;\
  p##_p[_i].n=N;p##_p[_i].u=N;}_i;})

// cr(pool,idx) — remove connection at idx (swap-with-last)
#define cr(p,i) do{p##_p[i]=p##_p[--p##_n];}while(0)

// ────── hash maps ──────
// mp(name,cap) — declare map: mp(nicks,64);  hs(nicks,"a",val);  hg(nicks,"a")
#define mp(n,c) static C* n##_k[c];static V* n##_v[c];static I n##_n
#define hs(n,k,v) do{I _f=0;fr(_i,n##_n)if(eq(n##_k[_i],k))\
  {n##_v[_i]=v;_f=1;br;}if(!_f&&n##_n<(I)(sizeof(n##_k)/sizeof(*n##_k)))\
  n##_k[n##_n]=k,n##_v[n##_n++]=v;}while(0)
#define hg(n,k) ({V*_r=N;fr(_i,n##_n)if(eq(n##_k[_i],k)){_r=n##_v[_i];br;}_r;})
#define hh(n,k) ({I _r=0;fr(_i,n##_n)if(eq(n##_k[_i],k)){_r=1;br;}_r;})
#define hd(n,k) fr(_i,n##_n)if(eq(n##_k[_i],k))\
  {n##_k[_i]=n##_k[--n##_n];n##_v[_i]=n##_v[n##_n];br;}

// ────── sets ──────
// sp(name,cap) — declare set: sp(ops,32);  sa(ops,p);  sh(ops,p);  sd(ops,p)
#define sp(n,c) static V* n##_s[c];static I n##_n
#define sa(n,v) do{if(n##_n<(I)(sizeof(n##_s)/sizeof(*n##_s)))\
  n##_s[n##_n++]=v;}while(0)
#define sd(n,v) fr(_i,n##_n)if(n##_s[_i]==v)\
  {n##_s[_i]=n##_s[--n##_n];br;}
#define sh(n,v) ({I _r=0;fr(_i,n##_n)if(n##_s[_i]==v){_r=1;br;}_r;})

// ────── ring buffer ──────
#define ri(t,n) struct{I h;I l;t d[n];}
#define rp(r,v) do{(r).d[(r).h++]=(v);(r).h%=rc(r);\
  if((r).l<rc(r))(r).l++;}while(0)
#define ro(r,i) ((r).d[((r).h-(r).l+(i)+rc(r))%rc(r)])
#define rl(r) ((r).l)
#define rc(r) (sizeof((r).d)/sizeof((r).d[0]))
#define rr(r) do{(r).h=0;(r).l=0;}while(0)

// ────── logging (2-char prefix to stderr) ──────
#define dbg(f,...) dprintf(2,"\x1b[90mDBG\x1b[m " f,##__VA_ARGS__)
#define inf(f,...) dprintf(2,"\x1b[36mINF\x1b[m " f,##__VA_ARGS__)
#define wrn(f,...) dprintf(2,"\x1b[33mWRN\x1b[m " f,##__VA_ARGS__)
#define err(f,...) dprintf(2,"\x1b[31mERR\x1b[m " f,##__VA_ARGS__)

// ────── color DSL — wrap string in ANSI ──────
#define rd(s) "\x1b[31m"s"\x1b[m"
#define gr(s) "\x1b[32m"s"\x1b[m"
#define yl(s) "\x1b[33m"s"\x1b[m"
#define lc(s) "\x1b[36m"s"\x1b[m"
#define up(s) "\x1b[1m"s"\x1b[m"

// ────── test framework ──────
#define tst(n) static V _t_##n()
#define as(c) do{if(!(c)){pf("  FAIL %s:%d: %s\n",__FILE__,__LINE__,#c);\
  _tfail=1;}}while(0)
#define de(m,e,a) do{if((e)!=(a)){pf("  FAIL %s:%d: %s: got %d want %d\n",\
  __FILE__,__LINE__,(m),(I)(a),(I)(e));_tfail=1;}}while(0)
static I _tfail;

// ────── error propagation ──────
#define tr(e) do{if((e)<0)rt-1;}while(0)
#define trp(e) do{if(!(e))rt N;}while(0)
#define trv(e) do{if((e)<0)rt;}while(0)

// ────── bit ops (sb/cb to avoid bs/bf collision with string builder) ──────
#define sb(x,b) ((x)|=1<<(b))
#define cb(x,b) ((x)&=~(1<<(b)))
#define bt(x,b) ((x)>>(b)&1)
#define fb(x,b) ((x)^=1<<(b))
#define bm(m,w) ((w)&(m))

// ────── state machine DSL ──────
// enum { HANDSHAKE, REGISTERED }; stm { se(HANDSHAKE,{go(REGISTERED)}); ... }
#define stm for(I _st=0;_st>=0;) sw(_st)
#define se(n,b) cs n:{b;br;}
#define go(n) do{_st=n;br;}while(0)
#define stp() do{_st=-1;br;}while(0)

// ────── dynamic arrays ──────
// da(I) v = {N,0,0}; ap(v,42); al(v); ax(v);
#define da(t) struct{t*d;I l;I c;}
#define ap(a,v) do{if((a).l+1>(a).c){(a).c=(a).c?(a).c*2:4;\
  (a).d=realloc((a).d,(a).c*sizeof(*(a).d));}(a).d[(a).l++]=(v);}while(0)
#define al(a) ((a).l)
#define av(a) ((a).c)
#define az(a) do{(a).l=0;}while(0)
#define ax(a) do{fe((a).d);(a).d=N;(a).l=0;(a).c=0;}while(0)

// ────── type-generic swap ──────
#define swp(a,b) do{C _t[sz(a)];\
  memcpy(_t,&(a),sz(a));memcpy(&(a),&(b),sz(a));memcpy(&(b),_t,sz(a));}while(0)

// ────── hex encoding / dump ──────
// hx(buf,n) → hex string (static buf). hp(buf,n) → hex+ascii dump to stderr.
static C _hx[4096];
#define hx(b,n) ({C*_h="0123456789abcdef";I _m=mn(n,1364);\
  fr(_i,_m){_hx[_i*3]=_h[((U*)(b))[_i]>>4];\
  _hx[_i*3+1]=_h[((U*)(b))[_i]&15];_hx[_i*3+2]=32;}_hx[_m*3]=0;_hx;})
// hp is a macro so __FILE__/__LINE__ capture the call site
#define hp(b,n) do{C*_h="0123456789abcdef";U*_d=(U*)(b);I _n=mn(n,256);\
  dp(2,"  %s:%d  %d bytes\n",__FILE__,__LINE__,_n);\
  for(I _i=0;_i<_n;_i++){\
  if(_i%16==0)dp(2,"%04x  ",_i);dp(2,"%02x ",_d[_i]);\
  if(_i%16==7)dp(2," ");if(_i%16==15){dp(2,"  ");\
  for(I _j=_i-15;_j<=_i;_j++)dp(2,"%c",_d[_j]>=32&&_d[_j]<127?_d[_j]:46);\
  dp(2,"\n");}}if(_n%16)dp(2,"\n");}while(0)

// ────── config from env vars (declare inside cfg block, semicolons) ──────
// cfg(i32(port,6667);str(host,"0.0.0.0");bln(debug,0););
#define cfg(...) __VA_ARGS__
#define i32(n,d) I n = ({C*_e=getenv(#n);_e?ao(_e):(d);})
#define str(n,d) C* n = ({C*_e=getenv(#n);_e?_e:(d);})
#define bln(n,d) I n = ({C*_e=getenv(#n);_e?eq(_e,"1")||eq(_e,"yes")||eq(_e,"true"):(d);})

// ────── flow ──────
#define sw switch
#define cs case
#define df default
#define sz sizeof

// ────── constants ──────
#define N ((V*)0)            // null
#define ao atoi              // ascii to int
#define CL "\x1b[2J\x1b[H"   // clear screen + home
#define RS "\x1b[m"          // reset attributes
#define RV "\x1b[7m"         // reverse video
#endif
