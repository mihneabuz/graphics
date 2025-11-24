#include "image.h"
#include "list.h"
#include "map.h"
#include "mmath.h"
#include "mstring.h"
#include "queue.h"
#include "util.h"
#include "vector.h"

struct test {
    char* name;
    void (*f)();
};

static struct test current_test;
static int assert_failed;

void _assert(int64_t value, int64_t line) {
    if (!value) {
        printf("assertion failed at line %ld\n  value = %ld\n", line, value);
        assert_failed = 1;
    }
}

void _assert_eq(int64_t left, int64_t right, int line) {
    if (left != right) {
        printf("assertion failed at line %d\n  left != right: %ld != %ld\n", line, left, right);
        assert_failed = 1;
    }
}

#define assert(value) _assert(value, __LINE__);
#define assert_eq(left, right) _assert_eq(left, right, __LINE__);

void test_vec_alloc() {
    struct vector v;
    vec_init(&v, sizeof(int));

    assert_eq(v.size, 0);
    assert_eq(v.capacity, 0);
    assert(vec_front(&v) == nullptr);
    assert(vec_back(&v) == nullptr);

    vec_realloc(&v, 16);
    assert_eq(v.size, 0);
    assert_eq(v.capacity, 16);

    vec_resize(&v, 8);
    assert_eq(v.size, 8);
    assert_eq(v.capacity, 16);

    vec_uninit(&v);
}

void test_vec_push() {
    struct vector v;
    vec_init(&v, sizeof(int));

    for (int i = 0; i < 20; i++) {
        vec_push(&v, &i);
    }

    assert_eq(v.size, 20);
    assert_eq(v.capacity, 32);

    for (int i = 0; i < 20; i++) {
        assert_eq(*(int*)vec_item(&v, i), i);
    }

    int items[4] = {0, 1, 2, 3};
    vec_extend(&v, items, 4);

    assert_eq(v.size, 24);
    assert_eq(v.capacity, 32);

    for (int i = 0; i < 4; i++) {
        assert_eq(*(int*)vec_item(&v, i + 20), i);
    }

    vec_uninit(&v);
}

void test_vec_pop() {
    struct vector v;
    vec_init(&v, sizeof(int));

    for (int i = 0; i < 20; i++) {
        vec_push(&v, &i);
    }

    assert_eq(v.size, 20);
    assert_eq(v.capacity, 32);

    for (int i = 19; i >= 0; i--) {
        int j = 0;
        vec_pop(&v, &j);
        assert_eq(j, i);
    }

    assert_eq(v.size, 0);

    vec_uninit(&v);
}

void test_vec_full() {
    struct vector v;
    vec_init(&v, sizeof(int));

    vec_resize(&v, 32);
    assert_eq(v.size, 32);
    assert_eq(v.capacity, 32);

    for (uint32_t i = 0; i < v.size; i++)
        assert(vec_set(&v, i, &i));

    assert(vec_front(&v) != nullptr);
    assert(vec_back(&v) != nullptr);

    int first = 10, last = 20;
    *(int*)vec_front(&v) = first;
    vec_push(&v, &last);

    assert_eq(*(int*)vec_front(&v), first);
    assert_eq(*(int*)vec_back(&v), last);
    assert_eq(v.size, 33);
    assert_eq(v.capacity, 64);

    struct vector vv;
    vec_clone(&v, &vv);
    for (uint32_t i = 0; i < 33; i++) {
        int left = -1, right = -2;
        assert(vec_get(&v, i, &right));
        assert(vec_get(&vv, i, &left));
        assert_eq(left, right);
    }
    vec_uninit(&vv);

    vec_uninit(&v);
}

void test_queue_alloc() {
    struct queue q;
    queue_init(&q, sizeof(int));

    assert_eq(q.size, 0);
    assert_eq(q.capacity, 0);

    queue_realloc(&q, 32);

    assert_eq(q.size, 0);
    assert_eq(q.capacity, 32);

    queue_uninit(&q);
}

