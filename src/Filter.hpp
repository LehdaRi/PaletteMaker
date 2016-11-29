#ifndef FILTER_HPP
#define FILTER_HPP


#include "Palette.hpp"


class Filter {
public:
    static void BoxFilter(Palette& palette);
    static void InertiaFilter(Palette& palette);
};


#endif // FILTER_HPP
