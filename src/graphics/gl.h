#ifndef IB_GL
#define IB_GL

/*
 * opengl function pointers
 * these are loaded at runtime with ib_gl_load()
 */

#include <GL/gl.h>

extern void   (APIENTRY * ib_glAttachShader)(GLuint prg, GLuint shader);
extern void   (APIENTRY * ib_glBindBuffer)(GLenum target, GLuint buf);
extern void   (APIENTRY * ib_glBindTexture)(GLenum target, GLuint tex);
extern void   (APIENTRY * ib_glBindVertexArray)(GLuint ar);
extern void   (APIENTRY * ib_glBufferData)(GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage);
extern void   (APIENTRY * ib_glBlendFunc)(GLenum src, GLenum dst);
extern void   (APIENTRY * ib_glClear)(GLbitfield mask);
extern void   (APIENTRY * ib_glClearColor)(GLclampf r, GLclampf g, GLclampf b, GLclampf a);
extern void   (APIENTRY * ib_glCompileShader)(GLuint shader);
extern GLuint (APIENTRY * ib_glCreateProgram)(void);
extern GLuint (APIENTRY * ib_glCreateShader)(GLenum type);
extern void   (APIENTRY * ib_glDeleteBuffers)(GLsizei n, const GLuint* p);
extern void   (APIENTRY * ib_glDeleteProgram)(GLuint prg);
extern void   (APIENTRY * ib_glDeleteShader)(GLuint shader);
extern void   (APIENTRY * ib_glDeleteTextures)(GLsizei n, const GLuint* p);
extern void   (APIENTRY * ib_glDeleteVertexArrays)(GLsizei n, const GLuint* p);
extern void   (APIENTRY * ib_glDisableVertexAttribArray)(GLuint index);
extern void   (APIENTRY * ib_glDrawArrays)(GLenum mode, GLint first, GLsizei count);
extern void   (APIENTRY * ib_glEnable)(GLenum cap);
extern void   (APIENTRY * ib_glEnableVertexAttribArray)(GLuint index);
extern void   (APIENTRY * ib_glGenBuffers)(GLsizei n, GLuint* p);
extern void   (APIENTRY * ib_glGenTextures)(GLsizei n, GLuint* textures);
extern void   (APIENTRY * ib_glGenVertexArrays)(GLsizei n, GLuint* p);
extern void   (APIENTRY * ib_glGetProgramiv)(GLuint prg, GLenum name, GLint* p);
extern void   (APIENTRY * ib_glGetShaderiv)(GLuint shd, GLenum name, GLint* p);
extern GLuint (APIENTRY * ib_glGetUniformLocation)(GLuint prg, const GLchar* name);
extern void   (APIENTRY * ib_glLinkProgram)(GLuint prg);
extern void   (APIENTRY * ib_glProgramGetInfoLog)(GLuint prg, GLsizei max_length, GLsizei* length, GLchar* log);
extern void   (APIENTRY * ib_glShaderGetInfoLog)(GLuint shader, GLsizei max_length, GLsizei* length, GLchar* log);
extern void   (APIENTRY * ib_glShaderSource)(GLuint shader, GLsizei count, const GLchar** strings, const GLint* lengths);
extern void   (APIENTRY * ib_glTexImage2D)(GLenum target, GLint level, GLint internalFormat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* data);
extern void   (APIENTRY * ib_glTexParameteri)(GLenum target, GLenum pname, GLint param);
extern void   (APIENTRY * ib_glUniform1f)(GLuint shader, GLfloat v);
extern void   (APIENTRY * ib_glUniform1i)(GLuint shader, GLint v);
extern void   (APIENTRY * ib_glUniform2fv)(GLuint shader, GLsizei count, const GLfloat* v);
extern void   (APIENTRY * ib_glUniform4fv)(GLuint shader, GLsizei count, const GLfloat* v);
extern void   (APIENTRY * ib_glUniformMatrix4fv)(GLuint shader, GLsizei count, const GLboolean transpose, const GLfloat* v);
extern void   (APIENTRY * ib_glUseProgram)(GLuint prg);
extern void   (APIENTRY * ib_glVertexAttribPointer)(GLuint ind, GLint size, GLenum type, GLboolean nm, GLsizei stride, const GLvoid* ptr);
extern void   (APIENTRY * ib_glViewport)(GLint x, GLint y, GLsizei width, GLsizei height);

/* load functions. SDL better be ready or this will explode */
int ib_gl_load();

#endif
