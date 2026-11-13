#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { TYPE_EMPTY = 0, TYPE_INT, TYPE_FLOAT, TYPE_CHAR, TYPE_STRING } DataType;

typedef union {
    int int_value;
    float float_value;
    char char_value;
    char *string_value;
} Data;

typedef struct {
    Data data;
    DataType type;
} Container;

typedef enum {
    CONTAINER_OK = 0,
    CONTAINER_ERR_NULL,
    CONTAINER_ERR_TYPE,
    CONTAINER_ERR_ALLOC
} ContainerStatus;

/* Test hook: forces the next allocation to fail. */
static int g_fail_next_alloc = 0;
void container_test_fail_next_alloc(void) { g_fail_next_alloc = 1; }

/* Replacement for non-standard strdup; NULL on failure. */
static char *copy_text(const char *text)
{
    size_t len = strlen(text);
    char *buf;
    if (g_fail_next_alloc) {
        g_fail_next_alloc = 0;
        return NULL;
    }
    buf = (char *)malloc(len + 1);
    if (buf != NULL) {
        memcpy(buf, text, len + 1);
    }
    return buf;
}

/* ---- Lifecycle ---- */

void container_init(Container *c)
{
    if (c == NULL) return;
    memset(&c->data, 0, sizeof c->data);
    c->type = TYPE_EMPTY;
}

/* Frees owned memory and resets to TYPE_EMPTY; safe to repeat. */
void container_clear(Container *c)
{
    if (c == NULL) return;
    if (c->type == TYPE_STRING) free(c->data.string_value);
    container_init(c);
}

/* ---- Setters ---- */

ContainerStatus container_set_int(Container *c, int v)
{
    if (c == NULL) return CONTAINER_ERR_NULL;
    container_clear(c);
    c->data.int_value = v;
    c->type = TYPE_INT;
    return CONTAINER_OK;
}

ContainerStatus container_set_float(Container *c, float v)
{
    if (c == NULL) return CONTAINER_ERR_NULL;
    container_clear(c);
    c->data.float_value = v;
    c->type = TYPE_FLOAT;
    return CONTAINER_OK;
}

ContainerStatus container_set_char(Container *c, char v)
{
    if (c == NULL) return CONTAINER_ERR_NULL;
    container_clear(c);
    c->data.char_value = v;
    c->type = TYPE_CHAR;
    return CONTAINER_OK;
}

/* Allocates the copy first: the old value survives an allocation failure,
 * and passing the container's own string as 'value' is safe. */
ContainerStatus container_set_string(Container *c, const char *value)
{
    char *copy;
    if (c == NULL || value == NULL) return CONTAINER_ERR_NULL;
    copy = copy_text(value);
    if (copy == NULL) return CONTAINER_ERR_ALLOC;
    container_clear(c);
    c->data.string_value = copy;
    c->type = TYPE_STRING;
    return CONTAINER_OK;
}

/* ---- Getters (value returned via out-parameter) ---- */

#define DEFINE_GETTER(name, ctype, tag, member)                          \
ContainerStatus name(const Container *c, ctype *out)                     \
{                                                                        \
    if (c == NULL || out == NULL) return CONTAINER_ERR_NULL;             \
    if (c->type != tag) return CONTAINER_ERR_TYPE;                       \
    *out = c->data.member;                                               \
    return CONTAINER_OK;                                                 \
}

DEFINE_GETTER(container_get_int,    int,          TYPE_INT,    int_value)
DEFINE_GETTER(container_get_float,  float,        TYPE_FLOAT,  float_value)
DEFINE_GETTER(container_get_char,   char,         TYPE_CHAR,   char_value)
/* The string pointer is borrowed: do not free it. */
DEFINE_GETTER(container_get_string, const char *, TYPE_STRING, string_value)

/* ---- Deep copy ---- */

/* NULL -> ERR_NULL; self-copy -> OK no-op; on failure destination is unchanged. */
ContainerStatus container_copy(Container *dst, const Container *src)
{
    if (dst == NULL || src == NULL) return CONTAINER_ERR_NULL;
    if (dst == src) return CONTAINER_OK;
    switch (src->type) {
    case TYPE_EMPTY:  container_clear(dst); return CONTAINER_OK;
    case TYPE_INT:    return container_set_int(dst, src->data.int_value);
    case TYPE_FLOAT:  return container_set_float(dst, src->data.float_value);
    case TYPE_CHAR:   return container_set_char(dst, src->data.char_value);
    case TYPE_STRING: return container_set_string(dst, src->data.string_value);
    }
    return CONTAINER_ERR_TYPE;
}

