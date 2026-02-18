//--------------------------------------------
// COW PROGRAMMING LANGUAGE - Instruction Implementation
//--------------------------------------------
#include "cow/instruction.hpp"

#include <cstring>

namespace cow {

std::string Instruction::toString() const {
    switch (op) {
        case OpCode::Moo_Lower:   return "moo";
        case OpCode::mOo:         return "mOo";
        case OpCode::moO:         return "moO";
        case OpCode::mOO:         return "mOO";
        case OpCode::Moo_Mixed:   return "Moo";
        case OpCode::MOo:         return "MOo";
        case OpCode::MoO:         return "MoO";
        case OpCode::MOO_Upper:   return "MOO";
        case OpCode::OOO:         return "OOO";
        case OpCode::MMM:         return "MMM";
        case OpCode::OOM:         return "OOM";
        case OpCode::oom:         return "oom";
        case OpCode::Invalid:     return "INVALID";
        default:                  return "UNKNOWN";
    }
}

OpCode tokenToOpCode(const char* token) {
    if (std::strncmp(token, "moo", 3) == 0) return OpCode::Moo_Lower;
    if (std::strncmp(token, "mOo", 3) == 0) return OpCode::mOo;
    if (std::strncmp(token, "moO", 3) == 0) return OpCode::moO;
    if (std::strncmp(token, "mOO", 3) == 0) return OpCode::mOO;
    if (std::strncmp(token, "Moo", 3) == 0) return OpCode::Moo_Mixed;
    if (std::strncmp(token, "MOo", 3) == 0) return OpCode::MOo;
    if (std::strncmp(token, "MoO", 3) == 0) return OpCode::MoO;
    if (std::strncmp(token, "MOO", 3) == 0) return OpCode::MOO_Upper;
    if (std::strncmp(token, "OOO", 3) == 0) return OpCode::OOO;
    if (std::strncmp(token, "MMM", 3) == 0) return OpCode::MMM;
    if (std::strncmp(token, "OOM", 3) == 0) return OpCode::OOM;
    if (std::strncmp(token, "oom", 3) == 0) return OpCode::oom;
    return OpCode::Invalid;
}

} // namespace cow
