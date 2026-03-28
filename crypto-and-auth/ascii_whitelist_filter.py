import sys
import os

def safe_path(path):
    name = os.path.basename(path)
    if name != path or not name or name in (".", ".."):
        raise ValueError("invalid path")
    return name

# Only allow alphanumeric characters plus comma and dash
ALLOWED = set(b"abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,-")

def main():
    # Expect exactly one argument: input file path
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} <inputfile>", file=sys.stderr)
        raise SystemExit(1)

    path = sys.argv[1]

    # Read file in binary mode to avoid implicit decoding issues
    try:
        # Earlier vulnerable part: with open(path, "rb") as f:
        # VULNERABLE: user-controlled path could allow path traversal
        # Fixed due to Snyk analysis
        with open(safe_path(path), "rb") as f:
            data = f.read()
    except OSError as e:
        print(f"Error reading file: {e}", file=sys.stderr)
        raise SystemExit(1)

    # Keep only bytes that match our whitelist
    filtered = bytes(b for b in data if b in ALLOWED)

    # Safe to decode since we only kept ASCII characters
    output = filtered.decode("ascii", errors="strict")

    print(output)


if __name__ == "__main__":
    main()
