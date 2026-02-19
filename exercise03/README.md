# Exercise 03 – Secure File Mixing (Python)

This exercise implements a defensive file processing utility.

The program:

- Reads two input files
- Interleaves their lines
- Writes output safely using exclusive file creation mode (`'x'`)

Security considerations:

- Checks for missing input files
- Prevents overwriting input files
- Fails safely if output already exists
- Handles common file-related exceptions

The focus is on predictable behavior and safe file handling.