/* ---- Display ---- */

const char *container_type_name(const Container *c)
{
    if (c == NULL) return "NULL";
    switch (c->type) {
    case TYPE_EMPTY:  return "EMPTY";
    case TYPE_INT:    return "INT";
    case TYPE_FLOAT:  return "FLOAT";
    case TYPE_CHAR:   return "CHAR";
    case TYPE_STRING: return "STRING";
    }
    return "UNKNOWN";
}

void container_print(const Container *c)
{
    if (c == NULL) { printf("(null container)"); return; }
    switch (c->type) {
    case TYPE_EMPTY:  printf("(empty)"); break;
    case TYPE_INT:    printf("%d", c->data.int_value); break;
    case TYPE_FLOAT:  printf("%g", (double)c->data.float_value); break;
    case TYPE_CHAR:   printf("'%c'", c->data.char_value); break;
    case TYPE_STRING: printf("\"%s\"", c->data.string_value); break;
    default:          printf("(corrupt)"); break;
    }
}

/* ---- Demonstration and tests ---- */

static int g_run = 0, g_failed = 0;

#define CHECK(cond, label)                                               \
    do {                                                                 \
        g_run++;                                                         \
        if (!(cond)) g_failed++;                                         \
        printf("  [%s] %s\n", (cond) ? "PASS" : "FAIL", (label));        \
    } while (0)

static void show(const char *prefix, const Container *c)
{
    printf("%s%-6s : ", prefix, container_type_name(c));
    container_print(c);
    printf("\n");
}

static void demo_part_a(void)
{
    Container c;
    int i; float f; char ch; const char *s;

    printf("=== Part A: one container, four types ===\n");
    container_init(&c);
    if (container_set_int(&c, 10) == CONTAINER_OK && container_get_int(&c, &i) == CONTAINER_OK)
        printf("Int: %d\n", i);
    if (container_set_float(&c, 3.14f) == CONTAINER_OK && container_get_float(&c, &f) == CONTAINER_OK)
        printf("Float: %f\n", (double)f);
    if (container_set_char(&c, 'A') == CONTAINER_OK && container_get_char(&c, &ch) == CONTAINER_OK)
        printf("Char: %c\n", ch);
    if (container_set_string(&c, "Hello, World!") == CONTAINER_OK &&
        container_get_string(&c, &s) == CONTAINER_OK)
        printf("String: %s\n", s);
    container_clear(&c);
    printf("\n");
}

static void demo_part_b(void)
{
    enum { COUNT = 6 };
    Container *items = (Container *)malloc(COUNT * sizeof *items);
    Container copy;
    const char *s = NULL;
    char prefix[16];
    size_t i;

    printf("=== Part B: array, deep copy, type transitions ===\n");
    if (items == NULL) { printf("allocation failed\n"); return; }
    for (i = 0; i < COUNT; i++) container_init(&items[i]);
    container_init(&copy);

    container_set_int(&items[0], 10);
    container_set_float(&items[1], 3.14f);
    container_set_char(&items[2], 'A');
    container_set_string(&items[3], "Hello");
    container_set_int(&items[4], 0);
    container_set_string(&items[5], "Data");

    for (i = 0; i < COUNT; i++) {
        sprintf(prefix, "[%zu] ", i);
        show(prefix, &items[i]);
    }

    printf("-- copy items[5], then change the original --\n");
    container_copy(&copy, &items[5]);
    container_set_string(&items[5], "Changed");
    show("original ", &items[5]);
    show("copy     ", &copy);
    CHECK(container_get_string(&copy, &s) == CONTAINER_OK && strcmp(s, "Data") == 0,
          "copy kept the initial text");

    printf("-- STRING -> INT -> STRING -> FLOAT --\n");
    container_set_string(&items[0], "first");  show("  ", &items[0]);
    container_set_int(&items[0], 42);          show("  ", &items[0]);
    container_set_string(&items[0], "second"); show("  ", &items[0]);
    container_set_float(&items[0], 2.5f);      show("  ", &items[0]);

    for (i = 0; i < COUNT; i++) container_clear(&items[i]);
    container_clear(&copy);
    free(items);
    printf("\n");
}

