#ifndef MMATH_H
#define MMATH_H

#include <math.h>

static inline float radians(float degrees) {
    return degrees * acos(-1.) / 180.;
}

static inline float degrees(float radians) {
    return radians * 180. / acos(1.);
}

typedef struct vec3 {
    float x, y, z;
} vec3;

typedef struct vec4 {
    float x, y, z, w;
} vec4;

typedef struct mat3 {
    struct vec3 x, y, z;
} mat3;

typedef struct mat4 {
    struct vec4 x, y, z, w;
} mat4;

static inline vec3 vec3_new(float value) {
    return (vec3){
        .x = value,
        .y = value,
        .z = value,
    };
}

static inline vec3 vec3_add(vec3 a, vec3 b) {
    return (vec3){
        .x = a.x + b.x,
        .y = a.y + b.y,
        .z = a.z + b.z,
    };
}

static inline vec3 vec3_sub(vec3 a, vec3 b) {
    return (vec3){
        .x = a.x - b.x,
        .y = a.y - b.y,
        .z = a.z - b.z,
    };
}

static inline vec4 vec4_new(float value) {
    return (vec4){
        .x = value,
        .y = value,
        .z = value,
        .w = value,
    };
}

static inline vec4 vec4_hom(vec3 v) {
    return (vec4){
        .x = v.x,
        .y = v.y,
        .z = v.z,
        .w = 1.,
    };
}

static inline vec4 vec4_add(vec4 a, vec4 b) {
    return (vec4){
        .x = a.x + b.x,
        .y = a.y + b.y,
        .z = a.z + b.z,
        .w = a.w + b.w,
    };
}

static inline vec4 vec4_sub(vec4 a, vec4 b) {
    return (vec4){
        .x = a.x - b.x,
        .y = a.y - b.y,
        .z = a.z - b.z,
        .w = a.w + b.w,
    };
}

static inline mat3 mat3_new(float value) {
    return (mat3){
        vec3_new(value),
        vec3_new(value),
        vec3_new(value),
    };
}

static inline mat4 mat4_new(float value) {
    return (mat4){
        vec4_new(value),
        vec4_new(value),
        vec4_new(value),
        vec4_new(value),
    };
}

static inline mat4 identity() {
    return (mat4){
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1},
    };
}

static inline mat4 translate(vec3 t) {
    return (mat4){
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {t.x, t.y, t.z, 1},
    };
}

static inline mat4 scale(vec3 s) {
    return (mat4){
        {s.x, 0, 0, 0},
        {0, s.y, 0, 0},
        {0, 0, s.z, 0},
        {0, 0, 0, 1},
    };
}

static inline mat4 rotate_x(float degrees) {
    float s = sin(radians(degrees));
    float c = cos(radians(degrees));
    return (mat4){
        {1, 0, 0, 0},
        {0, c, s, 0},
        {0, -s, c, 0},
        {0, 0, 0, 1},
    };
}

static inline mat4 rotate_y(float degrees) {
    float s = sin(radians(degrees));
    float c = cos(radians(degrees));
    return (mat4){
        {c, 0, -s, 0},
        {0, 1, 0, 0},
        {s, 0, c, 0},
        {0, 0, 0, 1},
    };
}

static inline mat4 rotate_z(float degrees) {
    float s = sin(radians(degrees));
    float c = cos(radians(degrees));
    return (mat4){
        {c, -s, 0, 0},
        {s, c, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1},
    };
}

static inline mat4 perspective(float fov, float aspect_ratio, float near, float far) {
    float tangent = tan(radians(fov / 2));
    float right = near * tangent;
    float top = right / aspect_ratio;

    mat4 m = mat4_new(0.);
    m.x.x = near / right;
    m.y.y = near / top;
    m.z.z = -(far + near) / (far - near);
    m.z.w = -1;
    m.w.z = -(2 * far * near) / (far - near);

    return m;
}

static inline mat4 mat4_mul(mat4 a, mat4 b) {
    return (mat4){
        {
            a.x.x * b.x.x + a.x.y * b.y.x + a.x.z * b.z.x + a.x.w * b.w.x,
            a.x.x * b.x.y + a.x.y * b.y.y + a.x.z * b.z.y + a.x.w * b.w.y,
            a.x.x * b.x.z + a.x.y * b.y.z + a.x.z * b.z.z + a.x.w * b.w.z,
            a.x.x * b.x.w + a.x.y * b.y.w + a.x.z * b.z.w + a.x.w * b.w.w,
        },
        {
            a.y.x * b.x.x + a.y.y * b.y.x + a.y.z * b.z.x + a.y.w * b.w.x,
            a.y.x * b.x.y + a.y.y * b.y.y + a.y.z * b.z.y + a.y.w * b.w.y,
            a.y.x * b.x.z + a.y.y * b.y.z + a.y.z * b.z.z + a.y.w * b.w.z,
            a.y.x * b.x.w + a.y.y * b.y.w + a.y.z * b.z.w + a.y.w * b.w.w,
        },
        {
            a.z.x * b.x.x + a.z.y * b.y.x + a.z.z * b.z.x + a.z.w * b.w.x,
            a.z.x * b.x.y + a.z.y * b.y.y + a.z.z * b.z.y + a.z.w * b.w.y,
            a.z.x * b.x.z + a.z.y * b.y.z + a.z.z * b.z.z + a.z.w * b.w.z,
            a.z.x * b.x.w + a.z.y * b.y.w + a.z.z * b.z.w + a.z.w * b.w.w,
        },
        {
            a.w.x * b.x.x + a.w.y * b.y.x + a.w.z * b.z.x + a.w.w * b.w.x,
            a.w.x * b.x.y + a.w.y * b.y.y + a.w.z * b.z.y + a.w.w * b.w.y,
            a.w.x * b.x.z + a.w.y * b.y.z + a.w.z * b.z.z + a.w.w * b.w.z,
            a.w.x * b.x.w + a.w.y * b.y.w + a.w.z * b.z.w + a.w.w * b.w.w,
        },
    };
}

static inline void mat4_comp(mat4* transform, mat4 m) {
    *transform = mat4_mul(m, *transform);
};

static inline vec4 mat4_apply(mat4 m, vec4 v) {
    return (vec4){
        .x = v.x * m.x.x + v.y * m.x.y + v.z * m.x.z + v.w * m.x.w,
        .y = v.x * m.y.x + v.y * m.y.y + v.z * m.y.z + v.w * m.y.w,
        .z = v.x * m.z.x + v.y * m.z.y + v.z * m.z.z + v.w * m.z.w,
        .w = v.x * m.w.x + v.y * m.w.y + v.z * m.w.z + v.w * m.w.w,
    };
}

#endif
