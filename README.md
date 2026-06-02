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

a full IRC server in **49 lines** of JavaScript, built on bun's TCP listener. fully compatible with **weechat**, **irssi**, **hexchat**, and any RFC-compliant IRC client.

```
bun server.js
```

listens on port **6667** on all interfaces. password auth via `PASS` env var.

### supported commands

| command | description |
|---------|-------------|
| `NICK` / `USER` / `PASS` | connection handshake with auth and collision detection |
| `JOIN` / `PART` | channel management with auto-operator on first join |
| `PRIVMSG` | messaging with `+n` (no external) mode enforcement |
| `TOPIC` | view or set channel topic (respects `+t` mode) |
| `KICK` | operator-only user removal |
| `MODE` | channel modes `+o`/`-o`, `+t`/`-t`, `+n`/`-n` |
| `LIST` | list all channels with user count and topic |
| `NAMES` | list channel users with `@` for operators |
| `WHO` | user information |
| `QUIT` | clean disconnect with cleanup |
| `PING` / `PONG` | keepalive |
| `CAP LS` | capability listing |

## tech

- **runtime:** [bun](https://bun.sh) (server), node/bun (client)
- **dependencies:** zero. everything is from scratch using built-in modules (`net`, `Bun.listen`)
- **client:** raw ANSI terminal rendering — 83 lines
- **server:** full IRC protocol subset — 49 lines

## why

because IRC is timeless and building things from scratch is fun (｡♥‿♥｡)

## license

MIT
