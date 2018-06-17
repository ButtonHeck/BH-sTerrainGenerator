#version 450

out vec4 o_FragColor;

in vec2  v_TexCoords;
in float v_PosHeight;
in float v_TextureFlatMixRatio;
in float v_TextureHillMixRatio;
in float v_DiffuseComponentHill;
in float v_DiffuseComponentFlat;
in float v_SpecularComponent;
in vec4  v_FragPosLightSpace;
in vec3  v_Normal;

uniform sampler2D u_flat_diffuse;
uniform sampler2D u_flat_diffuse2;
uniform sampler2D u_hills_diffuse;
uniform sampler2D u_hills_diffuse2;
uniform sampler2D u_hills_specular;
uniform sampler2D u_shadowMap;
uniform vec3      u_lightDir;
uniform bool      u_shadowEnable;

const int  FILTER_TYPE = v_PosHeight > 0.4 ? 1 : 0;
const float POISSON_SHADOW_VALUE_GAIN = clamp((v_PosHeight - 0.4) / 4.0, 0.0, 1.0);
const vec2 POISSON_DISK[4] = vec2[](
  vec2( -0.94201624, -0.39906216 ),
  vec2( 0.94558609, -0.76890725 ),
  vec2( -0.094184101, -0.92938870 ),
  vec2( 0.34495938, 0.29387760 )
);

float calculateShadowComponent(vec4 fragPosLightSpace, vec3 normal)
{
    //transform from [-1;1] to [0;1]
    vec3 projCoords = fragPosLightSpace.xyz * 0.5 + 0.5;
    float closestDepth = texture(u_shadowMap, projCoords.xy).r;
    float currentDepth = projCoords.z;
    float shadow = 0.0;
    float bias = max(0.0006 * (1.0 - dot(normal, u_lightDir)), 0.0004);
    vec2 texelSize = 1.0 / textureSize(u_shadowMap, 0);

    if (FILTER_TYPE == 1)
    {
        //poisson filtering
        for (int i = 0; i < 4; i++)
        {
            float poissonDiskDepth = texture(u_shadowMap, projCoords.xy + POISSON_DISK[i] * texelSize).r;
            shadow += currentDepth - bias / 2.0 > poissonDiskDepth ? 1.0 : 0.0;
        }
        shadow /= (4.0 - POISSON_SHADOW_VALUE_GAIN); //slightly hack equation to more dark shadowing
    }
    else
    {
        //PCF filtering
        for (int x = -2; x <= 2; ++x)
        {
            for (int y = -2; y <= 2; ++y)
            {
                float pcfDepth = texture(u_shadowMap, projCoords.xy + vec2(x,y) * texelSize).r;
                shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
            }
        }
        shadow /= 25.0;
    }

    if (projCoords.z > 1.0)
        shadow = 0.0;
    return shadow;
}

void main()
{
    vec4 sampledDiffuse =
        mix(mix(texture(u_flat_diffuse, v_TexCoords), texture(u_flat_diffuse2, v_TexCoords), v_TextureFlatMixRatio),
            mix(texture(u_hills_diffuse, v_TexCoords), texture(u_hills_diffuse2, v_TexCoords), v_TextureHillMixRatio),
            min(max(0.0, v_PosHeight), 1.0));
    vec4 sampledSpecular =
        mix(vec4(0.0), texture(u_hills_specular, v_TexCoords), min(max(0.0, v_PosHeight), 1.0));

    vec3 diffuseColor;
    if (u_shadowEnable)
    {
        float shadowComponent = calculateShadowComponent(v_FragPosLightSpace, v_Normal);
        diffuseColor = sampledDiffuse.rgb *
            ((1.0 - shadowComponent) * mix(v_DiffuseComponentFlat, v_DiffuseComponentHill, min(max(0.0, v_PosHeight), 1.0)) * 0.5 + 0.5);
    }
    else
    {
        diffuseColor = sampledDiffuse.rgb *
            (mix(v_DiffuseComponentFlat, v_DiffuseComponentHill, min(max(0.0, v_PosHeight), 1.0)) * 0.5 + 0.5);
    }

    vec3 specularColor = v_SpecularComponent * sampledSpecular.rgb;
    vec3 resultColor = diffuseColor + specularColor;

    o_FragColor = vec4(resultColor, sampledDiffuse.a);
}
