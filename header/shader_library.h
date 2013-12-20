#ifndef _SHADER_LIBRARY_GUARD
#define _SHADER_LIBRARY_GUARD

#include "shader.h"

typedef struct _ShaderLibrary {

    Shader* shader;
    struct _ShaderLibrary* suivant;

} ShaderLibrary;

void ShaderLibrary_Refresh();
Shader* ShaderLibrary_Get(const char* shader);
void ShaderLibrary_Add(Shader* shader);

#endif // SHADER_LIBRARY

