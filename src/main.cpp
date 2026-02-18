//--------------------------------------------
// COW PROGRAMMING LANGUAGE - CLI Entry Point
//--------------------------------------------
#include "cow/vm.hpp"
#include "cow/parser.hpp"
#include "cow/limits.hpp"

#include <iostream>
#include <string>
#include <cstring>

void printUsage(const char* program_name) {
    std::cout << "COW Programming Language Interpreter (Modern C++ Version)\n"
              << "Original by BigZaphod (Sean Heber) - Modernized version\n\n"
              << "Usage: " << program_name << " [options] <program.cow>\n\n"
              << "Options:\n"
              << "  -h, --help         Show this help message\n"
              << "  -s, --safe         Enable safe mode (limits execution)\n"
              << "  -m, --memory <n>   Set memory size (default: 30000)\n"
              << "  --max-steps <n>    Maximum execution steps (0 = unlimited)\n"
              << "  --max-output <n>   Maximum output bytes (0 = unlimited)\n"
              << "  -d, --debug        Debug mode (step-by-step execution)\n"
              << "  -q, --quiet        Quiet mode (no greetings)\n\n";
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    // Parse command line arguments
    std::string filename;
    bool safe_mode = false;
    bool debug_mode = false;
    bool quiet_mode = false;
    size_t memory_size = 30000;
    cow::Limits limits = cow::Limits::Unlimited();

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-h" || arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "-s" || arg == "--safe") {
            safe_mode = true;
            limits = cow::Limits::SafeDefaults();
        } else if (arg == "-d" || arg == "--debug") {
            debug_mode = true;
        } else if (arg == "-q" || arg == "--quiet") {
            quiet_mode = true;
        } else if ((arg == "-m" || arg == "--memory") && i + 1 < argc) {
            memory_size = std::stoul(argv[++i]);
        } else if (arg == "--max-steps" && i + 1 < argc) {
            limits.max_steps = std::stoul(argv[++i]);
        } else if (arg == "--max-output" && i + 1 < argc) {
            limits.max_output = std::stoul(argv[++i]);
        } else if (arg[0] != '-') {
            filename = arg;
        } else {
            std::cerr << "Unknown option: " << arg << "\n";
            return 1;
        }
    }

    if (filename.empty()) {
        std::cerr << "Error: No input file specified\n";
        printUsage(argv[0]);
        return 1;
    }

    try {
        // Parse the program
        if (!quiet_mode) {
            std::cout << "Loading program: " << filename << "...\n";
        }

        auto program = cow::Parser::parseFile(filename);

        if (!quiet_mode) {
            std::cout << "Parsed " << program.size() << " instructions\n";
        }

        // Create and configure VM
        cow::CowVM vm(limits, memory_size);
        vm.load(program);

        if (!quiet_mode) {
            std::cout << "\nExecuting program...\n";
            if (safe_mode) {
                std::cout << "Safe mode enabled:\n";
                std::cout << "  Memory limit: " << memory_size << " cells\n";
                std::cout << "  Step limit: " << (limits.max_steps > 0 ? std::to_string(limits.max_steps) : "unlimited") << "\n";
                std::cout << "  Output limit: " << (limits.max_output > 0 ? std::to_string(limits.max_output) + " bytes" : "unlimited") << "\n";
            }
            std::cout << "\n";
        }

        // Execute
        if (debug_mode) {
            // Step-by-step execution
            std::string cmd;
            while (vm.isRunning()) {
                std::cout << "PC=" << vm.programCounter()
                          << " MP=" << vm.memoryPointer()
                          << " MEM=" << vm.currentMemoryValue()
                          << " REG=" << (vm.hasRegisterValue() ? std::to_string(vm.registerValue()) : "empty")
                          << " > ";
                std::getline(std::cin, cmd);

                if (cmd == "q" || cmd == "quit") {
                    break;
                } else if (cmd == "run") {
                    debug_mode = false;
                    vm.run();
                    break;
                } else {
                    vm.step();
                }
            }
        } else {
            vm.run();
        }

        if (!quiet_mode) {
            std::cout << "\nExecution completed.\n";
            std::cout << "Total steps: " << vm.stepsExecuted() << "\n";
        }

        return 0;

    } catch (const cow::ParseException& e) {
        std::cerr << "Parse error: " << e.what() << "\n";
        return 1;
    } catch (const cow::LimitExceededException& e) {
        std::cerr << "Execution limit exceeded: " << e.what() << "\n";
        return 1;
    } catch (const cow::VMException& e) {
        std::cerr << "Runtime error: " << e.what() << "\n";
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
