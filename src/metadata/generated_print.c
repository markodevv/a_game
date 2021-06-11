void print_vec3 (vec3 var)
{
printf("%.2f", var.x);
printf("%.2f", var.y);
printf("%.2f", var.z);

}

void print_TestStruct (TestStruct var)
{
print_vec3(var.v);
printf("%d", var.i);

}

void print_UiWindow (UiWindow var)
{
print_vec3(var.position);
print_vec3(var.size);
printf("%lu", var.layer);

}

