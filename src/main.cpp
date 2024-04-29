#include <exception>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "debug/gl_debug.hpp"
#include "debug/glfw_debug.hpp"
#include "kate/interpreter.hpp"
#include "opengl/exceptions.hpp"
#include "opengl/mesh.hpp"
#include "opengl/shader.hpp"
#include "opengl/texture.hpp"
#include "util/io.hpp"
#include "util/options.hpp"

constexpr int window_width = 640;
constexpr int window_height = 320;
constexpr char window_title[] = "Kate";

GLFWwindow *init_opengl(
  int window_width, int window_height, const char window_title[]
);

void processInput(GLFWwindow *window);

int main(int argc, const char *argv[]) {
  utils::OPTIONS options = utils::parse_command_line(argc, argv);

  if (options.err) {
    return options.err;
  } else if (options.called_for_help) {
    return 0;
  }

  GLFWwindow *window = init_opengl(window_width, window_height, window_title);

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
  kate::Interpreter chip8 {};
  chip8.load_rom(rom);

  glClearColor(0.1, 0.1, 0.1, 1.0);
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    processInput(window);

    glClear(GL_COLOR_BUFFER_BIT);

    try {
      chip8.step();
    } catch (kate::interpreter_error &e) {
      std::cerr << e.what() << std::endl;
      glfwSetWindowShouldClose(window, true);
      break;
    }
    // std::cout << chip8.debug_line() << std::endl;

    // load new pixel data
    auto buffer = chip8.get_output_buffer();

    // {
    //   // dump buffer to console for debugging
    //   for (std::size_t i = 0; i < kate::SCR_W;  ++i) {
    //     std::cout << '-';
    //   }
    //
    //   std::cout << std::endl;
    //   for (std::size_t y = 0; y < kate::SCR_H; ++y) {
    //     std::size_t offset = y * kate::SCR_W;
    //     for (std::size_t x = 0; x < kate::SCR_W; ++x) {
    //       std::size_t index = x + offset;
    //
    //       std::cout << (buffer[index] ? '#' : ' ');
    //     }
    //     std::cout << '\n';
    //   }
    // }

    for (std::size_t y = 0; y < kate::SCR_H; ++y) {
      std::size_t offset = y * kate::SCR_W;
      for (std::size_t x = 0; x < kate::SCR_W; ++x) {
        std::size_t index = x + offset;

        // decrement value in display_texture to simulate fading
        if (display_buffer[index] > 0) {
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

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}
