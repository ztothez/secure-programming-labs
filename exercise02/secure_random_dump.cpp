#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <vector>
#include <string>
#include <stdexcept>
#include <cerrno>
#include <climits>

#if defined(_WIN32)
  #define NOMINMAX
  #include <windows.h>
  #include <bcrypt.h>
  #pragma comment(lib, "bcrypt.lib")
#elif defined(__linux__)
  #include <unistd.h>
  #include <fcntl.h>
  #include <sys/random.h>
#endif

static void die(const char* msg) {
    std::perror(msg);
    std::exit(1);
}

// Parse a positive size from argv (rejects junk like "12abc" or overflow).
static size_t parse_size(const char* s) {
    if (!s || *s == '\0') throw std::runtime_error("Invalid byte count");

    errno = 0;
    char* end = nullptr;
    unsigned long long v = std::strtoull(s, &end, 10);

    if (errno == ERANGE) throw std::runtime_error("Byte count out of range");
    if (end == s || *end != '\0') throw std::runtime_error("Byte count must be a number");
    if (v == 0) throw std::runtime_error("Bytes must be > 0");
    if (v > (unsigned long long)SIZE_MAX) throw std::runtime_error("Byte count too large");

    return (size_t)v;
}

// Fill buffer with cryptographically secure random bytes
static void fill_secure_random(uint8_t* buf, size_t len) {
#if defined(_WIN32)
    NTSTATUS status = BCryptGenRandom(nullptr, buf, (ULONG)len, BCRYPT_USE_SYSTEM_PREFERRED_RNG);
    if (status != 0) {
        throw std::runtime_error("BCryptGenRandom failed");
    }

#elif defined(__linux__)
    // Linux: try getrandom() first
    size_t offset = 0;
    while (offset < len) {
        ssize_t r = getrandom(buf + offset, len - offset, 0);
        if (r > 0) {
            offset += (size_t)r;
            continue;
        }
        if (r == 0) break;          // don't spin forever
        if (errno == EINTR) continue;

        // getrandom failed, fall back to /dev/urandom
        int fd = open("/dev/urandom", O_RDONLY);
        if (fd < 0) die("open /dev/urandom");

        while (offset < len) {
            ssize_t n = read(fd, buf + offset, len - offset);
            if (n > 0) {
                offset += (size_t)n;
                continue;
            }
            if (n == 0) break;      // shouldn't happen, but handle it
            if (errno == EINTR) continue;
            close(fd);
            die("read /dev/urandom");
        }

        close(fd);
        break;
    }

    if (offset != len) {
        throw std::runtime_error("Failed to read enough random bytes");
    }

#else
    FILE* f = std::fopen("/dev/urandom", "rb");
    if (!f) die("fopen /dev/urandom");

    size_t got = 0;
    while (got < len) {
        size_t n = std::fread(buf + got, 1, len - got, f);
        if (n > 0) {
            got += n;
            continue;
        }
        if (std::ferror(f)) {
            std::fclose(f);
            throw std::runtime_error("Failed reading /dev/urandom");
        }
        break;
    }

    std::fclose(f);
    if (got != len) {
        throw std::runtime_error("Failed to read enough random bytes");
    }
#endif
}

static void write_all(FILE* out, const uint8_t* data, size_t len) {
    size_t off = 0;
    while (off < len) {
        size_t n = std::fwrite(data + off, 1, len - off, out);
        if (n == 0) {
            throw std::runtime_error("Failed to write output file");
        }
        off += n;
    }
}

int main(int argc, char** argv) {
    if (argc < 2 || argc > 3) {
        std::fprintf(stderr, "Usage: %s <outfile> [bytes]\n", argv[0]);
        return 1;
    }

    std::string outpath = argv[1];

    // Default to 1KB
    size_t bytes = 1024;
    try {
        if (argc == 3) bytes = parse_size(argv[2]);
    } catch (const std::exception& e) {
        std::fprintf(stderr, "Error: %s\n", e.what());
        return 1;
    }

    std::vector<uint8_t> data(bytes);

    try {
        fill_secure_random(data.data(), data.size());
    } catch (const std::exception& e) {
        std::fprintf(stderr, "Error: %s\n", e.what());
        return 1;
    }

    FILE* out = std::fopen(outpath.c_str(), "wb");
    if (!out) die("fopen output file");

    try {
        write_all(out, data.data(), data.size());
    } catch (const std::exception& e) {
        std::fclose(out);
        std::fprintf(stderr, "Error: %s\n", e.what());
        return 1;
    }

    if (std::fclose(out) != 0) die("fclose output file");

    std::printf("Wrote %zu bytes to %s\n", bytes, outpath.c_str());
    return 0;
}
