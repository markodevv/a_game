#if !defined(OPENGL_RENDERER_H)
#define OPENGL_RENDERER_H

typedef unsigned int GLenum;
typedef unsigned char GLboolean;
typedef unsigned int GLbitfield;
typedef signed char GLbyte;
typedef short GLshort;
typedef int GLint;
typedef int GLsizei;
typedef unsigned char GLubyte;
typedef unsigned short GLushort;
typedef unsigned int GLuint;
typedef float GLfloat;
typedef float GLclampf;
typedef void GLvoid;
typedef int GLintptrARB;
typedef int GLsizeiptrARB;
typedef int GLfixed;
typedef int GLclampx;

/* Extensions */
#define GL_OES_VERSION_1_0                1
#define GL_OES_read_format                1
#define GL_OES_compressed_paletted_texture 1

/* ClearBufferMask */
#define GL_DEPTH_BUFFER_BIT               0x00000100
#define GL_STENCIL_BUFFER_BIT             0x00000400
#define GL_COLOR_BUFFER_BIT               0x00004000

/* Boolean */
#define GL_FALSE                          0
#define GL_TRUE                           1

/* BeginMode */
#define GL_POINTS                         0x0000
#define GL_LINES                          0x0001
#define GL_LINE_LOOP                      0x0002
#define GL_LINE_STRIP                     0x0003
#define GL_TRIANGLES                      0x0004
#define GL_TRIANGLE_STRIP                 0x0005
#define GL_TRIANGLE_FAN                   0x0006

/* AlphaFunction */
#define GL_NEVER                          0x0200
#define GL_LESS                           0x0201
#define GL_EQUAL                          0x0202
#define GL_LEQUAL                         0x0203
#define GL_GREATER                        0x0204
#define GL_NOTEQUAL                       0x0205
#define GL_GEQUAL                         0x0206
#define GL_ALWAYS                         0x0207

/* BlendingFactorDest */
#define GL_ZERO                           0
#define GL_ONE                            1
#define GL_SRC_COLOR                      0x0300
#define GL_ONE_MINUS_SRC_COLOR            0x0301
#define GL_SRC_ALPHA                      0x0302
#define GL_ONE_MINUS_SRC_ALPHA            0x0303
#define GL_DST_ALPHA                      0x0304
#define GL_ONE_MINUS_DST_ALPHA            0x0305

/* BlendingFactorSrc */
/*      GL_ZERO */
/*      GL_ONE */
#define GL_DST_COLOR                      0x0306
#define GL_ONE_MINUS_DST_COLOR            0x0307
#define GL_SRC_ALPHA_SATURATE             0x0308
/*      GL_SRC_ALPHA */
/*      GL_ONE_MINUS_SRC_ALPHA */
/*      GL_DST_ALPHA */
/*      GL_ONE_MINUS_DST_ALPHA */

/* ColorMaterialFace */
/*      GL_FRONT_AND_BACK */

/* ColorMaterialParameter */
/*      GL_AMBIENT_AND_DIFFUSE */

/* ColorPointerType */
/*      GL_UNSIGNED_BYTE */
/*      GL_FLOAT */
/*      GL_FIXED */

/* CullFaceMode */
#define GL_FRONT                          0x0404
#define GL_BACK                           0x0405
#define GL_FRONT_AND_BACK                 0x0408

/* DepthFunction */
/*      GL_NEVER */
/*      GL_LESS */
/*      GL_EQUAL */
/*      GL_LEQUAL */
/*      GL_GREATER */
/*      GL_NOTEQUAL */
/*      GL_GEQUAL */
/*      GL_ALWAYS */

