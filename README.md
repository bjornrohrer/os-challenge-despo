# os-challenge-despo

Group **despo**'s submission for the DTU OS Challenge (02159 / 02335 Operating Systems, 2026).

The server listens on a TCP port and answers reverse-hashing requests. Each request has a SHA256 hash and a range `[start, end)`. The server brute-forces the range to find the 64-bit number that produces the hash and sends that number back. The goal is to answer every request correctly (100% reliability) with the lowest priority-weighted latency.

## Group members

| Name | Student number | GitLab username |
|------|----------------|-----------------|
| TODO | TODO           | TODO            |
| TODO | TODO           | TODO            |

## Building and running

The server is built from the repository root with the Makefile:

```sh
make clean
make            # builds ./server
./server <port>
```

Run the unit test for the brute-force module:

```sh
make test       # builds and runs ./test-bruteforce
```

To generate requests, use the client and the `run-client*.sh` scripts from
[os-challenge-common](https://github.com/dtu-ese/os-challenge-common).

## Folder structure

Every file is in the repository root, as the challenge requires for the submitted server.

| File | Purpose |
|------|---------|
| `server.c` | Entry point. Parses the port, binds to `INADDR_ANY`, accepts connections and starts one thread per client. Each thread reads the 49-byte request, calls `crack()` and sends the 8-byte answer. |
| `bruteforce.c` / `bruteforce.h` | `crack()` searches `[start, end)` for the number whose hash matches. `hash_u64()` hashes a number in little-endian byte order, the same way the client does. |
| `test-bruteforce.c` | Assert-based unit tests for `crack()` (run with `make test`). |
| `messages.h` | Packet layout (sizes and offsets) and macOS versions of the `be64toh`/`htobe64` byte-order functions. Provided by the course. |
| `lonesha256.h` | SHA256 implementation. Provided by the course; the rules require it. |
| `Makefile` | Builds `server` and `test-bruteforce`. |
| `.clang-format`, `.editorconfig` | Code style settings. |

## Branch structure

| Branch | Status | Description |
|--------|--------|-------------|
| `main` | Milestone submission | Working server with one `pthread` per connection. |
| `learn-protocol` | Merged into `main` | Implementation of the request/response protocol. It was merged into `main` for the milestone and is kept for history. |
| `ny-protocol` | In progress (NicoTheBeast, Bjørn Arthur Rohrer) | Next version of the server. Builds on `learn-protocol` and adds priority handling: requests go into a priority structure protected by a mutex. It will replace `main` when it is finished. |
| `process-vs-threads` | Experiment | Compares one process per connection (`fork()`, with zombie children reaped by a `SIGCHLD` handler) against the thread-per-connection server on `main`. |
| `Protocol` | Obsolete | Early protocol attempt, replaced by `learn-protocol`. Can be ignored. |

## Experiments

| Experiment | Location | Owner | Compared against |
|------------|----------|-------|------------------|
| Processes vs. threads | branch `process-vs-threads` | Bjørn Arthur Rohrer | thread-per-connection server on `main` |

The setup, results and conclusion for each experiment are in the project report.
