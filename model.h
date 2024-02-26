#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.h"

class Model {
private:
    std::vector<Vec3f> verts_;
    std::vector<std::vector<int> > faces_;
    std::vector<Vec2f> vt_; // Texture coordinates
    std::vector<std::vector<int> > tidx_;
public:
    Model(const char *filename);
    ~Model();
    int nverts();
    int nfaces();
    Vec3f vert(int i);
    Vec2f tex(int i); // New method to get a texture coordinate
    std::vector<int> face(int idx);
    std::vector<int> tidx(int idx);
};

#endif //__MODEL_H__