# wuchatx

a tiny IRC ecosystem — a terminal client and a server, both written from scratch with zero dependencies ✨

## client

a C client using [tinyc.h](https://github.com/koharu-beepboop/tinyc) — compiled binary, no runtime deps.

```
cc -o client client.c -I/path/to/tinyc
```

or if tinyc.h is in your include path:

```
cc -o client client.c
```

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

## server

a full IRC server in **465 lines** of C using [tinyc.h](https://github.com/koharu-beepboop/tinyc). fully compatible with **weechat**, **irssi**, **hexchat**, and any RFC-compliant IRC client.

```
cc -o server server.c
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

- **runtime:** bare `cc` (client and server)
- **dependencies:** zero. everything from scratch using [tinyc.h](https://github.com/koharu-beepboop/tinyc)
- **client:** raw ANSI terminal rendering — 151 lines
- **server:** full IRC protocol subset — 465 lines

## why

because IRC is timeless and building things from scratch is fun (｡♥‿♥｡)

## license

MIT
