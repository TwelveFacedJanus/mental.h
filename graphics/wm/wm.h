#ifndef MENTAL_WM_H
#define MENTAL_WM_H

#include "../../core/Mental.h"

typedef struct MentalWindowManagerInfo {
    MentalStructureType     sType;
    int                     aWindowSizes[2];
    char*                   pWindowTitle;
    MentalBackendType       eMentalBackendType;
    MentalRenderAPIType     eMentalRenderAPIType;
    const void*             pNext;

} MentalWindowManagerInfo;

typedef struct MentalWindowManager {
    MentalStructureType     sType;
    MentalWindowManagerInfo *pInfo;
    const void              *pWindow;
    const void              *pNext;
    MentalResult            shouldClose;
} MentalWindowManager;

typedef struct MentalGLFWwindow {
    MentalStructureType     sType;
    const void *window;
} MentalGLFWwindow;

MentalResult __should_close(MentalWindowManager *pManager);

MentalResult mentalWMCreateGLFWwindow(MentalWindowManager *pManager);
MentalResult mentalWMRunGLFWwindow(MentalWindowManager *pManager);
MentalResult mentalWMDestroy(MentalWindowManager *pManager);

#endif // MENTAL_WM_H