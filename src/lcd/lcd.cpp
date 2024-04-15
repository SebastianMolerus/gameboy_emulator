#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <windows.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <lcd.hpp>
#include <sstream>
#include <string>

#pragma comment(lib, "opengl32.lib")

namespace
{
HGLRC rendering_context;
HDC device_context;

std::function<void()> quit_button_cb;
std::function<void(key_action, key)> keyboard_button_cb;

const unsigned int PIXEL_SIZE = 6;
const unsigned int SCR_WIDTH = 160 * PIXEL_SIZE;
const unsigned int SCR_HEIGHT = 144 * PIXEL_SIZE;
glm::mat4 const projection = glm::ortho<float>(0.0f, SCR_WIDTH, SCR_HEIGHT, 0.0f, -0.1f, 0.1f);

PIXELFORMATDESCRIPTOR get_pixelformat()
{
    PIXELFORMATDESCRIPTOR pfd = {sizeof(PIXELFORMATDESCRIPTOR),
                                 1,
                                 PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, // Flags
                                 PFD_TYPE_RGBA,                                              // The kind of framebuffer. RGBA or palette.
                                 32,                                                         // Colordepth of the framebuffer.
                                 0,
                                 0,
                                 0,
                                 0,
                                 0,
                                 0,
                                 0,
                                 0,
                                 0,
                                 0,
                                 0,
                                 0,
                                 0,
                                 24, // Number of bits for the depthbuffer
                                 8,  // Number of bits for the stencilbuffer
                                 0,  // Number of Aux buffers in the framebuffer.
                                 PFD_MAIN_PLANE,
                                 0,
                                 0,
                                 0,
                                 0};
    return pfd;
}

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
    PIXEL_SIZE, PIXEL_SIZE, 0.0f, // top right
    PIXEL_SIZE, 0.f,        0.0f, // bottom right
    0.f,        0.f,        0.0f, // bottom left
    0.f,        PIXEL_SIZE, 0.0f  // top left
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

LRESULT CALLBACK main_window_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_ESCAPE:
            ::PostQuitMessage(0);
            break;
        case VK_LEFT:
            std::invoke(keyboard_button_cb, key_action::down, key::LEFT);
            break;
        case VK_RIGHT:
            std::invoke(keyboard_button_cb, key_action::down, key::RIGHT);
            break;
        case VK_UP:
            std::invoke(keyboard_button_cb, key_action::down, key::UP);
            break;
        case VK_DOWN:
            std::invoke(keyboard_button_cb, key_action::down, key::DOWN);
            break;
        case 0x41: // A
            std::invoke(keyboard_button_cb, key_action::down, key::A);
            break;
        case 0x42: // B
            std::invoke(keyboard_button_cb, key_action::down, key::B);
            break;
        case 0x51: // Q
            std::invoke(keyboard_button_cb, key_action::down, key::START);
            break;
        case 0x57: // W
            std::invoke(keyboard_button_cb, key_action::down, key::SELECT);
            break;
        }
        return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_KEYUP:
        switch (wParam)
        {
        case VK_LEFT:
            std::invoke(keyboard_button_cb, key_action::up, key::LEFT);
            break;
        case VK_RIGHT:
            std::invoke(keyboard_button_cb, key_action::up, key::RIGHT);
            break;
        case VK_UP:
            std::invoke(keyboard_button_cb, key_action::up, key::UP);
            break;
        case VK_DOWN:
            std::invoke(keyboard_button_cb, key_action::up, key::DOWN);
            break;
        case 0x41: // A
            std::invoke(keyboard_button_cb, key_action::up, key::A);
            break;
        case 0x42: // B
            std::invoke(keyboard_button_cb, key_action::up, key::B);
            break;
        case 0x51: // Q
            std::invoke(keyboard_button_cb, key_action::up, key::START);
            break;
        case 0x57: // Q
            std::invoke(keyboard_button_cb, key_action::up, key::SELECT);
            break;
        }
        return DefWindowProc(hWnd, message, wParam, lParam);
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}

void init_opengl_components()
{
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

LRESULT CALLBACK opengl_window_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE: {
        auto pf = get_pixelformat();
        device_context = GetDC(hWnd);
        int let_window_choose_this_pixel_format = ::ChoosePixelFormat(device_context, &pf);
        ::SetPixelFormat(device_context, let_window_choose_this_pixel_format, &pf);
        rendering_context = ::wglCreateContext(device_context);

        glfwInit();
        ::wglMakeCurrent(device_context, rendering_context);
        glfwSwapInterval(0);

        if (!gladLoadGL())
            throw std::runtime_error("Failed to initialzie Glad\n");

        init_opengl_components();

        return 0;
    }
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}

lcd::lcd(std::function<void()> quit_cb, std::function<void(key_action, key)> keyboard_cb)
{
    quit_button_cb = quit_cb;
    keyboard_button_cb = keyboard_cb;
    HINSTANCE hInstance = GetModuleHandle(NULL);

    // ******************************************
    //              Main Window
    // ******************************************
    WNDCLASS wc{};
    wc.lpfnWndProc = main_window_proc;
    wc.hInstance = hInstance;
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszClassName = "main_window";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    if (!RegisterClass(&wc))
        throw std::runtime_error("Cannot register main window\n");

    RECT lcd_rect;
    lcd_rect.left = 0;
    lcd_rect.top = 0;
    lcd_rect.right = SCR_WIDTH;
    lcd_rect.bottom = SCR_HEIGHT;

    // No resieable main window
    auto const main_wnd_style = ((WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME) ^ WS_MAXIMIZEBOX) | WS_VISIBLE;

    ::AdjustWindowRect(&lcd_rect, main_wnd_style, wc.lpszMenuName != nullptr);

    HWND main_hwnd = CreateWindow(wc.lpszClassName, "Gameboy (DMG) Emulator. SM.", main_wnd_style, CW_USEDEFAULT, CW_USEDEFAULT,
                                  lcd_rect.right - lcd_rect.left, lcd_rect.bottom - lcd_rect.top, nullptr, nullptr, hInstance, 0);

    // ******************************************
    //              Opengl Window
    // ******************************************
    wc.lpfnWndProc = opengl_window_proc;
    wc.lpszClassName = "opengl_window";
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    if (!RegisterClass(&wc))
        throw std::runtime_error("Cannot register opengl window\n");

    CreateWindow(wc.lpszClassName, nullptr, WS_CHILDWINDOW | WS_VISIBLE, 0, 0, SCR_WIDTH, SCR_HEIGHT, main_hwnd, nullptr, hInstance, nullptr);
}

void lcd::draw_pixel(int x, int y, color c)
{
    glm::vec3 v{c.R, c.G, c.B};
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, glm::vec3(PIXEL_SIZE * x, PIXEL_SIZE * y, 0.0f));
    glUniformMatrix4fv(transform_loc, 1, GL_FALSE, glm::value_ptr(transform));
    glUniform3fv(pixel_color_loc, 1, glm::value_ptr(v));
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void lcd::push_pixel(color c)
{
    static int x{};
    static int y{};

    draw_pixel(x, y, c);
    ++x;
    if (x == 160)
    {
        x = 0;
        ++y;
        if (y == 144)
            y = 0;
    }
}

void lcd::after_frame()
{
    MSG msg{};
    while (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
            quit_button_cb();
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }
    ::SwapBuffers(device_context);
}