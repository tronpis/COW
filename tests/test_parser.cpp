//--------------------------------------------
// COW Tests - Parser
//--------------------------------------------
#include <catch2/catch_test_macros.hpp>
#include "cow/parser.hpp"
#include "cow/error.hpp"

using namespace cow;

TEST_CASE("Parser parses simple instructions", "[parser]") {
    auto program = Parser::parseString("MoO");
    REQUIRE(program.size() == 1);
    REQUIRE(program[0].op == OpCode::MoO);
}

TEST_CASE("Parser parses multiple instructions", "[parser]") {
    auto program = Parser::parseString("MoO MOo moO");
    REQUIRE(program.size() == 3);
    REQUIRE(program[0].op == OpCode::MoO);
    REQUIRE(program[1].op == OpCode::MOo);
    REQUIRE(program[2].op == OpCode::moO);
}

TEST_CASE("Parser ignores non-instruction characters", "[parser]") {
    auto program = Parser::parseString("hello MoO world");
    REQUIRE(program.size() == 1);
    REQUIRE(program[0].op == OpCode::MoO);
}

TEST_CASE("Parser detects unmatched moo", "[parser]") {
    REQUIRE_THROWS_AS(Parser::parseString("moo"), ParseError);
}

TEST_CASE("Parser detects unmatched MOO", "[parser]") {
    REQUIRE_THROWS_AS(Parser::parseString("MOO"), ParseError);
}

TEST_CASE("Parser validates nested loops", "[parser]") {
    // Valid nested loops: MOO MOO moo moo
    auto program = Parser::parseString("MOO MOO moo moo");
    REQUIRE(program.size() == 4);
    REQUIRE(program[0].op == OpCode::MOO_Upper);
    REQUIRE(program[1].op == OpCode::MOO_Upper);
    REQUIRE(program[2].op == OpCode::Moo_Lower);
    REQUIRE(program[3].op == OpCode::Moo_Lower);
}

TEST_CASE("Parser detects unbalanced nested loops", "[parser]") {
    // MOO MOO moo (missing one moo)
    REQUIRE_THROWS_AS(Parser::parseString("MOO MOO moo"), ParseError);
}

TEST_CASE("Parser parses all instructions", "[parser]") {
    auto program = Parser::parseString(
        "moo mOo moO mOO Moo MOo MoO MOO OOO MMM OOM oom"
    );
    REQUIRE(program.size() == 12);
    REQUIRE(program[0].op == OpCode::Moo_Lower);
    REQUIRE(program[1].op == OpCode::mOo);
    REQUIRE(program[2].op == OpCode::moO);
    REQUIRE(program[3].op == OpCode::mOO);
    REQUIRE(program[4].op == OpCode::Moo_Mixed);
    REQUIRE(program[5].op == OpCode::MOo);
    REQUIRE(program[6].op == OpCode::MoO);
    REQUIRE(program[7].op == OpCode::MOO_Upper);
    REQUIRE(program[8].op == OpCode::OOO);
    REQUIRE(program[9].op == OpCode::MMM);
    REQUIRE(program[10].op == OpCode::OOM);
    REQUIRE(program[11].op == OpCode::oom);
}

TEST_CASE("Parser optimization combines increments", "[parser]") {
    auto program = Parser::parseOptimized("MoO MoO MoO");
    REQUIRE(program.size() == 1);
    REQUIRE(program[0].op == OpCode::MoO);
    REQUIRE(program[0].argument == 3);
}

TEST_CASE("Parser optimization combines decrements", "[parser]") {
    auto program = Parser::parseOptimized("MOo MOo");
    REQUIRE(program.size() == 1);
    REQUIRE(program[0].op == OpCode::MOo);
    REQUIRE(program[0].argument == 2);
}

TEST_CASE("Parser optimization cancels increments and decrements", "[parser]") {
    auto program = Parser::parseOptimized("MoO MOo");
    REQUIRE(program.size() == 0);
}
