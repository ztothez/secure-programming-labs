# Exercise 04 – Process Hardening (Core Dumps)

This exercise demonstrates how to disable core dumps using `setrlimit()`.

File included:

- `disable_core_dumps.c`

Core dumps may contain sensitive memory data. Disabling them can be a basic hardening measure in production systems.

The program sets the core dump limit to zero for the current process.
