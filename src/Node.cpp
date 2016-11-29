#include "Node.hpp"

#include <cfloat>


RectNode::RectNode(Node<RectNode>* parent,
                   uint32_t x, uint32_t y, uint32_t w, uint32_t h) :
    Node(parent),
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

bool RectNode::checkAddEntry(Palette::Entry& entry) {
    if (entry.x >= _x && entry.x < _x+_w &&
        entry.y >= _y && entry.y < _y+_h) {
        entry.nodes.push_back(this);
        return true;
    }
    return false;
}

void RectNode::getSpec(uint32_t& x, uint32_t& y, uint32_t& w, uint32_t& h) const {
    x = _x;
    y = _y;
    w = _w;
    h = _h;
}

void RectNode::findPosForColor(uint32_t& x, uint32_t& y,
                               uint8_t r, uint8_t g, uint8_t b,
                               const std::vector<Palette::Entry>& data,
                               Palette::Rnd& rnd) {
    if (_children.size() > 0) {
        double minDiff = DBL_MAX;
        Node<RectNode>* minChild = _children[0].get();
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
        if (!data[y*4096 + x].filled) {
            //printf("A[%u, %u] %llu\n", x, y, _nFreeEntries);
            return;
        }
    }

    for (y=_y; y<_y+_h; ++y)
        for (x=_x; x<_x+_w; ++x)
            if (!data[y*4096 + x].filled) {
                //printf("B[%u, %u] %llu\n", x, y, _nFreeEntries);
                return;
            }

    printf("ERROR: [%u, %u] %llu\n", x, y, _nFreeEntries);
}


CircleNode::CircleNode(Node<CircleNode>* parent,
                       float x, float y, float rad) :
    Node(parent),
    _x              (x),
    _y              (y),
    _rad            (rad),
    _radSqr         (rad*rad),
    _yfill          (0)
{
}

CircleNode& CircleNode::addChild(float x, float y, float rad) {
    _children.push_back(std::make_unique<CircleNode>(this, x, y, rad));
    return static_cast<CircleNode&>(*_children.back());
}

void CircleNode::addChildrenPostProcess(void) {
    CircleNode* cc;
    for (auto& c : _children) {
        c->addChildrenPostProcess();
        cc = static_cast<CircleNode*>(c.get());
        float x, y, rad;
        cc->getSpec(x, y, rad);
        float dx = x - _x;
        float dy = y - _y;
        float cr = sqrt(dx*dx+dy*dy) + rad;
        if (cr > _rad) {
            _rad = cr;
            _radSqr = cr*cr;
        }
    }
}

bool CircleNode::checkAddEntry(Palette::Entry& entry) {
    float xd = _x-entry.x;
    float yd = _y-entry.y;
    float eradSqr = xd*xd + yd*yd;
    if (eradSqr <= _radSqr) {
        entry.nodes.push_back(this);
        return true;
    }

    return false;
}

void CircleNode::getSpec(float& x, float& y, float& rad) const {
    x = _x;
    y = _y;
    rad = _rad;
}

void CircleNode::findPosForColor(uint32_t& x, uint32_t& y,
                                 uint8_t r, uint8_t g, uint8_t b,
                                 const std::vector<Palette::Entry>& data,
                                 Palette::Rnd& rnd) {
    if (_children.size() > 0) {
        double minDiff = DBL_MAX;
        Node<CircleNode>* minChild = _children[0].get();
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

    int xs = 0;
    int ys = 0;

    for (auto i=0; i<256; ++i) {
        float xr = ((int)(rnd()%65537)-32768)/32768.0;
        float yr = ((int)(rnd()%65537)-32768)/32768.0;
        while (xs < 0 || xs >= 4096 || ys < 0 || ys >= 4096 || xr*xr+yr*yr > 1.0f) {
            xr = ((int)(rnd()%65537)-32768)/32768.0;
            yr = ((int)(rnd()%65537)-32768)/32768.0;
            xs = _x + _rad*xr;
            ys = _y + _rad*yr;
        }

        if (!data[ys*4096 + xs].filled) {
            x = xs;
            y = ys;
            //printf("A[%u, %u] %llu\n", x, y, _nFreeEntries);
            return;
        }
    }

    float xd = 0.0f;
    float yd = 0.0f;
    int xmin = std::max((int)(_x-_rad-1), 0);
    int xmax = std::min((int)(_x+_rad+1), 4096);
    int ymin = std::max((int)(_y-_rad-1), _yfill);
    int ymax = std::min((int)(_y+_rad+1), 4096);
    for (int ys=ymin; ys<ymax; ++ys) {
        for (int xs=xmin; xs<xmax; ++xs) {
            xd = _x - xs;
            yd = _y - ys;

            if (xd*xd+yd*yd > _radSqr)
                continue;

            if (!data[ys*4096 + xs].filled) {
                x = xs;
                y = ys;
                _yfill = ys;
                return;
            }

        }
    }

    printf("ERROR: [%u, %u] %llu\n", x, y, _nFreeEntries);
}
