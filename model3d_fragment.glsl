#version 330 core

in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

// Структура для материала
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;
uniform sampler2D texture1;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;
uniform bool hasTexture;
uniform vec3 objectColor;

void main()
{
    // Нормализуем нормаль
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 viewDir = normalize(viewPos - FragPos);
    
    // Окружающее освещение
    vec3 ambient = material.ambient * lightColor;
    
    // Диффузное освещение
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * material.diffuse * lightColor;
    
    // Бликовое освещение
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * material.specular * lightColor;
    
    // Результирующее освещение
    vec3 lighting = ambient + diffuse + specular;
    
    // Финальный цвет
    if (hasTexture) {
        vec4 texColor = texture(texture1, TexCoord);
        // Проверяем, что текстура не пустая (не черная)
        if (texColor.rgb != vec3(0.0, 0.0, 0.0)) {
            // Комбинируем текстуру с материалом
            FragColor = vec4(lighting * texColor.rgb, texColor.a);
        } else {
            // Если текстура черная, используем цвет объекта
            FragColor = vec4(lighting * objectColor, 1.0);
        }
    } else {
        FragColor = vec4(lighting * objectColor, 1.0);
    }
}