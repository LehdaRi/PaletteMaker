#include "Node.hpp"

#include <cfloat>


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

bool RectNode::checkAddEntry(Palette::Entry& entry) {
    if (entry.x >= _x && entry.x < _x+_w &&
        entry.y >= _y && entry.y < _y+_h) {
        entry.nodes.push_back(this);
        return true;
    }
    return false;
}

void RectNode::findPosForColor(uint32_t& x, uint32_t& y,
                               uint8_t r, uint8_t g, uint8_t b,
                               const std::vector<Palette::Entry>& data,
                               Palette::Rnd rnd) {
    if (_children.size() > 0) {
        double minDiff = DBL_MAX;
        NodeBase<RectNode>* minChild = _children[0].get();
        double cr, cg, cb, d;

        bool found = false;
        for (auto& c : _children) {
            c->getColor(cr, cg, cb);
            cr -= r;
            cg -= g;
            cb -= b;
            d = cr*cr + cg*cg + cb*cb;
            if (d < minDiff && c->_nFreeEntries > 0) {
                minChild = c.get();
                minDiff = d;
                found = true;
            }
        }

        if (found) {
            minChild->findPosForColor(x, y, r, g, b, data, rnd);
            return;
        }
    }

    for (auto i=0; i<256; ++i) {
        x = _x + (rnd()%_w);
        y = _y + (rnd()%_h);
        if (!data[y*4096 + x].filled)
            return;
    }

    for (y=_y; y<_y+_h; ++y)
        for (x=_y; x<_x+_w; ++x)
            if (!data[y*4096 + x].filled)
                return;

}
