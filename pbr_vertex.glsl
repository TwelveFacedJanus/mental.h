#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aTangent;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
out vec3 TangentLightPos;
out vec3 TangentViewPos;
out vec3 TangentFragPos;
out vec3 OriginalNormal; // Для отладки

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform bool hasNormalMap;
uniform bool hasHeightMap;
uniform sampler2D heightMap;
uniform float heightScale = 0.1;

void main()
{
    // Фиксим UV-координаты
    vec2 fixedTexCoord = fract(aTexCoord);
    
    // Смещение позиции с учетом карты высот
    vec3 displacedPos = aPos;
    if (hasHeightMap) {
        float heightValue = texture(heightMap, fixedTexCoord).r;
        displacedPos += aNormal * (heightValue - 0.5) * heightScale * 0.1;
    }
    
    // Позиция в мировых координатах
    FragPos = vec3(model * vec4(displacedPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    OriginalNormal = Normal; // Сохраняем для отладки
    TexCoord = fixedTexCoord;

    // Касательное пространство (только если есть карта нормалей)
    if (hasNormalMap) {
        vec3 T = normalize(vec3(model * vec4(aTangent, 0.0)));
        vec3 N = normalize(vec3(model * vec4(aNormal, 0.0)));
        T = normalize(T - dot(T, N) * N); // Ортогонализация
        vec3 B = cross(N, T);
        
        mat3 TBN = transpose(mat3(T, B, N));
        TangentLightPos = TBN * lightPos;
        TangentViewPos = TBN * viewPos;
        TangentFragPos = TBN * FragPos;
    }
    
    gl_Position = projection * view * model * vec4(displacedPos, 1.0);
}