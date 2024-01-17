#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <lcd.hpp>

namespace
{

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// Pixel 6x6
const unsigned int SCR_WIDTH = 960;  // 160 * 6
const unsigned int SCR_HEIGHT = 864; // 144 * 6
glm::mat4 const projection = glm::ortho<float>(0.0f, SCR_WIDTH, SCR_HEIGHT, 0.0f, -0.1f, 0.1f);

const char *vertexShaderSource = "#version 330 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "uniform mat4 transform;\n"
                                 "uniform mat4 projection;\n"
                                 "void main()\n"
                                 "{\n"
                                 "   gl_Position = projection * transform * vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                 "}\0";
const char *fragmentShaderSource = "#version 330 core\n"
                                   "uniform vec3 pixel_color;\n"
                                   "out vec4 FragColor;\n"
                                   "void main()\n"
                                   "{\n"
                                   "   FragColor = vec4(pixel_color, 1.0f);\n"
                                   "}\n\0";

float const vertices[] = {
    6.f, 6.f, 0.0f, // top right
    6.f, 0.f, 0.0f, // bottom right
    0.f, 0.f, 0.0f, // bottom left
    0.f, 6.f, 0.0f  // top left
};
unsigned int const indices[] = {
    // note that we start from 0!
    0, 1, 3, // first Triangle
    1, 2, 3  // second Triangle
};

int projection_loc{};
int transform_loc{};
int pixel_color_loc{};

void set_uniforms_locations(unsigned shared_program)
{
    projection_loc = glGetUniformLocation(shared_program, "projection");
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, glm::value_ptr(projection));
    transform_loc = glGetUniformLocation(shared_program, "transform");
    pixel_color_loc = glGetUniformLocation(shared_program, "pixel_color");
}

} // namespace

lcd::lcd(std::function<void(KEY)> on_key_cb) : m_on_key_cb{on_key_cb}
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    m_window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Gameboy (DMG) emulator, press ESC to exit.", NULL, NULL);
    if (m_window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
    }
    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(0);
    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }

    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(shaderProgram);

    set_uniforms_locations(shaderProgram);
}

void lcd::before_frame(color background)
{
    if (glfwGetKey(m_window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        m_on_key_cb(KEY::ESC);
    if (glfwGetKey(m_window, GLFW_KEY_UP) == GLFW_PRESS)
        m_on_key_cb(KEY::UP);
    if (glfwGetKey(m_window, GLFW_KEY_DOWN) == GLFW_PRESS)
        m_on_key_cb(KEY::DOWN);
    if (glfwGetKey(m_window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        m_on_key_cb(KEY::RIGHT);
    if (glfwGetKey(m_window, GLFW_KEY_LEFT) == GLFW_PRESS)
        m_on_key_cb(KEY::LEFT);
    if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS)
        m_on_key_cb(KEY::A);
    if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS)
        m_on_key_cb(KEY::S);

    glClearColor(background.R, background.G, background.B, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void lcd::draw_pixel(int x, int y, color c)
{
    assert(x >= 0 && x <= 159);
    assert(y >= 0 && y <= 143);
    glm::vec3 v{c.R, c.G, c.B};
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, glm::vec3(6.0f * x, 6.0f * y, 0.0f));
    glUniformMatrix4fv(transform_loc, 1, GL_FALSE, glm::value_ptr(transform));
    glUniform3fv(pixel_color_loc, 1, glm::value_ptr(v));
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void lcd::after_frame()
{
    glfwSwapBuffers(m_window);
    glfwPollEvents();
}
