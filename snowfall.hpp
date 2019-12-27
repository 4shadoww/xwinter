#include <random>
#include <vector>
#include <X11/Xlib.h>

struct flakespeed{
    //! Speed in x axis
    int speed_x = 0;
    //! Speed in y axis
    int speed_y = 0;

    //! Default constructor
    flakespeed(){}
    //! Constructor
    flakespeed(int x, int y){
        this->speed_x = x;
        this->speed_y = y;
    }
};

class sky{
private:
    //! Screen width
    unsigned int screen_width = 0;
    //! Screen heigth
    unsigned int screen_heigth = 0;
    //! Random number from device
    std::random_device rd;
    //! Generator
    std::mt19937 eng;

public:
    //! Snowflakes
    XRectangle* snowflakes;
    //! Snowflake speeds
    flakespeed* snowflakespeeds;
    //! Flake count
    unsigned int flake_count = 0;

    //! Constructor
    sky(unsigned int screen_width, unsigned int screen_heigth, unsigned int flake_count);
    //! Set screen size
    void set_screen_size(unsigned int screen_width, unsigned int screen_heigth);
    //! Get randomised position
    std::pair<int, int> get_random_position(int min_x, int max_x, int min_y, int max_y);
    //! Get randomised speed
    std::pair<int, int> get_random_speed(int min_x, int max_x, int min_y, int max_y);
    //! Get randomised size
    int get_random_size(int min, int max);
    //! Create randomised sky
    void create_sky(unsigned int flake_count);
    //! Update flake positions
    void update();
};
