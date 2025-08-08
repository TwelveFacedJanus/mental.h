#ifndef MENTAL_WM_H
#define MENTAL_WM_H

#include "../../core/Mental.h"
#include "../../core/Pipes.h"
#include "../render/render.h"

typedef MentalResult (*ShouldCloseCallbackF)(const void *);
typedef MentalResult (*MainDrawF)(const void*);


typedef struct MentalComponentInfo {
    MentalStructureType sType;
    MentalComponentType cType;
    char                *cName;
    void          *pNext;
} MentalComponentInfo;

typedef struct MentalShaderMaterialInfo {
    MentalStructureType sType;
    char*               pVertexPath;
    char*               pFragmentPath;
} MentalShaderMaterialInfo;

typedef struct MentalShaderMaterial {
    MentalStructureType sType;
    unsigned int        shaderProgram;
} MentalShaderMaterial;


typedef struct MentalComponent {
    MentalStructureType   sType;
    MentalComponentInfo  *pInfo;
    float                 aPositions[3];
    float                 aAngle[3];
    void                 *pNext;
    unsigned int          VBO, VAO, EBO;
} MentalComponent;


typedef struct MentalComponentManager {
    MentalComponent     components[2];
    uint32_t            component_count;
} MentalComponentManager;


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
    MentalComponentManager  *componentManager;
} MentalWindowManager;

typedef struct MentalGLFWwindow {
    MentalStructureType     sType;
    const void *window;
} MentalGLFWwindow;

MentalResult mentalRegisterComponent(MentalComponent *pComponent);
MentalResult mentalSetNext(MentalComponent* pComponent, MentalComponent* pComponent2);
MentalResult mentalDrawComponent(MentalComponent* pComponent);
MentalResult mentalDestroyComponent(MentalComponent* pComponent);

MentalResult __draw(MentalWindowManager* pManager);
MentalResult __should_close(MentalWindowManager *pManager);
MentalResult __set_should_close_f(MentalWindowManager *pManager, ShouldCloseCallbackF f);

MentalResult mentalInitializeWM(MentalWindowManager *pManager);
MentalResult mentalRunWM(MentalWindowManager *pManager);
MentalResult mentalDestroyWM(MentalWindowManager *pManager);
#endif // MENTAL_WM_H