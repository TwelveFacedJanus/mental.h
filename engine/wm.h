#ifndef mental_wm_h
#define mental_wm_h

#include "mental.h"
#include "engine.h"
#include "component.h"


typedef struct MentalWindowManagerInfo {
    MentalStructureType                     eType;
    int                                     aSizes[2];
    char                                    *pTitle;
} MentalWindowManagerInfo;

typedef struct MentalWindowManager {
    MentalStructureType                     eType;
    MentalWindowManagerInfo                 *pInfo;
    void                                    *pNext;
    MentalCamera                            camera;
    MentalSkybox                            skybox;
} MentalWindowManager;

MentalResult mentalCreateWM(MentalWindowManager *pManager);
MentalResult mentalRunWM(MentalWindowManager *pManager);
MentalResult mentalDestroyWM(MentalWindowManager *pManager);


#endif // mental_wm_h