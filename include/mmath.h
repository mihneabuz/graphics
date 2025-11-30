#ifndef MMATH_H
#define MMATH_H

#include <math.h>
#include <stdio.h>

static inline float radians(float degrees) {
    return degrees * acos(-1.) / 180.;
}

static inline float degrees(float radians) {
    return radians * 180. / acos(1.);
}

static inline float clamp(float value, float min, float max) {
    if (value < min)
        value = min;

    if (value > max)
        value = max;

    return value;
}

typedef struct vec2 {
    float x, y;
} vec2;

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

static inline vec3 vec3_neg(vec3 v) {
    return (vec3){
        .x = -v.x,
        .y = -v.y,
        .z = -v.z,
    };
}

static inline vec3 vec3_scale(vec3 v, float value) {
    return (vec3){
        .x = v.x * value,
        .y = v.y * value,
        .z = v.z * value,
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

static inline vec3 vec3_norm(vec3 v) {
    float len = sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
    return (vec3){v.x / len, v.y / len, v.z / len};
}

static inline vec3 vec3_cross(vec3 a, vec3 b) {
    return (vec3){
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x,
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

static inline void vec4_debug(vec4* v) {
    printf("[ %f ]\n[ %f ]\n[ %f ]\n[ %f ]\n", v->x, v->y, v->z, v->w);
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

static inline void mat4_debug(mat4* m) {
    printf("[ %f  %f  %f  %f ]\n", m->x.x, m->y.x, m->z.x, m->w.x);
    printf("[ %f  %f  %f  %f ]\n", m->x.y, m->y.y, m->z.y, m->w.y);
    printf("[ %f  %f  %f  %f ]\n", m->x.z, m->y.z, m->z.z, m->w.z);
    printf("[ %f  %f  %f  %f ]\n", m->x.w, m->y.w, m->z.w, m->w.w);
}

static inline mat4 mat4_trans(mat4 m) {
    return (mat4){
        {m.x.x, m.y.x, m.z.x, m.w.x},
        {m.x.y, m.y.y, m.z.y, m.w.y},
        {m.x.z, m.y.z, m.z.z, m.w.z},
        {m.x.w, m.y.w, m.z.w, m.w.w},
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
    float top = near * tangent;
    float right = top * aspect_ratio;

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
            a.x.x * b.x.x + a.y.x * b.x.y + a.z.x * b.x.z + a.w.x * b.x.w,
            a.x.y * b.x.x + a.y.y * b.x.y + a.z.y * b.x.z + a.w.y * b.x.w,
            a.x.z * b.x.x + a.y.z * b.x.y + a.z.z * b.x.z + a.w.z * b.x.w,
            a.x.w * b.x.x + a.y.w * b.x.y + a.z.w * b.x.z + a.w.w * b.x.w,
        },
        {
            a.x.x * b.y.x + a.y.x * b.y.y + a.z.x * b.y.z + a.w.x * b.y.w,
            a.x.y * b.y.x + a.y.y * b.y.y + a.z.y * b.y.z + a.w.y * b.y.w,
            a.x.z * b.y.x + a.y.z * b.y.y + a.z.z * b.y.z + a.w.z * b.y.w,
            a.x.w * b.y.x + a.y.w * b.y.y + a.z.w * b.y.z + a.w.w * b.y.w,
        },
        {
            a.x.x * b.z.x + a.y.x * b.z.y + a.z.x * b.z.z + a.w.x * b.z.w,
            a.x.y * b.z.x + a.y.y * b.z.y + a.z.y * b.z.z + a.w.y * b.z.w,
            a.x.z * b.z.x + a.y.z * b.z.y + a.z.z * b.z.z + a.w.z * b.z.w,
            a.x.w * b.z.x + a.y.w * b.z.y + a.z.w * b.z.z + a.w.w * b.z.w,
        },
        {
            a.x.x * b.w.x + a.y.x * b.w.y + a.z.x * b.w.z + a.w.x * b.w.w,
            a.x.y * b.w.x + a.y.y * b.w.y + a.z.y * b.w.z + a.w.y * b.w.w,
            a.x.z * b.w.x + a.y.z * b.w.y + a.z.z * b.w.z + a.w.z * b.w.w,
            a.x.w * b.w.x + a.y.w * b.w.y + a.z.w * b.w.z + a.w.w * b.w.w,
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

static inline mat4 look_at(vec3 pos, vec3 target, vec3 up) {
    vec3 d = vec3_norm(vec3_sub(pos, target));
    vec3 r = vec3_norm(vec3_cross(up, d));
    vec3 u = vec3_cross(d, r);

    mat4 a = {
        {r.x, u.x, d.x, 0},
        {r.y, u.y, d.y, 0},
        {r.z, u.z, d.z, 0},
        {0, 0, 0, 1},
    };

    mat4 b = translate(vec3_neg(pos));

    return mat4_mul(a, b);
}

#endif
