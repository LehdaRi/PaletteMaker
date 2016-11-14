#include "Palette.hpp"
#include "Node.hpp"


double Palette::rGlobal = 127.5;
double Palette::gGlobal = 127.5;
double Palette::bGlobal = 127.5;


Palette::Palette(void) :
    _r              (715517),
    _root           (new RectNode(nullptr, 0, 0, 4096, 4096)),
    _rGlobalSum     (2139095040),
    _gGlobalSum     (2139095040),
    _bGlobalSum     (2139095040),
    _nFreeEntries   (16777216)
{
    _data.reserve(16777216);

    for (auto y=0u; y<4096; ++y)
        for (auto x=0u; x<4096; ++x)
            _data.emplace_back(x, y);

    addChildNodes(*_root, 0);
}

Palette::~Palette(void) {
    delete _root;
}

void Palette::addColor(uint8_t r, uint8_t g, uint8_t b) {
    rGlobal = (double)_rGlobalSum/_nFreeEntries;
    gGlobal = (double)_gGlobalSum/_nFreeEntries;
    bGlobal = (double)_bGlobalSum/_nFreeEntries;
}

void Palette::addChildNodes(RectNode& node, uint32_t l) {
    if (++l > 4)
        return;

    uint32_t x, y, w, h;
    for (auto i=0u; i<4; ++i) {
        node.getSpec(x, y, w, h);
        x += _r()%(w/2);
        y += _r()%(h/2);
        w /= 2;
        h /= 2;
        printf("%u %u %u %u\n", x, y, w, h);
        RectNode& c = node.addChild(x, y, w, h);
        addChildNodes(c, l);
    }
}
