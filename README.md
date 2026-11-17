*This project has been created as part of the 42 curriculum by mchesnea and erocha--.*

# ft_IRC

![Score](https://img.shields.io/badge/score-100%2F100-brightgreen?style=flat-square) ![Language](https://img.shields.io/badge/language-C++98-blue?style=flat-square)

## Description

The goal of this project is to create your own IRC (Internet Relay Network) server in C++98. It must be non-blocking, handle multiple clients simultaneously using a single polling mechanism, and comply with the IRC protocol standards (RFC 1459 / RFC 2812).

Key concepts learned include:
* **Network Programming**: Working with sockets, non-blocking file descriptors, and multiplexing using `poll()`.
* **Protocol Parsing**: Interpreting custom IRC command syntax, prefixes, parameters, and trailing messages.
* **State Management**: Managing client authentication, registration, channels, modes, and operator privileges.
* **Robustness**: Handling partial TCP buffers, proper cleanup on disconnections, and avoiding memory leaks.

**Commands implemented:**

| Command | Syntax / Parameters | Example | Description |
| :--- | :--- | :--- | :--- |
| `PASS` | `PASS <password>` | `PASS 1234` | Authenticate with the server password (must be sent first). |
| `NICK` | `NICK <nickname>` | `NICK alice` | Set or change the client nickname. |
| `USER` | `USER <username> <mode> <unused> :<realname>` | `USER alice 0 * :Alice Wonderland` | Register the username and real name to complete registration. |
| `JOIN` | `JOIN <#channel> [key]` | `JOIN #general` / `JOIN #secret pass` | Create or join a channel (first user becomes operator `@`). |
| `PART` | `PART <#channel> [:<reason>]` | `PART #general :Leaving` | Leave a channel with an optional reason. |
| `PRIVMSG` | `PRIVMSG <target> :<message>` | `PRIVMSG #general :Hello!` | Send a message to a channel (`#chan`) or a user (`nick`). |
| `QUIT` | `QUIT [:<reason>]` | `QUIT :Goodbye` | Disconnect cleanly from the server. |
| `KICK` | `KICK <#channel> <nickname> [:<reason>]` | `KICK #general bob :Spamming` | Eject a user from a channel (operator only). |
| `INVITE` | `INVITE <nickname> <#channel>` | `INVITE bob #general` | Invite a user to a channel (operator only). |
| `TOPIC` | `TOPIC <#channel> [:<new_topic>]` | `TOPIC #general :New topic` | View (without `:`) or change (with `:`) the channel topic. |
| `MODE` | `MODE <#channel> [+<modes>/-<modes> [params]]` | `MODE #general +itkl secret 10` | View or manage channel modes (`i`, `t`, `k`, `l`, `o`). |
| `PING` | `PING <token>` | `PING 12345` | Respond with `PONG` to server/client heartbeat checks. |

## Instructions

### Compilation

You can compile the project using the provided Makefile.

```bash
make
```

## Usage

```bash
./ircserv <port> <password>
```

### Client Connection

You can connect to your server using any standard IRC client (such as Irssi or HexChat) or via netcat:
```bash
nc localhost <port>
```

## Resources

* **Documentation & Manuals:**
  * [RFC 1459: Internet Relay Chat Protocol](https://datatracker.ietf.org/doc/html/rfc1459)
  * [RFC 2812: Internet Relay Chat: Client Protocol](https://datatracker.ietf.org/doc/html/rfc2812)
  * `man 2 socket`, `man 2 bind`, `man 2 listen`, `man 2 accept`, `man 2 poll`, `man 2 fcntl`

* **Guides & Tutorials:**
  * Beej's Guide to Network Programming

## AI Usage

* **Role of AI:** AI was utilized as a collaborative technical coach and architectural advisor throughout the development process.
* **Specific Areas:**
  * Structuring the complex channel mode parser (`MODE` flags handling).
  * Reviewing edge cases for client registration, socket cleanup, and memory management.
