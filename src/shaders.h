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

global_variable const char* hue_quad_shader =
R"(
#version 330 core

out vec4 frag_color;

uniform vec2 u_size;
uniform vec2 u_position;

#define PI 3.1415926f

in vec2 uv;
in vec4 color;
in float tex_id;

vec3 hsb2rgb(in vec3 c)
{
    vec3 rgb = clamp(abs(mod(c.x*6.0+vec3(0.0,4.0,2.0),
                             6.0)-3.0)-1.0,
                     0.0,
                     1.0 );
    rgb = rgb*rgb*(3.0-2.0*rgb);
    return c.z * mix(vec3(1.0), rgb, c.y);
}


void main()
{
    vec2 st = (gl_FragCoord.xy - u_position) / u_size;


    vec3 col = hsb2rgb(vec3(st.y, 1.0f, 1.0f));
    

    frag_color = vec4(col, 1.0f);
}

)";


global_variable const char* sat_brigh_quad_shader =
R"(
#version 330 core

out vec4 frag_color;

uniform vec2 u_size;
uniform vec2 u_position;
uniform float u_hue;

#define PI 3.1415926f

in vec2 uv;
in vec4 color;
in float tex_id;

vec3 hsb2rgb(in vec3 c)
{
    vec3 rgb = clamp(abs(mod(c.x*6.0+vec3(0.0,4.0,2.0),
                             6.0)-3.0)-1.0,
                     0.0,
                     1.0 );
    rgb = rgb*rgb*(3.0-2.0*rgb);
    return c.z * mix(vec3(1.0), rgb, c.y);
}


void main()
{
    vec2 st = (gl_FragCoord.xy - u_position) / u_size;

    
    vec3 col = hsb2rgb(vec3(u_hue, st.x, st.y));

    frag_color = vec4(col, 1.0f);
}

)";
