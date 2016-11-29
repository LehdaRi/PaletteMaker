#ifndef NODE_HPP
#define NODE_HPP


#include <cstdint>
#include <vector>
#include <memory>

#include "Palette.hpp"


class NodeBase {
public:
    virtual ~NodeBase() {};
};

template <typename D>
class Node : public NodeBase {
public:
    friend D;
    friend class Palette;

    Node(Node<D>* parent);

    Node(const Node<D>&)     = delete;
    Node(Node<D>&&)          = delete;
    Node<D>& operator=(const Node<D>&)  = delete;
    Node<D>& operator=(Node<D>&&)       = delete;
    virtual ~Node(void) = default;

    Node<D>& addChild(void);
    void addChildrenPostProcess(void);
    void addEntry(Palette::Entry& entry);

    void getColor(double& r, double& g, double& b);

    void findPosForColor(uint32_t& x, uint32_t& y,
                         uint8_t r, uint8_t g, uint8_t b,
                         const std::vector<Palette::Entry>& data,
                         Palette::Rnd& rnd);

protected:
    Node<D>*        _parent;
    uint64_t        _nFreeEntries;
    uint64_t        _nTotalEntries;
    uint64_t        _r;
    uint64_t        _g;
    uint64_t        _b;

    std::vector<std::unique_ptr<Node<D>>>   _children;
};

class RectNode : public Node<RectNode> {
public:
    RectNode(Node<RectNode>* parent,
             uint32_t x, uint32_t y, uint32_t w, uint32_t h);

    RectNode& addChild(uint32_t x, uint32_t y, uint32_t w, uint32_t h);
    void addChildrenPostProcess(void) {}
    bool checkAddEntry(Palette::Entry& entry);

    void getSpec(uint32_t& x, uint32_t& y, uint32_t& w, uint32_t& h) const;

    void findPosForColor(uint32_t& x, uint32_t& y,
                         uint8_t r, uint8_t g, uint8_t b,
                         const std::vector<Palette::Entry>& data,
                         Palette::Rnd& rnd);

protected:
    uint32_t    _x;
    uint32_t    _y;
    uint32_t    _w;
    uint32_t    _h;
};


template <typename D>
Node<D>::Node(Node<D>* parent) :
    _parent         (parent),
    _nFreeEntries   (0),
    _nTotalEntries  (0),
    _r              (0),
    _g              (0),
    _b              (0)
{
}

class CircleNode : public Node<CircleNode> {
public:
    CircleNode(Node<CircleNode>* parent,
               float x, float y, float rad);

    CircleNode& addChild(float x, float y, float rad);
    void addChildrenPostProcess(void);
    bool checkAddEntry(Palette::Entry& entry);

    void getSpec(float& x, float& y, float& rad) const;

    void findPosForColor(uint32_t& x, uint32_t& y,
                         uint8_t r, uint8_t g, uint8_t b,
                         const std::vector<Palette::Entry>& data,
                         Palette::Rnd& rnd);

protected:
    float   _x;
    float   _y;
    float   _rad;

    float   _radSqr;
    int     _yfill;
};


template <typename D>
Node<D>& Node<D>::addChild(void) {
    _children.push_back(std::make_unique(this));
    return *_children.back();
}

template <typename D>
void Node<D>::addEntry(Palette::Entry& entry) {
    if (static_cast<D*>(this)->checkAddEntry(entry)) {
        ++_nFreeEntries;
        ++_nTotalEntries;
        for (auto& c : _children) {
            c->addEntry(entry);
        }
    }
}

template <typename D>
void Node<D>::addChildrenPostProcess(void) {
    static_cast<D*>(this)->addChildrenPostProcess();
}

template <typename D>
void Node<D>::getColor(double& r, double& g, double& b) {
    r = (_nFreeEntries*Palette::rGlobal + _r)/_nTotalEntries;
    g = (_nFreeEntries*Palette::gGlobal + _g)/_nTotalEntries;
    b = (_nFreeEntries*Palette::bGlobal + _b)/_nTotalEntries;
}

template <typename D>
void Node<D>::findPosForColor(uint32_t& x, uint32_t& y,
                                  uint8_t r, uint8_t g, uint8_t b,
                                  const std::vector<Palette::Entry>& data,
                                  Palette::Rnd& rnd) {
    static_cast<D*>(this)->findPosForColor(x, y, r, g, b, data, rnd);
}


#endif // NODE_HPP
