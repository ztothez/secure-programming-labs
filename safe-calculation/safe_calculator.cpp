#include <cerrno>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <cctype>

/*
  Simple command-line calculator with defensive input validation.

  Focus:
  - Safe numeric parsing
  - Rejecting NaN / Inf
  - Explicit error handling
*/

static bool parseLongDouble(const char* s, long double& out) {
    if (!s) return false;

    errno = 0;
    char* end = nullptr;
    out = std::strtold(s, &end);

    // No conversion performed
    if (end == s) return false;

    // Reject trailing garbage
    while (*end != '\0') {
        if (!std::isspace(static_cast<unsigned char>(*end))) return false;
        ++end;
    }

    if (errno == ERANGE) {
        if (!std::isfinite(out)) return false;
    }

    // Explicitly reject NaN / Inf
    if (!std::isfinite(out)) return false;

    return true;
}

static void printUsage(const char* prog) {
    std::cerr << "Usage: " << prog << " <number> <op> <number>\n"
              << "  <op> one of: +  -  *  /\n";
}

int main(int argc, char** argv) {
    if (argc != 4) {
        printUsage(argv[0]);
        return 1;
    }

    long double a = 0.0L, b = 0.0L;

    if (!parseLongDouble(argv[1], a)) {
        std::cerr << "Error: invalid first number: '" << argv[1] << "'\n";
        return 2;
    }

    if (!parseLongDouble(argv[3], b)) {
        std::cerr << "Error: invalid second number: '" << argv[3] << "'\n";
        return 2;
    }

    std::string op = argv[2];
    if (op.size() != 1) {
        std::cerr << "Error: operator must be one of + - * /\n";
        return 3;
    }

    long double result = 0.0L;

    switch (op[0]) {
        case '+': result = a + b; break;
        case '-': result = a - b; break;
        case '*': result = a * b; break;
        case '/':
            if (b == 0.0L) {
                std::cerr << "Error: division by zero.\n";
                return 4;
            }
            result = a / b;
            break;
        default:
            std::cerr << "Error: unknown operator '" << op << "'.\n";
            return 3;
    }

    if (!std::isfinite(result)) {
        std::cerr << "Error: result is not finite.\n";
        return 5;
    }

    std::cout << std::setprecision(std::numeric_limits<long double>::digits10 + 1)
              << result << "\n";

    return 0;
}
