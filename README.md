# Null-Gate

Null-Gate is a lightweight C++ DNS sinkhole server for Linux. It intercepts DNS queries, blocks domains listed in a local blocklist, and forwards allowed queries to an upstream resolver.

## What It Does

- Listens on UDP port `53`
- Loads blocked domains from `data/blocklist.txt`
- Uses a trie-based lookup for fast domain matching
- Returns sinkhole responses for blocked domains (`0.0.0.0` for A records and `::` for AAAA records)
- Forwards allowed queries to Cloudflare DNS at `1.1.1.1`
- Caches upstream responses in memory for faster repeated lookups
- Uses `epoll` and detached worker threads to handle traffic concurrently

## Requirements

Before you build or run the project, make sure the following are available:

- Linux
- `g++` with C++17 support or better
- `make`
- Permission to bind to port `53`

Because the server currently starts on port `53`, you will usually need to run it with `sudo`. If you want to use a different port, change the value passed to `DNSServer` in [src/main.cpp](src/main.cpp) and rebuild.

## Project Layout

- [src/main.cpp](src/main.cpp) contains the entry point and server startup.
- [src/server.cpp](src/server.cpp) contains the DNS server, sinkhole logic, cache handling, and upstream forwarding.
- [data/blocklist.txt](data/blocklist.txt) contains the blocked domains loaded at startup.
- [include/](include) contains the headers for the parser, cache, trie, and server.

## Build

From the project root, run:

```bash
make
```

This creates the binary at:

```bash
build/dns_server
```

If you want to rebuild from scratch, run:

```bash
make clean
make
```

## Run

The default server port is `53`, so the simplest way to start it is:

```bash
sudo ./build/dns_server
```

When the server starts successfully, it will:

1. Bind to the configured UDP port.
2. Load the blocklist from `data/blocklist.txt`.
3. Wait for DNS requests.

To stop the server, press `Ctrl+C`.

## How To Test

You can test the server locally with `dig` or `nslookup`.

Example using `dig`:

```bash
dig @127.0.0.1 example.com
```

To check that a blocked domain is sinkholed, query a domain that exists in `data/blocklist.txt`:

```bash
dig @127.0.0.1 blocked-domain.example
```

If you changed the port in `src/main.cpp`, replace `53` with that port in your test command.

## Editing The Blocklist

Blocked domains are read from `data/blocklist.txt` when the server starts. To add or remove domains:

1. Open `data/blocklist.txt`.
2. Add one domain per line.
3. Save the file.
4. Restart the server so the new blocklist is loaded.

## Notes

- The server forwards non-blocked queries to `1.1.1.1`.
- Responses for allowed domains are cached in memory.
- The current implementation uses UDP only.
- The blocklist is loaded on startup, so changes do not take effect until you restart the process.

## Troubleshooting

If the server does not start:

- Make sure port `53` is not already in use by another DNS service.
- Run the binary with `sudo`.
- Confirm that `data/blocklist.txt` exists.
- Rebuild with `make` if you changed any source files.

If `dig` returns no response, check whether your local firewall is blocking UDP traffic on the selected port.

## Clean Up

To remove build artifacts:

```bash
make clean
```