/* EnableCap */
#define GL_FOG                            0x0B60
#define GL_LIGHTING                       0x0B50
#define GL_TEXTURE_2D                     0x0DE1
#define GL_CULL_FACE                      0x0B44
#define GL_ALPHA_TEST                     0x0BC0
#define GL_BLEND                          0x0BE2
#define GL_COLOR_LOGIC_OP                 0x0BF2
#define GL_DITHER                         0x0BD0
#define GL_STENCIL_TEST                   0x0B90
#define GL_DEPTH_TEST                     0x0B71
/*      GL_LIGHT0 */
/*      GL_LIGHT1 */
/*      GL_LIGHT2 */
/*      GL_LIGHT3 */
/*      GL_LIGHT4 */
/*      GL_LIGHT5 */
/*      GL_LIGHT6 */
/*      GL_LIGHT7 */
#define GL_POINT_SMOOTH                   0x0B10
#define GL_LINE_SMOOTH                    0x0B20
#define GL_SCISSOR_TEST                   0x0C11
#define GL_COLOR_MATERIAL                 0x0B57
#define GL_NORMALIZE                      0x0BA1
#define GL_RESCALE_NORMAL                 0x803A
#define GL_POLYGON_OFFSET_FILL            0x8037
#define GL_VERTEX_ARRAY                   0x8074
#define GL_NORMAL_ARRAY                   0x8075
#define GL_COLOR_ARRAY                    0x8076
#define GL_TEXTURE_COORD_ARRAY            0x8078
#define GL_MULTISAMPLE                    0x809D
#define GL_SAMPLE_ALPHA_TO_COVERAGE       0x809E
#define GL_SAMPLE_ALPHA_TO_ONE            0x809F
#define GL_SAMPLE_COVERAGE                0x80A0

/* ErrorCode */
#define GL_NO_ERROR                       0
#define GL_INVALID_ENUM                   0x0500
#define GL_INVALID_VALUE                  0x0501
#define GL_INVALID_OPERATION              0x0502
#define GL_STACK_OVERFLOW                 0x0503
#define GL_STACK_UNDERFLOW                0x0504
#define GL_OUT_OF_MEMORY                  0x0505

/* FogMode */
/*      GL_LINEAR */
#define GL_EXP                            0x0800
#define GL_EXP2                           0x0801

/* FogParameter */
#define GL_FOG_DENSITY                    0x0B62
#define GL_FOG_START                      0x0B63
#define GL_FOG_END                        0x0B64
#define GL_FOG_MODE                       0x0B65
#define GL_FOG_COLOR                      0x0B66

/* FrontFaceDirection */
#define GL_CW                             0x0900
#define GL_CCW                            0x0901

/* GetPName */
#define GL_SMOOTH_POINT_SIZE_RANGE        0x0B12
#define GL_SMOOTH_LINE_WIDTH_RANGE        0x0B22
#define GL_ALIASED_POINT_SIZE_RANGE       0x846D
#define GL_ALIASED_LINE_WIDTH_RANGE       0x846E
#define GL_IMPLEMENTATION_COLOR_READ_TYPE_OES 0x8B9A
#define GL_IMPLEMENTATION_COLOR_READ_FORMAT_OES 0x8B9B
#define GL_MAX_LIGHTS                     0x0D31
#define GL_MAX_TEXTURE_SIZE               0x0D33
#define GL_MAX_MODELVIEW_STACK_DEPTH      0x0D36
#define GL_MAX_PROJECTION_STACK_DEPTH     0x0D38
#define GL_MAX_TEXTURE_STACK_DEPTH        0x0D39
#define GL_MAX_VIEWPORT_DIMS              0x0D3A
#define GL_MAX_ELEMENTS_VERTICES          0x80E8
#define GL_MAX_ELEMENTS_INDICES           0x80E9
#define GL_MAX_TEXTURE_UNITS              0x84E2
#define GL_NUM_COMPRESSED_TEXTURE_FORMATS 0x86A2
#define GL_COMPRESSED_TEXTURE_FORMATS     0x86A3
#define GL_SUBPIXEL_BITS                  0x0D50
#define GL_RED_BITS                       0x0D52
#define GL_GREEN_BITS                     0x0D53
#define GL_BLUE_BITS                      0x0D54
#define GL_ALPHA_BITS                     0x0D55
#define GL_DEPTH_BITS                     0x0D56
#define GL_STENCIL_BITS                   0x0D57

