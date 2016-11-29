#include "Palette.hpp"
#include "Filter.hpp"
#include <ctime>


inline uint32_t makeColorId(uint8_t r, uint8_t g, uint8_t b) {
    return (uint32_t)r | ((uint32_t)g << 8) | ((uint32_t)b << 16);
}

 void filter(void) {
    Palette palette("Palette1.png");
    //Filter::BoxFilter(palette);
    Filter::InertiaFilter(palette);
 }

int main(void) {
    //filter();
    //return 0;

    std::default_random_engine rnd(time(NULL));

    std::vector<bool>   colors; // used colors
    colors.resize(16777216, false);

    Palette palette(Palette::TYPE_CIRCLE);

    palette.writeNodesCountImg();

    uint8_t r, g, b;
    uint32_t cid;
    bool found = false;

    palette.addColor(255, 255, 255, 2048, 2048);
    colors[makeColorId(255, 255, 255)] = true;
    palette.addColor(255, 0, 0, 3720, 2048);
    colors[makeColorId(255, 0, 0)] = true;
    palette.addColor(255, 255, 0, 2884, 3496);
    colors[makeColorId(255, 255, 0)] = true;
    palette.addColor(0, 255, 0, 1212, 3496);
    colors[makeColorId(0, 255, 0)] = true;
    palette.addColor(0, 255, 255, 376, 2048);
    colors[makeColorId(0, 255, 255)] = true;
    palette.addColor(0, 0, 255, 1212, 600);
    colors[makeColorId(0, 0, 255)] = true;
    palette.addColor(255, 0, 255, 2884, 600);
    colors[makeColorId(255, 0, 255)] = true;

    auto i=7u;
    for (auto j=0u; j<128; ++j) {
        palette.addColor(255-j, 255-j, 255-j);
        colors[makeColorId(255-j, 255-j, 255-j)] = true;
        palette.addColor(255-j, 0, 0);
        colors[makeColorId(255-j, 0, 0)] = true;
        palette.addColor(255-j, 255-j, 0);
        colors[makeColorId(255-j, 255-j, 0)] = true;
        palette.addColor(0, 255-j, 0);
        colors[makeColorId(0, 255-j, 0)] = true;
        palette.addColor(0, 255-j, 255-j);
        colors[makeColorId(0, 255-j, 255-j)] = true;
        palette.addColor(0, 0, 255-j);
        colors[makeColorId(0, 0, 255-j)] = true;
        palette.addColor(255-j, 0, 255-j);
        colors[makeColorId(255-j, 0, 255-j)] = true;
        i += 7;
        printf("%0.4f\r", (i*100.0)/16777216);
    }

    for (; i<16777216; ++i) {
        //cid = i;
        found = false;
        for (auto j=0u; j<256; ++j) {
            r = rnd() % 256;
            g = rnd() % 256;
            b = rnd() % 256;
            cid = makeColorId(r, g, b);
            if (!colors[cid]) {
                found = true;
                break;
            }
        }
        if (!found) {
            for (cid=0; cid<16777216; ++cid) {
                if (!colors[cid]) {
                    found = true;
                    break;
                }
            }
        }
        if (!found) {
            printf("Critical error: could not find free color\n");
            return 1;
        }

        palette.addColor(cid & 0xff, (cid >> 8) & 0xff, (cid >> 16) & 0xff);
        colors[cid] = true;
        if (i%512 == 0)
            printf("%0.4f\r", (i*100.0)/16777216);
        if (i%1677721 == 0)
            palette.writeImg();
    }

    palette.writeImg();

    return 0;
}
