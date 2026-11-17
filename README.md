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
* `PASS`: Authenticate with the server password.
* `NICK`: Set or change the client nickname.
* `USER`: Register the username and real name.
* `JOIN`: Create or join a channel.
* `PART`: Leave a channel.
* `PRIVMSG`: Send private messages to users or channels.
* `QUIT`: Disconnect cleanly from the server.
* `KICK`: Eject a user from a channel (operator only).
* `INVITE`: Invite a user to an invite-only channel (operator only).
* `TOPIC`: View or change the channel topic.
* `MODE`: Manage channel modes (`i`, `t`, `k`, `l`, `o`).
* `PING`: Respond to server/client heartbeat checks.

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