/* HintMode */
#define GL_DONT_CARE                      0x1100
#define GL_FASTEST                        0x1101
#define GL_NICEST                         0x1102

/* HintTarget */
#define GL_PERSPECTIVE_CORRECTION_HINT    0x0C50
#define GL_POINT_SMOOTH_HINT              0x0C51
#define GL_LINE_SMOOTH_HINT               0x0C52
#define GL_POLYGON_SMOOTH_HINT            0x0C53
#define GL_FOG_HINT                       0x0C54

/* LightModelParameter */
#define GL_LIGHT_MODEL_AMBIENT            0x0B53
#define GL_LIGHT_MODEL_TWO_SIDE           0x0B52

/* LightParameter */
#define GL_AMBIENT                        0x1200
#define GL_DIFFUSE                        0x1201
#define GL_SPECULAR                       0x1202
#define GL_POSITION                       0x1203
#define GL_SPOT_DIRECTION                 0x1204
#define GL_SPOT_EXPONENT                  0x1205
#define GL_SPOT_CUTOFF                    0x1206
#define GL_CONSTANT_ATTENUATION           0x1207
#define GL_LINEAR_ATTENUATION             0x1208
#define GL_QUADRATIC_ATTENUATION          0x1209

/* DataType */
#define GL_BYTE                           0x1400
#define GL_UNSIGNED_BYTE                  0x1401
#define GL_SHORT                          0x1402
#define GL_UNSIGNED_SHORT                 0x1403
#define GL_INT	                          0x1404
#define GL_UNSIGNED_INT	                  0x1405
#define GL_FLOAT                          0x1406
#define GL_FIXED                          0x140C

/* LogicOp */
#define GL_CLEAR                          0x1500
#define GL_AND                            0x1501
#define GL_AND_REVERSE                    0x1502
#define GL_COPY                           0x1503
#define GL_AND_INVERTED                   0x1504
#define GL_NOOP                           0x1505
#define GL_XOR                            0x1506
#define GL_OR                             0x1507
#define GL_NOR                            0x1508
#define GL_EQUIV                          0x1509
#define GL_INVERT                         0x150A
#define GL_OR_REVERSE                     0x150B
#define GL_COPY_INVERTED                  0x150C
#define GL_OR_INVERTED                    0x150D
#define GL_NAND                           0x150E
#define GL_SET                            0x150F

/* MaterialFace */
/*      GL_FRONT_AND_BACK */

/* MaterialParameter */
#define GL_EMISSION                       0x1600
#define GL_SHININESS                      0x1601
#define GL_AMBIENT_AND_DIFFUSE            0x1602
/*      GL_AMBIENT */
/*      GL_DIFFUSE */
/*      GL_SPECULAR */

/* MatrixMode */
#define GL_MODELVIEW                      0x1700
#define GL_PROJECTION                     0x1701
#define GL_TEXTURE                        0x1702

/* NormalPointerType */
/*      GL_BYTE */
/*      GL_SHORT */
/*      GL_FLOAT */
/*      GL_FIXED */

/* PixelFormat */
#define GL_RED                            0x1903
#define GL_ALPHA                          0x1906
#define GL_RGB                            0x1907
#define GL_RGBA                           0x1908
#define GL_LUMINANCE                      0x1909
#define GL_LUMINANCE_ALPHA                0x190A

/* PixelStoreParameter */
#define GL_UNPACK_ALIGNMENT               0x0CF5
#define GL_PACK_ALIGNMENT                 0x0D05

/* PixelType */
/*      GL_UNSIGNED_BYTE */
#define GL_UNSIGNED_SHORT_4_4_4_4         0x8033
#define GL_UNSIGNED_SHORT_5_5_5_1         0x8034
#define GL_UNSIGNED_SHORT_5_6_5           0x8363

