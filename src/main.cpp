#include <exception>
#include <iostream>
#include <map>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "debug/gl_debug.hpp"
#include "debug/glfw_debug.hpp"
#include "kate/interpreter.hpp"
#include "opengl/exceptions.hpp"
#include "opengl/input.hpp"
#include "opengl/mesh.hpp"
#include "opengl/shader.hpp"
#include "opengl/texture.hpp"
#include "util/image.hpp"
#include "util/io.hpp"
#include "util/options.hpp"
#include "util/timer.hpp"

constexpr int window_width = 640;
constexpr int window_height = 320;
constexpr char window_title[] = "Kate";

GLFWwindow *init_opengl(
  int window_width, int window_height, const char window_title[]
);

void processInput(
  GLFWwindow *window, std::map<int, openglwrapper::KeyState> &key_states,
  kate::Interpreter &interpreter
);

void render_console(const std::vector<std::uint8_t> &buffer);
void render_opengl(
  const std::vector<std::uint8_t> &buffer,
  GLFWwindow *window, std::vector<std::uint8_t> &display_buffer,
  openglwrapper::Mesh &simple_mesh, openglwrapper::Shader &main_shader,
  openglwrapper::Texture &display_texture
);

// TODO: allow rebinding at runtime, and store user preferences.
// <internal number, GLFW name>
std::map<int, int> key_map = {
  {0x00, GLFW_KEY_X},
  {0x01, GLFW_KEY_1},
  {0x02, GLFW_KEY_2},
  {0x03, GLFW_KEY_3},
  {0x04, GLFW_KEY_Q},
  {0x05, GLFW_KEY_W},
  {0x06, GLFW_KEY_E},
  {0x07, GLFW_KEY_A},
  {0x08, GLFW_KEY_S},
  {0x09, GLFW_KEY_D},
  {0x0a, GLFW_KEY_Z},
  {0x0b, GLFW_KEY_C},
  {0x0c, GLFW_KEY_4},
  {0x0d, GLFW_KEY_R},
  {0x0e, GLFW_KEY_F},
  {0x0f, GLFW_KEY_V}
};

int main(int argc, const char *argv[]) {
  utils::OPTIONS options = utils::parse_command_line(argc, argv);

  if (options.err) {
    return options.err;
  } else if (options.called_for_help) {
    return 0;
  }

  GLFWwindow *window = init_opengl(window_width, window_height, window_title);
  glfwSetKeyCallback(window, openglwrapper::key_callback);

  if (window == nullptr) {
    return 1;
  }

  // mesh
  std::vector<openglwrapper::Vertex> vertex_data {
    {{-1.0, -1.0}, {0.0, 1.0, 1.0}, {0.0, 1.0}},
    {{ 1.0, -1.0}, {1.0, 0.0, 1.0}, {1.0, 1.0}},
    {{ 1.0,  1.0}, {0.0, 1.0, 1.0}, {1.0, 0.0}},
    {{-1.0,  1.0}, {1.0, 0.0, 1.0}, {0.0, 0.0}}
  };

  std::vector<GLuint> index_data {
    0, 1, 2,
    0, 2, 3
  };

  openglwrapper::Mesh simple_mesh(vertex_data, index_data);

  // shader
  std::filesystem::path vshader_path("./data/shaders/vertex.glsl");
  std::filesystem::path fshader_path("./data/shaders/fragment.glsl");

  openglwrapper::Shader main_shader;
  try {
    main_shader = {vshader_path, fshader_path};
  } catch (openglwrapper::shader_error &e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }

  // texture
  std::vector<std::uint8_t> display_buffer;
  display_buffer.resize(kate::SCR_W * kate::SCR_H);
  std::fill(display_buffer.begin(), display_buffer.end(), '0');
  openglwrapper::Texture display_texture(
    display_buffer, kate::SCR_W, kate::SCR_H, 1
  );

  std::vector<std::uint8_t> rom = utils::read_binary(options.rom_path);

  for (std::uint8_t i = 0; i <= 0xf; ++i) {
    openglwrapper::key_states[key_map[i]].is_pressed = false;
    openglwrapper::key_states[key_map[i]].is_released = false;
    openglwrapper::key_states[key_map[i]].is_handled = true;
  }

  kate::Interpreter chip8 {};
  chip8.load_rom(rom);

  utils::Clock clock;
  utils::seconds timer_begin {0.0};
  utils::seconds timer_end {0.0};

  utils::seconds system_timestep {1.0 / kate::instructions_per_second};
  utils::seconds system_accumulator {0.0};

  utils::seconds render_timestep {1.0 / kate::display_refresh_rate};
  utils::seconds render_accumulator {0.0};

  glClearColor(0.1, 0.1, 0.1, 1.0);
  glfwPollEvents();
  processInput(window, openglwrapper::key_states, chip8);
  while (!glfwWindowShouldClose(window)) {
    timer_begin = clock.get();

    if (system_accumulator >= system_timestep) {
      if (system_accumulator >= (system_timestep * 2)) {
        std::cout << "WARNING: system loop running slow! ";
        std::cout << system_accumulator.count() << std::endl;
      }

      glfwPollEvents();
      processInput(window, openglwrapper::key_states, chip8);

      try {
        chip8.step();
      } catch (kate::interpreter_error &e) {
        std::cerr << e.what() << std::endl;
        glfwSetWindowShouldClose(window, true);
        break;
      }
      system_accumulator -= system_timestep;
    }

    if (render_accumulator >= render_timestep) {
      if (render_accumulator >= (render_timestep * 2)) {
        std::cout << "WARNING: render loop running slow! ";
        std::cout << system_accumulator.count() << std::endl;
      }

      chip8.decrement_timers();

      glClear(GL_COLOR_BUFFER_BIT);

      const std::vector<std::uint8_t> &buffer = chip8.get_output_buffer();
      render_opengl(
        buffer, window, display_buffer,
        simple_mesh, main_shader,display_texture
      );

      render_accumulator -= render_timestep;
    }

    timer_end = clock.get();
    system_accumulator += (timer_end - timer_begin);
    render_accumulator += (timer_end - timer_begin);
  }

  glfwTerminate();
  return 0;
}

