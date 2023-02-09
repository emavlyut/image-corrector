//
// Created by mavlyut on 10/02/23.
//

#include "image.h"

#include <omp.h>

// header

bool header::parse(std::ifstream& fin) {
    size_t max_brightness;
    fin >> magic_number >> width >> height >> max_brightness;
    std::string ignore;
    getline(fin, ignore);
    if (magic_number != "P6" && magic_number != "P5" || max_brightness != 255) {
        std::cerr << "Not PNM file\n";
        return false;
    }
    return true;
}

void header::to_string(FILE* file) const {
    fprintf(file, "%s\n%zu %zu\n255\n", magic_number.c_str(), width, height);
}

size_t header::get_area() const {
    return (magic_number == "P6" ? 3 : 1) * width * height;
}

size_t header::get_width() const {
    return width;
}

size_t header::get_height() const {
    return height;
}

// image

image::~image() {
    delete[] pixels;
}

bool image::parse(std::ifstream& fin) {
    if (!header.parse(fin)) {
        return false;
    }
    size = header.get_area();
    pixels = new pp[size];
    size_t tmp = 0;
    pp x;
    while (fin >> std::noskipws >> x && tmp < size) {
        pixels[tmp++] = x;
    }
    if (size != tmp) {
        std::cerr << "The image has been damaged\n";
        return false;
    }
    return true;
}

void image::cnts_calc() {
#ifdef _OPENMP
#pragma omp parallel for shared(cnts) schedule(static) default(none)
#endif
    for (int i = 0; i < 256; i++)
        cnts[i] = 0;
#ifdef _OPENMP
#pragma omp parallel for shared(cnts) schedule(static) default(none)
#endif
    for (int i = 0; i < size; i++)
        cnts[pixels[i]]++;
}

pp image::new_color(pp tmp_color) const {
    int tmp_ans = tmp_color;
    if (max_color != min_color) {
        tmp_ans = (int) ((tmp_color - min_color) * 255 / (max_color - min_color + 0.0));
    }
    return std::max(0, std::min(255, tmp_ans));
}

void image::get_min_color(size_t skip) {
    int i = 0;
    while (i < 256 && skip >= cnts[i]) {
        skip -= cnts[i++];
    }
    min_color = i;
}

void image::get_max_color(size_t skip) {
    int i = 255;
    while (i >= 0 && skip >= cnts[i]) {
        skip -= cnts[i--];
    }
    max_color = i;
}

// coef in [0, 0.5)
void image::get_min_max(double coef) {
    auto skip = (size_t) (coef * header.get_width() * header.get_height());
#ifdef _OPENMP
    #pragma omp parallel sections shared(skip)
       {
           #pragma omp section
               get_min_color(skip);
           #pragma omp section
               get_max_color(skip);
       }
#else
    get_min_color(skip);
    get_max_color(skip);
#endif
}

void image::correction(double coef) {
    cnts_calc();
    get_min_max(coef);
    int new_colors[256];
#ifdef _OPENMP
#pragma omp parallel for shared(new_colors) schedule(static) default(none)
#endif
    for (int i = 0; i < 256; i++)
        new_colors[i] = new_color(i);
#ifdef _OPENMP
#pragma omp parallel for shared(pixels, new_colors) schedule(static) default(none)
#endif
    for (int i = 0; i < size; i++) {
        pixels[i] = new_colors[pixels[i]];
    }
}

void image::to_string(FILE *file) {
    header.to_string(file);
    for (int i = 0; i < size; i++)
        fprintf(file, "%c", pixels[i]);
}
