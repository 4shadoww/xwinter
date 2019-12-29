#include <iostream>
#include <unistd.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "snowfall.hpp"


sky::sky(unsigned int screen_width, unsigned int screen_heigth){
    // Set screen size
    sky::set_screen_size(screen_width, screen_heigth);
    // Seed rng
    this->eng.seed(this->rd());
}

sky::~sky(){
    sky::free_memory();
}

void sky::free_memory(){
    if(this->snowflakes) delete[] this->snowflakes;
    if(this->snowflakespeeds) delete[] this->snowflakespeeds;
    if(this->positions){
        for(unsigned int i = 0; i < this->particle_count; i++){
            delete[] this->positions[i];
        }
        delete[] this->positions;
    }
}

void sky::set_screen_size(unsigned int screen_width, unsigned int screen_heigth){
    this->screen_width = screen_width;
    this->screen_heigth = screen_heigth;
}

void sky::set_count(unsigned int count){
    this->particle_count = count;
}

void sky::set_speed(float min_x, float max_x, float min_y, float max_y){
    this->min_speed_x = min_x;
    this->max_speed_x = max_x;
    this->min_speed_y = min_y;
    this->max_speed_y = max_y;
}

void sky::set_x_spawn_intensity(int intensity){
    this->x_spawn_intensity = intensity;
}

void sky::set_size(unsigned int min, unsigned int max){
    this->min_size = min;
    this->max_size = max;
}

float sky::get_random_float(float min, float max) {
    return ((float)rand() / RAND_MAX) * (max - min) + min;
}


std::pair<int, int> sky::get_random_position(int min_x, int max_x, int min_y, int max_y){
    // Define x range
    std::uniform_int_distribution<> range_x(min_x, max_x);
    // Define y range
    std::uniform_int_distribution<> range_y(min_y, max_y);
    // Create random position in range
    return std::pair<int, int>(range_x(this->eng), range_y(this->eng));
}

std::pair<float, float> sky::get_random_speed(float min_x, float max_x, float min_y, float max_y){
    // Create random speed in range
    return std::pair<float, float>(sky::get_random_float(min_x, max_x), sky::get_random_float(min_y, max_y));
}

int sky::get_random_size(int min, int max){
    // Define range
    std::uniform_int_distribution<> range(min, max);
    // Create random number
    return range(this->eng);
}

void sky::create_sky(){
    // Check is there an old sky
    sky::free_memory();

    // Reverse memory
    this->snowflakes = new XRectangle[this->particle_count];
    this->snowflakespeeds = new flakespeed[this->particle_count];
    this->positions = new float*[this->particle_count];

    for(unsigned int i = 0; i < this->particle_count; i++){
        this->positions[i] = new float[2];
    }

    // Temp variables
    std::pair<int, int> temp_pair;
    int temp_int;

    // Create randomised flakes
    for(unsigned int i = 0; i < this->particle_count; i++){
        // Get random position
        temp_pair = sky::get_random_position(0, this->screen_width, 0, this->screen_heigth);
        // Set position on screen
        this->snowflakes[i].x = temp_pair.first;
        this->snowflakes[i].y = temp_pair.second;
        // Set position to float number table
        this->positions[i][0] = temp_pair.first;
        this->positions[i][1] = temp_pair.second;
        // Set random size
        temp_int = sky::get_random_size(this->min_size, this->max_size);
        this->snowflakes[i].height = temp_int;
        this->snowflakes[i].width = temp_int;
        // Set random speed
        temp_pair = this->get_random_speed(this->min_speed_x, this->max_speed_x, this->min_speed_y, this->max_speed_y);
        this->snowflakespeeds[i].speed_x = temp_pair.first;
        this->snowflakespeeds[i].speed_y = temp_pair.second;
    }
}

void sky::update(){
    // Temp variables
    std::pair<int, int> temp_pair;
    // Spawm flake positions
    int y = 0;

    // Loop
    for(unsigned int i = 0; i < this->particle_count; i++){
        // Update position
        this->positions[i][0] += this->snowflakespeeds[i].speed_x / 10.f;
        this->positions[i][1] += this->snowflakespeeds[i].speed_y / 10.f;
        this->snowflakes[i].x = this->positions[i][0];
        this->snowflakes[i].y = this->positions[i][1];

        // If out of screen spawn again
        if(this->snowflakes[i].x + this->snowflakes[i].width > (int)this->screen_width ||
           this->snowflakes[i].y + this->snowflakes[i].height > (int)this->screen_heigth ||
           this->snowflakes[i].x + this->snowflakes[i].width < 0 ||
           this->snowflakes[i].y + this->snowflakes[i].height < 0){

            // Get new random position
            if(y < this->x_spawn_intensity){
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
            this->positions[i][0] = temp_pair.first;
            this->positions[i][1] = temp_pair.second;
            this->snowflakes[i].x = temp_pair.first;
            this->snowflakes[i].y = temp_pair.second;
        }
    }
}

void print_usage(std::string path){
    std::cout << "usage: " + path + " [options]" << std::endl;
    std::cout << R"(Options:
  -c, --count         Set particle count
  -sx, --speed-x      Set speed on X-axis
  -sy, --speed-y      Set speed on Y-axis
  -cl, --color        Set particle color {white|blue|red|green}
  -rx, --random-x     Set randomness on speed on X-axis
  -ry, --random-y     Set randomness on speed on Y-axis
  -ys, --y-spawn      Set particle spawn intensity on X-axis
  -s, --size          Set particle size
  -rs, --random-size  Set randomness to size)" << std::endl;
}

