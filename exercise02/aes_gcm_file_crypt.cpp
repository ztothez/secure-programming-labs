#include <cstdio>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
#include <stdexcept>
#include <cctype>
#include <openssl/evp.h>
#include <openssl/rand.h>

static std::string sanitize_filename(const std::string& path) {
    std::string name = path;

    std::size_t pos = name.find_last_of("/\\");
    if (pos != std::string::npos) {
        name = name.substr(pos + 1);
    }

    if (name.empty() || name == "." || name == "..") {
        throw std::runtime_error("Invalid file path");
    }

    std::string cleaned;
    cleaned.reserve(name.size());

    for (unsigned char c : name) {
        if (std::isalnum(c) || c == '.' || c == '_' || c == '-') {
            cleaned.push_back((char)c);
        } else {
            throw std::runtime_error("Invalid file path");
        }
    }

    if (cleaned.empty()) {
        throw std::runtime_error("Invalid file path");
    }

    return cleaned;
}

static FILE* safe_fopen(const std::string& path, const char* mode) {
    std::string filename = sanitize_filename(path);
    return std::fopen(filename.c_str(), mode);
}

static const uint8_t MAGIC[8] = {'C','R','Y','P','T','0','1','\0'};

static const int SALT_LEN = 16;
static const int NONCE_LEN = 12;      // 96-bit nonce (recommended for GCM)
static const int TAG_LEN = 16;        // 128-bit auth tag
static const int KEY_LEN = 32;        // AES-256 key size
static const int PBKDF2_ITERS = 200000;

static void die(const std::string& msg) {
    throw std::runtime_error(msg);
}

struct Ctx {
    EVP_CIPHER_CTX* p;
    Ctx() : p(EVP_CIPHER_CTX_new()) {
        if (!p) die("EVP_CIPHER_CTX_new failed");
    }
    ~Ctx() { EVP_CIPHER_CTX_free(p); }
    Ctx(const Ctx&) = delete;
    Ctx& operator=(const Ctx&) = delete;
};

static std::vector<uint8_t> read_all(const std::string& path) {
    FILE* f = safe_fopen(path, "rb");
    if (!f) die("Cannot open input file");

    if (std::fseek(f, 0, SEEK_END) != 0) {
        std::fclose(f);
        die("fseek failed");
    }

    long sz = std::ftell(f);
    if (sz < 0) {
        std::fclose(f);
        die("ftell failed");
    }

    if (std::fseek(f, 0, SEEK_SET) != 0) {
        std::fclose(f);
        die("fseek failed");
    }

    std::vector<uint8_t> buf((size_t)sz);
    if (!buf.empty()) {
        size_t n = std::fread(buf.data(), 1, buf.size(), f);
        if (n != buf.size()) {
            std::fclose(f);
            die("read failed");
        }
    }

    std::fclose(f);
    return buf;
}

static void write_all(const std::string& path, const std::vector<uint8_t>& data) {
    FILE* f = safe_fopen(path, "wb");
    if (!f) die("Cannot open output file");

    for (uint8_t byte : data) {
        if (std::fputc(byte, f) == EOF) {
            std::fclose(f);
            die("write failed");
        }
    }

    std::fclose(f);
}

static void derive_key(const std::string& password, const uint8_t* salt, uint8_t* key_out) {
    int ok = PKCS5_PBKDF2_HMAC(
        password.c_str(),
        (int)password.size(),
        salt,
        SALT_LEN,
        PBKDF2_ITERS,
        EVP_sha256(),
        KEY_LEN,
        key_out
    );

    if (ok != 1) die("PBKDF2 failed");
}

static std::vector<uint8_t> encrypt_aes_gcm(
    const std::vector<uint8_t>& plaintext,
    const uint8_t* key,
    const uint8_t* nonce,
    uint8_t* tag_out
) {
    Ctx ctx;

    if (EVP_EncryptInit_ex(ctx.p, EVP_aes_256_gcm(), nullptr, nullptr, nullptr) != 1)
        die("EncryptInit cipher failed");

    if (EVP_CIPHER_CTX_ctrl(ctx.p, EVP_CTRL_GCM_SET_IVLEN, NONCE_LEN, nullptr) != 1)
        die("Set IV len failed");

    if (EVP_EncryptInit_ex(ctx.p, nullptr, nullptr, key, nonce) != 1)
        die("EncryptInit key/nonce failed");

    std::vector<uint8_t> ciphertext(plaintext.size() + 16);
    int outlen = 0;
    int tmplen = 0;

    if (!plaintext.empty()) {
        if (EVP_EncryptUpdate(ctx.p, ciphertext.data(), &outlen,
                             plaintext.data(), (int)plaintext.size()) != 1)
            die("EncryptUpdate failed");
    }

    if ((size_t)outlen >= ciphertext.size())
        die("output length overflow");
    
    uint8_t* outptr = ciphertext.data() + outlen;
    if (EVP_EncryptFinal_ex(ctx.p, outptr, &tmplen) != 1)
        die("EncryptFinal failed");

    outlen += tmplen;

    if (EVP_CIPHER_CTX_ctrl(ctx.p, EVP_CTRL_GCM_GET_TAG, TAG_LEN, tag_out) != 1)
        die("Get TAG failed");

    ciphertext.resize((size_t)outlen);
    return ciphertext;
}

