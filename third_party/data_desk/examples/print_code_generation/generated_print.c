void PrintFoo(Foo *object)
{
printf("{ ");
printf("%i", object->a);
printf(", ");
printf("%f", object->b);
printf(", ");
printf("%s", object->c);
printf(", ");
printf("%p", object->d);
printf(", ");
printf("{ ");
printf("%i", object->e.a);
printf(", ");
printf("%f", object->e.b);
printf(", ");
printf("%s", object->e.c);
printf(", ");
printf("}");
printf(", ");
printf("{ ");
printf("%i", object->f.g);
printf(", ");
printf("%f", object->f.h);
printf(", ");
printf("}");
printf(", ");
printf("}");
}

void PrintBar(Bar *object)
{
printf("{ ");
printf("%i", object->g);
printf(", ");
printf("%f", object->h);
printf(", ");
printf("}");
}

