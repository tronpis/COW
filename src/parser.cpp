//--------------------------------------------
// COW PROGRAMMING LANGUAGE - Parser Implementation
//--------------------------------------------
#include "cow/parser.hpp"
#include "cow/error.hpp"

#include <fstream>
#include <sstream>
#include <cstring>
#include <stack>
#include <map>

namespace cow {

struct Token {
    std::string text;
    SourceLocation location;
};

static std::vector<Token> tokenizeWithLocation(const std::string& source) {
    std::vector<Token> tokens;
    char buf[3] = {0, 0, 0};
    SourceLocation loc;
    std::map<size_t, SourceLocation> positionToLocation;

    for (size_t i = 0; i < source.size(); ++i) {
        char c = source[i];
        positionToLocation[i] = loc;

        // Shift buffer
        buf[0] = buf[1];
        buf[1] = buf[2];
        buf[2] = c;

        // Check if we have a valid instruction
        OpCode op = tokenToOpCode(buf);
        if (op != OpCode::Invalid) {
            Token tok;
            tok.text = std::string(buf, 3);
            // Location is at the end of the token (current position)
            tok.location = loc;
            tokens.push_back(tok);

            // Reset buffer after match
            buf[0] = 0;
            buf[1] = 0;
            buf[2] = 0;
        }

        // Update location
        if (c == '\n') {
            loc.line++;
            loc.column = 1;
        } else {
            loc.column++;
        }
        loc.offset++;
    }

    return tokens;
}

std::vector<LocatedInstruction> Parser::parseWithLocation(const std::string& source) {
    auto tokens = tokenizeWithLocation(source);
    std::vector<LocatedInstruction> program;
    program.reserve(tokens.size());

    for (const auto& tok : tokens) {
        Instruction inst(tokenToOpCode(tok.text.c_str()));
        program.emplace_back(inst, tok.location);
    }

    validateLoopsWithLocation(program);
    return program;
}

std::vector<Instruction> Parser::parseString(const std::string& source) {
    auto located = parseWithLocation(source);
    std::vector<Instruction> result;
    result.reserve(located.size());
    for (const auto& li : located) {
        result.push_back(li.instruction);
    }
    return result;
}

std::vector<Instruction> Parser::parseStream(std::istream& stream) {
    std::stringstream buffer;
    buffer << stream.rdbuf();
    return parseString(buffer.str());
}

std::vector<Instruction> Parser::parseFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw IOError("Cannot open source file: " + filename);
    }
    return parseStream(file);
}

std::vector<LocatedInstruction> Parser::parseFileWithLocation(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw IOError("Cannot open source file: " + filename);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return parseWithLocation(buffer.str());
}

std::vector<Instruction> Parser::parseOptimized(const std::string& source) {
    auto tokens = tokenizeWithLocation(source);
    std::vector<Instruction> program;
    program.reserve(tokens.size());

    // Optimization: combine consecutive MoO (increment) and MOo (decrement)
    int inc_count = 0;
    std::optional<SourceLocation> first_loc;

    for (const auto& tok : tokens) {
        OpCode op = tokenToOpCode(tok.text.c_str());

        // Count consecutive increments/decrements
        if (op == OpCode::MoO) {
            if (!first_loc) first_loc = tok.location;
            inc_count++;
        } else if (op == OpCode::MOo) {
            if (!first_loc) first_loc = tok.location;
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
                first_loc = std::nullopt;
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
            loop_stack.push(i);
        } else if (program[i].op == OpCode::Moo_Lower) {
            if (loop_stack.empty()) {
                throw ParseError("unmatched 'moo' (loop end) without corresponding 'MOO' (loop start)");
            }
            loop_stack.pop();
        }
    }

    if (!loop_stack.empty()) {
        throw ParseError("unmatched 'MOO' (loop start) without corresponding 'moo' (loop end)");
    }
}

void Parser::validateLoopsWithLocation(const std::vector<LocatedInstruction>& program) {
    std::stack<size_t> loop_stack;

    for (size_t i = 0; i < program.size(); ++i) {
        if (program[i].instruction.op == OpCode::MOO_Upper) {
            loop_stack.push(i);
        } else if (program[i].instruction.op == OpCode::Moo_Lower) {
            if (loop_stack.empty()) {
                throw ParseError("unmatched 'moo' (loop end) without corresponding 'MOO' (loop start)",
                                program[i].location);
            }
            loop_stack.pop();
        }
    }

    if (!loop_stack.empty()) {
        throw ParseError("unmatched 'MOO' (loop start) without corresponding 'moo' (loop end)",
                        program[loop_stack.top()].location);
    }
}

} // namespace cow
