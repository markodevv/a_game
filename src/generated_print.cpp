void print(char* name, vec2 var)
{
printf("@%s\n", name);
printf("x : %.2f\n", var.x);
printf("y : %.2f\n", var.y);
printf("-----------\n");
}

void print(char* name, vec2i var)
{
printf("@%s\n", name);
printf("x : %d\n", var.x);
printf("y : %d\n", var.y);
printf("-----------\n");
}

void print(char* name, vec3 var)
{
printf("@%s\n", name);
printf("x : %.2f\n", var.x);
printf("y : %.2f\n", var.y);
printf("z : %.2f\n", var.z);
printf("-----------\n");
}

void print(char* name, vec3i var)
{
printf("@%s\n", name);
printf("x : %d\n", var.x);
printf("y : %d\n", var.y);
printf("z : %d\n", var.z);
printf("-----------\n");
}

void print(char* name, vec4 var)
{
printf("@%s\n", name);
printf("x : %.2f\n", var.x);
printf("y : %.2f\n", var.y);
printf("z : %.2f\n", var.z);
printf("w : %.2f\n", var.w);
printf("-----------\n");
}

void print(char* name, vec4i var)
{
printf("@%s\n", name);
printf("x : %d\n", var.x);
printf("y : %d\n", var.y);
printf("z : %d\n", var.z);
printf("w : %d\n", var.w);
printf("-----------\n");
}

void print(char* name, Color var)
{
printf("@%s\n", name);
printf("r : %d\n", var.r);
printf("g : %d\n", var.g);
printf("b : %d\n", var.b);
printf("a : %d\n", var.a);
printf("-----------\n");
}

void print(char* name, VertexData var)
{
printf("@%s\n", name);
print("position", var.position);
print("uv", var.uv);
print("color", var.color);
printf("texture_slot : %.2f\n", var.texture_slot);
printf("-----------\n");
}

