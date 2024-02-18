#include <windows.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

#pragma comment(lib, "opengl32.lib")

namespace
{

const unsigned int PIXEL_SIZE = 6;
const unsigned int LCD_SCR_WIDTH = 160 * PIXEL_SIZE;
const unsigned int LCD_SCR_HEIGHT = 144 * PIXEL_SIZE;
const unsigned int DEBUGGER_WIDTH = 300;

glm::mat4 const projection = glm::ortho<float>(0.0f, LCD_SCR_WIDTH, LCD_SCR_HEIGHT, 0.0f, -0.1f, 0.1f);

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

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK lcd_proc(HWND, UINT, WPARAM, LPARAM);

HGLRC rendering_context;
HDC device_context;

PIXELFORMATDESCRIPTOR get_pixelformat()
{
    PIXELFORMATDESCRIPTOR pfd = {sizeof(PIXELFORMATDESCRIPTOR),
                                 1,
                                 PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, // Flags
                                 PFD_TYPE_RGBA, // The kind of framebuffer. RGBA or palette.
                                 32,            // Colordepth of the framebuffer.
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

void draw()
{
    glfwInit();
    ::wglMakeCurrent(device_context, rendering_context);

    if (!gladLoadGL())
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }

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

    while (true)
    {
        // glBegin(GL_TRIANGLES);
        // glColor3f(1.0f, 0.0f, 0.0f);
        // glVertex2f(0.0f, 1.0f);
        // glColor3f(0.0f, 1.0f, 0.0f);
        // glVertex2f(0.87f, -0.5f);
        // glColor3f(0.0f, 0.0f, 1.0f);
        // glVertex2f(-0.87f, -0.5f);
        // glEnd();

        glm::vec3 v{1.0f, 0.0f, 0.0f};
        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::translate(transform, glm::vec3(PIXEL_SIZE * 159, PIXEL_SIZE * 143, 0.0f));
        glUniformMatrix4fv(transform_loc, 1, GL_FALSE, glm::value_ptr(transform));
        glUniform3fv(pixel_color_loc, 1, glm::value_ptr(v));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        transform = glm::mat4(1.0f);
        transform = glm::translate(transform, glm::vec3(PIXEL_SIZE * 0, PIXEL_SIZE * 0, 0.0f));
        glUniformMatrix4fv(transform_loc, 1, GL_FALSE, glm::value_ptr(transform));
        glUniform3fv(pixel_color_loc, 1, glm::value_ptr(v));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        transform = glm::mat4(1.0f);
        transform = glm::translate(transform, glm::vec3(PIXEL_SIZE * 159, PIXEL_SIZE * 0, 0.0f));
        glUniformMatrix4fv(transform_loc, 1, GL_FALSE, glm::value_ptr(transform));
        glUniform3fv(pixel_color_loc, 1, glm::value_ptr(v));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        transform = glm::mat4(1.0f);
        transform = glm::translate(transform, glm::vec3(PIXEL_SIZE * 0, PIXEL_SIZE * 143, 0.0f));
        glUniformMatrix4fv(transform_loc, 1, GL_FALSE, glm::value_ptr(transform));
        glUniform3fv(pixel_color_loc, 1, glm::value_ptr(v));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        ::SwapBuffers(device_context);
    }
}

// int WinMain(__in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in_opt LPSTR lpCmdLine, __in int nShowCmd)
int main()
{
    HINSTANCE hInstance = GetModuleHandle(NULL);

    // Main Window
    WNDCLASS wc{};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
    wc.lpszClassName = "main_window";
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    if (!RegisterClass(&wc))
        return 1;

    RECT lcd_rect;
    lcd_rect.left = 0;
    lcd_rect.top = 0;
    lcd_rect.right = LCD_SCR_WIDTH;
    lcd_rect.bottom = LCD_SCR_HEIGHT;

    // No resieable main window
    auto const main_wnd_style = ((WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME) ^ WS_MAXIMIZEBOX) | WS_VISIBLE;

    ::AdjustWindowRect(&lcd_rect, main_wnd_style, wc.lpszMenuName != nullptr);

    HWND main_hwnd = CreateWindow(wc.lpszClassName, "Gameboy (DMG) Emulator. SM.", main_wnd_style, CW_USEDEFAULT,
                                  CW_USEDEFAULT, lcd_rect.right + DEBUGGER_WIDTH - lcd_rect.left,
                                  lcd_rect.bottom - lcd_rect.top, nullptr, nullptr, hInstance, 0);

    // Opengl Window
    wc.lpfnWndProc = lcd_proc;
    wc.lpszClassName = "lcd_window";
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    if (!RegisterClass(&wc))
        return 1;

    CreateWindow(wc.lpszClassName, nullptr, WS_CHILDWINDOW | WS_VISIBLE, 0, 0, LCD_SCR_WIDTH, LCD_SCR_HEIGHT, main_hwnd,
                 nullptr, hInstance, nullptr);

    MSG msg{};
    while (true)
    {
        if (GetMessage(&msg, NULL, 0, 0))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        draw();
    }

    return 0;
}

LRESULT CALLBACK lcd_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE: {
        // Put child window at right half of main window
        // HWND parent_hwnd = ::GetParent(hWnd);
        // assert(parent_hwnd);
        // RECT parent_rect;
        // BOOL res = ::GetClientRect(parent_hwnd, &parent_rect);
        // assert(res);
        // res = ::SetWindowPos(hWnd, nullptr, parent_rect.right / 2, 0, parent_rect.right / 2, parent_rect.bottom,
        //                      SWP_SHOWWINDOW);
        // assert(res);

        auto pf = get_pixelformat();
        device_context = GetDC(hWnd);
        int let_window_choose_this_pixel_format = ::ChoosePixelFormat(device_context, &pf);
        ::SetPixelFormat(device_context, let_window_choose_this_pixel_format, &pf);
        rendering_context = ::wglCreateContext(device_context);
        return S_OK;
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC dc = BeginPaint(hWnd, &ps);

        SetBkMode(dc, TRANSPARENT);

        TextOut(dc, LCD_SCR_WIDTH + 10, 0, "SIEMANKO", sizeof("SIEMANKO"));

        EndPaint(hWnd, &ps);
        return 0;
    }
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}