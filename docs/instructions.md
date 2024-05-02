# CHIP-8 Instructions

## Flow Control

### 0NNN : Execute machine language routine (deprecated)

### 2NNN and 00EE : call and return

Push/pop the current `PC` to/from the stack.

### 1NNN, BNNN : unconditional jumps

instruction | value
------------|------
1NNN        | `NNN`
BNNN        | `NNN` + `V0`

**Note: Implementation Quirk!**

In later implementation `BNNN` functions differently, treating the highest
nibble as the register index instead of 0.

### 3XNN, 4XNN, 5XY0, 9XY0 : conditional jumps

All instructions compare to `vX` and increment `PC` by two if true.

instruction | second | condition
------------|--------|----------
3XNN        | `NN`   | equal
4XNN        | `NN`   | not equal
5XY0        | `vY`   | equal
9XY0        | `vY`   | not equal
EX9E        | `KEY`  | equal
EXA1        | `KEY`  | not equal

## Registers

### 6XNN : move

Moves `NN` to `vX`.

### 7XNN : add

Add `NN` to `vX`.

Note: This does _not_ set any flag on overflow.

### ANNN : set index register

Moves `NNN` to `IR`.

### CXNN : random

A random number is generated, AND'd with `NN`, and stored in `vX`.

### FX07 : read delay timer

Sets `vX` to the current value of the delay timer.

### FX15 : set delay timer

Sets the delay timer to `vX`.

### FX18 : set sound timer

Sets the sound timer to `vX`.

### FX29 : get char

Loads a pointer to the character data for the value of the low nibble of `vX`
into `IR`.

### FX33 : bcd

Converts the value of `vX` into three binary-coded decimal digits and stores
them in memory beginning at `IR`.

### FX55 : store registers

The values of registers `V0` to `vX` (inclusive) are stored in memory, starting
at address `IR`.

**Note: Implementation Quirk!**

The original implementation incremented the `IR` register during execution,
modern implementation do not modify `IR`.

### FX65 : load registers

The values of registers `V0` to `vX` (inclusive) are retrieved from memory,
starting at address `IR`.

**Note: Implementation Quirk!**

The original implementation incremented the `IR` register during execution,
modern implementation do not modify `IR`.

### FX1E : add to index

Adds `vX` to `IR`.

**Note: Implementation Quirk!**

Some implementations have this instruction set `vF` to `1` if the value exceeds
`0x0fff` (maximum value of 12-bit address).

## ALU

These instruction were executed by the ALU of the original hardware, with the
flags being copied to `vF`. Because of this, there is some strange behaviour.

All instruction are in the form `8XY_` where the last nibble determines the
operation performed on `vX` and `vY`. The result is stored in `vX`.

### ---0 : set

### ---1 : bitwise OR

**Note: Implementation Quirk!**

Some implementations reset the `vF` register to 0.

### ---2 : bitwise AND

**Note: Implementation Quirk!**

Some implementations reset the `vF` register to 0.

### ---3 : bitwise XOR

**Note: Implementation Quirk!**

Some implementations reset the `vF` register to 0.

### ---4 : add

This instruction _will_ set the carry flag. `vF` is set to `1` if the result is
larger than 255, otherwise it will set it to `0`.

### ---5 and ---7 : subtract

Both of these instructions store the result in `vX`, but the order is different
for each; `8XY5` performs the operation `vX - vY`, `8XY7`; `vY - vX`.

The carry flag is set for these instructions, however, it is opposite to what
might be expected; A `1` indicates that the result is _not_ negative.

### ---6 and ---E : shift (left, right)

**Note: Implementation Quirk!**

In the original, `vX` was set to `vY` before being shifted, in later variations,
this step was skipped and `vY` is ignored.

In any case, `vX` is shifted by one bit (left, or right) and `vF` is set to the
shifted bit.

## I/O

### 00E0 : Clear screen.

### DXYN : draw

Register settings:

- `IR` register points to a location where a `N` pixel tall sprite is located.
- `vX` stores the horizontal offset
- `vY` stores the vertical offset

Each byte of data stores a whole row from to to bottom. Sprites data is XOR'd
to the screen, if any "on" pixels are turned "off" then `vF` is set to `1`,
otherwise `0`.

Drawing coordinates wrap around, e.g. `x == 70` is the same as `x == 7`. If
drawing leaves the screen, it should _not_ wrap.

Drawing only takes place during vblank, limiting drawing to 60Hz.

**Note: Implementation Quirk!**

Sprite clipping, and vblank wait, differ between implementations.

### FX0A : get key

Stores the value of the current key press to `vX`.

This is a soft-blocking instruction; Execution is not halted completely (timer
still decrement) but the `PC` will be decremented, thereby executing this
instruction until a key _is_ pressed.

**Note: Implementation Quirk!**

Some implementations register on `keypress`, others on `keyrelease`.
