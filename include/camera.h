#ifndef CAMERA_H
#define CAMERA_H

#include "mmath.h"

struct camera {
    float speed;
    vec3 pos;
    vec3 front;
    vec3 up;
};

static inline void camera_init(struct camera* cam, vec3 pos, vec3 front, vec3 up) {
    cam->pos = pos;
    cam->front = front;
    cam->up = up;
    cam->speed = 1;
}

static inline float camera_get_speed(struct camera* cam) {
    return cam->speed;
}

static inline void camera_set_speed(struct camera* cam, float speed) {
    cam->speed = speed;
}

static inline vec3 camera_pos(struct camera* cam) {
    return cam->pos;
}

static inline vec3 camera_front(struct camera* cam) {
    return cam->front;
}

static inline vec3 camera_back(struct camera* cam) {
    return vec3_neg(cam->front);
}

static inline vec3 camera_up(struct camera* cam) {
    return cam->up;
}

static inline vec3 camera_down(struct camera* cam) {
    return vec3_neg(cam->up);
}

static inline vec3 camera_right(struct camera* cam) {
    return vec3_norm(vec3_cross(cam->front, cam->up));
}

static inline vec3 camera_left(struct camera* cam) {
    return vec3_neg(camera_right(cam));
}

static inline mat4 camera_view(struct camera* cam) {
    return look_at(cam->pos, vec3_add(cam->pos, cam->front), cam->up);
}

static inline void camera_move_forward(struct camera* cam, float delta) {
    float speed = cam->speed * delta;
    cam->pos = vec3_add(cam->pos, vec3_scale(cam->front, speed));
}

static inline void camera_move_backward(struct camera* cam, float delta) {
    float speed = cam->speed * delta;
    cam->pos = vec3_sub(cam->pos, vec3_scale(cam->front, speed));
}

static inline void camera_move_left(struct camera* cam, float delta) {
    float speed = cam->speed * delta;
    cam->pos = vec3_sub(cam->pos, vec3_scale(camera_right(cam), speed));
}

static inline void camera_move_right(struct camera* cam, float delta) {
    float speed = cam->speed * delta;
    cam->pos = vec3_add(cam->pos, vec3_scale(camera_right(cam), speed));
}

static inline void camera_move_up(struct camera* cam, float delta) {
    float speed = cam->speed * delta;
    cam->pos = vec3_add(cam->pos, vec3_scale(cam->up, speed));
}

static inline void camera_move_down(struct camera* cam, float delta) {
    float speed = cam->speed * delta;
    cam->pos = vec3_sub(cam->pos, vec3_scale(cam->up, speed));
}

static inline void camera_set_front(struct camera* cam, float yaw, float pitch) {
    vec3 front = {
        .x = cos(radians(yaw)) * cos(radians(pitch)),
        .y = sin(radians(pitch)),
        .z = sin(radians(yaw)) * cos(radians(pitch)),
    };

    cam->front = vec3_norm(front);
}

#endif
