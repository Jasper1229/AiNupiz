
#include <stdio.h>

#include "manager.h"

#include "../std/nplib.h"
#include "../fileio/filelib.h"
#include "../vec/veclib.h"
#include "../maps/maplib.h"
#include "../http/httplib.h"

void defineAllLibraries(VM* vm) {
    defineLibrary(vm, "std", importNPLib);
    defineLibrary(vm, "iofile", importFileLib);
    defineLibrary(vm, "http", importHttpLib);
}
