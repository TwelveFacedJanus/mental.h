#ifndef MENTAL_WM_H
#define MENTAL_WM_H

#include "../../core/Mental.h"
#include "../../core/Pipes.h"
#include "../render/render.h"

typedef MentalResult (*ShouldCloseCallbackF)(const void *);
typedef MentalResult (*MainDrawF)(const void*);

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
    ShouldCloseCallbackF    closeCallbackF;
    MainDrawF               drawFunction;
} MentalWindowManager;

typedef struct MentalGLFWwindow {
    MentalStructureType     sType;
    const void *window;
} MentalGLFWwindow;

MentalResult __draw(MentalWindowManager* pManager);
MentalResult __should_close(MentalWindowManager *pManager);
MentalResult __set_should_close_f(MentalWindowManager *pManager, ShouldCloseCallbackF f);

MentalResult mentalInitializeWM(MentalWindowManager *pManager);
MentalResult mentalRunWM(MentalWindowManager *pManager);
MentalResult mentalDestroyWM(MentalWindowManager *pManager);
#endif // MENTAL_WM_H