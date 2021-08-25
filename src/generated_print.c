void Log(vec2 *object)
{
LogS("vec2\n");
LogS("");
LogS("{\n");
LogS("  ");
LogS("x : %f", object->x);
LogS(",\n");
LogS("  ");
LogS("y : %f", object->y);
LogS(",\n");
LogS("");
LogS("}");
LogS("\n\n");
}

void Log(vec2i *object)
{
LogS("vec2i\n");
LogS("");
LogS("{\n");
LogS("  ");
LogS("x : %i", object->x);
LogS(",\n");
LogS("  ");
LogS("y : %i", object->y);
LogS(",\n");
LogS("");
LogS("}");
LogS("\n\n");
}

void Log(vec3 *object)
{
LogS("vec3\n");
LogS("");
LogS("{\n");
LogS("  ");
LogS("x : %f", object->x);
LogS(",\n");
LogS("  ");
LogS("y : %f", object->y);
LogS(",\n");
LogS("  ");
LogS("z : %f", object->z);
LogS(",\n");
LogS("");
LogS("}");
LogS("\n\n");
}

void Log(vec3i *object)
{
LogS("vec3i\n");
LogS("");
LogS("{\n");
LogS("  ");
LogS("x : %i", object->x);
LogS(",\n");
LogS("  ");
LogS("y : %i", object->y);
LogS(",\n");
LogS("  ");
LogS("z : %i", object->z);
LogS(",\n");
LogS("");
LogS("}");
LogS("\n\n");
}

void Log(vec4 *object)
{
LogS("vec4\n");
LogS("");
LogS("{\n");
LogS("  ");
LogS("x : %f", object->x);
LogS(",\n");
LogS("  ");
LogS("y : %f", object->y);
LogS(",\n");
LogS("  ");
LogS("z : %f", object->z);
LogS(",\n");
LogS("  ");
LogS("w : %f", object->w);
LogS(",\n");
LogS("");
LogS("}");
LogS("\n\n");
}

void Log(vec4i *object)
{
LogS("vec4i\n");
LogS("");
LogS("{\n");
LogS("  ");
LogS("x : %i", object->x);
LogS(",\n");
LogS("  ");
LogS("y : %i", object->y);
LogS(",\n");
LogS("  ");
LogS("z : %i", object->z);
LogS(",\n");
LogS("  ");
LogS("w : %i", object->w);
LogS(",\n");
LogS("");
LogS("}");
LogS("\n\n");
}

void Log(Camera *object)
{
LogS("Camera\n");
LogS("");
LogS("{\n");
LogS("  ");
LogS("position\n");
LogS("  ");
LogS("{\n");
LogS("    ");
LogS("x : %f", object->position.x);
LogS(",\n");
LogS("    ");
LogS("y : %f", object->position.y);
LogS(",\n");
LogS("    ");
LogS("z : %f", object->position.z);
LogS(",\n");
LogS("  ");
LogS("}");
LogS(",\n");
LogS("  ");
LogS("direction\n");
LogS("  ");
LogS("{\n");
LogS("    ");
LogS("x : %f", object->direction.x);
LogS(",\n");
LogS("    ");
LogS("y : %f", object->direction.y);
LogS(",\n");
LogS("    ");
LogS("z : %f", object->direction.z);
LogS(",\n");
LogS("  ");
LogS("}");
LogS(",\n");
LogS("  ");
LogS("up\n");
LogS("  ");
LogS("{\n");
LogS("    ");
LogS("x : %f", object->up.x);
LogS(",\n");
LogS("    ");
LogS("y : %f", object->up.y);
LogS(",\n");
LogS("    ");
LogS("z : %f", object->up.z);
LogS(",\n");
LogS("  ");
LogS("}");
LogS(",\n");
LogS("");
LogS("}");
LogS("\n\n");
}

void Log(Sprite *object)
{
LogS("Sprite\n");
LogS("");
LogS("{\n");
LogS("  ");
LogS("main_sprite : %i", object->main_sprite);
LogS(",\n");
LogS("  ");
LogS("  ");
LogS("name : %s", object->name);
LogS(",\n");
LogS("  ");
LogS("data : %p", object->data);
LogS(",\n");
LogS("  ");
LogS("width : %i", object->width);
LogS(",\n");
LogS("  ");
LogS("height : %i", object->height);
LogS(",\n");
LogS("  ");
LogS("channels : %i", object->channels);
LogS(",\n");
LogS("  ");
LogS("id : %i", object->id);
LogS(",\n");
LogS("  ");
LogS("slot : %i", object->slot);
LogS(",\n");
LogS("  ");
LogS("type\n");
LogS("  ");
LogS("{\n");
LogS("    ");
LogS("    ");
LogS("  ");
LogS("}");
LogS(",\n");
LogS("");
LogS("}");
LogS("\n\n");
}