int main(int argc, char** argv){
    // Settings
    unsigned int count = 500;
    float speed_x = 20.f;
    float speed_y = 20.f;
    std::string particle_color = "white";
    float random_x = 20.f;
    float random_y = 20.f;
    int y_spawn = 3;
    unsigned int size = 2;
    unsigned int random_size = 2;

    // Chech is help asked to be printed
    if(argc > 1 && strcmp(argv[1], "-h") == 0){
        print_usage(argv[0]);
        return 0;
    }
    // Check is argument count correct
    if(argc % 2 == 1){
        std::cerr << "error: invalid argument count" << std::endl;
        return 1;
    }
    // Parse arguments
    for(int i = 1; i < argc - 1; i++){
        // Particle count
        if(strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--count") == 0){
            i++;
            count = std::stoul(argv[i]);
            continue;
        // Speed x
        }else if(strcmp(argv[i], "-sx") == 0 || strcmp(argv[i], "--speed-x") == 0){
            i++;
            speed_x = std::atof(argv[i]);
            continue;
        // Speed y
        }else if(strcmp(argv[i], "-sy") == 0 || strcmp(argv[i], "--speed-y") == 0){
            i++;
            speed_y = std::atof(argv[i]);
            continue;
        // Color
        }else if(strcmp(argv[i], "-cl") == 0 || strcmp(argv[i], "--color") == 0){
            i++;
            particle_color = argv[i];
            continue;
        // Random x
        }else if(strcmp(argv[i], "-rx") == 0 || strcmp(argv[i], "--random-x") == 0){
            i++;
            random_x = std::atof(argv[i]);
            continue;
        // Random y
        }else if(strcmp(argv[i], "-ry") == 0 || strcmp(argv[i], "--random-y") == 0){
            i++;
            random_y = std::atof(argv[i]);
            continue;
        // Spawn y
        }else if(strcmp(argv[i], "-xs") == 0 || strcmp(argv[i], "--x-spawn") == 0){
            i++;
            y_spawn = std::atoi(argv[i]);
            continue;
        // Size
        }else if(strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--size") == 0){
            i++;
            size = std::stoul(argv[i]);
            continue;
        // Random size
        }else if(strcmp(argv[i], "-rs") == 0 || strcmp(argv[i], "--random-size") == 0){
            i++;
            random_size = std::stoul(argv[i]);
            continue;
        }else{
            std::cerr << "error: unknown argument \"" << argv[i] << "\"" << std::endl;
            return 1;
        }
    }

    // Init xorg
    Display* display = XOpenDisplay (NULL);
    XVisualInfo vinfo;
    XMatchVisualInfo(display, DefaultScreen(display), 32, TrueColor, &vinfo);
    Screen* screen = XDefaultScreenOfDisplay(display);
    int screen_num = DefaultScreen(display);
    GC gc;
    XColor xcolor;

    // Get screen width and heigth
    unsigned int width = XWidthOfScreen(screen);
    unsigned int heigth = XHeightOfScreen(screen);
    // Set window attributes
    XSetWindowAttributes attr;
    attr.override_redirect = 1;
    attr.colormap = XCreateColormap(display, DefaultRootWindow(display), vinfo.visual, AllocNone);
    attr.border_pixel = 0;
    attr.background_pixel = 0;
    // Create window
    Window window = XCreateWindow(display, DefaultRootWindow(display), 0, 0, width, heigth, 0, vinfo.depth, InputOutput, vinfo.visual, CWColormap | CWBorderPixel | CWBackPixel | CWOverrideRedirect, &attr);
    // Move window to background
    XLowerWindow(display, window);

    XMapWindow(display, window);
    // Graphics context
    gc = XCreateGC(display, window, 0, 0);

    // Create the color
    if(particle_color == "red"){
        xcolor.red = 65535; xcolor.green = 0; xcolor.blue = 0;
    }else if(particle_color == "blue"){
        xcolor.red = 0; xcolor.green = 0; xcolor.blue = 65535;
    }else if(particle_color == "green"){
        xcolor.red = 0; xcolor.green = 65535; xcolor.blue = 0;
    }else{
        xcolor.red = 65535; xcolor.green = 65535; xcolor.blue = 65535;
    }
    xcolor.flags = DoRed | DoGreen | DoBlue;
    XAllocColor(display, DefaultColormap(display, screen_num), &xcolor);

    // Set snow color
    XSetForeground(display, gc, xcolor.pixel);
    // Flush
    XFlush(display);

    // Create sky
    sky skyen(width, heigth);
    skyen.set_count(count);
    skyen.set_speed(speed_x, speed_x + random_x, speed_y, speed_y + random_y);
    skyen.set_x_spawn_intensity(y_spawn);
    skyen.create_sky();

    while(true){
        usleep(1000*1000/144);
        XFillRectangles(display, window, gc, skyen.snowflakes, count);
        XFlush(display);
        XClearWindow(display, window);
        skyen.update();
    }

    return 0;
}
