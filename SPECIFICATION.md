# COW Programming Language Specification

## Version 2.0.0

## 1. Overview

COW is an esoteric programming language created by Sean Heber (BigZaphod). All instructions in COW are variations of the word "moo" with different capitalizations.

## 2. Language Structure

### 2.1 Character Set
- Source files use ASCII encoding
- Only letters 'm', 'o', 'O', 'M' are significant
- All other characters are treated as whitespace/comments

### 2.2 Instruction Format
Each instruction is exactly 3 characters forming a case-sensitive variation of "moo":

| Instruction | Code | Description |
|-------------|------|-------------|
| `moo` | 0 | Loop end / jump back to matching `MOO` |
| `mOo` | 1 | Move memory pointer backward |
| `moO` | 2 | Move memory pointer forward |
| `mOO` | 3 | Execute instruction at memory position |
| `Moo` | 4 | I/O: output char if mem≠0, else input char |
| `MOo` | 5 | Decrement memory at pointer |
| `MoO` | 6 | Increment memory at pointer |
| `MOO` | 7 | Loop start / jump forward if mem=0 |
| `OOO` | 8 | Zero memory at pointer |
| `MMM` | 9 | Memory/register exchange |
| `OOM` | 10 | Output memory as number |
| `oom` | 11 | Input number to memory |

## 3. Memory Model

### 3.1 Memory Tape
- Linear array of signed integers
- Default size: 30,000 cells
- Cells are initialized to 0
- Pointer starts at position 0

### 3.2 Memory Operations
- **Bounds**: Moving pointer below 0 is a runtime error
- **Growth**: Moving pointer beyond current size extends memory
- **Cell type**: `int` (platform-dependent, typically 32-bit)

## 4. Execution Model

### 4.1 Program State
- **Program Counter (PC)**: Points to current instruction
- **Memory Pointer (MP)**: Points to current memory cell
- **Register**: Single value for MMM instruction
- **Register Flag**: Boolean indicating if register holds a value

### 4.2 Instruction Semantics

#### `moo` (0) - Loop End
```
if memory[MP] != 0:
    PC = matching_MOO_position
else:
    PC++
```

#### `mOo` (1) - Move Backward
```
if MP == 0:
    error: memory underflow
else:
    MP--
    PC++
```

#### `moO` (2) - Move Forward
```
MP++
if MP >= memory_size:
    extend_memory()
PC++
```

#### `mOO` (3) - Execute from Memory
```
val = memory[MP]
if val == 3:
    halt_program()
else if 0 <= val <= 11:
    execute_instruction(val)
else:
    nop
```

#### `Moo` (4) - I/O
```
if memory[MP] != 0:
    output_char(memory[MP])
else:
    memory[MP] = input_char()
PC++
```

#### `MOo` (5) - Decrement
```
memory[MP]--
PC++
```

#### `MoO` (6) - Increment
```
memory[MP]++
PC++
```

#### `MOO` (7) - Loop Start
```
if memory[MP] == 0:
    PC = matching_moo_position + 1
else:
    PC++
```

#### `OOO` (8) - Zero
```
memory[MP] = 0
PC++
```

#### `MMM` (9) - Register Exchange
```
if register_has_value:
    memory[MP] = register
    register_has_value = false
else:
    register = memory[MP]
    register_has_value = true
PC++
```

#### `OOM` (10) - Output Number
```
output_number(memory[MP])
PC++
```

#### `oom` (11) - Input Number
```
memory[MP] = input_number()
PC++
```

## 5. Error Conditions

### 5.1 Parse Errors
- **Unmatched Loop**: `moo` without `MOO` or vice versa
- Result: Program fails to load

### 5.2 Runtime Errors
- **Memory Underflow**: Moving pointer below 0
- **Invalid mOO**: Executing undefined instruction code
- **Limit Exceeded**: Step/memory/output limit reached

### 5.3 Error Behavior
Implementation must halt execution and report error with location information.

## 6. Example Programs

### 6.1 Hello World (conceptual)
Note: COW is not well-suited for text output. Numbers are preferred.

### 6.2 Fibonacci Sequence
```
MoO
moO
MoO

mOo

MOO
    OOM
    MMM
    moO
    moO
    MMM
    mOo
    mOo
    moO
    MMM
    mOo
    MMM
    moO
    moO
    MOO
        MOo
        mOo
        MoO
        moO
    moo
    mOo
    mOo
moo
```

### 6.3 Countdown (99 Bottles)
See `examples/99.cow`

## 7. Implementation Notes

### 7.1 Optimization Opportunities
- Combine consecutive MoO/MOo into single add/subtract
- Pre-compute loop jump targets
- Use direct threading for dispatch

### 7.2 Security Considerations
- Always enforce memory limits
- Limit execution steps to prevent infinite loops
- Validate all input before execution

## 8. Compliance

A conforming implementation must:
1. Accept all 12 defined instructions
2. Implement the memory model as specified
3. Detect and report parse errors
4. Detect and report runtime errors
5. Produce identical output for conforming programs

## 9. Version History

- **1.0**: Original implementation by Sean Heber
- **2.0**: Modernized specification with formal semantics