void Log(UiWindow *object)
{
LogS("UiWindow\n");
LogS("");
LogS("{\n");
LogS("  ");
LogS("position\n");
LogS("  ");
LogS("{\n");
LogS("    ");
LogS("x : %f", object->position.x);
LogS(",\n");
LogS("    ");
LogS("y : %f", object->position.y);
LogS(",\n");
LogS("  ");
LogS("}");
LogS(",\n");
LogS("  ");
LogS("size\n");
LogS("  ");
LogS("{\n");
LogS("    ");
LogS("x : %f", object->size.x);
LogS(",\n");
LogS("    ");
LogS("y : %f", object->size.y);
LogS(",\n");
LogS("  ");
LogS("}");
LogS(",\n");
LogS("  ");
LogS("layer : %i", object->layer);
LogS(",\n");
LogS("  ");
LogS("id : %s", object->id);
LogS(",\n");
LogS("  ");
LogS("");
LogS("}");
LogS("\n\n");
}

void Log(Render *object)
{
LogS("Render\n");
LogS("");
LogS("{\n");
LogS("  ");
LogS("sprite : %i", object->sprite);
LogS(",\n");
LogS("  ");
LogS("color\n");
LogS("  ");
LogS("{\n");
LogS("    ");
LogS("r : %i", object->color.r);
LogS(",\n");
LogS("    ");
LogS("g : %i", object->color.g);
LogS(",\n");
LogS("    ");
LogS("b : %i", object->color.b);
LogS(",\n");
LogS("    ");
LogS("a : %i", object->color.a);
LogS(",\n");
LogS("  ");
LogS("}");
LogS(",\n");
LogS("  ");
LogS("layer : %f", object->layer);
LogS(",\n");
LogS("");
LogS("}");
LogS("\n\n");
}

void Log(Transform *object)
{
LogS("Transform\n");
LogS("");
LogS("{\n");
LogS("  ");
LogS("position\n");
LogS("  ");
LogS("{\n");
LogS("    ");
LogS("x : %f", object->position.x);
LogS(",\n");
LogS("    ");
LogS("y : %f", object->position.y);
LogS(",\n");
LogS("  ");
LogS("}");
LogS(",\n");
LogS("  ");
LogS("scale\n");
LogS("  ");
LogS("{\n");
LogS("    ");
LogS("x : %f", object->scale.x);
LogS(",\n");
LogS("    ");
LogS("y : %f", object->scale.y);
LogS(",\n");
LogS("  ");
LogS("}");
LogS(",\n");
LogS("  ");
LogS("rotation\n");
LogS("  ");
LogS("{\n");
LogS("    ");
LogS("x : %f", object->rotation.x);
LogS(",\n");
LogS("    ");
LogS("y : %f", object->rotation.y);
LogS(",\n");
LogS("  ");
LogS("}");
LogS(",\n");
LogS("");
LogS("}");
LogS("\n\n");
}

void Log(Rigidbody *object)
{
LogS("Rigidbody\n");
LogS("");
LogS("{\n");
LogS("  ");
LogS("velocity\n");
LogS("  ");
LogS("{\n");
LogS("    ");
LogS("x : %f", object->velocity.x);
LogS(",\n");
LogS("    ");
LogS("y : %f", object->velocity.y);
LogS(",\n");
LogS("  ");
LogS("}");
LogS(",\n");
LogS("  ");
LogS("acceleration\n");
LogS("  ");
LogS("{\n");
LogS("    ");
LogS("x : %f", object->acceleration.x);
LogS(",\n");
LogS("    ");
LogS("y : %f", object->acceleration.y);
LogS(",\n");
LogS("  ");
LogS("}");
LogS(",\n");
LogS("  ");
LogS("mass : %f", object->mass);
LogS(",\n");
LogS("");
LogS("}");
LogS("\n\n");
}

void Log(Particle *object)
{
LogS("Particle\n");
LogS("");
LogS("{\n");
LogS("  ");
LogS("position\n");
LogS("  ");
LogS("{\n");
LogS("    ");
LogS("x : %f", object->position.x);
LogS(",\n");
LogS("    ");
LogS("y : %f", object->position.y);
LogS(",\n");
LogS("  ");
LogS("}");
LogS(",\n");
LogS("  ");
LogS("velocity\n");
LogS("  ");
LogS("{\n");
LogS("    ");
LogS("x : %f", object->velocity.x);
LogS(",\n");
LogS("    ");
LogS("y : %f", object->velocity.y);
LogS(",\n");
LogS("  ");
LogS("}");
LogS(",\n");
LogS("  ");
LogS("acceleration\n");
LogS("  ");
LogS("{\n");
LogS("    ");
LogS("x : %f", object->acceleration.x);
LogS(",\n");
LogS("    ");
LogS("y : %f", object->acceleration.y);
LogS(",\n");
LogS("  ");
LogS("}");
LogS(",\n");
LogS("  ");
LogS("size\n");
LogS("  ");
LogS("{\n");
LogS("    ");
LogS("x : %f", object->size.x);
LogS(",\n");
LogS("    ");
LogS("y : %f", object->size.y);
LogS(",\n");
LogS("  ");
LogS("}");
LogS(",\n");
LogS("");
LogS("}");
LogS("\n\n");
}

