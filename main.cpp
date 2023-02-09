#include <iostream>
#include <fstream>
#include "image.h"
#include <chrono>
#include <omp.h>

using namespace std::chrono;

const std::string usage = "Usage: ./a.out <num_of_threads> <input_image> <output_image>";

int main(int argc, char **argv) {

    if (argc != 5) {
        std::cerr << "Invalid input data\n" << usage << "\n";
        return -1;
    }

    size_t num_of_threads = std::stoi(argv[1]);
    std::ifstream fin(argv[2], std::fstream::binary);
    FILE *file = fopen(argv[3], "wb");
    double coef = std::stod(argv[4]);

    if (!fin) {
        std::cerr << "Image not found\n"<< usage << "\n";
        return -1;
    }

    image img;
    if (!img.parse(fin)) {
        std::cerr << "Image parse error\n"<< usage << "\n";
        return -1;
    }

#ifdef _OPENMP
    if (num_of_threads == 0) {
       num_of_threads = 1;
   }
   omp_set_num_threads(num_of_threads);
#endif

    auto start_time = system_clock::now();

    img.correction(coef);

    auto end_time = system_clock::now();
    printf(
            "Time (%i thread(s)): %ld ms\n",
            num_of_threads,
            duration_cast<milliseconds>(end_time - start_time).count()
    );

    img.to_string(file);

    fclose(file);
    fin.close();

}
