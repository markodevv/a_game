#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include "generated.h"

int main(void)
{
    v1_Entry v1_entry = {0};

    v1_entry.to_remove = 12;

    v1_entry.type = v1_EntryType_C;

    v1_entry.flags = v1_EntryFlags_Value2 | v1_EntryFlags_Value3;

    v1_entry.color.components.r = 255;
    v1_entry.color.components.g = 128;
    v1_entry.color.components.b = 1;
    v1_entry.color.components.a = 42;

    v1_entry.p.x = 2;
    v1_entry.p.y = 3;

    Entry entry = convert_v1_Entry_to_Entry(v1_entry);

    assert(entry.type == EntryType_C);
    assert(entry.flags == (EntryFlags_Value2 | EntryFlags_Value3));
    assert(entry.color.components.r == 255);
    assert(entry.color.components.g == 128);
    assert(entry.color.components.b == 1);
    assert(entry.color.components.a == 42);
    assert(v1_entry.p.x == 2);
    assert(v1_entry.p.y == 3);

    printf("Success!\n");

    return 0;
}