GLFWwindow *init_opengl(
  int window_width, int window_height, const char window_title[]
) {
  debug::enableDebugGLFW();

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

  GLFWwindow* window = glfwCreateWindow(
    window_width, window_height, window_title, nullptr, nullptr
  );
  if (window == nullptr) {
    std::cerr << "Failed to create window" << std::endl;
    glfwTerminate();
    return nullptr;
  }

  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to load opengl functions" << std::endl;
    glfwTerminate();
    return nullptr;
  }

  glViewport(0, 0, window_width, window_height);

  // enable debugging
  if (!debug::enableDebug()) {
    std::cerr << "GL Debug Context not available." << std::endl;
  }

  return window;
}

void processInput(
  GLFWwindow *window, std::map<int, openglwrapper::KeyState> &key_states,
  kate::Interpreter &interpreter
) {
  if (key_states[GLFW_KEY_ESCAPE].is_pressed) {
    glfwSetWindowShouldClose(window, true);
  }

  if (key_states[GLFW_KEY_P].is_pressed && !key_states[GLFW_KEY_P].is_handled) {
    std::filesystem::path directory = "output";
    std::string fn = interpreter.debug_filename();

    utils::save_pnm(
      interpreter.get_output_buffer(), kate::SCR_W, kate::SCR_H,
      directory, fn, utils::PGM_RAW, true, "1"
    );

    std::cout << "Saved file " << fn << " to " << directory << std::endl;

    key_states[GLFW_KEY_P].is_handled = true;
  }

  for (int k = 0; k <= 0xf; ++k) {
    if (!key_states[key_map[k]].is_handled) {
      if (key_states[key_map[k]].is_pressed) {
        interpreter.keypress(k);
      } else if (key_states[key_map[k]].is_released) {
        interpreter.keyrelease(k);
      }

      key_states[key_map[k]].is_handled = true;
    }
  }
}

void render_console(const std::vector<std::uint8_t> &buffer) {
  // dump buffer to console for debugging
  for (std::size_t i = 0; i < kate::SCR_W;  ++i) {
    std::cout << '-';
  }

  std::cout << std::endl;
  for (std::size_t y = 0; y < kate::SCR_H; ++y) {
    std::size_t offset = y * kate::SCR_W;
    for (std::size_t x = 0; x < kate::SCR_W; ++x) {
      std::size_t index = x + offset;

      std::cout << (buffer[index] ? '#' : ' ');
    }
    std::cout << '\n';
  }
}

void render_opengl(
  const std::vector<std::uint8_t> &buffer,
  GLFWwindow *window, std::vector<std::uint8_t> &display_buffer,
  openglwrapper::Mesh &simple_mesh, openglwrapper::Shader &main_shader,
  openglwrapper::Texture &display_texture
) {
  for (std::size_t y = 0; y < kate::SCR_H; ++y) {
    std::size_t offset = y * kate::SCR_W;
    for (std::size_t x = 0; x < kate::SCR_W; ++x) {
      std::size_t index = x + offset;

      // decrement value in display_texture to simulate fading
      if (display_buffer[index] >= kate::FADERATE) {
        display_buffer[index] -= kate::FADERATE;
      }

      // set new pixels at max brightness
      if (buffer[index] == 1) {
        display_buffer[index] = 255;
      }
    }
  }
  // update display texture
  display_texture.update(display_buffer, kate::SCR_W, kate::SCR_H);

  main_shader.use();
  display_texture.bind();
  simple_mesh.draw();

  glfwSwapBuffers(window);
}