/* ShadingModel */
#define GL_FLAT                           0x1D00
#define GL_SMOOTH                         0x1D01

/* StencilFunction */
/*      GL_NEVER */
/*      GL_LESS */
/*      GL_EQUAL */
/*      GL_LEQUAL */
/*      GL_GREATER */
/*      GL_NOTEQUAL */
/*      GL_GEQUAL */
/*      GL_ALWAYS */

/* StencilOp */
/*      GL_ZERO */
#define GL_KEEP                           0x1E00
#define GL_REPLACE                        0x1E01
#define GL_INCR                           0x1E02
#define GL_DECR                           0x1E03
/*      GL_INVERT */

/* StringName */
#define GL_VENDOR                         0x1F00
#define GL_RENDERER                       0x1F01
#define GL_VERSION                        0x1F02
#define GL_EXTENSIONS                     0x1F03

/* TexCoordPointerType */
/*      GL_SHORT */
/*      GL_FLOAT */
/*      GL_FIXED */
/*      GL_BYTE */

/* TextureEnvMode */
#define GL_MODULATE                       0x2100
#define GL_DECAL                          0x2101
/*      GL_BLEND */
#define GL_ADD                            0x0104
/*      GL_REPLACE */

/* TextureEnvParameter */
#define GL_TEXTURE_ENV_MODE               0x2200
#define GL_TEXTURE_ENV_COLOR              0x2201

/* TextureEnvTarget */
#define GL_TEXTURE_ENV                    0x2300

/* TextureMagFilter */
#define GL_NEAREST                        0x2600
#define GL_LINEAR                         0x2601

/* TextureMinFilter */
/*      GL_NEAREST */
/*      GL_LINEAR */
#define GL_NEAREST_MIPMAP_NEAREST         0x2700
#define GL_LINEAR_MIPMAP_NEAREST          0x2701
#define GL_NEAREST_MIPMAP_LINEAR          0x2702
#define GL_LINEAR_MIPMAP_LINEAR           0x2703

/* TextureParameterName */
#define GL_TEXTURE_MAG_FILTER             0x2800
#define GL_TEXTURE_MIN_FILTER             0x2801
#define GL_TEXTURE_WRAP_S                 0x2802
#define GL_TEXTURE_WRAP_T                 0x2803

/* TextureTarget */
/*      GL_TEXTURE_2D */

/* TextureUnit */
#define GL_TEXTURE0                       0x84C0
#define GL_TEXTURE1                       0x84C1
#define GL_TEXTURE2                       0x84C2
#define GL_TEXTURE3                       0x84C3
#define GL_TEXTURE4                       0x84C4
#define GL_TEXTURE5                       0x84C5
#define GL_TEXTURE6                       0x84C6
#define GL_TEXTURE7                       0x84C7
#define GL_TEXTURE8                       0x84C8
#define GL_TEXTURE9                       0x84C9
#define GL_TEXTURE10                      0x84CA
#define GL_TEXTURE11                      0x84CB
#define GL_TEXTURE12                      0x84CC
#define GL_TEXTURE13                      0x84CD
#define GL_TEXTURE14                      0x84CE
#define GL_TEXTURE15                      0x84CF
#define GL_TEXTURE16                      0x84D0
#define GL_TEXTURE17                      0x84D1
#define GL_TEXTURE18                      0x84D2
#define GL_TEXTURE19                      0x84D3
#define GL_TEXTURE20                      0x84D4
#define GL_TEXTURE21                      0x84D5
#define GL_TEXTURE22                      0x84D6
#define GL_TEXTURE23                      0x84D7
#define GL_TEXTURE24                      0x84D8
#define GL_TEXTURE25                      0x84D9
#define GL_TEXTURE26                      0x84DA
#define GL_TEXTURE27                      0x84DB
#define GL_TEXTURE28                      0x84DC
#define GL_TEXTURE29                      0x84DD
#define GL_TEXTURE30                      0x84DE
#define GL_TEXTURE31                      0x84DF

