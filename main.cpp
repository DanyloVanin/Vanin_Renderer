#include <vector>
#include <cmath>
#include <limits>
#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

const int width  = 800;
const int height = 800;
const int depth  = 255;

Model *model = NULL;
TGAImage texture;
int *zbuffer = NULL;
Vec3f light_dir(0,0,-1);

Vec3f barycentric(Vec3i t0, Vec3i t1, Vec3i t2, Vec2i P) {
    Vec3f u = Vec3f(t2.x-t0.x, t1.x-t0.x, t0.x-P.x)^Vec3f(t2.y-t0.y, t1.y-t0.y, t0.y-P.y);
    /* `pts` and `P` has integer value as coordinates
       so `abs(u[2])` < 1 means `u[2]` is 0, that means
       triangle is degenerate, in this case return something with negative coordinates */
    if (std::abs(u.z)<1) return Vec3f(-1,1,1);
    return Vec3f(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z);
}

void triangle(Vec3i t0, Vec3i t1, Vec3i t2, Vec2f uv0, Vec2f uv1, Vec2f uv2, TGAImage &image, float intensity, TGAImage &texture, int *zbuffer) {
    if (t0.y==t1.y && t0.y==t2.y) return; // i dont care about degenerate triangles
    if (t0.y>t1.y) std::swap(t0, t1);
    if (t0.y>t2.y) std::swap(t0, t2);
    if (t1.y>t2.y) std::swap(t1, t2);
    int total_height = t2.y-t0.y;
    for (int i=0; i<total_height; i++) {
        bool second_half = i>t1.y-t0.y || t1.y==t0.y;
        int segment_height = second_half ? t2.y-t1.y : t1.y-t0.y;
        float alpha = (float)i/total_height;
        float beta  = (float)(i-(second_half ? t1.y-t0.y : 0))/segment_height; // be careful: with above conditions no division by zero here
        Vec3i A =               t0 + (t2-t0)*alpha;
        Vec3i B = second_half ? t1 + (t2-t1)*beta : t0 + (t1-t0)*beta;
        if (A.x>B.x) std::swap(A, B);
        for (int j=A.x; j<=B.x; j++) {
            float phi = B.x==A.x ? 1. : (float)(j-A.x)/(float)(B.x-A.x);
            Vec3i P = A + (B-A)*phi;
            P.x = j; P.y = t0.y+i; // a hack to fill holes (due to int cast precision problems)
            int idx = j+(t0.y+i)*width;
            Vec2i BBC_P(P.x,P.y);
            Vec3f babycentric = barycentric(t0, t1, t2, BBC_P);
            float text_x = babycentric.x*uv0.x + babycentric.y*uv1.x + babycentric.z*uv2.x;
            float text_y = babycentric.x*uv0.y + babycentric.y*uv1.y + babycentric.z*uv2.y;
            if (zbuffer[idx] < P.z) {
                zbuffer[idx] = P.z;
                TGAColor color = texture.get(text_x * texture.get_width(),  text_y * texture.get_height());
                TGAColor updatedColor(color.r*intensity, color.g*intensity, color.b*intensity, 255);
                image.set(P.x, P.y, updatedColor);
            }
        }
    }
}

int main(int argc, char** argv) {
    if (2==argc) {
        model = new Model(argv[1]);
    } else {
        model = new Model("../obj/african_head.obj");
    }
    texture.read_tga_file("../obj/african_head/african_head_diffuse.tga");
    texture.flip_vertically();

    zbuffer = new int[width*height];
    for (int i=0; i<width*height; i++) {
        zbuffer[i] = std::numeric_limits<int>::min();
    }

    { // draw the model
        TGAImage image(width, height, TGAImage::RGB);
        for (int i=0; i<model->nfaces(); i++) {
            std::vector<int> face = model->face(i);
            std::vector<int> tidx = model->tidx(i);
            Vec3i screen_coords[3];
            Vec3f world_coords[3];
            Vec2f text_coords[3];
            for (int j=0; j<3; j++) {
                Vec3f v = model->vert(face[j]);
                screen_coords[j] = Vec3i((v.x+1.)*width/2., (v.y+1.)*height/2., (v.z+1.)*depth/2.);
                world_coords[j]  = v;
                Vec2f text = model->tex(tidx[j]);
                text_coords[j] = text;
            }
            Vec3f n = (world_coords[2]-world_coords[0])^(world_coords[1]-world_coords[0]);
            n.normalize();
            float intensity = n*light_dir;
            if (intensity>0) {
                triangle(screen_coords[0], screen_coords[1], screen_coords[2],text_coords[0], text_coords[1], text_coords[2], image, intensity, texture, zbuffer);
            }
        }

        image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
        image.write_tga_file("../output/head_with_texture13.tga");
    }

    delete model;
    delete [] zbuffer;
    return 0;
}
