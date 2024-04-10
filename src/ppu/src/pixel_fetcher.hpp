#ifndef PIXEL_FETCHER_HPP
#define PIXEL_FETCHER_HPP

#include <common.hpp>
#include <optional>

class pixel_fetcher
{
  public:
    explicit pixel_fetcher(rw_device &rw_device);

    std::optional<uint16_t> fetch_tile_line(screen_coordinates sc);
    std::optional<uint16_t> fetch_sprite_line(uint8_t sprite_index, uint8_t line);

    void set_background_mode();
    void set_window_mode();

    void update_addresses();

  private:
    enum class fetching_mode
    {
        background,
        window
    } m_mode{fetching_mode::background};
    rw_device &m_rw;

    static constexpr int delay_value{0};

    int m_delay{delay_value};
    int m_sprite_delay{delay_value};
};

#endif