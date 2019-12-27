#include <iostream>
#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "snowfall.hpp"


sky::sky(unsigned int screen_width, unsigned int screen_heigth, unsigned int flake_count){
    // Set screen size
    sky::set_screen_size(screen_width, screen_heigth);
    // Set flake count
    this->flake_count = flake_count;
    // Seed rng
    this->eng.seed(this->rd());
    // Create randomised sky
    sky::create_sky(flake_count);
}

void sky::set_screen_size(unsigned int screen_width, unsigned int screen_heigth){
    this->screen_width = screen_width;
    this->screen_heigth = screen_heigth;
}

std::pair<int, int> sky::get_random_position(int min_x, int max_x, int min_y, int max_y){
    // Define x range
    std::uniform_int_distribution<> range_x(min_x, max_x);
    // Define y range
    std::uniform_int_distribution<> range_y(min_y, max_y);
    // Create random position in range
    return std::pair<int, int>(range_x(this->eng), range_y(this->eng));
}

std::pair<int, int> sky::get_random_speed(int min_x, int max_x, int min_y, int max_y){
    // Define x range
    std::uniform_int_distribution<> range_x(min_x, max_x);
    // Define y range
    std::uniform_int_distribution<> range_y(min_y, max_y);
    // Create random position in range
    return std::pair<int, int>(range_x(this->eng), range_y(this->eng));
}

int sky::get_random_size(int min, int max){
    // Define range
    std::uniform_int_distribution<> range(min, max);
    // Create random number
    return range(this->eng);
}

void sky::create_sky(unsigned int flake_count){
    // Reverse memory
    this->snowflakes = new XRectangle[flake_count];
    this->snowflakespeeds = new flakespeed[flake_count];

    // Temp variables
    std::pair<int, int> temp_pair;
    int temp_int;

    // Create randomised flakes
    for(unsigned int i = 0; i < flake_count; i++){
        // Set random position
        temp_pair = sky::get_random_position(0, this->screen_width, 0, this->screen_heigth);
        this->snowflakes[i].x = temp_pair.first;
        this->snowflakes[i].y = temp_pair.second;
        // Set random size
        temp_int = sky::get_random_size(2, 4);
        this->snowflakes[i].height = temp_int;
        this->snowflakes[i].width = temp_int;
        // Set random speed
        temp_pair = this->get_random_speed(3, 4, 3, 4);
        this->snowflakespeeds[i].speed_x = temp_pair.first;
        this->snowflakespeeds[i].speed_y = temp_pair.second;
    }
}

void sky::update(){
    // Temp variables
    std::pair<int, int> temp_pair;
    // Spawm flake positions
    int y = 0;
    int limit = 3;

    // Loop
    for(unsigned int i = 0; i < this->flake_count; i++){
        // Update position
        this->snowflakes[i].x += this->snowflakespeeds[i].speed_x;
        this->snowflakes[i].y += this->snowflakespeeds[i].speed_y;

        // If out of screen spawn again
        if(this->snowflakes[i].x + this->snowflakes[i].width > (int)this->screen_width ||
           this->snowflakes[i].y + this->snowflakes[i].height > (int)this->screen_heigth ||
           this->snowflakes[i].x + this->snowflakes[i].width < 0 ||
           this->snowflakes[i].y + this->snowflakes[i].height < 0){

            // Get new random position
            if(y < limit){
                if(this->snowflakespeeds[i].speed_y > 0){
                    temp_pair = sky::get_random_position(0, this->screen_width, 0, 0);
                }else{
                    temp_pair = sky::get_random_position(0, this->screen_width, this->screen_heigth, this->screen_heigth);
                }
                y++;
            }else{
                if(this->snowflakespeeds[i].speed_x > 0){
                    temp_pair = sky::get_random_position(0, 0, 0, this->screen_heigth);
                }else{
                    temp_pair = sky::get_random_position(this->screen_width, this->screen_width, 0, this->screen_heigth);
                y = 0;
                }
            }
            // Set position
            this->snowflakes[i].x = temp_pair.first;
            this->snowflakes[i].y = temp_pair.second;
        }
    }
}


int main(int argc, char** argv){
    // Init
    Display* display = XOpenDisplay (NULL);
    XVisualInfo vinfo;
    XMatchVisualInfo(display, DefaultScreen(display), 32, TrueColor, &vinfo);
    Screen* screen = XDefaultScreenOfDisplay(display);
    int screen_num = DefaultScreen(display);
    GC gc;

    unsigned int width = XWidthOfScreen(screen);
    unsigned int heigth = XHeightOfScreen(screen);


    XSetWindowAttributes attr;
    attr.override_redirect = 1;
    attr.colormap = XCreateColormap(display, DefaultRootWindow(display), vinfo.visual, AllocNone);
    attr.border_pixel = 0;
    attr.background_pixel = 0;

    Window window = XCreateWindow(display, DefaultRootWindow(display), 0, 0, width, heigth, 0, vinfo.depth, InputOutput, vinfo.visual, CWColormap | CWBorderPixel | CWBackPixel | CWOverrideRedirect, &attr);

    XLowerWindow(display, window);

    XMapWindow(display, window);

    gc = XCreateGC(display, window, 0, 0);

    XSetForeground(display, gc, WhitePixel(display, screen_num));

    XSetLineAttributes(display, gc, 2, LineSolid, CapRound, JoinRound);

    XFlush(display);

    // Create sky
    sky skyen(width, heigth, 1000);

    while(true){
        usleep(2000*10);
        XFillRectangles(display, window, gc, skyen.snowflakes, skyen.flake_count);
        XFlush(display);
        XClearWindow(display, window);
        skyen.update();
    }

    return 0;
}
