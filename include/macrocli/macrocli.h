#ifndef MACROCLI_H
#define MACROCLI_H

#include <stdbool.h>

typedef int (*MacroCliPrintf)(void*, const char* restrict, ...);

bool macrocli_run(MacroCliPrintf print, void* userdata, char* input);

#endif // MACROCLI_H