static std::vector<uint8_t> decrypt_aes_gcm(
    const std::vector<uint8_t>& ciphertext,
    const uint8_t* key,
    const uint8_t* nonce,
    const uint8_t* tag
) {
    Ctx ctx;

    if (EVP_DecryptInit_ex(ctx.p, EVP_aes_256_gcm(), nullptr, nullptr, nullptr) != 1)
        die("DecryptInit cipher failed");

    if (EVP_CIPHER_CTX_ctrl(ctx.p, EVP_CTRL_GCM_SET_IVLEN, NONCE_LEN, nullptr) != 1)
        die("Set IV len failed");

    if (EVP_DecryptInit_ex(ctx.p, nullptr, nullptr, key, nonce) != 1)
        die("DecryptInit key/nonce failed");

    std::vector<uint8_t> plaintext(ciphertext.size());
    int outlen = 0;
    int tmplen = 0;

    if (!ciphertext.empty()) {
        if (EVP_DecryptUpdate(ctx.p, plaintext.data(), &outlen,
                             ciphertext.data(), (int)ciphertext.size()) != 1)
            die("DecryptUpdate failed");
    }

    // Set expected tag before finalizing (GCM verification happens in Final)
    if (EVP_CIPHER_CTX_ctrl(ctx.p, EVP_CTRL_GCM_SET_TAG, TAG_LEN, (void*)tag) != 1)
        die("Set TAG failed");

    if ((size_t)outlen >= plaintext.size())
    die("output length overflow");

    uint8_t* outptr = plaintext.data() + outlen;
    int fin = EVP_DecryptFinal_ex(ctx.p, outptr, &tmplen);
    if (fin != 1) {
        die("Authentication failed: wrong password or file tampered");
    }

    outlen += tmplen;
    plaintext.resize((size_t)outlen);
    return plaintext;
}

int main(int argc, char** argv) {
    // Usage: enc|dec infile outfile password
    // Note: passing password on CLI is convenient, but visible via process listing.
    if (argc != 5) {
        std::fprintf(stderr, "Usage: %s enc|dec <infile> <outfile> <password>\n", argv[0]);
        return 1;
    }

    std::string mode(argv[1]);
    std::string infile(argv[2]);
    std::string outfile(argv[3]);
    std::string password(argv[4]);

    try {
        if (mode == "enc") {
            auto pt = read_all(infile);

            uint8_t salt[SALT_LEN];
            uint8_t nonce[NONCE_LEN];
            uint8_t key[KEY_LEN];
            uint8_t tag[TAG_LEN];

            if (RAND_bytes(salt, SALT_LEN) != 1) die("RAND_bytes salt failed");
            if (RAND_bytes(nonce, NONCE_LEN) != 1) die("RAND_bytes nonce failed");

            derive_key(password, salt, key);

            auto ct = encrypt_aes_gcm(pt, key, nonce, tag);

            // Output: MAGIC | salt | nonce | ciphertext | tag
            std::vector<uint8_t> out;
            out.insert(out.end(), MAGIC, MAGIC + sizeof(MAGIC));
            out.insert(out.end(), salt, salt + SALT_LEN);
            out.insert(out.end(), nonce, nonce + NONCE_LEN);
            out.insert(out.end(), ct.begin(), ct.end());
            out.insert(out.end(), tag, tag + TAG_LEN);

            write_all(outfile, out);
            std::puts("Encrypted OK.");

        } else if (mode == "dec") {
            auto in = read_all(infile);

            const size_t min_size = sizeof(MAGIC) + SALT_LEN + NONCE_LEN + TAG_LEN;
            if (in.size() < min_size) die("File too small");

            if (std::memcmp(in.data(), MAGIC, sizeof(MAGIC)) != 0) {
                die("Bad file format (magic)");
            }

            const uint8_t* salt = in.data() + sizeof(MAGIC);
            const uint8_t* nonce = salt + SALT_LEN;

            const size_t ct_off = sizeof(MAGIC) + SALT_LEN + NONCE_LEN;
            const size_t tag_off = in.size() - TAG_LEN;
            if (tag_off < ct_off) die("Bad file layout");

            if (ct_off > in.size() || tag_off > in.size() || ct_off > tag_off)
                die("invalid ciphertext bounds");
            
            std::vector<uint8_t> ct(in.begin() + (ptrdiff_t)ct_off, in.begin() + (ptrdiff_t)tag_off);
            if (tag_off > in.size())
                die("tag offset overflow");
            
            const uint8_t* tag = &in[tag_off];

            uint8_t key[KEY_LEN];
            derive_key(password, salt, key);

            auto pt = decrypt_aes_gcm(ct, key, nonce, tag);
            write_all(outfile, pt);
            std::puts("Decrypted OK.");

        } else {
            std::fprintf(stderr, "Unknown mode: %s\n", mode.c_str());
            return 1;
        }

    } catch (const std::exception& e) {
        std::fprintf(stderr, "Error: %s\n", e.what());
        return 2;
    }

    return 0;
}
