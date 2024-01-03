#include <Windows.h>
#include <iostream>
#include <sstream>
#include <string>
#include <set>
#include <cstdlib>

#ifndef RELEASE
#define dbg(x) std::cout << x << std::endl;
#else
#define dbg(x)
#endif

struct clipboard {
private:
    bool open;
public:
    clipboard(HWND owner = nullptr): open(OpenClipboard(owner)) {}
    ~clipboard() {
        if(open) {
            CloseClipboard();
        }
    }

    bool clear() const noexcept {
        return EmptyClipboard();
    }

    bool is_open() const noexcept {
        return open;
    }

    bool copy(const std::string& str) const {
        if(open) {
            clear();
            HGLOBAL buffer = GlobalAlloc(GMEM_DDESHARE, str.size() + 1);
            if(buffer) {
                std::copy(std::begin(str), std::end(str), static_cast<char*>(GlobalLock(buffer)));
                GlobalUnlock(buffer);
                return SetClipboardData(CF_TEXT, buffer) != nullptr;
            }
        }
        return false;
    }

    std::string paste() const {
        return static_cast<char*>(GetClipboardData(CF_TEXT));
    }
};

void help() {
    std::cout <<
    "xclip [OPTION]\n"
    "   -i, --in           reads text from stdin (default)\n"
    "   -o, --out          outputs text to stdout\n"
    "   -v, --version      outputs version information\n"
    "   -h, --help         prints this message and exits\n";
}

void version() {
    std::cout <<
    "xclip version (Windows native port)\n"
}

int read_text(const clipboard& clip) {
    std::ostringstream ss;
    for(std::string line; std::getline(std::cin, line); ) {
        ss << line;
    }
    return clip.copy(ss.str()) ? EXIT_SUCCESS : EXIT_FAILURE;
}

int main(int argc, char** argv) {
    clipboard clip;
    if(!clip.is_open()) {
        std::cout << "xclip: error: unable to open clipboard\n";
        return EXIT_FAILURE;
    }

    std::set<std::string> args{argv, argv + argc};

    if(args.count("-h") || args.count("--help")) {
        help();
        return EXIT_SUCCESS;
    }

    if(args.count("-v") || args.count("--version")) {
        version();
        return EXIT_SUCCESS;
    }

    if(argc < 2 || args.count("-i") || args.count("--in")) {
        return read_text(clip);
    }

    if(args.count("-o") || args.count("--out")) {
        std::cout << clip.paste();
        return EXIT_SUCCESS;
    }
}
