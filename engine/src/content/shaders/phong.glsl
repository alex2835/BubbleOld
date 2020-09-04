// Vertex shader
#shader vertex
#version 330 core
layout(location = 0) in vec3  a_Pos;
layout(location = 1) in vec3  a_Normal;
layout(location = 2) in vec2  a_TexCoords;
layout (location = 3) in vec3 a_Tangent;
layout (location = 4) in vec3 a_Bitangent;

out vec3 v_FragPos;
out vec3 v_Normal;
out vec2 v_TexCoords;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

void main()
{
    v_FragPos = vec3(u_Model * vec4(a_Pos, 1.0));
    v_Normal = mat3(transpose(inverse(u_Model))) * a_Normal;
    v_TexCoords = a_TexCoords;
    gl_Position = u_Projection * u_View * vec4(v_FragPos, 1.0);
}

// Fragment shader
#shader fragment
#version 330 core
out vec4 FragColor;

struct Material {
    sampler2D diffuse0;
    sampler2D specular0;
    sampler2D normal0;
    int shininess;
};

// Light type enum
const int DirLight = 0;
const int PointLight = 1;
const int SpotLight = 2;

struct Light
{
    int type;
    float brightness;
	vec3 color;

    // dir
    vec3 direction;
    vec3 position;

    // point
    float constant;
    float linear;
    float quadratic;

    // spot
    float cutOff;
    float outerCutOff;
};

in vec3 v_FragPos;
in vec3 v_Normal;
in vec2 v_TexCoords;

uniform vec3 u_ViewPos;

// Light
#define MAX_LIGHTS 5
uniform Light lights[MAX_LIGHTS];
uniform int nLights;

// Material
uniform Material material;

// Function prototypes
vec3 CalcDirLight(Light light, vec3 normal, vec3 view_dir);
vec3 CalcPointLight(Light light, vec3 normal, vec3 frag_pos, vec3 view_dir);
vec3 CalcSpotLight(Light light, vec3 normal, vec3 frag_pos, vec3 view_dir);

void main()
{
    vec3 norm = normalize(v_Normal);
    vec3 view_dir = normalize(u_ViewPos - v_FragPos);

    vec3 result;
    for (int i = 0; i < nLights; i++)
    {
        switch (lights[i].type)
        {
            case DirLight:
                result += CalcDirLight(lights[i], norm, view_dir);
                break;

            case PointLight:
                result += CalcPointLight(lights[i], norm, v_FragPos, view_dir);
                break;

            case SpotLight:
                result += CalcSpotLight(lights[i], norm, v_FragPos, view_dir);
                break;
        }
    }
    clamp(result, vec3(0.1f), vec3(1.0f));
    FragColor = vec4(result, 1.0);
}


// calculates the color when using a directional light.
vec3 CalcDirLight(Light light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    
    // combine results
    //vec3 ambient = vec3(texture(material.diffuse0, v_TexCoords));
    vec3 diffuse = diff * vec3(texture(material.diffuse0, v_TexCoords));
    vec3 specular = spec * vec3(texture(material.specular0, v_TexCoords));

    return diffuse + specular;
}

// calculates the color when using a point light.
vec3 CalcPointLight(Light light, vec3 normal, vec3 frag_pos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - frag_pos);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    // attenuation
    float distance = length(light.position - frag_pos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // combine results
    //vec3 ambient = vec3(texture(material.diffuse0, v_TexCoords));
    vec3 diffuse = light.color * diff * vec3(texture(material.diffuse0, v_TexCoords));
    vec3 specular = spec * vec3(texture(material.specular0, v_TexCoords));
    //ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (diffuse + specular);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(Light light, vec3 normal, vec3 frag_pos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - frag_pos);

    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);

    // specular shading
    vec3 halfwayDir = normalize(-light.direction + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

    // attenuation
    float distance = length(light.position - frag_pos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);

    // combine results
    //vec3 ambient = vec3(texture(material.diffuse0, v_TexCoords));
    vec3 diffuse = light.color * diff * vec3(texture(material.diffuse0, v_TexCoords));
    vec3 specular = spec * vec3(texture(material.specular0, v_TexCoords));
    //ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (diffuse + specular);
}