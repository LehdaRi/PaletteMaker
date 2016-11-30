#ifndef PALETTE_HPP
#define PALETTE_HPP


#include <vector>
#include <cstdint>
#include <memory>
#include <random>


class NodeBase;


class Palette {
public:
    friend class Filter;

    struct Entry {
        //std::vector<RectNode*> nodes;
        std::vector<NodeBase*> nodes;
        bool        filled;
        uint32_t    x;
        uint32_t    y;
        uint8_t     r;
        uint8_t     g;
        uint8_t     b;

        float       xInertia;
        float       yInertia;

        Entry(uint32_t x = 0, uint32_t y = 0) :
            filled(false),
            x(x), y(y),
            r(0), g(0), b(0),
            xInertia(0.0f), yInertia(0.0f) {}
    };

    enum Type {
        TYPE_RECT,
        TYPE_CIRCLE
    };

    using Rnd = std::default_random_engine;

    Palette(Type type);
    Palette(const std::string& fileName);
    ~Palette(void);

    Palette(const Palette&)     = delete;
    Palette(Palette&&)          = delete;
    Palette& operator=(const Palette&)  = delete;
    Palette& operator=(Palette&&)       = delete;

    void addColor(uint8_t r, uint8_t g, uint8_t b);
    void addColor(uint8_t r, uint8_t g, uint8_t b, uint32_t x, uint32_t y);

    void writeNodesCountImg(void) const;
    void writeInertiaImg(const std::string& fileName = "Inertia.png") const;
    void writeImg(const std::string& fileName = "Palette.png") const;

    static double  rGlobal;
    static double  gGlobal;
    static double  bGlobal;

private:
    Rnd                 _r;
    std::vector<Entry>  _data;

    Type                _type;
    NodeBase*           _root;

    uint64_t            _rGlobalSum;
    uint64_t            _gGlobalSum;
    uint64_t            _bGlobalSum;
    uint64_t            _nFreeEntries;

    void addRectChildNodes(NodeBase& node, uint32_t l);
    void addCircleChildNodes(NodeBase& node, uint32_t l);
};


#endif // PALETTE_HPP
