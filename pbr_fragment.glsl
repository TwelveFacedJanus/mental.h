#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;
in vec3 TangentLightPos;
in vec3 TangentViewPos;
in vec3 TangentFragPos;
in vec3 OriginalNormal;

out vec4 FragColor;

// Структура материала
struct Material {
    vec3 albedo;
    float metallic;
    float roughness;
    float ao;
};

// Константы
const float PI = 3.14159265359;
const float GAMMA = 2.2;

// Юниформы
uniform Material material;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform bool usePBR;

// Флаги текстур
uniform bool hasAlbedoMap;
uniform bool hasNormalMap;
uniform bool hasMetallicMap;
uniform bool hasRoughnessMap;
uniform bool hasAOMap;
uniform bool hasHeightMap;

// Текстуры
uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;
uniform sampler2D heightMap;

// Отладочные режимы
uniform bool debugUVs = false;
uniform bool debugNormals = false;
uniform bool debugWireframe = false;
uniform bool debugTangents = false;

// Функции PBR
float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    return a2 / (PI * denom * denom);
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    return GeometrySchlickGGX(NdotV, roughness) * GeometrySchlickGGX(NdotL, roughness);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec2 parallaxMapping(vec2 texCoords, vec3 viewDir) {
    if (!hasHeightMap) return texCoords;
    
    const float minLayers = 8.0;
    const float maxLayers = 32.0;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));
    float layerDepth = 1.0 / numLayers;
    float currentLayerDepth = 0.0;
    
    vec2 P = viewDir.xy * 0.1; // Уменьшенный heightScale
    vec2 deltaTexCoords = P / numLayers;
    
    vec2 currentTexCoords = texCoords;
    float currentDepth = texture(heightMap, currentTexCoords).r;
    
    while(currentLayerDepth < currentDepth) {
        currentTexCoords -= deltaTexCoords;
        currentDepth = texture(heightMap, currentTexCoords).r;
        currentLayerDepth += layerDepth;
    }
    
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;
    float afterDepth = currentDepth - currentLayerDepth;
    float beforeDepth = texture(heightMap, prevTexCoords).r - currentLayerDepth + layerDepth;
    float weight = afterDepth / (afterDepth - beforeDepth);
    
    return mix(currentTexCoords, prevTexCoords, weight);
}

void main()
{
    // Отладочные режимы
    if (debugUVs) {
        FragColor = vec4(fract(TexCoord), 0.0, 1.0);
        return;
    }
    
    if (debugNormals) {
        FragColor = vec4(normalize(OriginalNormal)*0.5+0.5, 1.0);
        return;
    }
    
    if (debugTangents) {
        vec3 tangent = normalize(cross(OriginalNormal, vec3(0.0, 1.0, 0.0)));
        FragColor = vec4(tangent*0.5+0.5, 1.0);
        return;
    }

    // Основной PBR-рендеринг
    vec2 finalTexCoord = hasHeightMap ? parallaxMapping(TexCoord, normalize(viewPos - FragPos)) : TexCoord;
    finalTexCoord = fract(finalTexCoord); // Гарантируем [0,1] диапазон
    
    // Получаем параметры материала
    vec3 albedo = hasAlbedoMap ? pow(texture(albedoMap, finalTexCoord).rgb, vec3(GAMMA)) : material.albedo;
    float metallic = hasMetallicMap ? texture(metallicMap, finalTexCoord).r : material.metallic;
    float roughness = hasRoughnessMap ? texture(roughnessMap, finalTexCoord).r : material.roughness;
    float ao = hasAOMap ? texture(aoMap, finalTexCoord).r : material.ao;
    
    // Нормаль
    vec3 N;
    vec3 V;
    vec3 L;
    
    if (hasNormalMap) {
        N = texture(normalMap, finalTexCoord).rgb;
        N = normalize(N * 2.0 - 1.0);
        N = normalize(N);
        
        V = normalize(TangentViewPos - TangentFragPos);
        L = normalize(TangentLightPos - TangentFragPos);
    } else {
        N = normalize(Normal);
        V = normalize(viewPos - FragPos);
        L = normalize(lightPos - FragPos);
    }
    
    // PBR расчеты
    vec3 H = normalize(V + L);
    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
    
    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    
    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
    vec3 specular = numerator / max(denominator, 0.001);
    
    vec3 kS = F;
    vec3 kD = (vec3(1.0) - kS) * (1.0 - metallic);
    
    float NdotL = max(dot(N, L), 0.0);
    vec3 Lo = (kD * albedo / PI + specular) * lightColor * NdotL;
    
    // Ambient освещение
    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + Lo;
    
    // Тонирование и гамма-коррекция
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/GAMMA));
    
    FragColor = vec4(color, 1.0);
    
    // Отладочная сетка
    if (debugWireframe) {
        float edgeX = smoothstep(0.99, 1.0, abs(dFdx(TexCoord.x)));
        float edgeY = smoothstep(0.99, 1.0, abs(dFdy(TexCoord.y)));
        float edge = min(edgeX + edgeY, 1.0);
        FragColor = mix(FragColor, vec4(1.0,0.0,0.0,1.0), edge);
    }
}