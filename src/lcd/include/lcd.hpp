#ifndef LCD_HPP
#define LCD_HPP

#include <functional>

enum class KEY
{
    ESC,
    UP,
    DOWN,
    LEFT,
    RIGHT,
    A,
    S
};

struct color
{
    float R{}, G{}, B{};
};

struct GLFWwindow;

// Screen has resolution 960x864
// It can draw 6x6 pixels
// 160 horizontally
// 144 vertivally
class lcd
{
    GLFWwindow *m_window;
    std::function<void(KEY)> m_on_key_cb;

  public:
    lcd(std::function<void(KEY)> on_key_cb);

    void before_frame(color background = {});
    void draw_pixel(int x, int y, color c);
    void after_frame();
};

#endif