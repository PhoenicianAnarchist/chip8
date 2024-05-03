# Kate: A CHIP8 Interpreter

A CHIP8 interpreter written in C++ using OpenGL.

## Usage

`kate --rom <path/to/ch8/file>`

The keys are mapped as follows:

CHIP8 Keypad:
```
1 2 3 C
4 5 6 D
7 8 9 E
A 0 B F
```

QWERTY Keyboard:
```
1 2 3 4
q w e r
a s d f
z x c v
```

These keys can be rebound in the file `src/kate/config.hpp` if necessary.

## Requirements

- GLFW3: https://www.glfw.org/
- glm: https://github.com/g-truc/glm
- glad: https://gen.glad.sh/ (Core Profile, Version 4.6)

Arch Linux:
```pacman -S glfw glm glad```

---

### Generating glad files

The glad loader files must be generated, either by the online generator, or by
a local installation.

#### Local generator

`$ mkdir /tmp/glad`
`$ cd /tmp/glad`
`$ glad --api gl:core=4.6 --out-path /tmp/glad`

Then the following commands are run, replacing `src/glad.c` with `src/gl.c`

#### Online generator

The following settings should be selected:
- generator: c/c++
- gl: version 4.6, core
- extensions: GL_KHR_debug

After unzipping the generated files, the following commands can be ran to
compile and install the files to `/usr/local/`, or they can be put anywhere
that is accessible.

```
$ cp -r include/* /usr/local/include/

$ gcc src/glad.c -shared -fPIC -o libglad.so.4.6.0 -Wl,-soname,libglad.so.4.6.0
$ ln -s libglad.so.4.6.0 libglad.so
$ mv libglad.so libglad.so.4.6.0 /usr/local/lib/
```

---

## TODO

I wrote this CHIP8 interpreter as a first step into emulator development and
other programming experiments.

Going forward there are several things that I would like to change or add:

### better documentation / cleaner code

I put this together pretty quickly, with most of the OpenGL code being mashed
together from various other old projects. I didn't really intend to publish
this code, so I didn't put much though into the design.

### breaking up the monolith

As it is, all of the interpreter code is located within one class, I would like
to break this up into dedicated chunks such that they may be more applicable to
future emulation projects.

This will likely consist of:

- the base "system" which handles basic I/O and ties everything together
- a processor
- memory controller (basic in this case, but extensible)
- dedicated ALU for `8000` instructions
- graphics processor which handles the draw calls

### debugging tools

I would like to add some debugging tools:

- memory viewer
- status indicators
- instruction history
- real-time disassembler

These will (hopefully) each be contained in their own dedicated window which
can be opened/closed by the user as needed. Some of these tools will need to be
tied into the implementation, but others could be standalone programs.

### variant support

There are many different variants, each with slightly different behaviour and
some with more functionality (colours, higher resolution, etc).

It should be easier to switch between variants in a modular system.

### assembler

Not strictly part of _this_ project, but it would be easier to write CHIP8
programs if I had some kind of assembler. It is also a good excuse to try and
write an assembler.

Perhaps I could even try a basic compiler/linker too, but that might not happen
any time soon.

### libraries / submodules

The interpreter/renderer could be repackaged as a library, or maybe a git
submodule. This probably wont serve a practical purpose, but simple a learning
exercise.

### windows version

I would like to make this program compatible with windows too, but I would
probably need to look into a different build system (CMake?).

## References used during development

- [System specifications and instruction set][1]
- [Test programs][2]


[1]: >https://tobiasvl.github.io/blog/write-a-chip-8-emulator/>
[2]: <https://github.com/Timendus/chip8-test-suite>
