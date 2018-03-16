#version 450

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;
in float PosHeight;

uniform sampler2D base_diffuse;
uniform sampler2D base_specular;
uniform sampler2D hills_diffuse;
uniform sampler2D hills_specular;
uniform vec3 viewPosition;
uniform vec3 lightDirTo;

void main()
{
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPosition - FragPos);
    vec3 lightDir = normalize(-lightDirTo);
    vec4 sampledDiffuse =
        mix(texture(base_diffuse, TexCoords), texture(hills_diffuse, TexCoords), min(max(0.0, PosHeight/1.5), 1.0));
    vec4 sampledSpecular =
        mix(texture(base_specular, TexCoords), texture(hills_specular, TexCoords), min(max(0.0, PosHeight/1.5), 1.0));
    //diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    //specular shading
    vec3 reflect = reflect(-lightDir, normal);
    float spec = pow(max(dot(reflect, viewDir), 0.0), 64.0);

    vec3 diffuse = diff * sampledDiffuse.rgb * 0.25 + 0.75 * sampledDiffuse.rgb;
    vec3 specular = spec * sampledSpecular.rgb;
    vec3 result = diffuse + specular;

    FragColor = vec4(result, sampledDiffuse.a);
}
