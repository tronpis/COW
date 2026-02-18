//--------------------------------------------
// COW PROGRAMMING LANGUAGE - CLI Entry Point
//--------------------------------------------
#include "cow/vm.hpp"
#include "cow/parser.hpp"
#include "cow/limits.hpp"
#include "cow/error.hpp"

#include <iostream>
#include <string>
#include <cstring>
#include <filesystem>
#include <optional>

namespace fs = std::filesystem;

#ifndef COW_VERSION_STRING
#define COW_VERSION_STRING "2.0.0"
#endif

void printVersion() {
    std::cout << "COW Programming Language v" << COW_VERSION_STRING << "\n"
              << "Maintainer: Trompis (github.com/tronpis)\n"
              << "Original by BigZaphod (Sean Heber)\n";
}

void printUsage(const char* program_name) {
    std::cout << "Usage: " << program_name << " [options] <file.cow>\n"
              << "       " << program_name << " --check <file.cow>\n"
              << "       " << program_name << " --version\n"
              << "\n"
              << "Options:\n"
              << "  -h, --help           Show this help message\n"
              << "  -v, --version        Show version information\n"
              << "  -c, --check          Check syntax only (no execution)\n"
              << "  -s, --safe           Enable safe mode (limits execution)\n"
              << "  -m, --memory <n>     Set memory size (default: 30000)\n"
              << "      --max-steps <n>  Maximum execution steps (0 = unlimited)\n"
              << "      --max-output <n> Maximum output bytes (0 = unlimited)\n"
              << "  -d, --debug          Debug mode (step-by-step execution)\n"
              << "  -q, --quiet          Quiet mode (no greetings)\n"
              << "\n"
              << "Examples:\n"
              << "  " << program_name << " program.cow          Run a COW program\n"
              << "  " << program_name << " --check file.cow     Validate syntax\n"
              << "  " << program_name << " --safe file.cow      Run with safety limits\n"
              << "  " << program_name << " -q file.cow          Run quietly\n";
}

enum class RunMode {
    Execute,
    CheckOnly,
    Debug
};

struct Options {
    RunMode mode = RunMode::Execute;
    std::string filename;
    bool safe_mode = false;
    bool quiet = false;
    size_t memory_size = 30000;
    size_t max_steps = 0;
    size_t max_output = 0;
};

std::optional<Options> parseArguments(int argc, char** argv) {
    Options opts;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return std::nullopt;
        } else if (arg == "-v" || arg == "--version") {
            printVersion();
            return std::nullopt;
        } else if (arg == "-c" || arg == "--check") {
            opts.mode = RunMode::CheckOnly;
        } else if (arg == "-s" || arg == "--safe") {
            opts.safe_mode = true;
        } else if (arg == "-q" || arg == "--quiet") {
            opts.quiet = true;
        } else if (arg == "-d" || arg == "--debug") {
            opts.mode = RunMode::Debug;
        } else if ((arg == "-m" || arg == "--memory") && i + 1 < argc) {
            try {
                opts.memory_size = std::stoul(argv[++i]);
            } catch (...) {
                std::cerr << "Error: Invalid memory size\n";
                return std::nullopt;
            }
        } else if (arg == "--max-steps" && i + 1 < argc) {
            try {
                opts.max_steps = std::stoul(argv[++i]);
            } catch (...) {
                std::cerr << "Error: Invalid step limit\n";
                return std::nullopt;
            }
        } else if (arg == "--max-output" && i + 1 < argc) {
            try {
                opts.max_output = std::stoul(argv[++i]);
            } catch (...) {
                std::cerr << "Error: Invalid output limit\n";
                return std::nullopt;
            }
        } else if (arg[0] == '-') {
            std::cerr << "Error: Unknown option " << arg << "\n";
            printUsage(argv[0]);
            return std::nullopt;
        } else {
            if (!opts.filename.empty()) {
                std::cerr << "Error: Multiple input files specified\n";
                return std::nullopt;
            }
            opts.filename = arg;
        }
    }

    if (opts.filename.empty()) {
        std::cerr << "Error: No input file specified\n\n";
        printUsage(argv[0]);
        return std::nullopt;
    }

    return opts;
}

