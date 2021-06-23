global_variable const char* basic_vertex_shader =
R"(
#version 330 core
layout (location = 0) in vec3 att_pos;
layout (location = 1) in vec2 att_uv;
layout (location = 2) in vec4 att_color;
layout (location = 3) in float att_tex_id;

out vec2 uv;
out vec4 color;
out float tex_id;

uniform mat4 u_viewproj;

void main()
{
   gl_Position = u_viewproj * vec4(att_pos, 1.0f);
   uv = att_uv;
   color = att_color;
   tex_id = att_tex_id;
}
)";

global_variable const char* basic_fragment_shader =
R"(
#version 330 core

out vec4 frag_color;

uniform sampler2D u_textures[9];

in vec2 uv;
in vec4 color;
in float tex_id;

void main()
{
    highp int index = int(tex_id);
    if (index == 0)
    {
        frag_color = color;
    }
    else if (index == 1)
    {
        frag_color = color * texture(u_textures[1], uv);
    }
    else if (index == 2)
    {
        frag_color = color * texture(u_textures[2], uv);
    }
    else if (index == 3)
    {
        frag_color = color * texture(u_textures[3], uv);
    }
    else if (index == 4)
    {
        frag_color = color * texture(u_textures[4], uv);
    }
}

)";

global_variable const char* colorpicker_fragment_shader =
R"(
#version 330 core

out vec4 frag_color;

in vec2 uv;
in float tex_id;

uniform vec4 col;
uniform vec4 col2;

void main()
{
    highp int index = int(tex_id);
    frag_color = col + col2;
}

)";

global_variable const char* test_shader =
R"(
#version 330 core

out vec4 frag_color;

uniform vec2 u_size;

in vec2 uv;
in vec4 color;
in float tex_id;


void main()
{
    vec2 st = gl_FragCoord.xy / u_size;

    float test = st.y * (1.0f - st.x);

    vec3 col = color.xyz * test;
    frag_color = vec4(col,1.0f);
}

)";




