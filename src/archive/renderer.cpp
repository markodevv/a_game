struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    f32 shininess;

    b8 has_texture;
};

struct Light
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Mesh
{
    VertexData* vertices;
    u32 num_vertices;

    u32* indices;
    u32 num_indices;

    i32 texture_index;

    Material material;
};

struct Model
{
    Mesh* meshes;
    u32 num_meshes;

    SpriteHandle loaded_sprites[8];
    u32 num_textures;
};

global_variable const char* vertex_shader_3D =
R"(
#version 330 core
layout (location = 0) in vec3 att_pos;
layout (location = 1) in vec3 att_normal;
layout (location = 2) in vec2 att_uv;
layout (location = 3) in float att_texid;

out vec3 frag_pos;
out vec3 normal;
out vec2 uv;
out float tex_id;

uniform mat4 u_viewproj;
uniform mat4 u_transform;
uniform mat4 u_normal_trans;

void main()
{
    vec4 frag = u_viewproj * u_transform * vec4(att_pos, 1.0f);
    uv = att_uv;
    normal = mat3(u_normal_trans) * att_normal;
    tex_id = att_texid;

    gl_Position = frag;

    frag_pos = frag.xyz;
}
)";

global_variable const char* fragment_shader_3D =
R"(
#version 330 core

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;

    bool has_texture;
};

struct Light
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec4 frag_color;

in vec3 frag_pos;
in vec3 normal;
in vec2 uv;
in float tex_id;

uniform vec3 u_cam_pos;
uniform vec3 u_light_pos;
uniform Material u_material;
uniform Light u_light;

uniform sampler2D u_textures[32];


void main()
{
    vec3 norm = normalize(normal);

    vec3 light_dir = normalize(u_light_pos - frag_pos);
    float diff = max(dot(light_dir, norm), 0.0f);

    vec3 view_dir = normalize(u_cam_pos - frag_pos);
    vec3 reflected = reflect(-light_dir, norm);

    float spec = pow(max(dot(reflected, view_dir), 0.0f), u_material.shininess);

    vec3 ambient = u_light.ambient * u_material.ambient;
    vec3 diffuse = u_light.diffuse * (u_material.diffuse * diff);
    vec3 specular = u_light.specular * (u_material.specular * spec);


    if (u_material.has_texture)
    {
        diffuse = u_light.diffuse * (texture(u_textures[0], uv) * diff).xyz;
        frag_color = vec4(specular + diffuse + ambient, 1.0f);
    }
    else
    {
        frag_color = vec4(diffuse + ambient + specular, 1.0f);
    }
}
)";
internal void
set_material_uniform(u32 shader, Material* material)
{
    i32 amb, spec, diff, shin, tex;
    amb = opengl_get_uniform_location(shader, "u_material.ambient");
    spec = opengl_get_uniform_location(shader, "u_material.specular");
    diff = opengl_get_uniform_location(shader, "u_material.diffuse");
    shin = opengl_get_uniform_location(shader, "u_material.shininess");
    tex =  opengl_get_uniform_location(shader, "u_material.has_texture");

    glUniform3fv(amb, 1, material->ambient.data);
    glUniform3fv(spec, 1, material->specular.data);
    glUniform3fv(diff, 1, material->diffuse.data);
    glUniform1f(shin, material->shininess);
    glUniform1i(tex, material->has_texture);
}

internal void
set_light_uniform(u32 shader, Light* light)
{
    i32 amb, spec, diff;
    amb = opengl_get_uniform_location(shader, "u_light.ambient");
    spec = opengl_get_uniform_location(shader, "u_light.specular");
    diff = opengl_get_uniform_location(shader, "u_light.diffuse");

    glUniform3fv(amb, 1, light->ambient.data);
    glUniform3fv(spec, 1, light->specular.data);
    glUniform3fv(diff, 1, light->diffuse.data);
}

