#include "Filter.hpp"


void Filter::BoxFilter(Palette& palette) {
    uint32_t x, y;
    uint64_t minDiff = 4294967296llu;
    uint64_t diff = 0;
    int32_t filterScale = 1024;
    int32_t filterSize = 3;
    bool scaleDir = true;
    bool sizeDir = true;

    for (auto i=0llu; i<16777216llu*1024; ++i) {
        minDiff = 4294967296llu;

        x = palette._r()%4096;
        y = palette._r()%4096;

        auto& e1 = palette._data[y*4096 + x];
        Palette::Entry* eMin = nullptr;
        int32_t rd, gd, bd;

        for (int ix=-filterSize; ix<filterSize+1; ++ix) {
            for (int iy=-filterSize; iy<filterSize+1; ++iy) {
                if (ix == 0 && iy == 0)
                    continue;
                if (x+filterScale*ix >= 0 && x+filterScale*ix < 4096 &&
                    y+filterScale*iy >= 0 && y+filterScale*iy < 4096) {
                    auto& e2 = palette._data[(y+filterScale*iy)*4096 + (x+filterScale*ix)];
                    rd = e1.r-e2.r;
                    gd = e1.g-e2.g;
                    bd = e1.b-e2.b;
                    diff = rd*rd + gd*gd + bd*bd;
                    if (diff < minDiff) {
                        minDiff = diff;
                        eMin = &e2;
                        //printf("1 Orig: [%u, %u] Min: [%u, %u]\n", e1.x, e1.y, eMin->x, eMin->y);
                    }
                }
            }
        }
        //printf("MinDiff: %llu\n", minDiff);
        //printf("Orig: [%u, %u] Min: [%u, %u]\n", e1.x, e1.y, eMin->x, eMin->y);

        if (eMin != nullptr) {
            uint32_t xMiddle = (e1.x+eMin->x)/2;
            uint32_t yMiddle = (e1.y+eMin->y)/2;
            //printf("Orig: [%u, %u] Middle: [%u, %u]\n",e1.x, e1.y, xMiddle, yMiddle);
            auto& eMiddle = palette._data[yMiddle*4096 + xMiddle];
            auto eMiddleCopy = eMiddle;
            eMiddle.r = e1.r;
            eMiddle.g = e1.g;
            eMiddle.b = e1.b;
            e1.r = eMiddleCopy.r;
            e1.g = eMiddleCopy.g;
            e1.b = eMiddleCopy.b;
        }

        if ((i+1)%16777216 == 0) {
            printf("Saving img: i=%llu\n", i);
            palette.writeImg("Palette_boxFilter.png");

            if (sizeDir) {
                if (++filterSize >= 5)
                    sizeDir = !sizeDir;
            }
            else {
                if (--filterSize == 1)
                    sizeDir = !sizeDir;
            }
/*
            if (scaleDir) {
                if (++filterScale >= 120)
                    scaleDir = !scaleDir;
            }
            else {
                if (--filterScale == 2)
                    scaleDir = !scaleDir;
            }*/

            if (filterScale > 2)
                filterScale /= 2;
            else
                filterScale = 128;

            printf("Filter size: %d Filter scale: %d\n", filterSize, filterScale);
        }
    }
}

void Filter::InertiaFilter(Palette& palette) {
    const uint32_t filterSize = 32;
    const uint32_t nSamples = 32;
    const float dampFactor = 0.9f;

    int32_t x, y, xs, ys, xsd, ysd;
    float rd, gd, bd, diff, inertiaf, xi, yi;
    for (auto i=0llu; i<16777216llu*1024; ++i) {
        x = palette._r()%4096;
        y = palette._r()%4096;
        auto& e = palette._data[y*4096 + x];

        for (auto j=0; j<nSamples; ++j) {
            xsd = 1+palette._r()%filterSize;
            ysd = 1+palette._r()%filterSize;
            if (palette._r()%2)
                xs = x+xsd*xsd;
            else
                xs = x-xsd*xsd;

            if (palette._r()%2)
                ys = y+ysd*ysd;
            else
                ys = y-ysd*ysd;

            while (xs < 0 || xs >= 4096 || ys < 0 || ys >= 4096 || (xs == x && ys == y)) {
                xs = x-256+palette._r()%512;
                ys = y-256+palette._r()%512;
            }
            auto& es = palette._data[ys*4096 + xs];
            rd = e.r-es.r;
            gd = e.g-es.g;
            bd = e.b-es.b;
            diff = rd*rd + gd*gd + bd*bd;
            inertiaf = (1.0f/diff)-0.0002f;
            if (xs-x != 0)
                e.xInertia += (xs-x)*inertiaf;
            if (ys-y != 0)
                e.yInertia += (ys-y)*inertiaf;
            //if (palette._r() % 12800000 == 0) {
            //    printf("xs-x: %0.3f, ys-y: %0.3f, inertiaf: %0.6f\n", (float)(xs-x), (float)(ys-y), inertiaf);
            //    printf("components: [%0.4f, %0.4f]\n", (float)(xs-x)*inertiaf, (ys-y)*inertiaf);
            //    printf("Inertia: [%0.4f, %0.4f]\n", e.xInertia, e.yInertia);
            //}
        }

        e.xInertia *= dampFactor;
        e.yInertia *= dampFactor;

        if (palette._r() % 1677721 == 0)
            printf("Inertia: [%0.4f, %0.4f]\n", e.xInertia, e.yInertia);
        xi = x+e.xInertia;
        yi = y+e.yInertia;
        if (xi < 0.0f)      xi = 0.0f;
        if (xi > 4095.99f)  xi = 4095.99f;
        if (yi < 0.0f)      yi = 0.0f;
        if (yi > 4095.99f)  yi = 4095.99f;
        xs = xi;
        ys = yi;

        if (xs != x || ys != y) {
            auto& es = palette._data[ys*4096 + xs];
            auto esc = es;
            es.r = e.r;
            es.g = e.g;
            es.b = e.b;
            es.xInertia = e.xInertia;
            es.yInertia = e.yInertia;
            e.r = esc.r;
            e.g = esc.g;
            e.b = esc.b;
            e.xInertia = esc.xInertia;
            e.yInertia = esc.yInertia;
        }

        if ((i+1)%16777216 == 0) {
            printf("Saving img: i=%llu\n", i);
            palette.writeImg("Palette_inertiaFilter.png");
        }
    }
}
