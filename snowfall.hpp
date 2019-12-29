#include <random>
#include <vector>
#include <X11/Xlib.h>

struct flakespeed{
    //! Speed in x axis
    float speed_x = 0.f;
    //! Speed in y axis
    float speed_y = 0.f;

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
    //! Particle count
    unsigned int particle_count = 500;
    //! Min speed x
    float min_speed_x = 20.f;
    //! Max speed x
    float max_speed_x = 40.f;
    //! Min speed y
    float min_speed_y = 20.f;
    //! Max speed y
    float max_speed_y = 40.f;
    //! X-spawn intensity
    int x_spawn_intensity = 3;
    //! Min size
    unsigned int min_size = 2;
    //! Max size
    unsigned int max_size = 4;
    //! Random number from device
    std::random_device rd;
    //! Generator
    std::mt19937 eng;

public:
    //! Snowflakes
    XRectangle* snowflakes = nullptr;
    //! Snowflake positions
    float** positions = nullptr;
    //! Snowflake speeds
    flakespeed* snowflakespeeds = nullptr;

    //! Constructor
    sky(unsigned int screen_width, unsigned int screen_heigth);
    //! Destructor
    ~sky();
    //! Free reserved memory
    void free_memory();
    //! Set screen size
    void set_screen_size(unsigned int screen_width, unsigned int screen_heigth);
    //! Set flake count
    void set_count(unsigned int count);
    //! Set speed
    void set_speed(float min_x, float max_x, float min_y, float max_y);
    //! Set x-spawn intensity
    void set_x_spawn_intensity(int intensity);
    //! Set size
    void set_size(unsigned int min, unsigned int max);
    //! Get random float
    float get_random_float(float min, float max);
    //! Get randomised position
    std::pair<int, int> get_random_position(int min_x, int max_x, int min_y, int max_y);
    //! Get randomised speed
    std::pair<float, float> get_random_speed(float min_x, float max_x, float min_y, float max_y);
    //! Get randomised size
    int get_random_size(int min, int max);
    //! Create randomised sky
    void create_sky();
    //! Update flake positions
    void update();
};
