// wuchatx IRC server — tinyc.h.  cc -o server server.c
#include "tinyc.h"
typedef struct{I fd;C b[512];I bl;C*n,*u,*p;C*h[32];I hn;I live;}Sk;
typedef struct{Sk**d;I l;I c;}Ml;
typedef struct{C n[64];Ml m,o;C t[320];C d[8];}Ch;
Sk Ss[256];I Sn;mp(Nk,64);mp(Cn,64);C*Pw;
V re(Sk*s,I n,C*t){dp(s->fd,":server %03d %s %s\r\n",n,s->n?s->n:"*",t);}
V bc(Ch*c,C*x){fr(i,al(c->m)){Sk*y=c->m.d[i];if(y->fd>=0)wr(y->fd,x);}}
I isch(C*cn,Sk*s){Ch*c=hg(Cn,cn);if(!c)rt 0;fr(i,al(c->m))if(c->m.d[i]==s)rt 1;rt 0;}
I isop(C*cn,Sk*s){Ch*c=hg(Cn,cn);if(!c)rt 0;fr(i,al(c->o))if(c->o.d[i]==s)rt 1;rt 0;}
V rm(Sk*s,Ch*c){
 fr(i,al(c->m))if(c->m.d[i]==s){c->m.d[i]=c->m.d[al(c->m)-1];c->m.l--;br;}
 fr(i,al(c->o))if(c->o.d[i]==s){c->o.d[i]=c->o.d[al(c->o)-1];c->o.l--;br;}
 fr(i,s->hn)if(eq(s->h[i],c->n)){s->h[i]=s->h[--s->hn];br;}
 if(!al(c->m)){hd(Cn,c->n);ax(c->m);ax(c->o);fe(c);}}
V qs(Sk*s,C*msg){
 if(s->n)hd(Nk,s->n);
 wl(s->hn){Ch*c=hg(Cn,s->h[0]);
  if(c){if(msg&&*msg){C b[512];sf(b,":%s!%s@0 QUIT :%s\r\n",s->n?s->n:"*",s->u?s->u:(s->n?s->n:"*"),msg);bc(c,b);}rm(s,c);}el s->hn=0;}
 cl(s->fd);s->fd=-1;s->live=0;}