internal void
opengl_draw_model(Renderer* ren, Model* model, vec3 position, vec3 size)
{
   
     mat4 transfrom = mat4_translate(position) *
                      mat4_scale(size);
  
     mat4 mvp = ren->projection * ren->view * transfrom;
     mat4 normal_transform = mat4_transpose(mat4_inverse(transfrom));
     b8 do_transpose = true;
  
     i32 transform_loc = opengl_get_uniform_location(ren->shader_program_3D, "u_transform");
     glUniformMatrix4fv(transform_loc, 1, do_transpose, (f32*)transfrom.data);
  
     i32 normal_loc =  opengl_get_uniform_location(ren->shader_program_3D, "u_normal_trans");
     glUniformMatrix4fv(normal_loc, 1, do_transpose, (f32*)normal_transform.data);
  

     for (u32 i = 0; i < model->num_meshes; ++i)
     {
        Mesh* mesh = &model->meshes[i];
        set_material_uniform(ren->shader_program_3D, &mesh->material);
        if (mesh->material.has_texture)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, model->loaded_images[mesh->texture_index].id);
        }
        glBindVertexArray(mesh->VAO);
  
  
        if (mesh->indices)
        {
            glDrawElements(GL_TRIANGLES, mesh->num_indices, GL_UNSIGNED_INT, 0);
        }
        else
        {
            glDrawArrays(GL_TRIANGLES, 0, mesh->num_vertices);
        }
     }

   
}

// Loading models with ASSIMP


internal b8
load_material_texture(Assets* assets,
                      Model* model,
                      char* path,
                      aiMaterial* material, 
                      aiTextureType texture_type)
{

    b8 result = false;
    char* sprite_path;
    aiString str;
    if (aiGetMaterialTexture(material, texture_type, 0, &str) == AI_SUCCESS)
    {
        TemporaryArena temp_memory = begin_temporary_memory(&assets->arena);
        result = true;
        b8 skip = false;
        char* sprite_name = const_cast<char*>(str.C_Str());

        sprite_path = PushMemory(&assets->arena, char, 256);
        string_copy(sprite_path, path, last_backslash_index(path) + 1);
        string_append(sprite_path, sprite_name, (u32)str.length);

        for (u32 j = 0; j < model->num_textures; ++j)
        {
            // If texture already loaded
            if (sprite_is_loaded(assets, sprite_path))
            {
                skip = true;
            }
        }
            
        end_temporary_memory(&temp_memory);

        if (!skip)
        {

            model->loaded_sprites[model->num_textures] = stb_load_sprite(assets, sprite_path);

            ++model->num_textures;
        }

    }

    return result;
}

