void Log(vec2 *object)
{
LogM("vec2\n");
LogM("");
LogM("{\n");
LogM("  ");
LogM("x : %f", object->x);
LogM(",\n");
LogM("  ");
LogM("y : %f", object->y);
LogM(",\n");
LogM("");
LogM("}");
LogM("\n\n");
}

void Log(vec2i *object)
{
LogM("vec2i\n");
LogM("");
LogM("{\n");
LogM("  ");
LogM("x : %i", object->x);
LogM(",\n");
LogM("  ");
LogM("y : %i", object->y);
LogM(",\n");
LogM("");
LogM("}");
LogM("\n\n");
}

void Log(vec3 *object)
{
LogM("vec3\n");
LogM("");
LogM("{\n");
LogM("  ");
LogM("x : %f", object->x);
LogM(",\n");
LogM("  ");
LogM("y : %f", object->y);
LogM(",\n");
LogM("  ");
LogM("z : %f", object->z);
LogM(",\n");
LogM("");
LogM("}");
LogM("\n\n");
}

void Log(vec3i *object)
{
LogM("vec3i\n");
LogM("");
LogM("{\n");
LogM("  ");
LogM("x : %i", object->x);
LogM(",\n");
LogM("  ");
LogM("y : %i", object->y);
LogM(",\n");
LogM("  ");
LogM("z : %i", object->z);
LogM(",\n");
LogM("");
LogM("}");
LogM("\n\n");
}

void Log(vec4 *object)
{
LogM("vec4\n");
LogM("");
LogM("{\n");
LogM("  ");
LogM("x : %f", object->x);
LogM(",\n");
LogM("  ");
LogM("y : %f", object->y);
LogM(",\n");
LogM("  ");
LogM("z : %f", object->z);
LogM(",\n");
LogM("  ");
LogM("w : %f", object->w);
LogM(",\n");
LogM("");
LogM("}");
LogM("\n\n");
}

void Log(vec4i *object)
{
LogM("vec4i\n");
LogM("");
LogM("{\n");
LogM("  ");
LogM("x : %i", object->x);
LogM(",\n");
LogM("  ");
LogM("y : %i", object->y);
LogM(",\n");
LogM("  ");
LogM("z : %i", object->z);
LogM(",\n");
LogM("  ");
LogM("w : %i", object->w);
LogM(",\n");
LogM("");
LogM("}");
LogM("\n\n");
}

void Log(Camera *object)
{
LogM("Camera\n");
LogM("");
LogM("{\n");
LogM("  ");
LogM("position\n");
LogM("  ");
LogM("{\n");
LogM("    ");
LogM("x : %f", object->position.x);
LogM(",\n");
LogM("    ");
LogM("y : %f", object->position.y);
LogM(",\n");
LogM("    ");
LogM("z : %f", object->position.z);
LogM(",\n");
LogM("  ");
LogM("}");
LogM(",\n");
LogM("  ");
LogM("direction\n");
LogM("  ");
LogM("{\n");
LogM("    ");
LogM("x : %f", object->direction.x);
LogM(",\n");
LogM("    ");
LogM("y : %f", object->direction.y);
LogM(",\n");
LogM("    ");
LogM("z : %f", object->direction.z);
LogM(",\n");
LogM("  ");
LogM("}");
LogM(",\n");
LogM("  ");
LogM("up\n");
LogM("  ");
LogM("{\n");
LogM("    ");
LogM("x : %f", object->up.x);
LogM(",\n");
LogM("    ");
LogM("y : %f", object->up.y);
LogM(",\n");
LogM("    ");
LogM("z : %f", object->up.z);
LogM(",\n");
LogM("  ");
LogM("}");
LogM(",\n");
LogM("");
LogM("}");
LogM("\n\n");
}

void Log(UiWindow *object)
{
LogM("UiWindow\n");
LogM("");
LogM("{\n");
LogM("  ");
LogM("position\n");
LogM("  ");
LogM("{\n");
LogM("    ");
LogM("x : %f", object->position.x);
LogM(",\n");
LogM("    ");
LogM("y : %f", object->position.y);
LogM(",\n");
LogM("  ");
LogM("}");
LogM(",\n");
LogM("  ");
LogM("size\n");
LogM("  ");
LogM("{\n");
LogM("    ");
LogM("x : %f", object->size.x);
LogM(",\n");
LogM("    ");
LogM("y : %f", object->size.y);
LogM(",\n");
LogM("  ");
LogM("}");
LogM(",\n");
LogM("  ");
LogM("layer : %i", object->layer);
LogM(",\n");
LogM("  ");
LogM("id : %s", object->id);
LogM(",\n");
LogM("  ");
LogM("");
LogM("}");
LogM("\n\n");
}

