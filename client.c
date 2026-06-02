// wuchatx C client — IRC terminal client using tinyc.h.  cc -o client client.c
#include "tinyc.h"
#include <netdb.h>
#include <errno.h>
#define _wr(s) wr(1,s)

// ── message ring ──
typedef struct { C t; C n[32]; C x[320]; C tm[6]; } Msg;  // t:'!'=sys '>'=own '<'=other
ri(Msg, 1024) M;

// ── state ──
C _nk[32]="u", _sv[256]="localhost", _ch[64]="#chat";
I _pt=6667, _fd=-1, _sc, _ts=1, _rb=1, _rq, _wt;
volatile I _rr;
static C _nb[8192]; static I _nn;
C _in[1024]; I _il;

// ── push message ──
V _ps(C t,C*n,C*x){Msg m={t};I l=mn(ln(x),319);mv(m.x,x,l);m.x[l]=0;
 if(n){I nl=mn(ln(n),31);mv(m.n,n,nl);m.n[nl]=0;}z(m.tm);rp(M,m);}

// ── check if nick appears in text (case-insensitive) ──
I _hm(C*t,C*nk){C b[320],u[32];I i=0;wl(*t&&i<319)b[i++]=*t<'a'||*t>'z'?*t:*t-32;b[i]=0;
 i=0;wl(*nk&&i<31)u[i++]=*nk<'a'||*nk>'z'?*nk:*nk-32;u[i]=0;rt f(b,u)>=0;}

// ── render screen ──
V _rn(){Buf o=bk(32768);
 bw(o,"\x1b]0;%s%s\x1b\\",_ch,_sc?" ▲":"");
 bw(o,CL RV";36m %s %s%d/%d "RS"\r\n",_ch,_sc?"▲":" ",_sc,rl(M));
 I tot=rl(M),vis=He-2,end=mx(0,tot-_sc),start=mx(0,end-vis);
 for(I i=start;i<end;i++){Msg*m=&ro(M,i);C*co="36";
  if(m->t=='!')co="33";
  el if(m->t=='>')co="32";
  el if(m->t=='<'&&_hm(m->x,_nk))co="1;36";
  if(_ts)bw(o,"\x1b[2m[%s]\x1b[22m ",m->tm);
  if(m->t=='!'){
   bw(o,"\x1b[%sm%s"RS"\r\n",co,m->x);
  }el{C hdr[64];I hl=0;
   if(_ts){sf(hdr,"[%s] <%s>",m->tm,m->n);hl=ln(hdr);}
   el{sf(hdr,"<%s>",m->n);hl=ln(hdr);}
   C*t=m->x;I fl=1;
   wl(*t){I w=fl?Wi-1-hl:Wi-2;if(w<4)w=Wi-3;I tl=ln(t);
    if(tl<=w){bw(o,"\x1b[%sm%s%s"RS"\r\n",co,fl?hdr:"  ",t);br;}
    I b=w;while(b>0&&t[b]!=32)b--;if(!b||b>w)b=w;
    bw(o,"\x1b[%sm%s%.*s"RS"\r\n",co,fl?hdr:"  ",b,t);
    t+=b+(t[b]==32);fl=0;}}
 }
 I shown=mx(0,end-start);
 fr(i,mx(0,He-shown-1))bw(o,"\r\n");
 C pnk[32];I nkl=mn(ln(_nk),Wi-5);mv(pnk,_nk,nkl);pnk[nkl]=0;
 C pi[1024];I pil=mn(_il,Wi-7-nkl);mv(pi,_in,pil);pi[pil]=0;
 bw(o,RV"[%s] > %s"RS,pnk,pi);
 bw(o,"\x1b[%d;1H\x1b[?25l",He);
 _wr(bd(o));bf(o);}

// ── signal handlers ──
V _sw(I sig){(V)sig;s();_rr=1;}
V _si(I sig){(V)sig;_rq=1;}

