# Kate: A CHIP-8 Interpreter

## Requirements

- GLFW3: https://www.glfw.org/
- glm: https://github.com/g-truc/glm
- glad: https://glad.dav1d.de/ (Core Profile, Version 4.6)

Arch Linux:
```pacman -S glfw glm```

Note: The generator from the package `glad` uses the newer format which is
incompatible with this program.

---

After unzipping the generated files for glad, the following commands can be ran
to install the files, or they can be put anywhere that is accessible.

```
$ cp -r include/* /usr/local/include/

$ gcc src/glad.c -shared -fPIC -o libglad.so.4.6.0 -Wl,-soname,libglad.so.4.6.0
$ ln -s libglad.so.4.6.0 libglad.so
$ mv libglad.so libglad.so.4.6.0 /usr/local/lib/
```
