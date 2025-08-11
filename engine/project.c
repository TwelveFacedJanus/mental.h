#include "project.h"

MentalResult mentalCreateProject(MentalProject* pProject)
{
    if (!pProject) {
        MENTAL_DEBUG("Failed to create project. Pointer to MentalProject is null.");
        return MENTAL_POINTER_IS_NULL;
    };

    MentalWindowManager* pWindowManager = (MentalWindowManager*)malloc(sizeof(MentalWindowManager));
    if (!pWindowManager) {
        MENTAL_DEBUG("Failed to allocate memory for MentalWindowManager");
        return MENTAL_FAILED_TO_ALLOCATE_MEMORY;
    }

    pWindowManager->eType = MENTAL_STRUCTURE_TYPE_WINDOW_MANAGER;
    pWindowManager->pInfo = pProject->pInfo->pWindowManagerInfo;

    pProject->pWindowManager = pWindowManager;
    if (mentalCreateWM(pProject->pWindowManager) != MENTAL_OK)
    {
        mentalDestroyProject(pProject);
        return MENTAL_ERROR;
    }

    return MENTAL_OK;
}

MentalResult mentalRunProject(MentalProject* pProject)
{
    if (!pProject) {
        MENTAL_DEBUG("Failed to run project. Pointer to MentalProject is null.");
        return MENTAL_POINTER_IS_NULL;
    }
    mentalRunWM(pProject->pWindowManager);
    return MENTAL_OK;
}

MentalResult mentalDestroyProject(MentalProject* pProject)
{
    if (!pProject)
    {
        MENTAL_DEBUG("Failed to destroy project. Pointer to MentalProject is null.");
        return MENTAL_POINTER_IS_NULL;
    }

    if (pProject->pWindowManager) {
        mentalDestroyWM(pProject->pWindowManager);
    }
    free(pProject->pWindowManager);
    return MENTAL_OK;
}