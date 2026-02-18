//--------------------------------------------
// COW PROGRAMMING LANGUAGE - VM Implementation
//--------------------------------------------
#include "cow/vm.hpp"

#include <iostream>
#include <limits>

namespace cow {

CowVM::CowVM(size_t memory_size) : limits_(Limits::Unlimited()) {
    memory_.resize(memory_size, 0);
}

CowVM::CowVM(const Limits& limits, size_t memory_size) : limits_(limits) {
    if (limits.max_memory > 0 && limits.max_memory < memory_size) {
        memory_size = limits.max_memory;
    }
    memory_.resize(memory_size, 0);
}

void CowVM::load(const std::vector<Instruction>& program) {
    program_ = program;
    reset();
}

void CowVM::reset() {
    std::fill(memory_.begin(), memory_.end(), 0);
    pc_ = 0;
    memory_ptr_ = 0;
    register_val_ = 0;
    has_register_val_ = false;
    running_ = false;
    status_ = VMStatus::Ready;
    steps_executed_ = 0;
}

int CowVM::memoryAt(size_t pos) const {
    if (pos >= memory_.size()) {
        throw RuntimeError("memory access out of bounds");
    }
    return memory_[pos];
}

int CowVM::defaultInput() {
    int c = std::getchar();
    if (c == EOF && std::ferror(stdin)) {
        throw IOError("failed to read input");
    }
    return c;
}

void CowVM::defaultOutputChar(char c) {
    if (std::putchar(c) == EOF) {
        throw IOError("failed to write output");
    }
}

void CowVM::defaultOutputInt(int i) {
    if (std::printf("%d\n", i) < 0) {
        throw IOError("failed to write output");
    }
}

void CowVM::checkLimits() {
    if (limits_.max_steps > 0 && steps_executed_ > limits_.max_steps) {
        throw LimitError("maximum execution steps exceeded (limit: " +
                         std::to_string(limits_.max_steps) + ")");
    }
}

size_t CowVM::findLoopStart(size_t from_pos) {
    int level = 1;
    size_t pos = from_pos;

    // Skip previous command
    if (pos == 0) {
        throw RuntimeError("invalid loop structure: cannot find matching MOO");
    }
    pos--;

    while (level > 0 && pos > 0) {
        pos--;
        if (program_[pos].op == OpCode::Moo_Lower) {
            level++;
        } else if (program_[pos].op == OpCode::MOO_Upper) {
            level--;
        }
    }

    if (level != 0) {
        throw RuntimeError("invalid loop structure: unbalanced moo/MOO");
    }

    return pos;
}

size_t CowVM::findLoopEnd(size_t from_pos) {
    int level = 1;
    size_t pos = from_pos + 1;  // Start after current MOO

    while (level > 0 && pos < program_.size()) {
        if (program_[pos].op == OpCode::MOO_Upper) {
            level++;
        } else if (program_[pos].op == OpCode::Moo_Lower) {
            level--;
        }
        if (level > 0) {
            pos++;
        }
    }

    if (level != 0) {
        throw RuntimeError("invalid loop structure: unbalanced MOO/moo");
    }

    return pos;
}

void CowVM::execute(const Instruction& inst) {
    switch (inst.op) {
        case OpCode::Moo_Lower: {
            // Jump back to matching MOO if current memory is non-zero
            if (memory_[memory_ptr_] != 0) {
                pc_ = findLoopStart(pc_);
            }
            break;
        }

        case OpCode::mOo: {
            // Move memory pointer back
            if (memory_ptr_ == 0) {
                throw RuntimeError("memory pointer underflow (attempted to move below position 0)");
            }
            memory_ptr_--;
            break;
        }

        case OpCode::moO: {
            // Move memory pointer forward
            memory_ptr_++;
            if (memory_ptr_ >= memory_.size()) {
                if (limits_.max_memory > 0 && memory_.size() >= limits_.max_memory) {
                    throw LimitError("maximum memory size exceeded (limit: " +
                                     std::to_string(limits_.max_memory) + " cells)");
                }
                memory_.push_back(0);
            }
            break;
        }

        case OpCode::mOO: {
            // Execute instruction at memory position
            int mem_val = memory_[memory_ptr_];
            if (mem_val == 3) {
                // Special case: exit
                running_ = false;
                status_ = VMStatus::Halted;
                return;
            }
            if (mem_val >= 0 && mem_val <= 11) {
                // Execute the instruction stored in memory
                execute(Instruction(static_cast<OpCode>(mem_val)));
                // Don't increment PC - we already executed one
                return;
            }
            // Invalid instruction in memory - do nothing (as per spec)
            break;
        }

        case OpCode::Moo_Mixed: {
            // I/O operation
            if (memory_[memory_ptr_] != 0) {
                // Output character
                if (output_char_handler_) {
                    output_char_handler_(static_cast<char>(memory_[memory_ptr_]));
                } else {
                    defaultOutputChar(static_cast<char>(memory_[memory_ptr_]));
                }
            } else {
                // Input character
                int c;
                if (input_handler_) {
                    c = input_handler_();
                } else {
                    c = defaultInput();
                }
                if (c == EOF) {
                    c = 0;
                }
                memory_[memory_ptr_] = c;
                // Consume rest of line
                if (c != '\n') {
                    while ((c = defaultInput()) != '\n' && c != EOF);
                }
            }
            break;
        }

        case OpCode::MOo: {
            // Decrement memory
            memory_[memory_ptr_]--;
            break;
        }

        case OpCode::MoO: {
            // Increment memory
            if (inst.argument > 1) {
                memory_[memory_ptr_] += inst.argument;
            } else {
                memory_[memory_ptr_]++;
            }
            break;
        }

        case OpCode::MOO_Upper: {
            // Loop start - if zero, jump to matching moo
            if (memory_[memory_ptr_] == 0) {
                pc_ = findLoopEnd(pc_);
            }
            break;
        }

        case OpCode::OOO: {
            // Zero memory
            memory_[memory_ptr_] = 0;
            break;
        }

        case OpCode::MMM: {
            // Memory/register exchange
            if (has_register_val_) {
                memory_[memory_ptr_] = register_val_;
            } else {
                register_val_ = memory_[memory_ptr_];
            }
            has_register_val_ = !has_register_val_;
            break;
        }

        case OpCode::OOM: {
            // Output number
            if (output_int_handler_) {
                output_int_handler_(memory_[memory_ptr_]);
            } else {
                defaultOutputInt(memory_[memory_ptr_]);
            }
            break;
        }

        case OpCode::oom: {
            // Input number
            char buf[100];
            int c = 0;
            int i = 0;
            while (i < static_cast<int>(sizeof(buf)) - 1) {
                if (input_handler_) {
                    c = input_handler_();
                } else {
                    c = defaultInput();
                }
                buf[i] = static_cast<char>(c);
                i++;
                buf[i] = '\0';
                if (c == '\n' || c == EOF) {
                    break;
                }
            }
            // Swallow remaining input if buffer full
            if (i == static_cast<int>(sizeof(buf)) - 1) {
                while ((c = defaultInput()) != '\n' && c != EOF);
            }
            memory_[memory_ptr_] = std::atoi(buf);
            break;
        }

        case OpCode::Invalid:
        default: {
            throw RuntimeError("invalid instruction encountered");
        }
    }
}

void CowVM::step() {
    if (!running_) {
        return;
    }

    if (pc_ >= program_.size()) {
        running_ = false;
        status_ = VMStatus::Halted;
        return;
    }

    steps_executed_++;
    checkLimits();

    const Instruction& inst = program_[pc_];
    execute(inst);

    // Only increment PC if we're still running (execute might halt)
    if (running_) {
        pc_++;
    }
}

void CowVM::run() {
    if (program_.empty()) {
        status_ = VMStatus::Halted;
        return;
    }

    running_ = true;
    status_ = VMStatus::Running;

    // Main execution loop - no recursion
    while (running_ && pc_ < program_.size()) {
        step();
    }

    if (status_ == VMStatus::Running) {
        status_ = VMStatus::Halted;
    }
}

} // namespace cow
