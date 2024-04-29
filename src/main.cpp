#include <exception>
#include <iostream>

#include "kate/interpreter.hpp"
#include "util/io.hpp"
#include "util/options.hpp"

int main(int argc, const char *argv[]) {
  utils::OPTIONS options = utils::parse_command_line(argc, argv);

  if (options.err) {
    return options.err;
  } else if (options.called_for_help) {
    return 0;
  }

  std::vector<std::uint8_t> rom = utils::load_file(options.rom_path);

  kate::Interpreter chip8 {};
  chip8.load_rom(rom);

  while (1) {
    try {
      chip8.step();
    } catch (kate::interpreter_error &e) {
      std::cerr << e.what() << std::endl;
      break;
    }
  }

  return 0;
}
