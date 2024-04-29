# CHIP-8 Instructions

## Flow Control

### 0NNN : Execute machine language routine (deprecated)
### 1NNN, BNNN, BXNN : jumps

instruction | value
------------|------
1NNN        | `NNN`
BNNN        | `NNN` + `V0`

**Note: Implementation Quirk!**

In later implementation `BNNN` functions differently, treating the highest
nibble as the register index instead of 0.

### 2NNN and 00EE : call and return

Push/pop the current `PC` to/from the stack.

### 3XNN, 4XNN, 5XY0, 9XY0 : conditional jumps

All instructions compare to `VX` and increment `PC` by two if true.

instruction | second | condition
------------|--------|----------
3XNN        | `NN`   | equal
4XNN        | `NN`   | not equal
5XY0        | `VY`   | equal
9XY0        | `VY`   | not equal
EX9E        | `KEY`  | equal
EXA1        | `KEY`  | not equal

## Registers

### 6XNN : move

Moves `NN` to `VX`.

### 7XNN : add

Add `NN` to `VX`.

Note: This does _not_ set any flag on overflow.

### ANNN : set index register

### CXNN : random

A random number is generated, AND'd with `NN`, and stored in `VX`.

### FX07 : read delay timer

Sets `VX` to the current value of the delay timer.

### FX15 : set delay timer

Sets the delay timer to `VX`.

### FX18 : set sound timer

Sets the sound timer to `VX`.

### FX29 : get char

Loads a pointer to the character data for the value of the low nibble of `VX`
into `I`.

### FX33 : bcd

Converts the value of `VX` into three binary-coded decimal digits and stores
them in memory beginning at `I`.


### FX55 : store registers

**Note: This instruction has multiple implementations!**

The values of registers `V0` to `VX` (inclusive) are stored in memory, starting
at address `I`.

The original implementation incremented the `I` register during execution,
modern implementation do not modify `I`.

### FX65 : load registers

**Note: This instruction has multiple implementations!**

The values of registers `V0` to `VX` (inclusive) are retrieved from memory,
starting at address `I`.

The original implementation incremented the `I` register during execution,
modern implementation do not modify `I`.

### FX1E : add to index

**Note: This instruction has multiple implementations!**

Adds `VX` to `I`.

Some implementations have this instruction set `VF` to `1` if the value exceeds
`0x0fff` (maximum value of 12-bit address).

## ALU

These instruction were executed by the ALU of the original hardware, with the
flags being copied to `VF`. Because of this, there is some strange behaviour.

All instruction are in the form `8XY_` where the last nibble determines the
operation performed on `VX` and `VY`. The result is stored in `VX`.

### ---0 : set

### ---1 : bitwise OR

### ---2 : bitwise AND

### ---3 : bitwise XOR

### ---4 : add

This instruction _will_ set the carry flag. `VF` is set to `1` if the result is
larger than 255, otherwise it will set it to `0`.

### ---5 and ---7 : subtract

Both of these instructions store the result in `VX`, but the order is different
for each; `8XY5` performs the operation `VX - VY`, `8XY7`; `VY - VX`.

The carry flag is set for these instructions, however, it is opposite to what
might be expected; A `1` indicates that the result is _not_ negative.

### ---6 and ---E : shift (left, right)

**Note: This instruction has multiple implementations!**

In the original, `VX` was set to `VY` before being shifted, in later variations,
this step was skipped and `VY` is ignored.

In any case, `VX` is shifted by one bit (left, or right) and `VF` is set to the
shifted bit.

## I/O

### 00E0 : Clear screen.

### DXYN : draw

Register settings:

- `I` register points to a location where a `N` pixel tall sprite is located.
- `VX` stores the horizontal offset
- `VY` stores the vertical offset

Each byte of data stores a whole row from to to bottom. Sprites data is XOR'd
to the screen, if any "on" pixels are turned "off" then `VF` is set to `1`,
otherwise `0`.

Drawing coordinates wrap around, e.g. `x == 70` is the same as `x == 7`. If
drawing leaves the screen, it should _not_ wrap.

### FX0A : get key

**Note: This instruction has multiple implementations!**

Stores the value of the current key press to `VX`.

This is a soft-blocking instruction; Execution is not halted completely (timer
still decrement) but the `PC` will be decremented, thereby executing this
instruction until a key _is_ pressed.

Some implementations register on `keypress`, others on `keyrelease`.
