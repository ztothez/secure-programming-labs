import sys

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
        with open(path, "rb") as f:
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
