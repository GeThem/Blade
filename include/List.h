#pragma once

#include "vfx.h"
#include "malloc.h"

void ListAppend(VList&, const VanishText& elem);
void ListRemoveCurrent(VList&);
VLElem* ListNext(VList&);
void ListClear(VList&);