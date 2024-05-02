# Kate Documentation

This program consists of two parts, the interpreter core, and a minimal OpenGL
renderer which copies the display buffer to a texture and draws that to the
screen.

## General Operation

The interpreter begins by clearing all registers, flags, and memory, before
copying the character sprite data into ram at `0x0050`.

The user then provides the program (via the `load_rom()` function) which is
copied into ram at `0x0200`.

## Main Loop

There are two parts to the main loop:

### System

The "system" runs at approximately 700 instructions/second (this value is
configurable), this loop is where the `.step()` function is called. This will
perform one fetch/decode/execute cycle, and increment the cycle counter.

This loop is also when input is processed. The `.keypress()` and
`.keyrelease()` functions are used to update the internal state of the
interpreter.

### Render

The "render" loop runs at 60 updates/second. Two functions must be called in
this loop; `.decrement_timers()` and `.vblank_trigger()`.

The output data is then retrieved via `.get_output_buffer()` and passed to the
renderer.

## Further Information

Most instructions are handled directly, but the `8000` set are sent to the
"ALU" and the `DXYN` instruction is passed to the "GPU". Any draw calls will
be buffered and only executed during "vblank", limiting the program to drawing
at a rate of 60Hz.

The `FX0A` instruction is a "blocking" instruction in that execution will not
advance until a key is pressed (and released).

## Quirks

Different implementations have some differences in their behaviour. This one
follows the original COSMAC VIP CHIP-8 variant.

- Instructions `8XY1`, `8XY2`, and `8XY3` reset the flags register to zero
- Instructions `FX55` and `FX65` increment the index register
- Sprite drawing waits for the vblank interupt
- Sprite coordinates wrap, but sprites are clipped
- Instructions `8XY6` and `8XYE` use the `vY` register as expected
- The `BNNN` instruction uses register `v0` as the additional offset