static void run_edge_tests(void)
{
    Container a, b;
    int i = -1; float f = -1.0f; char ch = 'x'; const char *s = NULL;
    char buf[16];

    printf("=== Edge-case tests ===\n");
    container_init(&a);
    container_init(&b);

    container_set_int(&a, 0);
    CHECK(container_get_int(&a, &i) == CONTAINER_OK && i == 0, "valid integer zero");

    CHECK(container_get_float(&a, &f) == CONTAINER_ERR_TYPE, "wrong-type getter -> ERR_TYPE");
    CHECK(container_get_char(&a, &ch) == CONTAINER_ERR_TYPE && ch == 'x',
          "wrong-type getter leaves output untouched");
    CHECK(container_get_int(NULL, &i) == CONTAINER_ERR_NULL, "NULL container -> ERR_NULL");
    CHECK(container_get_int(&a, NULL) == CONTAINER_ERR_NULL, "NULL out pointer -> ERR_NULL");
    CHECK(container_set_string(&a, NULL) == CONTAINER_ERR_NULL, "NULL text -> ERR_NULL");

    container_clear(&a);
    CHECK(a.type == TYPE_EMPTY && container_get_int(&a, &i) == CONTAINER_ERR_TYPE,
          "empty container: getter fails");
    show("  empty prints as: ", &a);

    CHECK(container_set_string(&a, "") == CONTAINER_OK &&
          container_get_string(&a, &s) == CONTAINER_OK && s[0] == '\0',
          "empty string is a valid string");

    container_set_string(&a, "keep me");
    container_test_fail_next_alloc();
    CHECK(container_set_string(&a, "new") == CONTAINER_ERR_ALLOC, "set_string alloc failure -> ERR_ALLOC");
    CHECK(container_get_string(&a, &s) == CONTAINER_OK && strcmp(s, "keep me") == 0,
          "previous string preserved after failed set");

    container_set_int(&b, 7);
    container_test_fail_next_alloc();
    CHECK(container_copy(&b, &a) == CONTAINER_ERR_ALLOC, "copy alloc failure -> ERR_ALLOC");
    CHECK(container_get_int(&b, &i) == CONTAINER_OK && i == 7,
          "destination preserved after failed copy");

    container_set_string(&a, "temporary");
    CHECK(container_set_int(&a, 99) == CONTAINER_OK && a.type == TYPE_INT,
          "switch STRING -> INT (old string freed)");

    container_set_string(&a, "clear me");
    container_clear(&a); container_clear(&a); container_clear(&a);
    CHECK(a.type == TYPE_EMPTY, "repeated clear is safe");

    container_set_string(&a, "original");
    container_copy(&b, &a);
    CHECK(a.data.string_value != b.data.string_value, "copy has a separate buffer");
    container_set_string(&a, "modified");
    container_get_string(&b, &s);
    CHECK(strcmp(s, "original") == 0, "changing source does not affect copy");
    container_clear(&a);
    CHECK(container_get_string(&b, &s) == CONTAINER_OK && strcmp(s, "original") == 0,
          "clearing source does not invalidate copy");

    CHECK(container_copy(&b, &b) == CONTAINER_OK &&
          container_get_string(&b, &s) == CONTAINER_OK && strcmp(s, "original") == 0,
          "self-copy is a safe no-op");
    CHECK(container_copy(&b, NULL) == CONTAINER_ERR_NULL, "copy from NULL -> ERR_NULL");

    container_set_string(&a, "same pointer");
    container_get_string(&a, &s);
    CHECK(container_set_string(&a, s) == CONTAINER_OK, "set_string with own buffer succeeds");
    container_get_string(&a, &s);
    CHECK(strcmp(s, "same pointer") == 0, "text intact after self-assignment");

    strcpy(buf, "temp");
    container_set_string(&a, buf);
    strcpy(buf, "XXXX");
    container_get_string(&a, &s);
    CHECK(strcmp(s, "temp") == 0, "container does not alias caller's buffer");

    container_clear(&a);
    container_clear(&b);
    printf("\n");
}

int main(void)
{
    demo_part_a();
    demo_part_b();
    run_edge_tests();
    printf("=== Summary: %d tests, %d failed ===\n", g_run, g_failed);
    return g_failed == 0 ? 0 : 1;
}