void test_queue_back() {
    struct queue q;
    queue_init(&q, sizeof(int));

    for (int i = 0; i < 5; i++)
        queue_push_back(&q, &i);

    assert_eq(q.size, 5);

    int out = 0;
    assert(queue_pop_back(&q, &out));
    assert_eq(out, 4);
    assert(queue_pop_back(&q, &out));
    assert_eq(out, 3);

    for (int i = 0; i < 20; i++)
        queue_push_back(&q, &i);

    assert_eq(q.size, 5 - 2 + 20);

    assert(queue_pop_back(&q, &out));
    assert_eq(out, 19);

    assert_eq(q.size, 5 - 2 + 20 - 1);

    queue_uninit(&q);
}

void test_queue_front() {
    struct queue q;
    queue_init(&q, sizeof(int));

    for (int i = 0; i < 5; i++)
        queue_push_front(&q, &i);

    assert_eq(q.size, 5);

    int out = 0;
    assert(queue_pop_front(&q, &out));
    assert_eq(out, 4);
    assert(queue_pop_front(&q, &out));
    assert_eq(out, 3);

    for (int i = 0; i < 20; i++)
        queue_push_front(&q, &i);

    assert_eq(q.size, 5 - 2 + 20);

    assert(queue_pop_front(&q, &out));
    assert_eq(out, 19);

    assert_eq(q.size, 5 - 2 + 20 - 1);

    queue_uninit(&q);
}

void test_queue_full() {
    struct queue q;
    queue_init(&q, sizeof(int));

    for (int i = 0; i < 20; i++) {
        if (i % 2) {
            queue_push_front(&q, &i);
        } else {
            queue_push_back(&q, &i);
        }
    }

    assert_eq(q.size, 20);

    int out = 0;
    assert(queue_pop_front(&q, &out));
    assert_eq(out, 19);
    assert(queue_pop_back(&q, &out));
    assert_eq(out, 18);
    assert(queue_pop_front(&q, &out));
    assert_eq(out, 17);
    assert(queue_pop_back(&q, &out));
    assert_eq(out, 16);

    for (int i = 0; i < 20; i++) {
        if (i % 2) {
            queue_push_front(&q, &i);
        } else {
            queue_push_back(&q, &i);
        }
    }

    assert(queue_pop_front(&q, &out));
    assert_eq(out, 19);
    assert(queue_pop_front(&q, &out));
    assert_eq(out, 17);
    assert(queue_pop_back(&q, &out));
    assert_eq(out, 18);
    assert(queue_pop_back(&q, &out));
    assert_eq(out, 16);

    while (queue_pop_back(&q, &out)) {
    }

    assert_eq(q.size, 0);

    queue_uninit(&q);
}

void test_list_inline() {
    struct list l;
    list_init(&l);

    assert(list_empty(&l));
    assert_eq(list_len(&l), 0);

    for (uint64_t i = 0; i < 5; i++)
        list_push_front(&l, (void*)i);

    assert_eq(list_len(&l), 5);

    uint64_t out = 0;
    assert(list_pop_front(&l, (void*)&out));
    assert_eq(out, 4);
    assert(list_pop_front(&l, (void*)&out));
    assert_eq(out, 3);

    assert_eq(list_len(&l), 3);

    for (uint64_t i = 0; i < 5; i++)
        list_push_front(&l, (void*)i);

    assert_eq(list_len(&l), 8);

    assert(list_pop_front(&l, (void*)&out));
    assert_eq(out, 4);
    assert(list_pop_front(&l, (void*)&out));
    assert_eq(out, 3);

    assert_eq(list_len(&l), 6);

    list_uninit(&l);
}

void test_list_pointer() {
    struct list l;
    list_init(&l);

    assert(list_empty(&l));
    assert_eq(list_len(&l), 0);

    for (int i = 0; i < 2; i++) {
        char* p = calloc(50, 1);
        memcpy(p, "Hello World!\n", 13);
        list_push_front(&l, p);
    }

    assert_eq(list_len(&l), 2);

    char* out1 = 0;
    assert(list_pop_front(&l, (void*)&out1));
    if (out1) {
        assert_eq(strlen(out1), 13);
        free(out1);
    }

    assert_eq(list_len(&l), 1);

    for (int i = 0; i < 3; i++) {
        char* p = calloc(50, 1);
        memcpy(p, "Hello World!\n", 13);
        list_push_front(&l, p);
    }

    assert_eq(list_len(&l), 4);

    char* out2 = 0;
    assert(list_pop_front(&l, (void*)&out2));
    if (out2) {
        assert_eq(strlen(out2), 13);
        free(out2);
    }

    assert_eq(list_len(&l), 3);

    list_for_each(&l, free);

    list_uninit(&l);
}

