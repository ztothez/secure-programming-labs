import base64
import hashlib
import hmac
import os
import secrets
import sys
from getpass import getpass

# Store the DB next to this script (avoids surprises with working directory)
DB_PATH = os.path.join(os.path.dirname(__file__), "users.db")

# PBKDF2 config
ITERATIONS = 200_000
SALT_LEN = 16  # 128-bit salt
DK_LEN = 32    # 256-bit derived key


def b64e(b: bytes) -> str:
    return base64.urlsafe_b64encode(b).decode("ascii")


def b64d(s: str) -> bytes:
    return base64.urlsafe_b64decode(s.encode("ascii"))


def derive(password: str, salt: bytes) -> bytes:
    return hashlib.pbkdf2_hmac(
        "sha256",
        password.encode("utf-8"),
        salt,
        ITERATIONS,
        dklen=DK_LEN,
    )


def valid_username(username: str) -> bool:
    # Keep it simple: avoid separators/newlines so the DB format stays sane.
    if not username:
        return False
    if ":" in username or "\n" in username or "\r" in username:
        return False
    return True


def load_db(path: str) -> dict:
    users = {}

    if not os.path.exists(path):
        return users

    with open(path, "r", encoding="utf-8") as f:
        for raw in f:
            line = raw.strip()

            if not line or line.startswith("#"):
                continue

            parts = line.split(":", 2)
            if len(parts) != 3:
                continue

            username, salt_s, hash_s = parts
            try:
                users[username] = (b64d(salt_s), b64d(hash_s))
            except Exception:
                # Malformed base64 etc. Just ignore the entry.
                continue

    return users


def ensure_private_file(path: str) -> None:
    # Best-effort: make sure the db isn't world-readable on Unix.
    try:
        if os.path.exists(path):
            os.chmod(path, 0o600)
    except Exception:
        pass


def save_user(path: str, username: str, salt: bytes, pw_hash: bytes) -> None:
    # Append-only for the exercise (simple, easy to inspect)
    with open(path, "a", encoding="utf-8") as f:
        f.write(f"{username}:{b64e(salt)}:{b64e(pw_hash)}\n")
    ensure_private_file(path)


def register(username: str) -> None:
    if not valid_username(username):
        print("Invalid username.")
        return

    users = load_db(DB_PATH)
    if username in users:
        print("User already exists.")
        return

    pw1 = getpass("New password: ")
    pw2 = getpass("Repeat password: ")
    if pw1 != pw2:
        print("Passwords do not match.")
        return

    salt = secrets.token_bytes(SALT_LEN)
    pw_hash = derive(pw1, salt)

    save_user(DB_PATH, username, salt, pw_hash)
    print("User registered.")


def login(username: str) -> bool:
    if not valid_username(username):
        print("Invalid username.")
        return False

    users = load_db(DB_PATH)
    if username not in users:
        print("Unknown user.")
        return False

    salt, stored_hash = users[username]

    pw = getpass("Password: ")
    candidate = derive(pw, salt)

    # Constant-time compare to avoid timing leaks
    if hmac.compare_digest(candidate, stored_hash):
        print("Authentication OK.")
        return True

    print("Authentication FAILED.")
    return False


def main():
    if len(sys.argv) != 3 or sys.argv[1] not in {"register", "login"}:
        print(f"Usage: {sys.argv[0]} register|login <username>")
        raise SystemExit(1)

    cmd = sys.argv[1]
    username = sys.argv[2]

    if cmd == "register":
        register(username)
        raise SystemExit(0)

    ok = login(username)
    raise SystemExit(0 if ok else 2)


if __name__ == "__main__":
    main()
