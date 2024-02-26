# Renderer Implementation

### Triangle rasterization

Lesson: https://github.com/ssloy/tinyrenderer/wiki/Lesson-2:-Triangle-rasterization-and-back-face-culling

Code for creating triangles:
https://github.com/ssloy/tinyrenderer/blob/024ad4619b824f9179c86dc144145e2b8b155f52/main.cpp

Moved to using the following idea:
```
triangle(vec2 points[3]) { 
    vec2 bbox[2] = find_bounding_box(points); 
    for (each pixel in the bounding box) { 
        if (inside(points, pixel)) { 
            put_pixel(pixel); 
        } 
    } 
}
```

Need to use barycentric coordinates for solving this one.