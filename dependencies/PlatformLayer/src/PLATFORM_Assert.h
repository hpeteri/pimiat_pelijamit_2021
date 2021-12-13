#pragma once
#include <iostream>

#define ASSERT(expression){if(!(expression)){printf("Assertion Failed :: %s :: %d\n", __FILE__, __LINE__);*(int*)nullptr = 0;}}
