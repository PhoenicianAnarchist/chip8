# CHIP-8 Specs


- 4kB ram
- 64 * 32 pixel monochrome display output
- 16-bit stack
- registers
  - PC (16-bit program counter)
  - I (16-bit index register)
  - 8-bit delay timer (decrements @ 60Hz until zero)
  - 8-bit sound timer (decrements @ 60Hz until zero, beep in non-zero)
  - 16 * 8-bit general purpose registers (V0 - VF);
    - VF is also used as a flag register

## Memory

A total of 4kB (4096 bytes) of RAM is available. There is no ROM, program is
stored in RAM and self-modifiable.

Note: CHIP-8 has a 16-bit address bus, but only 12-bits are used.

The original CHIP-8 interpreter was also loaded into RAM from `0x0000` to
`0x01ff` with the program loaded immediately afterwards (at address `0x0200`).
This area can be left blank (except for the font data), but the offset should
be maintained for compatibility with older programs.

## Font

Font data is stored in the RAM as part of the interpreter (commonly found at
address `0x50` to `0x9f`).

The built-in font consists of hexadecimal digits (0 to F) stored as 4*5 pixel
sprites in five bytes (each row of four pixels occupies the highest bits).

## Display

The display is a monochrome 64 * 32 pixel screen, refreshed at 60Hz.

Graphics are drawn to the screen via sprites. Each sprite is 8 pixels wide, and
between 1 and 15 pixels tall, with each row being stored sequentially. Each
pixel is xor'd to the screen, not plainly written.

Note: When a sprite is moved, is it first erased (by drawing it in place)
before being redrawn. This may cause flickering on a modern display. In order
to mitigate this, the screen buffer will not be drawn directly, but drawn via
an intermediate buffer which slowly fades non-lit pixels over time.

## Stack

A 16-bit wide stack (last in, firs out) is used to store return addresses.
Early interpreters reserved 32 bytes (16 addresses) of RAM for this purpose.

## Timers

There are two 8-bit timers which decrease at 60Hz until reaching 0. One of the
timers will cause a tone as long as its value is non-zero.

## Keypad

A 4 by 4 keypad is used as input, the following keypad is standard but there
should be provisions for alternate layouts as required.

```
1 2 3 C
4 5 6 D
7 8 9 E
A 0 B F
```

Most interpreters map this to the left side of the keyboard as such:

```
1 2 3 4
q w e r
a s d f
z x c v
```

## Fetch / Decode / Execute Loop

### Timing

Various older hardware ran at different speeds from 1MHz to 4MHz. A standard of
around 700 instructions per second works well for most programs.

The speed should be configurable.

### Fetch

Each instruction is two bytes wide.

```c++

// read instruction
instruction <<= 8;
instruction |= ram[pc];
++pc;
instruction <<= 8;
instruction |= ram[pc];
++pc;

```

### Decode

Some similar instructions are grouped by the first nibble, with the other three
being operands/data.

Instructions will be written with placeholders, e.g. `DXYN`, `BXNN`, etc. where
`X` and `Y` are register indices, `N` and `NN` and `NNN` are numbers of various
widths.

Note: This is the reason why addresses are only 12-bits wide.

### Execute

Each instruction is executed after decoding. This will likely be combined with
the Decode stage in a `switch` statement.