// ── parse IRC line ──
V _ir(C*l){C*pf="",*rest=l,nn[32]="",*tr="";
 if(*l==':'){pf=l+1;rest=a(l," ");}
 I cl=0;wl(rest[cl]&&rest[cl]!=32)cl++;
 if(cl==4&&!strncmp(rest,"PING",4)){dp(_fd,"PONG %s\r\n",a(rest," "));rt;}
 if(*pf){I ei=f(pf,"!"),ai=f(pf,"@"),en=ei>=0?ei:(ai>=0?ai:ln(pf));
  I nl=mn(en,31);mv(nn,pf,nl);nn[nl]=0;}
 tr=a(l," :");if(!*tr)tr="";
 if(cl==7&&!strncmp(rest,"PRIVMSG",7)){_ps('<',nn,tr);}
 el if(cl==4&&!strncmp(rest,"JOIN",4)){C*ch=a(rest," ");if(*ch==':')ch++;C buf[96];sf(buf,"%s joined %s",nn,ch);_ps('!',"",buf);}
 el if(cl==4&&!strncmp(rest,"QUIT",4)){C buf[96];sf(buf,"%s quit%s%s",nn,*tr?" ":"",tr);_ps('!',"",buf);}
 el if(cl==4&&!strncmp(rest,"PART",4)){C*ch=tk(rest,1);if(*ch==':')ch++;C buf[96];sf(buf,"%s left %s",nn,ch);_ps('!',"",buf);}
 el if(cl==4&&!strncmp(rest,"NICK",4)){C*ne=a(rest," ");if(*ne==':')ne++;C buf[64];sf(buf,"%s → %s",nn,ne);_ps('!',"",buf);}
 el if(*rest>='0'&&*rest<='9'){if(*tr)_ps('!',"",tr);}
}

// ── connect (non-blocking with 5s timeout) ──
I _cn(){struct hostent*he=gethostbyname(_sv);if(!he)rt-1;
 A.sin_family=2;A.sin_port=htons(_pt);mv(&A.sin_addr,he->h_addr_list[0],he->h_length);
 _fd=socket(2,1,0);if(_fd<0)rt-1;
 nb(_fd);
 I cr=connect(_fd,(V*)&A,sz A);
 if(cr<0&&errno!=EINPROGRESS){cl(_fd);_fd=-1;rt-1;}
 if(cr!=0){fd_set wf;FD_ZERO(&wf);FD_SET(_fd,&wf);struct timeval tv={5,0};
  if(select(_fd+1,N,&wf,N,&tv)<=0){cl(_fd);_fd=-1;rt-1;}
  I e=0;socklen_t elen=sz e;getsockopt(_fd,SOL_SOCKET,SO_ERROR,&e,&elen);if(e){cl(_fd);_fd=-1;rt-1;}}
 fcntl(_fd,F_SETFL,fcntl(_fd,F_GETFL)&~O_NONBLOCK);
 dp(_fd,"NICK %s\r\nUSER %s 0 * :%s\r\nJOIN %s\r\n",_nk,_nk,_nk,_ch);_rb=1;_nn=0;rt 0;}

