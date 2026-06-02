const C=new Map,N=new Map,T=new Map,O=new Map,M=new Map,P=process.env.PASS||null
const R=(s,n,t)=>s.write(`:server ${("00"+n).slice(-3)} ${s.n||"*"} ${t}\r\n`)
Bun.listen({port:6667,hostname:"0.0.0.0",socket:{open:s=>(s.b="",s.p=null),data(s,d){s.b+=d
let X=s=>s.split(" :").slice(1).join(" :"),i;while(~(i=s.b.indexOf("\r\n"))){let l=s.b.slice(0,i),a=l.split(" "),c=a[0].toUpperCase()
s.b=s.b.slice(i+2)
if(c=="PASS"){s.p=a[1]}
else if(c=="NICK"){let n=a[1];if(N.has(n)&&N.get(n)!==s){s.write(`:server 433 * ${n} :Nickname is already in use\r\n`)}else{let o=s.n;s.n=n;if(o){N.delete(o);N.set(s.n,s)
if(s.h)for(let h of s.h)for(let x of C.get(h))x.write(`:${o}!${s.u||o}@0 NICK :${s.n}\r\n`)}}}
else if(c=="USER"){let u=a[1];if(s.n&&N.has(s.n)&&N.get(s.n)!==s){s.write(`:server 433 * ${s.n} :Nickname is already in use\r\n`)}
else if(P&&s.p!==P){R(s,464,`:Password incorrect`);s.end()}
else{s.u=u;N.set(s.n,s)
R(s,1,`:Welcome ${s.n}!${u}@0`);R(s,4,`server tiny * *`);R(s,372,`:- wuna chat`);R(s,372,`:- be epiku`);R(s,372,`:- dont fak things up`);R(s,376,`:End of /MOTD`)}
}else if(c=="JOIN"){let h=a[1];let nw=!C.has(h);if(nw){C.set(h,new Set);O.set(h,new Set);M.set(h,"nt")}C.get(h).add(s);if(nw)O.get(h).add(s);(s.h||=[]).push(h)
for(let x of C.get(h))x.write(`:${s.n}!${s.u||s.n}@0 JOIN ${h}\r\n`)}
else if(c=="PART"){let h=a[1];C.get(h)?.delete(s);O.get(h)?.delete(s);s.h=s.h?.filter(x=>x!==h)
for(let x of C.get(h)||[])x.write(`:${s.n}!${s.u||s.n}@0 PART ${h}\r\n`);let ch=C.get(h);if(ch&&!ch.size){C.delete(h);T.delete(h);O.delete(h);M.delete(h)}}
else if(c=="PRIVMSG"){let h=a[1],m=X(l)||a[2]||""
if(h[0]=="#"&&M.has(h)&&M.get(h).includes("n")&&!C.get(h)?.has(s))s.write(`:server 404 ${s.n} ${h} :Cannot send to channel\r\n`)
else{let t=h[0]==="#"?C.get(h)||[]:[N.get(h)].filter(Boolean)
for(let x of t)x!==s&&x.write(`:${s.n}!${s.u||s.n}@0 PRIVMSG ${h} :${m}\r\n`)}}
else if(c=="QUIT"){let m=X(l)||"gone";N.delete(s.n)
if(s.h)for(let h of s.h){let ch=C.get(h);if(ch){ch.delete(s);O.get(h)?.delete(s);for(let x of ch)x.write(`:${s.n}!${s.u||s.n}@0 QUIT :${m}\r\n`);if(!ch.size){C.delete(h);T.delete(h);O.delete(h);M.delete(h)}}}
s.h=null,s.end()}
else if(c=="TOPIC"){let h=a[1],t=X(l);if(!C.has(h))s.write(`:server 442 ${s.n} ${h} :Not on that channel\r\n`)
else if(!l.includes(" :")){let tp=T.get(h);tp?R(s,332,`${h} :${tp}`):R(s,331,`${h} :No topic is set`)}
else{if(M.get(h)?.includes("t")&&!O.get(h)?.has(s))s.write(`:server 482 ${s.n} ${h} :You're not channel operator\r\n`)
else{T.set(h,t);for(let x of C.get(h))x.write(`:${s.n}!${s.u||s.n}@0 TOPIC ${h} :${t}\r\n`)}}}
else if(c=="KICK"){let h=a[1],n=a[2],m=X(l)||n;if(!C.has(h)||!C.get(h).has(s))s.write(`:server 442 ${s.n} ${h} :Not on that channel\r\n`)
else if(!O.get(h)?.has(s))s.write(`:server 482 ${s.n} ${h} :You're not channel operator\r\n`)
else{let t=N.get(n);if(!t||!C.get(h).has(t))s.write(`:server 441 ${s.n} ${n} ${h} :They aren't on that channel\r\n`)
else{for(let x of C.get(h))x.write(`:${s.n}!${s.u||s.n}@0 KICK ${h} ${n} :${m}\r\n`);C.get(h).delete(t);O.get(h)?.delete(t);t.h=t.h?.filter(x=>x!==h);if(!C.get(h).size){C.delete(h);T.delete(h);O.delete(h);M.delete(h)}}}}
else if(c=="MODE"){let h=a[1];if(!h||h[0]!="#")s.write(`:server 221 ${s.n||"*"} +i\r\n`)
else if(!C.has(h))s.write(`:server 442 ${s.n} ${h} :Not on that channel\r\n`)
else if(!a[2]){let md=M.get(h)||"";s.write(`:server 324 ${s.n} ${h} +${md}\r\n`)}
else if(!O.get(h)?.has(s))s.write(`:server 482 ${s.n} ${h} :You're not channel operator\r\n`)
else{let md=M.get(h)||"",fl=a[2],ad=fl[0]=="+",ch=fl.slice(1)
if(ch=="o"){let n=a[3],t=N.get(n);if(!t||!C.get(h)?.has(t))s.write(`:server 441 ${s.n} ${n} ${h} :They aren't on that channel\r\n`)
else{if(ad){O.get(h)||O.set(h,new Set);O.get(h).add(t)}else O.get(h)?.delete(t)
for(let x of C.get(h))x.write(`:${s.n}!${s.u||s.n}@0 MODE ${h} ${fl} ${n}\r\n`)}}
else if(ch=="t"||ch=="n"){if(ad&&!md.includes(ch))md+=ch;else if(!ad)md=md.replace(ch,"");M.set(h,md)
for(let x of C.get(h))x.write(`:${s.n}!${s.u||s.n}@0 MODE ${h} ${fl}\r\n`)}
else s.write(`:server 472 ${s.n} ${ch} :Unknown mode\r\n`)}}
else if(c=="LIST"){R(s,321,`Channel :Users  Name`);for(let[h,ch]of C)R(s,322,`${h} ${ch.size} :${T.get(h)||""}`);R(s,323,`:End of /LIST`)}
else if(c=="NAMES"){let h=a[1];if(!C.has(h))R(s,366,`${h} :End of /NAMES list`)
else{let ns=[...C.get(h)].map(x=>(O.get(h)?.has(x)?"@":"")+x.n).join(" ");R(s,353,`= ${h} :${ns}`);R(s,366,`${h} :End of /NAMES list`)}}
else if(c=="PING")s.write(`PONG ${a[1]||""}\r\n`)
else if(c=="CAP"&&a[1]?.toUpperCase()=="LS")s.write(`CAP * LS :\r\n`)
else if(c=="WHO"){let h=a[1],u=C.get(h);if(u){for(let x of u)R(s,352,`${h} ${x.n} 0 server ${x.n} H :0 ${x.n}`);R(s,315,`${h} :End of /WHO list`)}}
}},close(s){N.delete(s.n);if(s.h!=null)for(let h of s.h){let ch=C.get(h);if(ch){ch.delete(s);O.get(h)?.delete(s);for(let x of ch)x.write(`:${s.n}!${s.u||s.n}@0 QUIT :gone\r\n`);if(!ch.size){C.delete(h);T.delete(h);O.delete(h);M.delete(h)}}}}}})
