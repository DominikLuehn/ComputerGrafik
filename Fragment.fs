#version 330 core
struct Material {
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct SpotLight {
    vec3  position;
    vec3  direction;
    float cutOff;
    float outerCutOff;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 specular;
    vec3 diffuse;
};  

out vec4 FragColor;

#define SPOTLIGHT_COUNT 66

in vec4 PosLightSpace;
in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;

uniform bool dir_light;
uniform bool spot_light;
uniform bool skybox_bool;
uniform bool shadow;

uniform sampler2D shadowMap;
uniform samplerCube skybox;
uniform float factor;
uniform vec3 cameraPos;
uniform DirLight light;
uniform SpotLight spotLights[SPOTLIGHT_COUNT];
uniform Material material;

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 viewDir);

void main(){
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(cameraPos - Position);

    // directional lighting
    vec3 result = vec3(0.0);
    if(dir_light){
        result = CalcDirLight(light, norm, viewDir);
    }

    // spotlights
    if(spot_light){
        for(int i = 0; i < SPOTLIGHT_COUNT; i++) {
            result += CalcSpotLight(spotLights[i], norm, viewDir);  
        }
    }
    
    // Enviroment Mapping/Skybox
    vec3 I = normalize(Position - cameraPos);
    vec3 R = reflect(I, norm);

    // skybox
    if(skybox_bool){
        FragColor = (factor * vec4(texture(skybox, R).rgb, 1.0)) + ((1.0 - factor) * vec4(result, 1.0));
    } else {
        FragColor = vec4(result, 1.0);
    }
}

// calculates shadows for directional light
float ShadowCalculation(DirLight light, vec3 norm){
    // perform perspective divide
    vec3 projCoords = PosLightSpace.xyz / PosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from lights perspective
    float closestDepth = texture(shadowMap, projCoords.xy).r;
    // get depth of current fragment from lights perspective
    float currentDepth = projCoords.z;
    // calculate bias
    vec3 lightDir = normalize(light.direction - Position);
    float bias = max(0.05 * (1.0 - dot(norm, lightDir)), 0.005);
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
   
   // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoord));
    
    // shadow
    float shadow = ShadowCalculation(light, normal);

    // result
    return (ambient + (1.0 - shadow) * (diffuse + specular));
    //return (ambient + diffuse + specular);
}


// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - Position);
    
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    
    // attenuation
    float distance = length(light.position - Position);
    float attenuation = 1.0 / (1.0 + 0.022 * distance + 0.0019 * (distance * distance));

    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, TexCoord));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, TexCoord));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;

    return (ambient + diffuse + specular);
}