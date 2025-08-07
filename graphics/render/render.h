#ifndef mental_render_h
#define mental_render_h

#include "../../core/Mental.h"
#include "components.h"


typedef struct MentalRenderBackend {
    MentalStructureType sType;
} MentalRenderBackend;

MentalResult mentalInitializeBackend(MentalRenderAPIType rApi);

MentalResult mentalOGLInitGLEW();

#endif // mental_render_h