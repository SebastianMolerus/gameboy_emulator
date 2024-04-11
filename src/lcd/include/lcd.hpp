#ifndef LCD_HPP
#define LCD_HPP

#include <functional>
#include <common.hpp>
#include <string>

enum class key
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
    START,
    SELECT,
    A,
    B
};

enum class key_action
{
    up,
    down
};

class lcd : public drawing_device
{
  public:
    lcd(std::function<void()> quit_cb, std::function<void(key_action, key)> keyboard_cb);
    void push_pixel(color c);
    void after_frame();

  private:
    void draw_pixel(int x, int y, color c);
};

#endif