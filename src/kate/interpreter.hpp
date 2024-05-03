#ifndef __KATE_CHIP8_INTERPRETER__
#define __KATE_CHIP8_INTERPRETER__

#include <array>
#include <exception>
#include <random>
#include <string>
#include <vector>

#include <cstdint>

#include "config.hpp"
#include "memory_controller.hpp"

namespace kate {
  class interpreter_error : public std::exception {
  public:
    explicit interpreter_error(const std::string &msg);
    explicit interpreter_error(const char *msg);
    virtual ~interpreter_error() noexcept;

    virtual const char *what() const noexcept;
  protected:
    std::string msg;
  };

  class invalid_address : public interpreter_error {
  public:
    explicit invalid_address(const std::string& msg);
    explicit invalid_address(const char* msg);
  };

  class invalid_instruction : public interpreter_error {
  public:
    explicit invalid_instruction(const std::string& msg);
    explicit invalid_instruction(const char* msg);
  };

  class stack_overflow : public interpreter_error {
  public:
    explicit stack_overflow(const std::string& msg);
    explicit stack_overflow(const char* msg);
  };

  enum INSTRUCTION {
    NOP         = 0x00,
    CLEAR       = 0xe0,
    RET         = 0xee,
    JMP         = 0x01,
    CALL        = 0x02,
    SKIP_EQ_IMM = 0x03,
    SKIP_NE_IMM = 0x04,
    SKIP_EQ_REG = 0x05,
    SKIP_NE_REG = 0x09,
    MOV         = 0x06,
    ADD         = 0x07,
    ALU         = 0x08,
    LDI         = 0x0a,
    JMP_OFF     = 0x0b,
    RANDOM      = 0x0c,
    DRAW        = 0x0d,
    KEY_EQ      = 0x9e,
    KEY_NE      = 0xa1,
    MISC        = 0x0f
  };

  enum class ALU_OP {
    MOV   = 0x00,
    OR    = 0x01,
    AND   = 0x02,
    XOR   = 0x03,
    ADD   = 0x04,
    SUB   = 0x05,
    RSUB  = 0x07,
    SHR   = 0x06,
    SHL   = 0x0e
  };

  enum class MISC_OP {
    GET_DT    = 0x07,
    GET_KEY   = 0x0A,
    SET_DT    = 0x15,
    SET_ST    = 0x18,
    GET_CHAR  = 0x29,
    ADD_IR    = 0x1E,
    BCD       = 0x33,
    STORE_REG = 0x55,
    LOAD_REG  = 0x65
  };

  enum class KEY_EVENT {
    NONE,
    PRESS,
    RELEASE
  };

  std::string decode_INSTRUCTION(INSTRUCTION inst);
  std::string hex_string(std::size_t i, std::size_t w, bool b=true);

  // will be able to hold data for any kind of instruction
  struct Instruction {
    std::uint16_t raw;

    INSTRUCTION inst;
    std::uint8_t x;
    std::uint8_t y;
    std::uint16_t n;
  };

  class Interpreter {
  public:
    Interpreter();

    void reset();
    void load_rom(const std::vector<std::uint8_t> &rom);
    const std::vector<std::uint8_t> &get_output_buffer() const;
    std::uint8_t get_sound_timer() const;
    std::string crashdump(const std::string &msg) const;
    std::string debug_line() const;
    std::string debug_filename() const;

    void decrement_timers();
    void keypress(std::uint8_t k);
    void keyrelease(std::uint8_t k);
    std::uint8_t random_uint8();

    void step();
    void vblank_trigger();

    void fetch();
    void decode();
    void execute();

    void _8XYN();
    void _DXYN();
    void _FXNN();

  private:
    std::default_random_engine e;
    std::uniform_int_distribution<int> dist;

    // std::array<std::uint8_t, 0x4000> ram;
    MemoryController memory;
    std::array<std::uint8_t, 16> registers;
    std::array<std::uint8_t, 16> key_states;
    std::array<std::uint16_t, 16> stack;
    std::uint16_t program_counter;
    std::uint16_t stack_pointer;
    std::uint16_t index_register;
    std::uint8_t delay_timer;
    std::uint8_t sound_timer;

    // each pixel is stored as a whole byte.
    // this is inneficcient but inconsequential.
    std::vector<std::uint8_t> output_buffer;
    bool is_blocking;
    bool is_vblank;
    Instruction cur_inst;
    std::uint64_t cycle_counter;
    std::uint16_t prev_program_counter;
    std::pair<int, KEY_EVENT> last_key_event;
  };
}

#endif // __KATE_CHIP8_INTERPRETER__
