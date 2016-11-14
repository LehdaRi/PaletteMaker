#include "Palette.hpp"


inline uint32_t makeColorId(uint8_t r, uint8_t g, uint8_t b) {
    return (uint32_t)r | ((uint32_t)g << 8) | ((uint32_t)g << 16);
}

int main(void) {
    std::default_random_engine rnd;

    std::vector<bool>   colors; // used colors
    colors.resize(16777216, false);

    Palette palette;

    palette.writeNodesCountImg();

    uint8_t r, g, b;
    uint32_t cid;
    bool found = false;
    for (auto i=0u; i<1677721/*6*/; ++i) {
        cid = i;
        /*found = false;
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
        }*/

        palette.addColor(cid & 0xff, (cid >> 8) & 0xff, (cid >> 16) & 0xff);
        colors[cid] = true;
        printf("%0.4f\r", (i*100.0)/16777216);
    }

    palette.writeImg();

    return 0;
}
