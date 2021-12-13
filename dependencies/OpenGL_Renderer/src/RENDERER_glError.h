#ifndef RENDERER_GLERROR_H
#define RENDERER_GLERROR_H

#define CHECK_GL_ERROR(msg) {auto err = glGetError(); if(err) printf("ERROR %d :: %s\n", err, msg);}

#endif