// ── main ──
I main(I c,C**v){
 if(c>1){I l=mn(ln(v[1]),31);mv(_nk,v[1],l);_nk[l]=0;}
 if(c>2){I l=mn(ln(v[2]),255);mv(_sv,v[2],l);_sv[l]=0;}
 if(c>3)_pt=ao(v[3]);
 if(c>4){I l=mn(ln(v[4]),63);mv(_ch,v[4],l);_ch[l]=0;}
 if(eq(_nk,"u")){C rb[4];rn(rb,4);
  fr(i,4){C x=(U)rb[i]%36;_nk[1+i]=x<10?'0'+x:'a'+x-10;}_nk[5]=0;}
 r(1);s();sg(SIGPIPE,SIG_IGN);sg(SIGWINCH,_sw);sg(SIGINT,_si);
 _wr("\x1b[?25l");
 fi{
  if(_rq)br;
  if(_fd<0&&time(0)>=_wt){
   _ps('!',"","connecting...");_rn();_rr=0;
   if(_cn()<0){if(_rq)br;
    _rb=mn(_rb*2,60);_wt=time(0)+_rb;
    C b[48];sf(b,"retrying in %ds",_rb);_ps('!',"",b);_rn();_rr=0;}
   el{_ps('!',"","connected");_rn();_rr=0;}
  }
  if(_rr){_rr=0;_rn();}
  fd_set F;FD_ZERO(&F);FD_SET(0,&F);
  I mf=0;struct timeval tv,*tp=N;
  if(_fd>=0){FD_SET(_fd,&F);mf=_fd;}
  el{I w=_wt-time(0);if(w<0)w=0;tv.tv_sec=mn(w,1);tv.tv_usec=0;tp=&tv;}
  I r=select(mf+1,&F,N,N,tp);
  if(r<0){if(_rq)br;ct;}
  if(_fd>=0&&FD_ISSET(_fd,&F)){
   if(_nn>=(I)sizeof(_nb)-1){_nn=0;_nb[0]=0;}
   I nr=read(_fd,_nb+_nn,sizeof(_nb)-_nn-1);
   if(nr<=0){_ps('!',"","disconnected");cl(_fd);_fd=-1;_nn=0;_wt=time(0);_rn();ct;}
   _nn+=nr;_nb[_nn]=0;
   C*s=_nb;wl(1){C*e=s?strchr(s,'\n'):N;if(!e)br;
    *e=0;if(e>s&&e[-1]=='\r')e[-1]=0;_ir(s);s=e+1;}
   _nn-=s-_nb;if(_nn>0)mv(_nb,s,_nn);_nb[_nn]=0;}
  if(FD_ISSET(0,&F)){C kb[16];I n=read(0,kb,16);if(n<1)ct;
   fr(i,n){C k=kb[i];
    if(k==3){_rq=1;br;}el if(k==12){_rn();}
    el if(k==21){_il=0;_in[0]=0;}el if(k==23){wl(_il>0&&_in[_il-1]==32)_il--;wl(_il>0&&_in[_il-1]!=32)_il--;_in[_il]=0;}
    el if(k==13||k==10){C*t=_in;wl(*t==32)t++;
     if(*t=='/'){C*cmd=t+1;I cl=ln(cmd),sp=f(cmd," ");C*arg=cmd+cl;
      if(sp>=0){cl=sp;arg=cmd+sp+1;}
      if(!cl);
      el if(!strncmp(cmd,"j",cl)||!strncmp(cmd,"join",cl)){I al=mn(ln(arg),63);mv(_ch,arg,al);_ch[al]=0;
       if(_fd>=0)dp(_fd,"JOIN %s\r\n",_ch);}
      el if(!strncmp(cmd,"p",cl)||!strncmp(cmd,"part",cl)){if(_fd>=0)dp(_fd,"PART %s\r\n",_ch);}
      el if(!strncmp(cmd,"q",cl)||!strncmp(cmd,"quit",cl)){if(_fd>=0)dp(_fd,"QUIT :%s\r\n",*arg?arg:"bye");_rq=1;}
      el if(!strncmp(cmd,"n",cl)||!strncmp(cmd,"nick",cl)){I al=mn(ln(arg),31);mv(_nk,arg,al);_nk[al]=0;
       if(_fd>=0)dp(_fd,"NICK %s\r\n",_nk);}
      el if(!strncmp(cmd,"m",cl)||!strncmp(cmd,"msg",cl)){C*msg=a(arg," ");I sp2=f(arg," ");
       if(sp2>=0){arg[sp2]=0;}if(_fd>=0)dp(_fd,"PRIVMSG %s :%s\r\n",arg,msg);}
      el if(!strncmp(cmd,"t",cl)){_ts=!_ts;_ps('!',"",_ts?"timestamps on":"timestamps off");}
      el if(!strncmp(cmd,"clear",cl)){rr(M);}
      el if(!strncmp(cmd,"h",cl)||!strncmp(cmd,"help",cl)){
       C*hlp[]={"/j #chan  join channel","/p        part channel","/q [msg]  quit",
        "/n nick   change nick","/m u msg  private msg","/t        toggle timestamps",
        "/clear    clear buffer","Ctrl+L    redraw screen","Ctrl+U    clear input",
        "Ctrl+W    delete word",N};
       for(I hi=0;hlp[hi];hi++)_ps('!',"",hlp[hi]);}
      el{if(_fd>=0)dp(_fd,"%s\r\n",t+1);}
     }el if(*t){_ps('>',_nk,t);if(_fd>=0)dp(_fd,"PRIVMSG %s :%s\r\n",_ch,t);}
     _il=0;_in[0]=0;_sc=0;}
    el if(k==127){if(_il>0)_in[--_il]=0;}
    el if(k==27&&i+2<n&&kb[i+1]==91){C d=kb[i+2];
     if(d==65)_sc++;el if(d==66&&_sc>0)_sc--;el if(d==53)_sc+=He;el if(d==54)_sc-=mn(_sc,He);i+=2;}
    el if(k>=32&&k<127&&_il<1023){_in[_il++]=k;_in[_il]=0;}
   }}
  _rn();}
 if(_fd>=0){wr(_fd,"QUIT :bye\r\n");cl(_fd);}
 r(0);_wr("\x1b[?25h"RS"\n");rt 0;
}
