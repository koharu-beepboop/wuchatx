let p=process,[nick="u"+Math.random().toString(36).slice(-4),srv="localhost",prt="6667",chan="#chat"]=p.argv.slice(2),M=[],I="",S=0,B="",T=1
p.stdin.isTTY&&p.stdin.setRawMode(1)
let{W=80,H=24}=p.stdout,c
let Q=m=>{Q.q=1;p.stdout.write("\x1b[?25h\x1b[m\n"+(m||""));p.stdin.isTTY&&p.stdin.setRawMode(0);c.end();p.exit()}
let ts=()=>{let d=new Date();return("0"+d.getHours()).slice(-2)+":"+("0"+d.getMinutes()).slice(-2)}
let R=()=>{
  let o=`\x1b]0;${chan}${S?" ▲"+S:""}\x1b\\\x1b[2J\x1b[H\x1b[7;36m ${chan} ${S?"▲"+S+"/"+M.length:" "}\x1b[m\r\n`
  let D=[]
  for(let m of M){
    let co=m[0]==nick?"32":m[0]=="!"?"33":m[0]!=nick&&m[1].toLowerCase().includes(nick.toLowerCase())?"1;36":"36"
    let tms=T?`\x1b[2m[${m[2]}]\x1b[22m `:""
    if(m[0]=="!"){
      D.push([co,tms+m[1]])
    }else{
      let h=tms+`<${m[0]}>`,t=m[1],fl=1,hl=h.replace(/\x1b\[[0-9;]*m/g,"").length
      while(t){
        let w=fl?W-1-hl:W-2
        if(w<4)w=W-3
        if(t.length<=w){D.push([co,(fl?h+" ":"  ")+t]);break}
        let b=t.lastIndexOf(" ",w)
        if(b<=0||b>w)b=w
        D.push([co,(fl?h+" ":"  ")+t.slice(0,b)])
        t=t.slice(b+(t[b]===" "?1:0));fl=0
      }
    }
  }
  let v=D.slice(Math.max(0,D.length-H+2-S),D.length-S)
  for(let m of v)o+=`\x1b[${m[0]}m${m[1]}\x1b[m\r\n`
  o+="\r\n".repeat(Math.max(0,H-v.length-1))
  let pr=`[${nick.slice(0,W-5)}] > `
  let pi=I.slice(-Math.max(1,W-pr.length-1))
  o+=`\x1b[7m${pr}${pi}\x1b[m\x1b[${H+1};${pr.length+pi.length+1}H\x1b[?25h`
  p.stdout.write(o)
}
let connect=()=>{c=(prt=="6697"?require("tls"):require("net")).connect({port:+prt,host:srv,rejectUnauthorized:false},()=>{Q.b=1;if(Q.c)M.push(["!","reconnected",ts()]),Q.c=0,R();c.write(`NICK ${nick}\r\nUSER ${nick} 0 * :${nick}\r\nJOIN ${chan}\r\n`)});c.on("error",e=>{Q.e=e});c.on("close",()=>{if(Q.q)return;let e=Q.e;Q.e=null;Q.c=1;let b=Q.b=Math.min((Q.b||1)*2,60);M.push(["!",`disconnected${e?`: ${e.code||e.message||e}`:""} — retrying in ${b}s`,ts()]);R();setTimeout(connect,b*1e3)});c.on("data",d=>{
  B+=d;let i,l,a,r,n
  while(~(i=B.indexOf("\r\n"))){
    l=B.slice(0,i);B=B.slice(i+2)
    if(!l)continue
    a=l.split(" ");r=a[0];n=r.slice(1).split("!")[0]
    if(r=="PING"){c.write("PONG "+a.slice(1).join(" ")+"\r\n");continue}
    if(a[1]=="PRIVMSG")M.push([n,l.split(" :").slice(1).join(" :"),ts()])
    else if(a[1]=="JOIN")M.push(["!",n+" joined "+a[2].replace(/^:/,""),ts()])
    else if(a[1]=="PART")M.push(["!",n+" left "+a[2],ts()])
    else if(a[1]=="QUIT")M.push(["!",n+" quit",ts()])
    else if(a[1]=="NICK")M.push(["!",n+" → "+a[2].slice(1),ts()])
    else if(+a[1]>99){let m=l.split(" :").slice(1).join(" :");if(m)M.push(["!",m,ts()])}
  }
  R()
})}
connect()
p.stdin.on("data",d=>{
  let k=d[0]
  if(k==3)Q()
  else if(k==12){R();return}
  else if(k==21){I="";S=0}
  else if(k==23)I=I.replace(/\S+\s*$/,"")
  else if(k==13){let t=I.trim();I="";S=0
    if(t[0]=="/"){let[x,...r]=t.slice(1).split(" "),j=r.join(" ")
      if(x=="j"||x=="join"){chan=j;c.write(`JOIN ${chan}\r\n`)}
      else if(x=="p"||x=="part")c.write(`PART ${chan}\r\n`)
      else if(x=="q"||x=="quit"){c.write(`QUIT :${j||"bye"}\r\n`);Q()}
      else if(x=="n"||x=="nick"){nick=j;c.write(`NICK ${nick}\r\n`)}
      else if(x=="m"||x=="msg"){let[n,...m]=r;c.write(`PRIVMSG ${n} :${m.join(" ")}\r\n`)}
      else if(x=="t"){T=!T;M.push(["!","timestamps "+(T?"on":"off"),ts()])}
      else if(x=="clear")M=[]
      else if(x=="h"||x=="help"){
        for(let l of["/j #chan  join channel","/p        part channel","/q [msg]  quit","/n nick   change nick","/m u msg  private msg","/t        toggle timestamps","/clear    clear buffer","Ctrl+L    redraw screen","Ctrl+U    clear input","Ctrl+W    delete word"])M.push(["!",l,ts()])
      }
      else c.write(t.slice(1)+"\r\n")
    }else if(t){M.push([nick,t,ts()]);c.write(`PRIVMSG ${chan} :${t}\r\n`)}
  }
  else if(k==127)I=I.slice(0,-1)
  else if(k==27&&d[1]==91){let x=d[2];S+=x==65?1:x==66?-1:x==53?H:x==54?-H:0;S<0&&(S=0)}
  else if(k>=32&&d.length==1)I+=d
  R()
})
p.on("SIGWINCH",()=>(W=p.stdout.columns,H=p.stdout.rows,R()))
p.on("SIGINT",Q)
p.stdin.resume()
p.stdout.write("\x1b[?25l")
setTimeout(R,100)
