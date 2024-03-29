#pragma once
#include "GL/glew.h"
#include <cstdlib>

namespace Graphics
{
void glClearErrors();

bool glLogErrors(const char* function, const char* file, int line);

/**
 * @brief Clears previous errors, runs the function and logs and
 * breaks in the case of an error.
 */
#define glCheck(x)                                                                                                     \
    glClearErrors();                                                                                                   \
    x;                                                                                                                 \
    if (!glLogErrors(#x, __FILE__, __LINE__))                                                                          \
        exit(1);

} // namespace Graphics