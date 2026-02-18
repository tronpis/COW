//--------------------------------------------
// COW PROGRAMMING LANGUAGE - Parser Implementation
//--------------------------------------------
#include "cow/parser.hpp"

#include <fstream>
#include <sstream>
#include <cstring>
#include <stack>

namespace cow {

std::vector<std::string> Parser::tokenize(const std::string& source) {
    std::vector<std::string> tokens;
    char buf[3] = {0, 0, 0};

    for (char c : source) {
        // Shift buffer
        buf[0] = buf[1];
        buf[1] = buf[2];
        buf[2] = c;

        // Check if we have a valid instruction
        OpCode op = tokenToOpCode(buf);
        if (op != OpCode::Invalid) {
            tokens.emplace_back(buf, 3);
            // Reset buffer after match
            buf[0] = 0;
            buf[1] = 0;
            buf[2] = 0;
        }
    }

    return tokens;
}

std::vector<Instruction> Parser::parseString(const std::string& source) {
    auto tokens = tokenize(source);
    std::vector<Instruction> program;
    program.reserve(tokens.size());

    for (const auto& token : tokens) {
        program.emplace_back(tokenToOpCode(token.c_str()));
    }

    validateLoops(program);
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
        throw ParseException("Cannot open source file: " + filename);
    }
    return parseStream(file);
}

std::vector<Instruction> Parser::parseOptimized(const std::string& source) {
    auto tokens = tokenize(source);
    std::vector<Instruction> program;
    program.reserve(tokens.size());

    // Optimization: combine consecutive MoO (increment) and MOo (decrement)
    int inc_count = 0;

    for (const auto& token : tokens) {
        OpCode op = tokenToOpCode(token.c_str());

        // Count consecutive increments/decrements
        if (op == OpCode::MoO) {
            inc_count++;
        } else if (op == OpCode::MOo) {
            inc_count--;
        } else {
            // Flush pending increments
            if (inc_count != 0) {
                if (inc_count > 0) {
                    program.emplace_back(OpCode::MoO, inc_count);
                } else {
                    program.emplace_back(OpCode::MOo, -inc_count);
                }
                inc_count = 0;
            }
            program.emplace_back(op);
        }
    }

    // Flush any remaining increments
    if (inc_count != 0) {
        if (inc_count > 0) {
            program.emplace_back(OpCode::MoO, inc_count);
        } else {
            program.emplace_back(OpCode::MOo, -inc_count);
        }
    }

    validateLoops(program);
    return program;
}

void Parser::validateLoops(const std::vector<Instruction>& program) {
    std::stack<size_t> loop_stack;

    for (size_t i = 0; i < program.size(); ++i) {
        if (program[i].op == OpCode::MOO_Upper) {
            // Loop start
            loop_stack.push(i);
        } else if (program[i].op == OpCode::Moo_Lower) {
            // Loop end
            if (loop_stack.empty()) {
                throw ParseException("Unmatched 'moo' instruction at position " + std::to_string(i));
            }
            loop_stack.pop();
        }
    }

    if (!loop_stack.empty()) {
        throw ParseException("Unmatched 'MOO' instruction at position " + std::to_string(loop_stack.top()));
    }
}

} // namespace cow
