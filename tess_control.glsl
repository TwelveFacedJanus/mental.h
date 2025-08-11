#version 410 core
layout (vertices = 3) out;

in vec3 Position[];
in vec2 TexCoord[];
in vec3 Normal[];

out vec3 tcPosition[];
out vec2 tcTexCoord[];
out vec3 tcNormal[];

uniform float TessLevel;

void main() {
    tcPosition[gl_InvocationID] = Position[gl_InvocationID];
    tcTexCoord[gl_InvocationID] = TexCoord[gl_InvocationID];
    tcNormal[gl_InvocationID] = Normal[gl_InvocationID];
    
    if (gl_InvocationID == 0) {
        gl_TessLevelInner[0] = TessLevel;
        gl_TessLevelOuter[0] = TessLevel;
        gl_TessLevelOuter[1] = TessLevel;
        gl_TessLevelOuter[2] = TessLevel;
    }
}