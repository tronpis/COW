//--------------------------------------------
// COW PROGRAMMING LANGUAGE - Optimized VM Implementation
//--------------------------------------------
#include "cow/vm.hpp"
#include <iostream>
#include <cstring>
#include <stack>

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
    // Convert instructions to optimized format with pre-computed jump targets
    program_.clear();
    program_.reserve(program.size());

    // First pass: copy instructions
    for (const auto& inst : program) {
        OptimizedInstruction opt;
        opt.op = inst.op;
        opt.argument = inst.argument;
        opt.jump_target = 0;
        program_.push_back(opt);
    }

    // Second pass: pre-compute loop jump targets (O(n) once, not every iteration)
    std::stack<size_t> loop_stack;
    for (size_t i = 0; i < program_.size(); ++i) {
        if (program_[i].op == OpCode::MOO_Upper) {
            loop_stack.push(i);
        } else if (program_[i].op == OpCode::Moo_Lower) {
            if (!loop_stack.empty()) {
                size_t start = loop_stack.top();
                loop_stack.pop();
                program_[i].jump_target = start;      // moo jumps back to MOO
                program_[start].jump_target = i;      // MOO jumps forward to moo
            }
        }
    }

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

void CowVM::execute(const OptimizedInstruction& inst) {
    // Inline dispatch for performance - compiler can optimize this better
    switch (inst.op) {
        case OpCode::Moo_Lower: {
            // Jump back to matching MOO if memory is non-zero
            if (memory_[memory_ptr_] != 0) {
                pc_ = inst.jump_target;
                return;  // Don't increment PC
            }
            break;
        }

        case OpCode::mOo: {
            if (memory_ptr_ == 0) {
                throw RuntimeError("memory pointer underflow");
            }
            memory_ptr_--;
            break;
        }

        case OpCode::moO: {
            memory_ptr_++;
            if (memory_ptr_ >= memory_.size()) {
                if (limits_.max_memory > 0 && memory_.size() >= limits_.max_memory) {
                    throw LimitError("maximum memory exceeded");
                }
                memory_.push_back(0);
            }
            break;
        }

        case OpCode::mOO: {
            int val = memory_[memory_ptr_];
            if (val == 3) {
                running_ = false;
                status_ = VMStatus::Halted;
            } else if (val >= 0 && val <= 11) {
                // Execute instruction from memory
                OptimizedInstruction temp;
                temp.op = static_cast<OpCode>(val);
                temp.argument = 0;
                temp.jump_target = 0;
                execute(temp);
            }
            break;
        }

        case OpCode::Moo_Mixed: {
            if (memory_[memory_ptr_] != 0) {
                if (output_char_handler_) {
                    output_char_handler_(static_cast<char>(memory_[memory_ptr_]));
                } else {
                    defaultOutputChar(static_cast<char>(memory_[memory_ptr_]));
                }
            } else {
                int c = input_handler_ ? input_handler_() : defaultInput();
                if (c == EOF) c = 0;
                memory_[memory_ptr_] = c;
                if (c != '\n') {
                    while ((c = defaultInput()) != '\n' && c != EOF);
                }
            }
            break;
        }

        case OpCode::MOo: {
            memory_[memory_ptr_] -= inst.argument > 0 ? inst.argument : 1;
            break;
        }

        case OpCode::MoO: {
            memory_[memory_ptr_] += inst.argument > 0 ? inst.argument : 1;
            break;
        }

        case OpCode::MOO_Upper: {
            // Jump to matching moo if memory is zero
            if (memory_[memory_ptr_] == 0) {
                pc_ = inst.jump_target;
                return;  // Don't increment PC
            }
            break;
        }

        case OpCode::OOO: {
            memory_[memory_ptr_] = 0;
            break;
        }

        case OpCode::MMM: {
            if (has_register_val_) {
                memory_[memory_ptr_] = register_val_;
            } else {
                register_val_ = memory_[memory_ptr_];
            }
            has_register_val_ = !has_register_val_;
            break;
        }

        case OpCode::OOM: {
            if (output_int_handler_) {
                output_int_handler_(memory_[memory_ptr_]);
            } else {
                defaultOutputInt(memory_[memory_ptr_]);
            }
            break;
        }

        case OpCode::oom: {
            char buf[100];
            int c = 0;
            int i = 0;
            while (i < static_cast<int>(sizeof(buf)) - 1) {
                c = input_handler_ ? input_handler_() : defaultInput();
                buf[i] = static_cast<char>(c);
                i++;
                buf[i] = '\0';
                if (c == '\n' || c == EOF) break;
            }
            if (i == static_cast<int>(sizeof(buf)) - 1) {
                while ((c = defaultInput()) != '\n' && c != EOF);
            }
            memory_[memory_ptr_] = std::atoi(buf);
            break;
        }

        case OpCode::Invalid:
        default: {
            throw RuntimeError("invalid instruction");
        }
    }
}

void CowVM::step() {
    if (!running_) return;
    if (pc_ >= program_.size()) {
        running_ = false;
        status_ = VMStatus::Halted;
        return;
    }

    steps_executed_++;
    checkLimits();

    const OptimizedInstruction& inst = program_[pc_];
    execute(inst);

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

    while (running_ && pc_ < program_.size()) {
        step();
    }

    if (status_ == VMStatus::Running) {
        status_ = VMStatus::Halted;
    }
}

} // namespace cow
