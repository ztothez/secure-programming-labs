#!/usr/bin/env python3
import os

def mix_files(file1, file2, outfile):
  # Error 1: missing input file(s)
  if not os.path.exists(file1):
    raise FileNotFoundError("Missing input file: %s" % file1)
  if not os.path.exists(file2):
    raise FileNotFoundError("Missing input file: %s" % file2)

  # Error 2: prevent overwriting inputs by accident
  if os.path.abspath(outfile) in {os.path.abspath(file1), os.path.abspath(file2)}:
    raise ValueError("Output must be different from input files.")

  # Error 3: output exists (use 'x' to fail safely)
  with open(safe_path(file1), "r", encoding="utf-8") as a, \
       open(safe_path(file2), "r", encoding="utf-8") as b, \
       open(safe_path(outfile), "x", encoding="utf-8") as out:

    while True:
      la = a.readline()
      lb = b.readline()
      if not la and not lb:
        break
      if la:
        out.write(la)
      if lb:
        out.write(lb)

def main():
  f1 = input("First input filename: ").strip()
  f2 = input("Second input filename: ").strip()
  out = input("Output filename: ").strip()

  if not f1 or not f2 or not out:
    print("ERROR: filenames cannot be empty")
    return 1

  try:
    mix_files(f1, f2, out)
    print("OK: wrote mixed output to %s" % out)
    return 0

  except FileNotFoundError as e:
    print("ERROR (file not found):", e)

  except PermissionError as e:
    print("ERROR (permission denied):", e)

  except FileExistsError as e:
    print("ERROR (output exists):", e)

  except ValueError as e:
    print("ERROR (invalid input):", e)

  except UnicodeDecodeError as e:
    print("ERROR (encoding):", e)

  except Exception as e:
    print("ERROR (unexpected): %s: %s" % (type(e).__name__, e))

  return 1

if __name__ == "__main__":
  raise SystemExit(main())
