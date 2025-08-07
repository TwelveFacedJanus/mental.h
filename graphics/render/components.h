#ifndef mental_components_h
#define mental_components_h

#include "../../core/Mental.h"

typedef struct MentalComponentInfo {
    MentalStructureType sType;
    MentalComponentType cType;
    char                *cName;
} MentalComponentInfo;


typedef struct MentalComponent {
    MentalStructureType sType;
    MentalComponentInfo *pInfo;
    Vec3_float          *pPosition;
    Vec3_float          *pAngle;
    const void          *pNext;
} MentalComponent;

MentalResult mentalRegisterComponent(MentalComponent *pComponent);
MentalResult mentalSetNext(MentalComponent* pComponent, MentalComponent* pComponent2);
MentalResult mentalDrawComponent(MentalComponent* pComponent);
MentalResult mentalDestroyComponent(MentalComponent* pComponent);

#endif // mental_components_h