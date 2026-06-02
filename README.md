# wuchatx

a tiny IRC ecosystem — a terminal client and a server, both written from scratch with zero dependencies ✨

## client.js

a terminal-based IRC client with a raw-ANSI interface. connects to any IRC server on port 6667.

```
bun client.js [nick] [channel]
```

if no nick is given, a random one is generated (e.g. `u4x8q`). default channel is `#chat`.

### slash commands

| cmd | description |
|-----|-------------|
| `/j #chan` | join a channel |
| `/p` | part current channel |
| `/q [msg]` | quit with optional message |
| `/n nick` | change nick |
| `/m user msg` | private message a user |
| `/t` | toggle timestamps |
| `/clear` | clear message buffer |
| `/h` | show help |

### keyboard shortcuts

| key | action |
|-----|--------|
| `↑` / `↓` | scroll buffer |
| `PgUp` / `PgDn` | scroll page |
| `Ctrl+L` | redraw screen |
| `Ctrl+U` | clear input |
| `Ctrl+W` | delete word |
| `Ctrl+C` | quit |

## server.js

a minimal IRC server built on bun's TCP listener. implements the core IRC protocol:

- `NICK`, `USER` — connection handshake
- `JOIN`, `PART` — channel management
- `PRIVMSG` — messaging
- `QUIT` — disconnect
- `PING`/`PONG` — keepalive
- `CAP LS`, `WHO`, `MODE` — basic capabilities

```
bun server.js
```

listens on port **6667** on all interfaces.

## tech

- **runtime:** [bun](https://bun.sh) (server), node/bun (client)
- **dependencies:** zero. everything is from scratch using built-in modules (`net`, `Bun.listen`)
- **client:** raw ANSI terminal rendering — 83 lines
- **server:** full IRC protocol subset — 23 lines

## why

because IRC is timeless and building things from scratch is fun (｡♥‿♥｡)

## license

MIT
