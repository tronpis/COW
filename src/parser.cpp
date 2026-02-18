//--------------------------------------------
// COW PROGRAMMING LANGUAGE - Parser Implementation
//--------------------------------------------
#include "cow/parser.hpp"
#include "cow/error.hpp"
#include <fstream>
#include <sstream>
#include <cstring>
#include <stack>

namespace cow {

std::vector<Instruction> Parser::parseString(const std::string& source) {
    std::vector<Instruction> program;
    char buf[3] = {0, 0, 0};

    for (char c : source) {
        buf[0] = buf[1];
        buf[1] = buf[2];
        buf[2] = c;

        OpCode op = tokenToOpCode(buf);
        if (op != OpCode::Invalid) {
            program.emplace_back(op);
            buf[0] = 0;
            buf[1] = 0;
            buf[2] = 0;
        }
    }

    // Validate loop structure
    std::stack<size_t> loop_stack;
    for (size_t i = 0; i < program.size(); ++i) {
        if (program[i].op == OpCode::MOO_Upper) {
            loop_stack.push(i);
        } else if (program[i].op == OpCode::Moo_Lower) {
            if (loop_stack.empty()) {
                throw ParseError("unmatched 'moo' (loop end) without corresponding 'MOO'");
            }
            loop_stack.pop();
        }
    }
    if (!loop_stack.empty()) {
        throw ParseError("unmatched 'MOO' (loop start) without corresponding 'moo'");
    }

    return program;
}

std::vector<Instruction> Parser::parseStream(std::istream& stream) {
    std::stringstream buffer;
    buffer << stream.rdbuf();
    return parseString(buffer.str());
}

std::vector<Instruction> Parser::parseFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw IOError("cannot open source file: " + filename);
    }
    return parseStream(file);
}

std::vector<Instruction> Parser::parseOptimized(const std::string& source) {
    std::vector<Instruction> program;
    char buf[3] = {0, 0, 0};
    int inc_count = 0;

    auto flush_increments = [&]() {
        if (inc_count != 0) {
            if (inc_count > 0) {
                program.emplace_back(OpCode::MoO, inc_count);
            } else {
                program.emplace_back(OpCode::MOo, -inc_count);
            }
            inc_count = 0;
        }
    };

    for (char c : source) {
        buf[0] = buf[1];
        buf[1] = buf[2];
        buf[2] = c;

        OpCode op = tokenToOpCode(buf);
        if (op != OpCode::Invalid) {
            if (op == OpCode::MoO) {
                inc_count++;
            } else if (op == OpCode::MOo) {
                inc_count--;
            } else {
                flush_increments();
                program.emplace_back(op);
            }
            buf[0] = 0;
            buf[1] = 0;
            buf[2] = 0;
        }
    }
    flush_increments();

    // Validate loops
    std::stack<size_t> loop_stack;
    for (size_t i = 0; i < program.size(); ++i) {
        if (program[i].op == OpCode::MOO_Upper) {
            loop_stack.push(i);
        } else if (program[i].op == OpCode::Moo_Lower) {
            if (loop_stack.empty()) {
                throw ParseError("unmatched 'moo' (loop end)");
            }
            loop_stack.pop();
        }
    }
    if (!loop_stack.empty()) {
        throw ParseError("unmatched 'MOO' (loop start)");
    }

    return program;
}

} // namespace cow