void test_map_alloc() {
    struct map m;
    map_init(&m, uint_comparator, uint_hasher);

    assert_eq(m.size, 0);
    assert_eq(m.buckets.capacity, 0);

    map_realloc(&m, 64);

    assert_eq(m.size, 0);
    assert_eq(m.buckets.size, 64);
    assert_eq(m.buckets.capacity, 64);

    map_realloc(&m, 200);

    assert_eq(m.size, 0);
    assert_eq(m.buckets.capacity, 256);

    map_uninit(&m);
}

void test_map_insert() {
    struct map m;
    map_init(&m, uint_comparator, uint_hasher);

    uint64_t items[10] = {7, 10, 16, 17, 19, 21, 33, 45, 77, 101};
    for (int i = 0; i < 10; i++)
        assert(map_insert(&m, (void*)items[i], (void*)items[i]) == nullptr);

    assert_eq(m.size, 10);

    struct map_entry* e = map_insert(&m, (void*)items[3], (void*)items[3]);
    assert(e != nullptr);
    assert_eq((int64_t)e->key, items[3]);
    assert_eq((int64_t)e->item, items[3]);
    free(e);

    e = map_find(&m, (void*)17);
    assert(e != nullptr);
    assert_eq((int64_t)e->key, 17);
    assert_eq((int64_t)e->item, 17);

    e = map_find(&m, (void*)101);
    assert(e != nullptr);
    assert_eq((int64_t)e->key, 101);
    assert_eq((int64_t)e->item, 101);

    e = map_find(&m, (void*)18);
    assert(e == nullptr);

    assert_eq(m.size, 10);

    map_uninit(&m);
}

void test_map_remove() {
    struct map m;
    map_init(&m, uint_comparator, uint_hasher);

    uint64_t items[10] = {7, 10, 16, 17, 19, 21, 33, 45, 77, 101};
    for (int i = 0; i < 10; i++)
        assert(map_insert(&m, (void*)items[i], (void*)items[i]) == nullptr);

    assert_eq(m.size, 10);

    assert(map_remove(&m, (void*)17));
    assert(map_remove(&m, (void*)21));
    assert(map_remove(&m, (void*)101));

    assert_eq(m.size, 7);

    assert(!map_remove(&m, (void*)18));

    assert_eq(m.size, 7);

    map_uninit(&m);
}

void test_map_str() {
    struct map m;
    map_init(&m, str_comparator, str_hasher);

    char* keys[4] = {"one", "two", "three", "four"};
    uint64_t vals[4] = {1, 2, 3, 4};

    for (int i = 0; i < 4; i++)
        assert(map_insert(&m, keys[i], (void*)vals[i]) == nullptr);

    struct map_entry* e = map_insert(&m, (void*)keys[3], (void*)vals[3]);
    assert(e != nullptr);
    assert_eq((int64_t)e->key, (int64_t)keys[3]);
    assert_eq((int64_t)e->item, vals[3]);
    free(e);

    e = map_find(&m, "one");
    assert(e != nullptr) assert_eq((int64_t)e->item, 1);

    e = map_find(&m, "four");
    assert(e != nullptr) assert_eq((int64_t)e->item, 4);

    e = map_find(&m, "five");
    assert(e == nullptr);

    assert(map_remove(&m, "two"));
    assert(!map_remove(&m, "seven"));

    map_uninit(&m);
}

void test_string_append() {
    struct string s;
    string_init(&s);

    string_append(&s, "Hello", 5);
    string_push(&s, ' ');
    string_append(&s, "World", 5);
    string_push(&s, '!');

    assert_eq(string_len(&s), 12);
    assert(strcmp(string_ptr(&s), "Hello World!") == 0);

    string_uninit(&s);
}

void test_string_pop() {
    struct string s;
    string_init(&s);

    string_append(&s, "Hello blahblahblah", 18);
    string_pop(&s, 4);
    string_pop(&s, 4);
    string_pop(&s, 4);
    string_append(&s, "World!", 6);

    assert_eq(string_len(&s), 12);
    assert(strcmp(string_ptr(&s), "Hello World!") == 0);

    string_uninit(&s);
}

static int feq(float a, float b) {
    return fabsf(a - b) < 1e-5f;
}