/* TextureWrapMode */
#define GL_REPEAT                         0x2901
#define GL_CLAMP_TO_EDGE                  0x812F

/* PixelInternalFormat */
#define GL_PALETTE4_RGB8_OES              0x8B90
#define GL_PALETTE4_RGBA8_OES             0x8B91
#define GL_PALETTE4_R5_G6_B5_OES          0x8B92
#define GL_PALETTE4_RGBA4_OES             0x8B93
#define GL_PALETTE4_RGB5_A1_OES           0x8B94
#define GL_PALETTE8_RGB8_OES              0x8B95
#define GL_PALETTE8_RGBA8_OES             0x8B96
#define GL_PALETTE8_R5_G6_B5_OES          0x8B97
#define GL_PALETTE8_RGBA4_OES             0x8B98
#define GL_PALETTE8_RGB5_A1_OES           0x8B99

/* VertexPointerType */
/*      GL_SHORT */
/*      GL_FLOAT */
/*      GL_FIXED */
/*      GL_BYTE */

/* LightName */
#define GL_LIGHT0                         0x4000
#define GL_LIGHT1                         0x4001
#define GL_LIGHT2                         0x4002
#define GL_LIGHT3                         0x4003
#define GL_LIGHT4                         0x4004
#define GL_LIGHT5                         0x4005
#define GL_LIGHT6                         0x4006
#define GL_LIGHT7                         0x4007


/*************************************************************/

#define GL_ARRAY_BUFFER                   0x8892 // Acquired from:
#define GL_ARRAY_BUFFER_BINDING           0x8894 // https://www.opengl.org/registry/api/GL/glext.h
#define GL_COLOR_ATTACHMENT0              0x8CE0
#define GL_COMPILE_STATUS                 0x8B81
#define GL_CURRENT_PROGRAM                0x8B8D
#define GL_DYNAMIC_DRAW                   0x88E8
#define GL_ELEMENT_ARRAY_BUFFER           0x8893
#define GL_ELEMENT_ARRAY_BUFFER_BINDING   0x8895
#define GL_LINK_STATUS                    0x8B82
#define GL_FRAGMENT_SHADER                0x8B30
#define GL_FRAMEBUFFER                    0x8D40
#define GL_FRAMEBUFFER_COMPLETE           0x8CD5
#define GL_FUNC_ADD                       0x8006
#define GL_INVALID_FRAMEBUFFER_OPERATION  0x0506
#define GL_MAJOR_VERSION                  0x821B
#define GL_MINOR_VERSION                  0x821C
#define GL_STATIC_DRAW                    0x88E4
#define GL_STREAM_DRAW                    0x88E0
#define GL_TEXTURE0                       0x84C0
#define GL_VERTEX_SHADER                  0x8B31

typedef char GLchar;
typedef ptrdiff_t GLintptr;
typedef ptrdiff_t GLsizeiptr;
#define GLDECL 
#define OpenGLFunction(ret, name, ...) typedef ret GLDECL name##proc(__VA_ARGS__); name##proc * ##name

