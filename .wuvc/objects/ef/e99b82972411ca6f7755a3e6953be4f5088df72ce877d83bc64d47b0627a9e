const C=new Map,N=new Map,R=(s,n,t)=>s.write(`:server ${("00"+n).slice(-3)} ${s.n} ${t}\r\n`)
Bun.listen({port:6667,hostname:"0.0.0.0",socket:{open:s=>s.b="",data(s,d){s.b+=d
let T=s=>s.split(" :").slice(1).join(" :"),i;while(~(i=s.b.indexOf("\r\n"))){let l=s.b.slice(0,i),a=l.split(" "),c=a[0].toUpperCase()
s.b=s.b.slice(i+2)
if(c=="NICK"){let o=s.n;s.n=a[1];if(o){N.delete(o);N.set(s.n,s)
if(s.h)for(let h of s.h)for(let x of C.get(h))x.write(`:${o}!${s.u||o}@0 NICK :${s.n}\r\n`)}}
else if(c=="USER"){let u=a[1];s.u=u;N.set(s.n,s)
R(s,1,`:Welcome ${s.n}!${u}@0`);R(s,4,`server tiny * *`);R(s,376,`:End of /MOTD`)}
else if(c=="JOIN"){let h=a[1];C.has(h)||C.set(h,new Set),C.get(h).add(s);(s.h||=[]).push(h)
for(let x of C.get(h))x.write(`:${s.n}!${s.u||s.n}@0 JOIN ${h}\r\n`)}
else if(c=="PART"){let h=a[1];C.get(h)?.delete(s);s.h=s.h?.filter(x=>x!==h)
for(let x of C.get(h)||[])x.write(`:${s.n}!${s.u||s.n}@0 PART ${h}\r\n`)}
else if(c=="PRIVMSG"){let h=a[1],m=T(l)||a[2]||""
let t=h[0]==="#"?C.get(h)||[]:[N.get(h)].filter(Boolean)
for(let x of t)x!==s&&x.write(`:${s.n}!${s.u||s.n}@0 PRIVMSG ${h} :${m}\r\n`)}
else if(c=="QUIT"){let m=T(l)||"gone";N.delete(s.n)
if(s.h)for(let h of s.h){let ch=C.get(h);if(ch){ch.delete(s);for(let x of ch)x.write(`:${s.n}!${s.u||s.n}@0 QUIT :${m}\r\n`)}}
s.h=0,s.end()}
else if(c=="PING")s.write(`PONG ${a[1]||""}\r\n`)
else if(c=="CAP"&&a[1]?.toUpperCase()=="LS")s.write(`CAP * LS :\r\n`)
else if(c=="WHO"){let h=a[1],u=C.get(h);if(u){for(let x of u)R(s,352,`${h} ${x.n} 0 server ${x.n} H :0 ${x.n}`);R(s,315,`${h} :End of /WHO list`)}}
else if(c=="MODE")s.write(`:server 221 ${s.n} +i\r\n`)
}},close(s){N.delete(s.n);if(s.h)for(let h of s.h){let ch=C.get(h);if(ch){ch.delete(s);for(let x of ch)x.write(`:${s.n}!${s.u||s.n}@0 QUIT :gone\r\n`)}}}}})
