#ifndef LCD_HPP
#define LCD_HPP

#include <functional>
#include <string>

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

struct drawing_device
{
    virtual ~drawing_device() = default;
    virtual void before_frame() = 0;
    virtual void after_frame() = 0;
    virtual void draw_pixel(int x, int y, color c) = 0;
};

// Screen has resolution 960x864
// It can draw 6x6 pixels
// 160 horizontally
// 144 vertivally
class lcd : public drawing_device
{
    GLFWwindow *m_window;
    std::function<void(KEY)> m_on_key_cb;

  public:
    lcd(std::function<void(KEY)> on_key_cb);

    void before_frame();
    void draw_pixel(int x, int y, color c);
    void after_frame();
};

#endif