OpenGLFunction(void,     glAttachShader,            GLuint program, GLuint shader);
OpenGLFunction(void,     glBindBuffer,              GLenum target, GLuint buffer);
OpenGLFunction(void,     glBindFramebuffer,         GLenum target, GLuint framebuffer);
OpenGLFunction(void,     glBufferData,              GLenum target, GLsizeiptr size, const GLvoid *data, GLenum usage);
OpenGLFunction(void,     glBufferSubData,           GLenum target, GLintptr offset, GLsizeiptr size, const GLvoid * data);
OpenGLFunction(GLenum,   glCheckFramebufferStatus,  GLenum target);
OpenGLFunction(void,     glClearBufferfv,           GLenum buffer, GLint drawbuffer, const GLfloat * value);
OpenGLFunction(void,     glCompileShader,           GLuint shader);
OpenGLFunction(GLuint,   glCreateProgram,           void);
OpenGLFunction(GLuint,   glCreateShader,            GLenum type);
OpenGLFunction(void,     glDeleteBuffers,           GLsizei n, const GLuint *buffers);
OpenGLFunction(void,     glDeleteFramebuffers,      GLsizei n, const GLuint *framebuffers);
OpenGLFunction(void,     glEnableVertexAttribArray, GLuint index);
OpenGLFunction(void,     glDrawBuffers,             GLsizei n, const GLenum *bufs);
OpenGLFunction(void,     glFramebufferTexture2D,    GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
OpenGLFunction(void,     glGenBuffers,              GLsizei n, GLuint *buffers);
OpenGLFunction(void,     glGenFramebuffers,         GLsizei n, GLuint * framebuffers);
OpenGLFunction(GLint,    glGetAttribLocation,       GLuint program, const GLchar *name);
OpenGLFunction(void,     glGetShaderInfoLog,        GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
OpenGLFunction(void,     glGetShaderiv,             GLuint shader, GLenum pname, GLint *params);
OpenGLFunction(GLint,    glGetUniformLocation,      GLuint program, const GLchar *name);
OpenGLFunction(void,     glLinkProgram,             GLuint program);
OpenGLFunction(void,     glShaderSource,            GLuint shader, GLsizei count, const GLchar* const *string, const GLint *length);
OpenGLFunction(void,     glUniform1i,               GLint location, GLint v0);
OpenGLFunction(void,     glUniform1f,               GLint location, GLfloat v0);
OpenGLFunction(void,     glUniform2f,               GLint location, GLfloat v0, GLfloat v1);
OpenGLFunction(void,     glUniform4f,               GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
OpenGLFunction(void,     glUniform1fv,              GLint location, GLsizei size, const GLfloat* value);
OpenGLFunction(void,     glUniform2fv,              GLint location, GLsizei size, const GLfloat* value);
OpenGLFunction(void,     glUniform3fv,              GLint location, GLsizei size, const GLfloat* value);
OpenGLFunction(void,     glUniform4fv,              GLint location, GLsizei size, const GLfloat* value);
OpenGLFunction(void,     glUniformMatrix4fv,        GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
OpenGLFunction(void,     glUseProgram,              GLuint program);
OpenGLFunction(void,     glVertexAttribPointer,     GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid * pointer);
OpenGLFunction(void,     glGenVertexArrays,         GLsizei n, GLuint *arrays);
OpenGLFunction(void,     glBindVertexArray,         GLuint array);
OpenGLFunction(void,     glDeleteShader,            GLuint shader);
OpenGLFunction(void,     glGetProgramInfoLog,       GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog);
OpenGLFunction(void,     glGetProgramiv,            GLuint program, GLenum pname, GLint *params);
OpenGLFunction(void,     glActiveTexture,           GLenum texture);
OpenGLFunction(void,     glGenTextures,             GLsizei n, GLuint* texture);
OpenGLFunction(void,     glBindTexture,             GLenum target, GLuint texture);
OpenGLFunction(void,     glPixelStorei,             GLenum pnamel, GLint param);
OpenGLFunction(void,     glTexParameteri,           GLenum target, GLenum pname, GLint param);
OpenGLFunction(void,     glTexImage2D,	            GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid * data);
OpenGLFunction(void,     glEnable,                  GLenum cap);
OpenGLFunction(void,     glDisable,                  GLenum cap);
OpenGLFunction(void,     glBlendFunc,               GLenum sfactor, GLenum dfactor);
OpenGLFunction(void,     glClearColor,              GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha);
OpenGLFunction(void,     glClear,                   GLbitfield mask);
OpenGLFunction(void,     glViewport,                GLint x, GLint y, GLsizei width, GLsizei height);
OpenGLFunction(void,     glDrawArrays,	            GLenum mode, GLint first, GLsizei count);
OpenGLFunction(void,     glDrawElements,            GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);



#endif
