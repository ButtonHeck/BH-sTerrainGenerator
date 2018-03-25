#version 450

out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform vec3 viewPosition;
uniform vec3 lightDirTo;

void main()
{
    float distanceToFragment = distance(FragPos, viewPosition);
    if (distanceToFragment > 160.0)
        discard;
    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPosition - FragPos);
    vec3 lightDir = normalize(-lightDirTo);
    vec4 sampledDiffuse = texture(texture_diffuse, vec2(TexCoords.x + Normal.r * 0.15, TexCoords.y));
    vec4 sampledSpecular = texture(texture_specular, TexCoords);
    //diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    //specular shading
    vec3 reflect = reflect(-lightDir, normal);
    float spec = pow(max(dot(reflect, viewDir), 0.0), 8.0) * 0.33;

    vec3 ambient = 0.2 * sampledDiffuse.rgb;
    vec3 diffuse = diff * sampledDiffuse.rgb * 0.67 + 0.33 * sampledDiffuse.rgb;
    vec3 specular = spec * sampledSpecular.rgb;
    vec3 result = ambient + diffuse + specular;

    FragColor = vec4(result, sampledDiffuse.a);
}
