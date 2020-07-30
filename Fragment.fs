#version 330 core
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3  position;
    vec3  direction;
    vec3 color;
    float cutOff;
    float outerCutOff;
    vec3 ambient;
    vec3 specular;
    vec3 diffuse;
};  

layout(location = 0)out vec4 FragColor;
#define SPOTLIGHT_COUNT 66

in vec3 Position;
in vec3 Normal;
in vec3 Color;
in vec2 TexCoord;

uniform Light light;
uniform SpotLight spotlight1;
uniform SpotLight spotlight2;
uniform SpotLight spotLights[SPOTLIGHT_COUNT];
uniform Material material;
uniform vec3 cameraPos;
uniform samplerCube skybox;
uniform sampler2D ourTexture;
uniform vec3 lightPos;
uniform vec3 lightColor;

// function prototypes
vec3 CalcDirLight(Light light, vec3 normal, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main(){
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(cameraPos - Position);
    
    // == =====================================================
    // Our lighting is set up in 3 phases: directional, point lights and an optional flashlight
    // For each phase, a calculate function is defined that calculates the corresponding color
    // per lamp. In the main() function we take all the calculated colors and sum them up for
    // this fragment's final color.
    // == =====================================================
    // phase 1: directional lighting
    vec3 result = CalcDirLight(light, norm, viewDir);
   
    for(int i = 0; i < SPOTLIGHT_COUNT; i++) {
        result += CalcSpotLight(spotLights[i], norm, Position, viewDir);  
    }
          
    // phase 3: spot light
    result += CalcSpotLight(spotlight1, norm, Position, viewDir);
    result += CalcSpotLight(spotlight2, norm, Position, viewDir);
    FragColor = vec4(result, 1.0);
}

// calculates the color when using a directional light.
vec3 CalcDirLight(Light light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - Position);
   
   // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    
    // combine results
    vec3 ambient = light.ambient * vec3(texture(ourTexture, TexCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(ourTexture, TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(ourTexture, TexCoord));
    
    return (ambient + diffuse + specular);
}


// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - Position);
    
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    
    // combine results
    vec3 ambient = light.ambient * vec3(texture(ourTexture, TexCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(ourTexture, TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(ourTexture, TexCoord));
    ambient *=  intensity;
    diffuse *=  intensity;
    specular *= intensity;

    return (ambient + diffuse + specular);
}