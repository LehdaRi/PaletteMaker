#ifndef NODE_HPP
#define NODE_HPP


#include <cstdint>
#include <vector>
#include <memory>

#include "Palette.hpp"


template <typename D>
class NodeBase {
public:
    NodeBase(NodeBase<D>* parent);
    virtual ~NodeBase(void) = default;

    NodeBase<D>& addChild(void);
    void addFreeEntry();
    void getColor(double& r, double& g, double& b);

    void addEntry(Palette::Entry& entry);

protected:
    NodeBase<D>*    _parent;
    uint32_t        _nFreeEntries;
    uint32_t        _nTotalEntries;
    uint64_t        _r;
    uint64_t        _g;
    uint64_t        _b;

    std::vector<std::unique_ptr<NodeBase<D>>>   _children;
};

class RectNode : public NodeBase<RectNode> {
public:
    RectNode(NodeBase<RectNode>* parent,
             uint32_t x, uint32_t y, uint32_t w, uint32_t h);

    RectNode& addChild(uint32_t x, uint32_t y, uint32_t w, uint32_t h);

    void addEntry(Palette::Entry& entry);
    void getSpec(uint32_t& x, uint32_t& y, uint32_t& w, uint32_t& h) const;

private:
    uint32_t    _x;
    uint32_t    _y;
    uint32_t    _w;
    uint32_t    _h;
};


template <typename D>
NodeBase<D>::NodeBase(NodeBase<D>* parent) :
    _parent         (parent),
    _nFreeEntries   (0),
    _nTotalEntries  (0),
    _r              (0),
    _g              (0),
    _b              (0)
{
}

template <typename D>
NodeBase<D>& NodeBase<D>::addChild(void) {
    _children.push_back(std::make_unique(this));
    return *_children.back();
}

template <typename D>
void NodeBase<D>::addFreeEntry(void) {
    ++_nFreeEntries;
    ++_nTotalEntries;
}

template <typename D>
void NodeBase<D>::getColor(double& r, double& g, double& b) {
    r = (_nFreeEntries*Palette::rGlobal + _r)/_nTotalEntries;
    g = (_nFreeEntries*Palette::gGlobal + _g)/_nTotalEntries;
    b = (_nFreeEntries*Palette::bGlobal + _b)/_nTotalEntries;
}


#endif // NODE_HPP
