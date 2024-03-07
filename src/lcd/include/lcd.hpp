#ifndef LCD_HPP
#define LCD_HPP

#include <functional>
#include <common.hpp>
#include <string>

class lcd : public drawing_device
{
  public:
    lcd(std::function<void()> quit_cb);
    void push_pixel(color c);
    void after_frame();

  private:
    void draw_pixel(int x, int y, color c);
};

#endif