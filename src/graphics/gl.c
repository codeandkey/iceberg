#include "gl.h"
#include "../log.h"

#include <SDL2/SDL.h>

void   (APIENTRY * ib_glAttachShader)(GLuint prg, GLuint shader);
void   (APIENTRY * ib_glBindBuffer)(GLenum target, GLuint buf);
void   (APIENTRY * ib_glBindTexture)(GLenum target, GLuint tex);
void   (APIENTRY * ib_glBindVertexArray)(GLuint ar);
void   (APIENTRY * ib_glBufferData)(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage);
void   (APIENTRY * ib_glBlendFunc)(GLenum src, GLenum dst);
void   (APIENTRY * ib_glClear)(GLbitfield mask);
void   (APIENTRY * ib_glClearColor)(GLclampf r, GLclampf g, GLclampf b, GLclampf a);
void   (APIENTRY * ib_glCompileShader)(GLuint shader);
GLuint (APIENTRY * ib_glCreateProgram)(void);
GLuint (APIENTRY * ib_glCreateShader)(GLenum type);
void   (APIENTRY * ib_glDeleteBuffers)(GLsizei n, const GLuint* p);
void   (APIENTRY * ib_glDeleteProgram)(GLuint prg);
void   (APIENTRY * ib_glDeleteShader)(GLuint shader);
void   (APIENTRY * ib_glDeleteTextures)(GLsizei n, const GLuint* p);
void   (APIENTRY * ib_glDeleteVertexArrays)(GLsizei n, const GLuint* p);
void   (APIENTRY * ib_glDisableVertexAttribArray)(GLuint index);
void   (APIENTRY * ib_glDrawArrays)(GLenum mode, GLint first, GLsizei count);
void   (APIENTRY * ib_glEnable)(GLenum cap);
void   (APIENTRY * ib_glEnableVertexAttribArray)(GLuint index);
void   (APIENTRY * ib_glGenBuffers)(GLsizei n, GLuint* p);
void   (APIENTRY * ib_glGenTextures)(GLsizei n, GLuint* textures);
void   (APIENTRY * ib_glGenVertexArrays)(GLsizei n, GLuint* p);
void   (APIENTRY * ib_glGetProgramiv)(GLuint prg, GLenum name, GLint* p);
void   (APIENTRY * ib_glGetShaderiv)(GLuint shd, GLenum name, GLint* p);
GLuint (APIENTRY * ib_glGetUniformLocation)(GLuint prg, const GLchar* name);
void   (APIENTRY * ib_glLinkProgram)(GLuint prg);
void   (APIENTRY * ib_glProgramGetInfoLog)(GLuint prg, GLsizei max_length, GLsizei* length, GLchar* log);
void   (APIENTRY * ib_glShaderGetInfoLog)(GLuint shader, GLsizei max_length, GLsizei* length, GLchar* log);
void   (APIENTRY * ib_glShaderSource)(GLuint shader, GLsizei count, const GLchar** strings, const GLint* lengths);
void   (APIENTRY * ib_glTexImage2D)(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* data);
void   (APIENTRY * ib_glTexParameteri)(GLenum target, GLenum pname, GLint param);
void   (APIENTRY * ib_glUniform1f)(GLuint shader, GLfloat v);
void   (APIENTRY * ib_glUniform1i)(GLuint shader, GLint v);
void   (APIENTRY * ib_glUniform2fv)(GLuint shader, GLsizei count, const GLfloat* v);
void   (APIENTRY * ib_glUniform4fv)(GLuint shader, GLsizei count, const GLfloat* v);
void   (APIENTRY * ib_glUniformMatrix4fv)(GLuint shader, GLsizei count, const GLboolean transpose, const GLfloat* v);
void   (APIENTRY * ib_glUseProgram)(GLuint prg);
void   (APIENTRY * ib_glVertexAttribPointer)(GLuint ind, GLint size, GLenum type, GLboolean nm, GLsizei stride, const GLvoid* ptr);
void   (APIENTRY * ib_glViewport)(GLint x, GLint y, GLsizei width, GLsizei height);

