#ifndef RENDERER_ASSERT_H
#define RENDERER_ASSERT_H

#define ASSERT(expression) {if(!(expression)){ printf("ASSERTION :: %s :: %s %d\n", __FILE__, __func__, __LINE__); *(int*)nullptr = 0;}}

#endif
