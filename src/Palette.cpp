#include "Palette.hpp"
#include "Node.hpp"

#include <SFML/Graphics.hpp>


double Palette::rGlobal = 127.5;
double Palette::gGlobal = 127.5;
double Palette::bGlobal = 127.5;


Palette::Palette(Type type) :
    _r              (45147630345987llu),
    _type           (type),
    _rGlobalSum     (2139095040),
    _gGlobalSum     (2139095040),
    _bGlobalSum     (2139095040),
    _nFreeEntries   (16777216llu)
{
    _data.reserve(16777216);

    for (auto y=0u; y<4096; ++y)
        for (auto x=0u; x<4096; ++x)
            _data.emplace_back(x, y);

    switch (_type) {
    case TYPE_RECT:
        _root = new RectNode(nullptr, 0, 0, 4096, 4096);
        addRectChildNodes(*_root, 0);

        for (auto& e : _data)
            static_cast<RectNode*>(_root)->addEntry(e);
    break;
    case TYPE_CIRCLE:
        //_root = new CircleNode(nullptr, 2048, 2048, 1024);
        _root = new CircleNode(nullptr, 2048, 2048, 2897);
        addCircleChildNodes(*_root, 0);
        static_cast<CircleNode*>(_root)->addChildrenPostProcess();

        printf("Adding Entries..\n");
        uint64_t i=0;
        for (auto& e : _data) {
            static_cast<CircleNode*>(_root)->addEntry(e);
            if (++i%16777 == 0)
                printf("%0.2f%%\r", i/167772.16);
        }
        printf("\n");

    break;
    }
}

Palette::Palette(const std::string& fileName) :
    _r              (45147630345987llu),
    _root           (nullptr),
    _rGlobalSum     (0),
    _gGlobalSum     (0),
    _bGlobalSum     (0),
    _nFreeEntries   (0llu)
{
    sf::Image img;
    img.loadFromFile(fileName);

    _data.reserve(16777216);

    for (auto y=0u; y<4096; ++y) {
        for (auto x=0u; x<4096; ++x) {
            _data.emplace_back(x, y);
            auto& e = _data.back();
            auto p = img.getPixel(x, y);
            e.r = p.r;
            e.g = p.g;
            e.b = p.b;
            e.filled = true;
        }
    }
}

Palette::~Palette(void) {
    delete _root;
}

