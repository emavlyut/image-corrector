//
// Created by mavlyut on 10/02/23.
//

#ifndef IMAGE_H
#define IMAGE_H

#include <fstream>
#include <iostream>

typedef unsigned char pp;

struct header {
    bool parse(std::ifstream&);
    void to_string(FILE*) const;
    size_t get_area() const;
    size_t get_width() const;
    size_t get_height() const;

private:
    size_t width, height;
    std::string magic_number;
};


struct image {
    image() = default;
    ~image();
    image(image const&) = delete;
    image& operator=(image const&) = delete;
    bool parse(std::ifstream&);
    void correction(double);
    void to_string(FILE*);

private:
    void cnts_calc();
    pp new_color(pp) const;
    void get_min_color(size_t);
    void get_max_color(size_t);
    void get_min_max(double);

private:
    pp* pixels;
    header header;
    size_t cnts[256];
    int max_color, min_color;
    size_t size;
};


#endif //IMAGE_H
