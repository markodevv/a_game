typedef struct Foo Foo;
struct Foo
{
    int a;
    float b;
    char *c;
    void *d;

    struct
    {
        int a;
        float b;
        char *c;
    }e;






    Bar f;
};
typedef struct Bar Bar;
struct Bar
{
    int g;
    float h;

    // @NoPrint
    void *i;
};
