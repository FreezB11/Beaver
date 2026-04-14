# BEAVER — Project Targets

> A fast, reliable transport protocol built over UDP.
> Fixes UDP's core flaws: no delivery guarantee, no ordering, no flow control.

---

## T1 — Raw Foundation
- [ ] UDP socket wrapper (`beaver_socket`, `beaver_bind`, `beaver_send`, `beaver_recv`)
- [ ] Header struct + serialize/deserialize (no alignment tricks, explicit byte packing)
- [ ] Basic send/recv loop — echo server works over Beaver sockets
- [ ] Logging/debug layer (`BEAVER_LOG` macro, verbosity levels)

---

## T2 — Reliability Core
- [ ] Sequence numbers on every segment
- [ ] Cumulative ACK from receiver
- [ ] Retransmit queue (circular buffer of unACKed segments with timestamps)
- [ ] Fixed RTO retransmit timer (start with 200ms hardcoded)
- [ ] Survives 5% packet loss (verified with `tc netem`)

---

## T3 — Ordering + SACK
- [ ] Receiver reorder buffer (hold out-of-order, deliver in-order to app)
- [ ] SACK block generation (up to 3 ranges in header)
- [ ] Sender parses SACK, skips already-received segments on retransmit
- [ ] Fast retransmit on 3 duplicate ACKs (no waiting for RTO)

---

## T4 — Adaptive Timing
- [ ] RTT sampling per ACK
- [ ] Dynamic RTO via Jacobson/Karels algorithm
- [ ] RTO backoff on timeout (exponential, cap at 60s)
- [ ] RTT stats exposed in debug output (min/avg/max)

---

## T5 — Flow + Congestion Control
- [ ] Receiver window (`rwnd`) advertised in every ACK
- [ ] Sender respects `rwnd` (never sends beyond receiver capacity)
- [ ] `cwnd` + slow start (exponential growth from 1 MSS)
- [ ] AIMD — additive increase after ssthresh, halve on loss
- [ ] Congestion window stats visible in logs

---

## T6 — Connection Lifecycle
- [ ] 3-way handshake (SYN → SYN-ACK → ACK)
- [ ] Initial sequence number randomized
- [ ] FIN / FIN-ACK graceful teardown
- [ ] RST for hard close / error conditions
- [ ] Half-open connection detection + timeout cleanup

---

## T7 — Demo App
- [ ] `beaver_send_file` / `beaver_recv_file` API
- [ ] CLI: `beaver-send <host> <port> <file>` and `beaver-recv <port> <outfile>`
- [ ] Transfer a 100MB file over localhost with 10% loss, 50ms delay — succeeds and matches checksum (SHA-256)
- [ ] Print transfer stats: throughput, retransmits, RTT

---

## T8 — Stress & Benchmarks *(stretch)*
- [ ] Multi-stream support (multiple logical channels over one UDP socket)
- [ ] Benchmark vs raw TCP on localhost (latency + throughput)
- [ ] Fuzzer for header parser (libfuzzer or manual bit-flip loop)
- [ ] Simulate reorder-only (no loss) — protocol still correct

---

## Suggested Order

```
T1 → T2 → T6 → T3 → T4 → T5 → T7 → T8
```

T6 (connection lifecycle) is worth doing early so you're always
testing with real connect/close semantics rather than bolting it on later.