int checkSyntax(const std::string& filename, bool quiet) {
    try {
        if (!fs::exists(filename)) {
            std::cerr << "Error: File not found: " << filename << "\n";
            return 1;
        }

        auto program = cow::Parser::parseFile(filename);

        if (!quiet) {
            std::cout << "✓ Syntax OK\n";
            std::cout << "  Instructions: " << program.size() << "\n";
        }
        return 0;

    } catch (const cow::CowError& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}

int execute(const Options& opts) {
    try {
        if (!fs::exists(opts.filename)) {
            std::cerr << "Error: File not found: " << opts.filename << "\n";
            return 1;
        }

        // Parse the program
        if (!opts.quiet) {
            std::cerr << "Loading: " << opts.filename << "\n";
        }

        auto program = cow::Parser::parseFile(opts.filename);

        if (!opts.quiet) {
            std::cerr << "Instructions: " << program.size() << "\n\n";
        }

        // Setup limits
        cow::Limits limits = cow::Limits::Unlimited();
        if (opts.safe_mode) {
            limits = cow::Limits::SafeDefaults();
            if (!opts.quiet) {
                std::cerr << "Safe mode enabled\n";
                std::cerr << "  Memory: " << opts.memory_size << " cells\n";
                std::cerr << "  Steps: " << limits.max_steps << "\n";
                std::cerr << "\n";
            }
        }
        if (opts.max_steps > 0) {
            limits.max_steps = opts.max_steps;
        }
        if (opts.max_output > 0) {
            limits.max_output = opts.max_output;
        }

        // Create and configure VM
        cow::CowVM vm(limits, opts.memory_size);
        vm.load(program);

        if (opts.mode == RunMode::Debug) {
            // Debug mode
            std::cout << "Debug mode. Commands: s=step, r=run, q=quit\n";
            std::cout << "PC=program counter, MP=memory pointer, MEM=current memory\n\n";

            std::string cmd;
            while (vm.isRunning()) {
                std::cout << "PC=" << vm.programCounter()
                          << " MP=" << vm.memoryPointer()
                          << " MEM=" << vm.currentMemoryValue();
                if (vm.hasRegisterValue()) {
                    std::cout << " REG=" << vm.registerValue();
                }
                std::cout << " > ";

                if (!std::getline(std::cin, cmd)) {
                    break;
                }

                if (cmd == "q" || cmd == "quit") {
                    break;
                } else if (cmd == "r" || cmd == "run") {
                    vm.run();
                    break;
                } else if (cmd == "s" || cmd == "step" || cmd.empty()) {
                    vm.step();
                } else {
                    std::cout << "Unknown command: " << cmd << "\n";
                }
            }
        } else {
            // Normal execution
            vm.run();
        }

        if (!opts.quiet) {
            std::cerr << "\nCompleted. Steps: " << vm.stepsExecuted() << "\n";
        }

        return 0;

    } catch (const cow::ParseError& e) {
        std::cerr << "Parse error: " << e.what() << "\n";
        return 1;
    } catch (const cow::RuntimeError& e) {
        std::cerr << "Runtime error: " << e.what() << "\n";
        return 1;
    } catch (const cow::LimitError& e) {
        std::cerr << "Limit exceeded: " << e.what() << "\n";
        return 1;
    } catch (const cow::IOError& e) {
        std::cerr << "I/O error: " << e.what() << "\n";
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    auto opts = parseArguments(argc, argv);
    if (!opts) {
        return 0;  // --help or --version
    }

    if (opts->mode == RunMode::CheckOnly) {
        return checkSyntax(opts->filename, opts->quiet);
    }

    return execute(*opts);
}