internal Model
load_3D_model(Assets* assets, char* path)
{
    const aiScene* scene = aiImportFile(path, aiProcess_CalcTangentSpace |
                                              aiProcess_Triangulate      |
                                              aiProcess_SortByPType);
    Model out = {};

    if (scene)
    {

        out.num_meshes = scene->mNumMeshes;
        out.meshes = PushMemory(&assets->arena, Mesh, out.num_meshes);

        for (u32 mesh_index = 0; mesh_index < out.num_meshes; ++mesh_index)
        {
            aiMesh* ai_mesh = scene->mMeshes[mesh_index];
            Mesh* mesh = out.meshes + mesh_index;

            out.meshes[mesh_index].num_vertices = ai_mesh->mNumVertices;
            out.meshes[mesh_index].vertices = PushMemory(&assets->arena, VertexData, ai_mesh->mNumVertices);
            VertexData* vertices = out.meshes[mesh_index].vertices;

            for (u32 vertex_index = 0; vertex_index < ai_mesh->mNumVertices; ++vertex_index)
            {
                vertices->position = V3(ai_mesh->mVertices[vertex_index].x,
                                        ai_mesh->mVertices[vertex_index].y,
                                        ai_mesh->mVertices[vertex_index].z);

                vertices->normal = V3(ai_mesh->mNormals[vertex_index].x,
                                      ai_mesh->mNormals[vertex_index].y,
                                      ai_mesh->mNormals[vertex_index].z);

                if (ai_mesh->mTextureCoords[0])
                {
                    vertices->uv = V2(ai_mesh->mTextureCoords[0][vertex_index].x,
                                      ai_mesh->mTextureCoords[0][vertex_index].y);
                }

                ++vertices;
            }

            u32 num_indices = 0;
            for (u32 i = 0; i < ai_mesh->mNumFaces; ++i)
            {
                num_indices += ai_mesh->mFaces->mNumIndices;
            }
            out.meshes[mesh_index].num_indices = num_indices;
            out.meshes[mesh_index].indices = PushMemory(&assets->arena, u32, num_indices);

            u32 indices_index = 0;
            for (u32 i = 0; i < ai_mesh->mNumFaces; ++i)
            {
                aiFace* face = ai_mesh->mFaces + i;
                for (u32 j = 0; j < face->mNumIndices; ++j)
                {
                    mesh->indices[indices_index] = face->mIndices[j];
                    indices_index++;
                }
            }

            if (ai_mesh->mMaterialIndex >= 0)
            {
                aiMaterial* mtl = scene->mMaterials[ai_mesh->mMaterialIndex];

                aiColor4D color = {};
                if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &color))
                {
                    mesh->material.diffuse = V3(color.r, color.g, color.b);
                }
                if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_SPECULAR, &color))
                {
                    mesh->material.specular = V3(color.r, color.g, color.b);
                }
                if(AI_SUCCESS == aiGetMaterialColor(mtl, AI_MATKEY_COLOR_AMBIENT, &color))
                {
                    mesh->material.ambient = V3(color.r, color.g, color.b);
                }
                f32 shininess = 0.0f;
                mesh->material.shininess = 32.0f;
                if (AI_SUCCESS == aiGetMaterialFloat(mtl, 
                                                     AI_MATKEY_SHININESS_STRENGTH,
                                                     &shininess))
                {
                    PRINT("%.2f shininess", shininess);
                    mesh->material.shininess = shininess;
                }

                if (load_material_texture(assets, &out, path, mtl, aiTextureType_DIFFUSE))
                {
                    mesh->texture_index = out.num_textures - 1;
                    mesh->material.has_texture = true;
                }
            }
        }
        aiReleaseImport(scene);
    }
    else
    {
        PRINT("%s", aiGetErrorString());
        ASSERT(0);
    }


    return out;
}

internal void
opengl_load_model_to_gpu(Model* model)
{
    u32 stride = sizeof(f32) * 9;

    for (u32 mesh_index = 0; mesh_index < model->num_meshes; ++mesh_index)
    {
        Mesh* mesh = &model->meshes[mesh_index];

        glGenVertexArrays(1, &mesh->VAO);
        glGenBuffers(1, &mesh->VBO);

        glBindVertexArray(mesh->VAO);
        glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);

        if (mesh->indices)
        {
            glGenBuffers(1, &mesh->EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
            u32 size = mesh->num_indices * sizeof(u32);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, (void*)mesh->indices, GL_STATIC_DRAW);
        }

        u32 size = sizeof(VertexData) * mesh->num_vertices;
        glBufferData(GL_ARRAY_BUFFER, size, (void*)mesh->vertices, GL_STATIC_DRAW);
         
        // position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(0); 

        // normal
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3*sizeof(f32)));
        glEnableVertexAttribArray(1); 

        // uv
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6*sizeof(f32)));
        glEnableVertexAttribArray(2); 

        // texture id
        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, stride, (void*)(8*sizeof(f32)));
        glEnableVertexAttribArray(3); 

        glBindVertexArray(0);

    }
    for (u32 i = 0; i < model->num_textures; ++i)
    {
        opengl_load_texture(model->loaded_images + i,
                            texture_slot,
                            &model->loaded_images[i].id);
        texture_slot++;
    }

}
