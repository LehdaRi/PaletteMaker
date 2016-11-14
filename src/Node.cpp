#include "Node.hpp"


RectNode::RectNode(NodeBase<RectNode>* parent,
                   uint32_t x, uint32_t y, uint32_t w, uint32_t h) :
    NodeBase(parent),
    _x  (x),
    _y  (y),
    _w  (w),
    _h  (h)
{
}

RectNode& RectNode::addChild(uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
    _children.push_back(std::make_unique<RectNode>(this, x, y, w, h));
    return static_cast<RectNode&>(*_children.back());
}

void RectNode::getSpec(uint32_t& x, uint32_t& y, uint32_t& w, uint32_t& h) const {
    x = _x;
    y = _y;
    w = _w;
    h = _h;
}