static int mat4_feq(mat4 a, mat4 b) {
    return feq(a.x.x, b.x.x) && feq(a.x.y, b.x.y) && feq(a.x.z, b.x.z) && feq(a.x.w, b.x.w) &&
           feq(a.y.x, b.y.x) && feq(a.y.y, b.y.y) && feq(a.y.z, b.y.z) && feq(a.y.w, b.y.w) &&
           feq(a.z.x, b.z.x) && feq(a.z.y, b.z.y) && feq(a.z.z, b.z.z) && feq(a.z.w, b.z.w) &&
           feq(a.w.x, b.w.x) && feq(a.w.y, b.w.y) && feq(a.w.z, b.w.z) && feq(a.w.w, b.w.w);
}

void test_mat4_mul_identity() {
    mat4 A = translate((vec3){1, 2, 3});
    assert(mat4_feq(mat4_mul(identity(), A), A));

    mat4 B = translate((vec3){4, 5, 6});
    assert(mat4_feq(mat4_mul(B, identity()), B));
}

void test_mat4_mul_associativity() {
    mat4 A = translate((vec3){1, 2, 3});
    mat4 B = rotate_y(45);
    mat4 C = scale((vec3){2, 2, 2});

    mat4 L = mat4_mul(mat4_mul(A, B), C);
    mat4 R = mat4_mul(A, mat4_mul(B, C));

    assert(mat4_feq(L, R));
}

void test_mat4_mul_chain() {
    mat4 A = identity();
    mat4_comp(&A, scale((vec3){2, 2, 2}));
    mat4_comp(&A, rotate_x(90));
    mat4_comp(&A, rotate_z(90));
    mat4_comp(&A, translate((vec3){1, 2, 3}));

    mat4 expected = {
        {0, -2, 0, 0},  //
        {0, 0, 2, 0},   //
        {-2, 0, 0, 0},  //
        {1, 2, 3, 1},   //
    };

    assert(mat4_feq(A, expected));
}

void test_image_png() {
    struct image img;
    assert(image_load("assets/blue.png", &img));

    assert_eq(img.width, 32);
    assert_eq(img.height, 32);
    assert_eq(img.channels, 3);
    assert(img.data != nullptr);

    if (img.data && img.width * img.height * img.channels > 100) {
        assert_eq(img.data[30 * 3 + 0], 18);   // R
        assert_eq(img.data[30 * 3 + 1], 66);   // G
        assert_eq(img.data[30 * 3 + 2], 216);  // B
    }

    image_uninit(&img);
}

#define test_func(fun)         \
    (struct test) {            \
        .name = #fun, .f = fun \
    }

int main() {
    struct vector tests;
    vec_init(&tests, sizeof(struct test));

    vec_push(&tests, &test_func(test_vec_alloc));
    vec_push(&tests, &test_func(test_vec_push));
    vec_push(&tests, &test_func(test_vec_pop));
    vec_push(&tests, &test_func(test_vec_full));

    vec_push(&tests, &test_func(test_queue_alloc));
    vec_push(&tests, &test_func(test_queue_back));
    vec_push(&tests, &test_func(test_queue_front));
    vec_push(&tests, &test_func(test_queue_full));

    vec_push(&tests, &test_func(test_list_inline));
    vec_push(&tests, &test_func(test_list_pointer));

    vec_push(&tests, &test_func(test_map_alloc));
    vec_push(&tests, &test_func(test_map_insert));
    vec_push(&tests, &test_func(test_map_remove));
    vec_push(&tests, &test_func(test_map_str));

    vec_push(&tests, &test_func(test_string_append));
    vec_push(&tests, &test_func(test_string_pop));

    vec_push(&tests, &test_func(test_mat4_mul_identity));
    vec_push(&tests, &test_func(test_mat4_mul_associativity));
    vec_push(&tests, &test_func(test_mat4_mul_chain));

    vec_push(&tests, &test_func(test_image_png));

    for (int i = 0; i < (int)tests.size; i++) {
        vec_get(&tests, i, &current_test);
        assert_failed = 0;

        printf("running %s...\n", current_test.name);
        current_test.f();

        if (assert_failed) {
            printf(C_RED "failed\n" C_NORMAL);
        } else {
            printf(C_GREEN "ok\n" C_NORMAL);
        }
    }

    vec_uninit(&tests);
}
