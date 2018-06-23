#version 450

out vec4 o_FragColor;

in vec2  vg_TexCoords;
in float vg_PosHeight;
in float vg_TextureFlatMixRatio;
in float vg_TextureHillMixRatio;
in float vg_DiffuseComponentHill;
in float vg_DiffuseComponentFlat;
in float vg_SpecularComponent;
in vec4  vg_FragPosLightSpace;
in vec3  vg_Normal;
in vec3  vg_ProjectedCoords;

uniform sampler2D u_flat_diffuse;
uniform sampler2D u_flat_diffuse2;
uniform sampler2D u_hills_diffuse;
uniform sampler2D u_hills_diffuse2;
uniform sampler2D u_hills_specular;
uniform sampler2D u_shadowMap;
uniform vec3      u_lightDir;
uniform bool      u_shadowEnable;

const float TEXEL_SIZE_MULTIPLIER_OFFSET = 1.0 + clamp((vg_PosHeight * 10.0), 0.0, 1.5);
const vec2  TEXTURE_TEXEL_UNIT = 1.0 / textureSize(u_shadowMap, 0);
const float SHADOW_INFLUENCE = 0.3;

float calculateShadowComponent(vec4 fragPosLightSpace, vec3 normal)
{
    float closestDepth = texture(u_shadowMap, vg_ProjectedCoords.xy).r;
    float currentDepth = vg_ProjectedCoords.z;
    float shadow = 0.0;
    float bias = max(0.0004 * (1.0 - dot(normal, u_lightDir)), 0.000264);   //[0.0006; 0.0004]*0.66;
    vec2 texelSize = (TEXEL_SIZE_MULTIPLIER_OFFSET - dot(normal, u_lightDir) * TEXEL_SIZE_MULTIPLIER_OFFSET) * TEXTURE_TEXEL_UNIT;

    //PCF filtering
    for (int x = -1; x <= 1; ++x)
    {
        for (int y = -2; y <= 2; ++y)
        {
            float pcfDepth = texture(u_shadowMap, vg_ProjectedCoords.xy + vec2(x,y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 0.066 : 0.0;
        }
    }
    return (1.0 - shadow * SHADOW_INFLUENCE);
}

void main()
{
    vec4 sampledDiffuse =
        mix(mix(texture(u_flat_diffuse, vg_TexCoords), texture(u_flat_diffuse2, vg_TexCoords), vg_TextureFlatMixRatio),
            mix(texture(u_hills_diffuse, vg_TexCoords), texture(u_hills_diffuse2, vg_TexCoords), vg_TextureHillMixRatio),
            clamp(vg_PosHeight, 0.0, 1.0));
    vec4 sampledSpecular =
        mix(vec4(0.0), texture(u_hills_specular, vg_TexCoords), clamp(vg_PosHeight, 0.0, 1.0));

    vec3 diffuseColor;
    float diffuseComponent = mix(vg_DiffuseComponentFlat, vg_DiffuseComponentHill, clamp(vg_PosHeight, 0.0, 1.0));
    if (u_shadowEnable)
    {
        float shadowComponent = calculateShadowComponent(vg_FragPosLightSpace, vg_Normal);
        diffuseColor = shadowComponent * mix(sampledDiffuse.rgb * diffuseComponent, sampledDiffuse.rgb, 0.5);
    }
    else
        diffuseColor = mix(sampledDiffuse.rgb * diffuseComponent, sampledDiffuse.rgb, 0.5);

    vec3 specularColor = vg_SpecularComponent * sampledSpecular.rgb;
    vec3 resultColor = diffuseColor + specularColor;

    o_FragColor = vec4(resultColor, sampledDiffuse.a);
}
