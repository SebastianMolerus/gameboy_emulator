#ifndef LCD_HPP
#define LCD_HPP

#include <functional>
#include <common.hpp>
#include <string>

class lcd : public drawing_device
{
  public:
    lcd(std::function<void()> quit_cb, std::function<void()> step_cb = nullptr,
        std::function<void()> continue_cb = nullptr);
    void draw_pixel(int x, int y, color c);
    void push_pixel(color c);
    void after_frame();
    void display_pc(std::string s);

  private:
};

#endif