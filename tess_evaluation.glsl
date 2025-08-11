#version 410 core
layout (triangles, equal_spacing, ccw) in;

in vec3 tcPosition[];
in vec2 tcTexCoord[];
in vec3 tcNormal[];

out vec3 tePosition;
out vec2 teTexCoord;
out vec3 teNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform sampler2D heightMap;
uniform float heightScale;

void main() {
    vec3 pos = gl_TessCoord.x * tcPosition[0] + 
               gl_TessCoord.y * tcPosition[1] + 
               gl_TessCoord.z * tcPosition[2];
    
    vec2 texCoord = gl_TessCoord.x * tcTexCoord[0] + 
                    gl_TessCoord.y * tcTexCoord[1] + 
                    gl_TessCoord.z * tcTexCoord[2];
    
    vec3 normal = normalize(gl_TessCoord.x * tcNormal[0] + 
                         gl_TessCoord.y * tcNormal[1] + 
                         gl_TessCoord.z * tcNormal[2]);
    
    // Применяем карту высот
    float heightValue = texture(heightMap, texCoord).r;
    pos += normal * (heightValue - 0.5) * heightScale;
    
    tePosition = pos;
    teTexCoord = texCoord;
    teNormal = normal;
    
    gl_Position = projection * view * model * vec4(pos, 1.0);
}