int ib_gl_load() {
    /* nice long function to load all the GL function pointers */

    ib_glAttachShader =             SDL_GL_GetProcAddress("glAttachShader");
    ib_glBindBuffer =               SDL_GL_GetProcAddress("glBindBuffer");
    ib_glBindTexture =              SDL_GL_GetProcAddress("glBindTexture");
    ib_glBindVertexArray =          SDL_GL_GetProcAddress("glBindVertexArray");
    ib_glBlendFunc =                SDL_GL_GetProcAddress("glBlendFunc");
    ib_glBufferData =               SDL_GL_GetProcAddress("glBufferData");
    ib_glClear =                    SDL_GL_GetProcAddress("glClear");
    ib_glClearColor =               SDL_GL_GetProcAddress("glClearColor");
    ib_glCompileShader =            SDL_GL_GetProcAddress("glCompileShader");
    ib_glCreateProgram =            SDL_GL_GetProcAddress("glCreateProgram");
    ib_glCreateShader =             SDL_GL_GetProcAddress("glCreateShader");
    ib_glDeleteBuffers =            SDL_GL_GetProcAddress("glDeleteBuffers");
    ib_glDeleteProgram =            SDL_GL_GetProcAddress("glDeleteProgram");
    ib_glDeleteShader =             SDL_GL_GetProcAddress("glDeleteShader");
    ib_glDeleteTextures =           SDL_GL_GetProcAddress("glDeleteTextures");
    ib_glDeleteVertexArrays =       SDL_GL_GetProcAddress("glDeleteVertexArrays");
    ib_glDisableVertexAttribArray = SDL_GL_GetProcAddress("glDisableVertexAttribArray");
    ib_glDrawArrays =               SDL_GL_GetProcAddress("glDrawArrays");
    ib_glEnable =                   SDL_GL_GetProcAddress("glEnable");
    ib_glEnableVertexAttribArray =  SDL_GL_GetProcAddress("glEnableVertexAttribArray");
    ib_glGenBuffers =               SDL_GL_GetProcAddress("glGenBuffers");
    ib_glGenTextures =              SDL_GL_GetProcAddress("glGenTextures");
    ib_glGenVertexArrays =          SDL_GL_GetProcAddress("glGenVertexArrays");
    ib_glGetProgramiv =             SDL_GL_GetProcAddress("glGetProgramiv");
    ib_glGetShaderiv =              SDL_GL_GetProcAddress("glGetShaderiv");
    ib_glGetUniformLocation =       SDL_GL_GetProcAddress("glGetUniformLocation");
    ib_glLinkProgram =              SDL_GL_GetProcAddress("glLinkProgram");
    ib_glProgramGetInfoLog =        SDL_GL_GetProcAddress("glProgramGetInfoLog");
    ib_glShaderGetInfoLog =         SDL_GL_GetProcAddress("glShaderGetInfoLog");
    ib_glShaderSource =             SDL_GL_GetProcAddress("glShaderSource");
    ib_glTexImage2D =               SDL_GL_GetProcAddress("glTexImage2D");
    ib_glTexParameteri =            SDL_GL_GetProcAddress("glTexParameteri");
    ib_glUniform1f =                SDL_GL_GetProcAddress("glUniform1f");
    ib_glUniform1i =                SDL_GL_GetProcAddress("glUniform1i");
    ib_glUniform2fv =               SDL_GL_GetProcAddress("glUniform2fv");
    ib_glUniform4fv =               SDL_GL_GetProcAddress("glUniform4fv");
    ib_glUniformMatrix4fv =         SDL_GL_GetProcAddress("glUniformMatrix4fv");
    ib_glUseProgram =               SDL_GL_GetProcAddress("glUseProgram");
    ib_glVertexAttribPointer =      SDL_GL_GetProcAddress("glVertexAttribPointer");
    ib_glViewport =                 SDL_GL_GetProcAddress("glViewport");

    /* well that was a task */

    return ib_ok("loaded GL functions");
}
