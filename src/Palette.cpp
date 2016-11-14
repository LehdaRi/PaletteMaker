#include "Palette.hpp"
#include "Node.hpp"

#include <SFML/Graphics.hpp>


double Palette::rGlobal = 127.5;
double Palette::gGlobal = 127.5;
double Palette::bGlobal = 127.5;


Palette::Palette(void) :
    _r              (45147630345987),
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

    for (auto& e : _data)
        _root->addEntry(e);
}

Palette::~Palette(void) {
    delete _root;
}

void Palette::addColor(uint8_t r, uint8_t g, uint8_t b) {
    uint32_t x, y;
    _root->findPosForColor(x, y, r, g, b, _data, _r);
    auto& e = _data[y*4096 + x];
    if (_data[y*4096 + x].filled) {
        printf("Critical Error: data already filled at pos %u, %u\n", x, y);
        return;
    }

    e.r = r;
    e.g = g;
    e.b = b;
    e.filled = true;

    //printf("[%0.2f, %0.2f, %0.2f]\n", rGlobal, gGlobal, bGlobal);
    //double rr, gg, bb;
    //_root->getColor(rr, gg, bb);
    //printf("[%0.2f, %0.2f, %0.2f]\n", rr, gg, bb);
    //printf("[%llu, %llu, %llu]\n", _rGlobalSum, _gGlobalSum, _bGlobalSum);

    _rGlobalSum -= r;
    _gGlobalSum -= g;
    _bGlobalSum -= b;
    --_nFreeEntries;
    rGlobal = (double)_rGlobalSum/_nFreeEntries;
    gGlobal = (double)_gGlobalSum/_nFreeEntries;
    bGlobal = (double)_bGlobalSum/_nFreeEntries;
}

void Palette::writeNodesCountImg(void) const {
    sf::Image img;
    img.create(4096, 4096);

    for (auto& e : _data) {
        auto n = e.nodes.size();
        img.setPixel(e.x, e.y, sf::Color(n*2, n*2, n*2));
    }

    img.saveToFile("NodesCount.png");
}

void Palette::writeImg(void) const {
    sf::Image img;
    img.create(4096, 4096);

    for (auto& e : _data) {
        auto n = e.nodes.size();
        img.setPixel(e.x, e.y, sf::Color(e.r, e.g, e.b));
    }

    img.saveToFile("Palette.png");
}

void Palette::addChildNodes(RectNode& node, uint32_t l) {
    if (++l > 5)
        return;

    uint32_t x, y, w, h;
    for (auto i=0u; i<4; ++i) {
        node.getSpec(x, y, w, h);
        x += _r()%(w/2);
        y += _r()%(h/2);
        w /= 2;
        h /= 2;
        RectNode& c = node.addChild(x, y, w, h);
        addChildNodes(c, l);
    }
}
