#version 330 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec4 base_color;
uniform float roughness;
uniform float metallic;
uniform sampler2D texture_diffuse;
uniform sampler2D normal_map; // Add uniform for normal map
uniform bool use_texture;
uniform bool use_normal_map; // Add uniform to control normal map usage

void main() {
    vec4 color = base_color;

    if (use_texture) {
        vec4 texColor = texture(texture_diffuse, TexCoords);
        color *= texColor; // Combine texture color with base color
    }

    vec3 norm = normalize(Normal);
    
    if (use_normal_map) {
        // Sample the normal map and transform it into world space
        vec3 normalMap = texture(normal_map, TexCoords).rgb;
        normalMap = normalMap * 2.0 - 1.0; // Convert from [0,1] to [-1,1]
        norm = normalize(norm + normalMap); // Add normal map to the original normal
    }

    vec3 lightPos = vec3(0.0, 10.0, 0.0); // Example light position
    vec3 lightColor = vec3(1.0, 1.0, 1.0);  // White light
    vec3 lightDir = normalize(lightPos - FragPos);

    // Ambient
    vec3 ambient = 0.1 * color.rgb;

    // Diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * color.rgb;

    // Specular (simplified for PBR-like approach)
    vec3 viewDir = normalize(-FragPos); // Assuming the camera is at the origin
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0); // Adjust the shininess as needed
    vec3 specular = vec3(roughness * spec);

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, color.a);
}