V hl(Sk*s,C*l){
 I cl=0;wl(l[cl]&&l[cl]!=32)cl++;C cm[32];I cml=mn(cl,31);mv(cm,l,cml);cm[cml]=0;us(cm);
 C*ar=l+cl;wl(*ar==32)ar++;C*tr=a(l," :");if(!*tr)tr="";
 if(eq(cm,"PASS")){C*tk0=tk(ar,0);I tl=ln(tk0);s->p=aa(tl+1);cp(s->p,tk0);}
 el if(eq(cm,"NICK")){C nn[32];I nl=mn(ln(tk(ar,0)),31);mv(nn,tk(ar,0),nl);nn[nl]=0;
  if(hh(Nk,nn)&&hg(Nk,nn)!=s){dp(s->fd,":server 433 * %s :Nickname is already in use\r\n",nn);rt;}
  C*on=s->n;s->n=aa(nl+1);cp(s->n,nn);
  if(on){hd(Nk,on);hs(Nk,s->n,s);C b[512];sf(b,":%s!%s@0 NICK :%s\r\n",on,s->u?s->u:on,s->n);fr(i,s->hn){Ch*c=hg(Cn,s->h[i]);if(c)bc(c,b);}}}
 el if(eq(cm,"USER")){C*un=tk(ar,0);I ul=mn(ln(un),31);
  if(s->n&&hh(Nk,s->n)&&hg(Nk,s->n)!=s){dp(s->fd,":server 433 * %s :Nickname is already in use\r\n",s->n);rt;}
  if(*Pw&&(!s->p||!eq(s->p,Pw))){re(s,464,":Password incorrect");cl(s->fd);s->fd=-1;s->live=0;rt;}
  s->u=aa(ul+1);cp(s->u,un);s->u[ul]=0;hs(Nk,s->n,s);
  re(s,1,":Welcome to wuchatx!");re(s,2,":Your host is tinyc");re(s,3,":This server was created never");
  re(s,4,"tinyc * *");re(s,372,":- wuna chat");re(s,372,":- be epiku");re(s,372,":- dont fak things up");re(s,376,":End of /MOTD");}
 el if(eq(cm,"JOIN")&&s->n){C cn[64];I cnl=mn(ln(tk(ar,0)),63);mv(cn,tk(ar,0),cnl);cn[cnl]=0;
  if(cn[0]==':'){mv(cn,cn+1,--cnl);cn[cnl]=0;}Ch*c=hg(Cn,cn);I nw=!c;
  if(nw){c=ml(Ch,1);mv(c->n,cn,cnl+1);c->m.d=N;c->m.l=0;c->m.c=0;c->o.d=N;c->o.l=0;c->o.c=0;c->t[0]=0;cp(c->d,"nt");hs(Cn,c->n,c);}
  ap(c->m,s);if(nw)ap(c->o,s);if(s->hn<32)s->h[s->hn++]=c->n;
  C b[512];sf(b,":%s!%s@0 JOIN %s\r\n",s->n,s->u?s->u:s->n,cn);bc(c,b);}
 el if(eq(cm,"PART")){C cn[64];I cnl=mn(ln(tk(ar,0)),63);mv(cn,tk(ar,0),cnl);cn[cnl]=0;
  if(cn[0]==':'){mv(cn,cn+1,--cnl);cn[cnl]=0;}Ch*c=hg(Cn,cn);
  if(c){C b[512];sf(b,":%s!%s@0 PART %s :%s\r\n",s->n?s->n:"*",s->u?s->u:(s->n?s->n:"*"),cn,*tr?tr:"");bc(c,b);rm(s,c);}}
 el if(eq(cm,"PRIVMSG")){C tgb[64];C*tg0=tk(ar,0);I tgl=mn(ln(tg0),63);mv(tgb,tg0,tgl);tgb[tgl]=0;C*tg=tgb;C*m_=tr;
  if(!*m_){m_=tk(ar,1);if(!*m_)m_="";}
  if(tg[0]=='#'){Ch*c=hg(Cn,tg);
   if(!c){dp(s->fd,":server 401 %s %s :No such nick/channel\r\n",s->n?s->n:"*",tg);rt;}
   I no=fc(c->d,"n")>=0,in_=0;fr(j,al(c->m))if(c->m.d[j]==s){in_=1;br;}
   if(no&&!in_){dp(s->fd,":server 404 %s %s :Cannot send to channel\r\n",s->n?s->n:"*",tg);rt;}
   C b[1024];sf(b,":%s!%s@0 PRIVMSG %s :%s\r\n",s->n?s->n:"*",s->u?s->u:(s->n?s->n:"*"),tg,m_);
   fr(j,al(c->m)){Sk*x=c->m.d[j];if(x!=s&&x->fd>=0)wr(x->fd,b);}}
  el{Sk*t=hg(Nk,tg);if(t&&t->fd>=0)dp(t->fd,":%s!%s@0 PRIVMSG %s :%s\r\n",s->n?s->n:"*",s->u?s->u:(s->n?s->n:"*"),tg,m_);}}
 el if(eq(cm,"QUIT")){qs(s,*tr?tr:"gone");}
 el if(eq(cm,"TOPIC")){C cn[64];I cnl=mn(ln(tk(ar,0)),63);mv(cn,tk(ar,0),cnl);cn[cnl]=0;
  if(cn[0]==':'){mv(cn,cn+1,--cnl);cn[cnl]=0;}Ch*c=hg(Cn,cn);
  if(!c){dp(s->fd,":server 442 %s %s :Not on that channel\r\n",s->n?s->n:"*",cn);rt;}
  if(f(l," :")<0){C rb[512];if(*c->t){sf(rb,"%s :%s",cn,c->t);re(s,332,rb);}el{sf(rb,"%s :No topic is set",cn);re(s,331,rb);}}
  el{I to=fc(c->d,"t")>=0;if(to&&!isop(cn,s)){dp(s->fd,":server 482 %s %s :You're not channel operator\r\n",s->n?s->n:"*",cn);rt;}
   I tl=mn(ln(tr),319);mv(c->t,tr,tl);c->t[tl]=0;C b[512];sf(b,":%s!%s@0 TOPIC %s :%s\r\n",s->n?s->n:"*",s->u?s->u:(s->n?s->n:"*"),cn,c->t);bc(c,b);}}
 el if(eq(cm,"KICK")){C cn[64];I cnl=mn(ln(tk(ar,0)),63);mv(cn,tk(ar,0),cnl);cn[cnl]=0;
  if(cn[0]==':'){mv(cn,cn+1,--cnl);cn[cnl]=0;}C*tn=tk(ar,1);C*km=tr;if(!*km)km=tn;
  if(!hh(Cn,cn)||!isch(cn,s)){dp(s->fd,":server 442 %s %s :Not on that channel\r\n",s->n?s->n:"*",cn);rt;}
  if(!isop(cn,s)){dp(s->fd,":server 482 %s %s :You're not channel operator\r\n",s->n?s->n:"*",cn);rt;}
  Sk*t=hg(Nk,tn);if(!t||!isch(cn,t)){dp(s->fd,":server 441 %s %s %s :They aren't on that channel\r\n",s->n?s->n:"*",tn,cn);rt;}
  Ch*c=hg(Cn,cn);C b[512];sf(b,":%s!%s@0 KICK %s %s :%s\r\n",s->n?s->n:"*",s->u?s->u:(s->n?s->n:"*"),cn,tn,km);bc(c,b);rm(t,c);}
 el if(eq(cm,"MODE")){C*h_=tk(ar,0);
  if(!*h_||h_[0]!='#'){dp(s->fd,":server 221 %s +i\r\n",s->n?s->n:"*");rt;}
  C cn[64];I cnl=mn(ln(h_),63);mv(cn,h_,cnl);cn[cnl]=0;
  if(cn[0]==':'){mv(cn,cn+1,--cnl);cn[cnl]=0;}
  if(!hh(Cn,cn)){dp(s->fd,":server 442 %s %s :Not on that channel\r\n",s->n?s->n:"*",cn);rt;}
  C flb[8];C*fl0=tk(ar,1);I fll=mn(ln(fl0),7);mv(flb,fl0,fll);flb[fll]=0;C*fl=flb;
  if(!*fl){Ch*c=hg(Cn,cn);dp(s->fd,":server 324 %s %s +%s\r\n",s->n?s->n:"*",cn,c->d);rt;}
  if(!isop(cn,s)){dp(s->fd,":server 482 %s %s :You're not channel operator\r\n",s->n?s->n:"*",cn);rt;}
  Ch*c=hg(Cn,cn);I ad=(fl[0]=='+');C ch=fl[1];
  if(ch=='o'){C*rn=tk(ar,2);Sk*t=hg(Nk,rn);
   if(!t||!isch(cn,t)){dp(s->fd,":server 441 %s %s %s :They aren't on that channel\r\n",s->n?s->n:"*",rn,cn);rt;}
   if(ad){I fnd=0;fr(j,al(c->o))if(c->o.d[j]==t){fnd=1;br;}if(!fnd)ap(c->o,t);}
   el{fr(j,al(c->o))if(c->o.d[j]==t){c->o.d[j]=c->o.d[al(c->o)-1];c->o.l--;br;}}
   C b[512];sf(b,":%s!%s@0 MODE %s %s %s\r\n",s->n?s->n:"*",s->u?s->u:(s->n?s->n:"*"),cn,fl,rn);bc(c,b);}
  el if(ch=='t'||ch=='n'){C*mp=strchr(c->d,ch);
   if(ad&&!mp){I l=ln(c->d);c->d[l]=ch;c->d[l+1]=0;}el if(!ad&&mp){I rest=ln(mp+1);mv(mp,mp+1,rest+1);}
   C b[512];sf(b,":%s!%s@0 MODE %s %s\r\n",s->n?s->n:"*",s->u?s->u:(s->n?s->n:"*"),cn,fl);bc(c,b);}
  el{C mb[2]={ch,0};dp(s->fd,":server 472 %s %s :Unknown mode\r\n",s->n?s->n:"*",mb);}}
 el if(eq(cm,"LIST")){re(s,321,"Channel :Users  Name");fr(i,Cn_n){Ch*c=(Ch*)Cn_v[i];C b[512];sf(b,"%s %d :%s",Cn_k[i],al(c->m),*c->t?c->t:"");re(s,322,b);}re(s,323,":End of /LIST");}
 el if(eq(cm,"NAMES")){C*h_=tk(ar,0);C cn[64];I cnl=mn(ln(h_),63);mv(cn,h_,cnl);cn[cnl]=0;
  if(cn[0]==':'){mv(cn,cn+1,--cnl);cn[cnl]=0;}Ch*c=hg(Cn,cn);
  if(!c){C nb[128];sf(nb,"%s :End of /NAMES list",cn);re(s,366,nb);rt;}
  Buf nb=bk(512);fr(i,al(c->m)){Sk*x=c->m.d[i];I is_op=0;fr(j,al(c->o))if(c->o.d[j]==x){is_op=1;br;}if(i>0)bs(nb," ");if(is_op)bs(nb,"@");bs(nb,x->n?x->n:"*");}
  C rb[512];sf(rb,"= %s :%s",cn,bd(nb));re(s,353,rb);C n2[128];sf(n2,"%s :End of /NAMES list",cn);re(s,366,n2);bf(nb);}
 el if(eq(cm,"PING")){dp(s->fd,"PONG %s\r\n",tk(ar,0));}
 el if(eq(cm,"CAP")&&eq(tk(ar,0),"LS")){wr(s->fd,"CAP * LS :\r\n");}
 el if(eq(cm,"WHO")){C*h_=tk(ar,0);C cn[64];I cnl=mn(ln(h_),63);mv(cn,h_,cnl);cn[cnl]=0;
  if(cn[0]==':'){mv(cn,cn+1,--cnl);cn[cnl]=0;}Ch*c=hg(Cn,cn);
  if(c){fr(i,al(c->m)){Sk*x=c->m.d[i];C b[512];sf(b,"%s %s 0 server %s H :0 %s",cn,x->n?x->n:"*",x->n?x->n:"*",x->n?x->n:"*");re(s,352,b);}}
  C wb[128];sf(wb,"%s :End of /WHO list",cn);re(s,315,wb);}}
