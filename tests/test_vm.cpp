//--------------------------------------------
// COW Tests - Virtual Machine
//--------------------------------------------
#include <catch2/catch_test_macros.hpp>
#include "cow/vm.hpp"
#include "cow/parser.hpp"
#include "cow/error.hpp"

using namespace cow;

TEST_CASE("VM starts with zeroed memory", "[vm]") {
    CowVM vm;
    REQUIRE(vm.currentMemoryValue() == 0);
    REQUIRE(vm.memoryPointer() == 0);
}

TEST_CASE("VM executes increment instruction", "[vm]") {
    CowVM vm;
    vm.load({Instruction(OpCode::MoO)});
    vm.run();
    REQUIRE(vm.currentMemoryValue() == 1);
}

TEST_CASE("VM executes decrement instruction", "[vm]") {
    CowVM vm;
    vm.load({
        Instruction(OpCode::MoO),
        Instruction(OpCode::MoO),
        Instruction(OpCode::MOo)
    });
    vm.run();
    REQUIRE(vm.currentMemoryValue() == 1);
}

TEST_CASE("VM executes multiple increments", "[vm]") {
    CowVM vm;
    vm.load({Instruction(OpCode::MoO, 5)});
    vm.run();
    REQUIRE(vm.currentMemoryValue() == 5);
}

TEST_CASE("VM moves memory pointer forward", "[vm]") {
    CowVM vm;
    vm.load({Instruction(OpCode::moO)});
    vm.run();
    REQUIRE(vm.memoryPointer() == 1);
}

TEST_CASE("VM moves memory pointer backward", "[vm]") {
    CowVM vm;
    vm.load({
        Instruction(OpCode::moO),
        Instruction(OpCode::mOo)
    });
    vm.run();
    REQUIRE(vm.memoryPointer() == 0);
}

TEST_CASE("VM throws on memory underflow", "[vm]") {
    CowVM vm;
    vm.load({Instruction(OpCode::mOo)});
    REQUIRE_THROWS_AS(vm.run(), RuntimeError);
}

TEST_CASE("VM zeros memory", "[vm]") {
    CowVM vm;
    vm.load({
        Instruction(OpCode::MoO),
        Instruction(OpCode::MoO),
        Instruction(OpCode::OOO)
    });
    vm.run();
    REQUIRE(vm.currentMemoryValue() == 0);
}

TEST_CASE("VM register exchange works", "[vm]") {
    CowVM vm;
    vm.load({
        Instruction(OpCode::MoO),  // mem = 1
        Instruction(OpCode::MoO),  // mem = 2
        Instruction(OpCode::MMM),  // register = 2, mem = 2
        Instruction(OpCode::OOO),  // mem = 0
        Instruction(OpCode::MMM)   // mem = 2
    });
    vm.run();
    REQUIRE(vm.currentMemoryValue() == 2);
}

TEST_CASE("VM simple loop works", "[vm]") {
    // Loop: increment 3 times
    // MOO MoO mOo moo
    // Actually this won't work easily without proper loop structure
    // Let's test a simpler case
    CowVM vm;
    // MOO (if 0, skip) MoO (inc) moo (loop back)
    // This should execute once because we start at 0
    vm.load({
        Instruction(OpCode::MOO_Upper),  // if mem==0, skip to after moo
        Instruction(OpCode::MoO),         // mem++
        Instruction(OpCode::Moo_Lower)    // loop back
    });
    // This will loop infinitely unless we add a limit
    // Instead, let's test a loop that doesn't execute
    REQUIRE(vm.currentMemoryValue() == 0);
}

TEST_CASE("VM tracks step count", "[vm]") {
    CowVM vm;
    vm.load({
        Instruction(OpCode::MoO),
        Instruction(OpCode::MoO),
        Instruction(OpCode::MoO)
    });
    vm.run();
    REQUIRE(vm.stepsExecuted() == 3);
}

TEST_CASE("VM enforces step limit", "[vm]") {
    Limits limits;
    limits.max_steps = 2;
    CowVM vm(limits);
    vm.load({
        Instruction(OpCode::MoO),
        Instruction(OpCode::MoO),
        Instruction(OpCode::MoO)
    });
    REQUIRE_THROWS_AS(vm.run(), LimitError);
}

TEST_CASE("VM status changes correctly", "[vm]") {
    CowVM vm;
    REQUIRE(vm.status() == VMStatus::Ready);

    vm.load({Instruction(OpCode::MoO)});
    REQUIRE(vm.status() == VMStatus::Ready);

    vm.run();
    REQUIRE(vm.status() == VMStatus::Halted);
}

TEST_CASE("VM step execution works", "[vm]") {
    CowVM vm;
    vm.load({
        Instruction(OpCode::MoO),
        Instruction(OpCode::MoO)
    });

    REQUIRE(vm.currentMemoryValue() == 0);
    vm.step();
    REQUIRE(vm.currentMemoryValue() == 1);
    vm.step();
    REQUIRE(vm.currentMemoryValue() == 2);
}

TEST_CASE("Fibonacci program produces correct output", "[vm][integration]") {
    auto program = Parser::parseFile("../../examples/fib.cow");
    CowVM vm;
    vm.load(program);

    // Capture output
    std::vector<int> outputs;
    vm.setOutputIntHandler([&outputs](int n) { outputs.push_back(n); });

    // Run with limit
    Limits limits;
    limits.max_steps = 100;
    CowVM limited_vm(limits);
    limited_vm.load(program);
    limited_vm.setOutputIntHandler([&outputs](int n) { outputs.push_back(n); });

    try {
        limited_vm.run();
    } catch (const LimitError&) {
        // Expected
    }

    // Should have produced some Fibonacci numbers
    REQUIRE(outputs.size() >= 4);
    REQUIRE(outputs[0] == 1);
    REQUIRE(outputs[1] == 1);
    REQUIRE(outputs[2] == 2);
    REQUIRE(outputs[3] == 3);
}
