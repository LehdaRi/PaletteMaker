#ifndef PALETTE_HPP
#define PALETTE_HPP


#include <vector>
#include <cstdint>
#include <memory>
#include <random>


class RectNode;


class Palette {
public:
    struct Entry {
        std::vector<RectNode*> nodes;
        bool        filled;
        uint32_t    x;
        uint32_t    y;
        uint8_t     r;
        uint8_t     g;
        uint8_t     b;

        Entry(uint32_t x = 0, uint32_t y = 0) :
            filled(false), x(x), y(y), r(0), g(0), b(0) {}
    };

    using Rnd = std::default_random_engine;

    Palette(void);
    ~Palette(void);

    Palette(const Palette&)     = delete;
    Palette(Palette&&)          = delete;
    Palette& operator=(const Palette&)  = delete;
    Palette& operator=(Palette&&)       = delete;

    void addColor(uint8_t r, uint8_t g, uint8_t b);

    void writeNodesCountImg(void) const;
    void writeImg(void) const;

    static double  rGlobal;
    static double  gGlobal;
    static double  bGlobal;

private:
    Rnd                 _r;
    std::vector<Entry>  _data;
    RectNode*           _root;

    uint64_t            _rGlobalSum;
    uint64_t            _gGlobalSum;
    uint64_t            _bGlobalSum;
    uint64_t            _nFreeEntries;

    void addChildNodes(RectNode& node, uint32_t l);
};


#endif // PALETTE_HPP