I main(){
 an(65536);Pw=getenv("PASS");if(!Pw)Pw="";I fd=l(6667);nb(fd);inf("wuchatx on 6667\n");
 fi{fd_set F;FD_ZERO(&F);FD_SET(fd,&F);I mf=fd;
  fr(i,Sn)if(Ss[i].live&&Ss[i].fd>=0){FD_SET(Ss[i].fd,&F);mf=mx(mf,Ss[i].fd);}
  select(mf+1,&F,N,N,N);
  if(FD_ISSET(fd,&F)){I cf=ac(fd);
   if(cf>=0&&Sn<256){nb(cf);Sk*s=&Ss[Sn++];s->fd=cf;s->bl=0;s->n=N;s->u=N;s->p=N;s->hn=0;s->live=1;}el if(cf>=0)cl(cf);}
  fr(i,Sn)if(Ss[i].live&&Ss[i].fd>=0&&FD_ISSET(Ss[i].fd,&F)){Sk*s=&Ss[i];I cap=sz(s->b)-s->bl-1;
   if(cap<=0){s->bl=0;s->b[0]=0;cap=sz(s->b)-1;}I nr=read(s->fd,s->b+s->bl,cap);
   if(nr<=0){qs(s,"gone");ct;}s->bl+=nr;s->b[s->bl]=0;C*p=s->b;
   wl(1){C*e=strchr(p,'\n');if(!e)br;*e=0;if(e>p&&e[-1]=='\r')e[-1]=0;hl(s,p);p=e+1;}
   s->bl-=p-s->b;if(s->bl>0)mv(s->b,p,s->bl);s->b[s->bl]=0;}}
}
