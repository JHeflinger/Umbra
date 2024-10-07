#ifndef CHAIN_H
#define CHAIN_H

#include "utils/files.h"

void UpdateChain(ARRLIST_PathString* paths);

void DrawChain(float x, float y, float w, float h);

size_t ShaderChainSize();

Shader GetShaderInChain(size_t index);

void CleanChain();

PathString* MatchPathInChain(const char* path);

#endif