void Log(Render *object)
{
LogM("Render\n");
LogM("");
LogM("{\n");
LogM("  ");
LogM("sprite : %i", object->sprite);
LogM(",\n");
LogM("  ");
LogM("color\n");
LogM("  ");
LogM("{\n");
LogM("    ");
LogM("r : %i", object->color.r);
LogM(",\n");
LogM("    ");
LogM("g : %i", object->color.g);
LogM(",\n");
LogM("    ");
LogM("b : %i", object->color.b);
LogM(",\n");
LogM("    ");
LogM("a : %i", object->color.a);
LogM(",\n");
LogM("  ");
LogM("}");
LogM(",\n");
LogM("  ");
LogM("layer : %f", object->layer);
LogM(",\n");
LogM("");
LogM("}");
LogM("\n\n");
}

void Log(Transform *object)
{
LogM("Transform\n");
LogM("");
LogM("{\n");
LogM("  ");
LogM("position\n");
LogM("  ");
LogM("{\n");
LogM("    ");
LogM("x : %f", object->position.x);
LogM(",\n");
LogM("    ");
LogM("y : %f", object->position.y);
LogM(",\n");
LogM("  ");
LogM("}");
LogM(",\n");
LogM("  ");
LogM("scale\n");
LogM("  ");
LogM("{\n");
LogM("    ");
LogM("x : %f", object->scale.x);
LogM(",\n");
LogM("    ");
LogM("y : %f", object->scale.y);
LogM(",\n");
LogM("  ");
LogM("}");
LogM(",\n");
LogM("  ");
LogM("rotation\n");
LogM("  ");
LogM("{\n");
LogM("    ");
LogM("x : %f", object->rotation.x);
LogM(",\n");
LogM("    ");
LogM("y : %f", object->rotation.y);
LogM(",\n");
LogM("  ");
LogM("}");
LogM(",\n");
LogM("");
LogM("}");
LogM("\n\n");
}

void Log(Rigidbody *object)
{
LogM("Rigidbody\n");
LogM("");
LogM("{\n");
LogM("  ");
LogM("velocity\n");
LogM("  ");
LogM("{\n");
LogM("    ");
LogM("x : %f", object->velocity.x);
LogM(",\n");
LogM("    ");
LogM("y : %f", object->velocity.y);
LogM(",\n");
LogM("  ");
LogM("}");
LogM(",\n");
LogM("  ");
LogM("acceleration\n");
LogM("  ");
LogM("{\n");
LogM("    ");
LogM("x : %f", object->acceleration.x);
LogM(",\n");
LogM("    ");
LogM("y : %f", object->acceleration.y);
LogM(",\n");
LogM("  ");
LogM("}");
LogM(",\n");
LogM("  ");
LogM("mass : %f", object->mass);
LogM(",\n");
LogM("");
LogM("}");
LogM("\n\n");
}

void Log(Particle *object)
{
LogM("Particle\n");
LogM("");
LogM("{\n");
LogM("  ");
LogM("position\n");
LogM("  ");
LogM("{\n");
LogM("    ");
LogM("x : %f", object->position.x);
LogM(",\n");
LogM("    ");
LogM("y : %f", object->position.y);
LogM(",\n");
LogM("  ");
LogM("}");
LogM(",\n");
LogM("  ");
LogM("velocity\n");
LogM("  ");
LogM("{\n");
LogM("    ");
LogM("x : %f", object->velocity.x);
LogM(",\n");
LogM("    ");
LogM("y : %f", object->velocity.y);
LogM(",\n");
LogM("  ");
LogM("}");
LogM(",\n");
LogM("  ");
LogM("acceleration\n");
LogM("  ");
LogM("{\n");
LogM("    ");
LogM("x : %f", object->acceleration.x);
LogM(",\n");
LogM("    ");
LogM("y : %f", object->acceleration.y);
LogM(",\n");
LogM("  ");
LogM("}");
LogM(",\n");
LogM("  ");
LogM("size\n");
LogM("  ");
LogM("{\n");
LogM("    ");
LogM("x : %f", object->size.x);
LogM(",\n");
LogM("    ");
LogM("y : %f", object->size.y);
LogM(",\n");
LogM("  ");
LogM("}");
LogM(",\n");
LogM("");
LogM("}");
LogM("\n\n");
}