void Palette::addColor(uint8_t r, uint8_t g, uint8_t b) {
    uint32_t x(0), y(0);

    switch (_type) {
    case TYPE_RECT:
        static_cast<RectNode*>(_root)->findPosForColor(x, y, r, g, b, _data, _r);
        break;
    case TYPE_CIRCLE:
        static_cast<CircleNode*>(_root)->findPosForColor(x, y, r, g, b, _data, _r);
        break;
    }

    auto& e = _data[y*4096 + x];
    if (_data[y*4096 + x].filled) {
        printf("Critical Error: data already filled at pos %u, %u\n", x, y);
        return;
    }
    for (auto* nn : e.nodes) {
        switch (_type) {
        case TYPE_RECT: {
            auto* n = static_cast<RectNode*>(nn);
            --n->_nFreeEntries;
            n->_r += r;
            n->_g += g;
            n->_b += b;
            } break;
        case TYPE_CIRCLE: {
            auto* n = static_cast<CircleNode*>(nn);
            --n->_nFreeEntries;
            n->_r += r;
            n->_g += g;
            n->_b += b;
            } break;
        }

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

void Palette::addColor(uint8_t r, uint8_t g, uint8_t b, uint32_t x, uint32_t y) {
    auto& e = _data[y*4096 + x];
    if (_data[y*4096 + x].filled) {
        printf("Critical Error: data already filled at pos %u, %u\n", x, y);
        return;
    }
    for (auto* nn : e.nodes) {
        switch (_type) {
        case TYPE_RECT: {
            auto* n = static_cast<RectNode*>(nn);
            --n->_nFreeEntries;
            n->_r += r;
            n->_g += g;
            n->_b += b;
            } break;
        case TYPE_CIRCLE: {
            auto* n = static_cast<CircleNode*>(nn);
            --n->_nFreeEntries;
            n->_r += r;
            n->_g += g;
            n->_b += b;
            } break;
        }

    }
    e.r = r;
    e.g = g;
    e.b = b;
    e.filled = true;

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
        img.setPixel(e.x, e.y, sf::Color(n*8, n*8, n*8));
    }

    img.saveToFile("NodesCount.png");
}

void Palette::writeInertiaImg(const std::string& fileName) const {
    float minInertia = 0.0f;
    float maxInertia = 0.0f;
    float minClusterSize = 0.0f;
    float maxClusterSize = 1.0f;
    for (auto& e : _data) {
        minInertia = std::min(minInertia, std::min(e.xInertia, e.yInertia));
        maxInertia = std::max(maxInertia, std::max(e.xInertia, e.yInertia));
        minClusterSize = std::min(minClusterSize, std::min(e.clusterSize, e.clusterSize));
        maxClusterSize = std::max(maxClusterSize, std::max(e.clusterSize, e.clusterSize));
    }

    printf("minInertia: %0.5f, maxInertia: %0.5f\n", minInertia, maxInertia);
    printf("minClusterSize: %0.5f, maxClusterSize: %0.5f\n", minClusterSize, maxClusterSize);

    float inertiaScale = 255.0f / (maxInertia-minInertia);
    float clusterSizeScale = 255.0f / (maxClusterSize-minClusterSize);

    sf::Image img;
    img.create(4096, 4096);

    for (auto& e : _data) {
        img.setPixel(e.x, e.y, sf::Color((e.xInertia-minInertia)*inertiaScale,
                                         (e.yInertia-minInertia)*inertiaScale,
                                         (e.clusterSize-minClusterSize)*clusterSizeScale
                                         );
    }

    img.saveToFile(fileName);
}

void Palette::writeImg(const std::string& fileName) const {
    sf::Image img;
    img.create(4096, 4096);

    for (auto& e : _data) {
        //auto n = e.nodes.size();
        img.setPixel(e.x, e.y, sf::Color(e.r, e.g, e.b));
    }

    img.saveToFile(fileName);
}

void Palette::addRectChildNodes(NodeBase& node, uint32_t l) {
    if (++l > 6)
        return;

    auto& n = *static_cast<RectNode*>(&node);
    uint32_t x, y, w, h;
    for (auto i=0u; i<8; ++i) {
        n.getSpec(x, y, w, h);
        x += _r()%(w/2);
        y += _r()%(h/2);
        w /= 2;
        h /= 2;
        RectNode& c = n.addChild(x, y, w, h);
        addRectChildNodes(c, l);
    }
}

void Palette::addCircleChildNodes(NodeBase& node, uint32_t l) {
    if (++l > 6)
        return;

    //const float radScale = 2.0f / 3;
    //const float radRecScale = 1.0f / 3;
    const float radScale = 0.577;
    const float radRecScale = 0.333333f;

    auto& n = *static_cast<CircleNode*>(&node);

    float x, y, rad;
    n.getSpec(x, y, rad);

    float xr, yr, xs, ys;
    for (auto i=0u; i<6; ++i) {
        //xr = ((int)(_r()%65537)-32768)/32768.0;
        //yr = ((int)(_r()%65537)-32768)/32768.0;
        xr = radScale*cos(1.0471975*i);
        yr = radScale*sin(1.0471975*i);
        xs = x + rad*xr;
        ys = y + rad*yr;
        /*while (xs < 0 || xs >= 4096 || ys < 0 || ys >= 4096 || xr*xr+yr*yr > 1.0f) {
            xr = ((int)(_r()%65537)-32768)/32768.0;
            yr = ((int)(_r()%65537)-32768)/32768.0;
            xs = x + rad*xr;
            ys = y + rad*yr;
        }*/

        CircleNode& c = n.addChild(xs, ys, rad*radRecScale);
        addCircleChildNodes(c, l);
    }

    CircleNode& c = n.addChild(x, y, rad*radRecScale);
    addCircleChildNodes(c, l);
}
