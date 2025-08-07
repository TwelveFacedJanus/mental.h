#ifndef mental_render_h
#define mental_render_h

#include "../../core/Mental.h"

typedef struct MentalRenderBackend {
    MentalStructureType sType;
} MentalRenderBackend;

MentalResult mentalOGLinitGLEW();

#endif // mental_render_h