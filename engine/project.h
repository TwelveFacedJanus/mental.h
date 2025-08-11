#ifndef mental_project_h
#define mental_project_h

#include "mental.h"
#include "wm.h"

typedef struct MentalProjectInfo {
    MentalStructureType             eType;
    char                            *pName;
    uint32_t                        version[3];
    MentalWindowManagerInfo         *pWindowManagerInfo;
} MentalProjectInfo;

typedef struct MentalProject {
    MentalStructureType             eType;
    MentalProjectInfo               *pInfo;
    MentalWindowManager             *pWindowManager;
} MentalProject;

MentalResult mentalCreateProject(MentalProject* pProject);
MentalResult mentalRunProject(MentalProject* pProject);
MentalResult mentalDestroyProject(MentalProject* pProject);



#endif // mental_project_h