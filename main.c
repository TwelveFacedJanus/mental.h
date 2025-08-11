#include "engine/historical.h"
#include "engine/mental.h"
#include "engine/wm.h"
#include "engine/project.h"

int main(int argc, const char * argv[])
{
    (void)argc;  // Suppress unused parameter warning
    (void)argv;  // Suppress unused parameter warning
    MentalWindowManagerInfo windowManagerInfo = {
        .eType = MENTAL_STRUCTURE_TYPE_WINDOW_MANAGER_INFO,
        .pTitle = "Mental WM",
        .aSizes = {800, 600},
    };
    
    MentalProjectInfo projectInfo = {
        .eType = MENTAL_STRUCTURE_TYPE_PROJECT_INFO,
        .pName = "Mental project",
        .version = {1, 0, 0},
        .pWindowManagerInfo = &windowManagerInfo,
    };
    
    MentalProject project = {
        .eType = MENTAL_STRUCTURE_TYPE_PROJECT,
        .pInfo = &projectInfo,
    };

    if (mentalCreateProject(&project) != MENTAL_OK) {
        mentalDestroyProject(&project);
        return -1;
    }

    mentalRunProject(&project);
    mentalDestroyProject(&project);
    
    return 0;
}