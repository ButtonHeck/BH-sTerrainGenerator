#version 450

out vec4 o_FragColor;

in vec3  v_FragPos;
in float v_PositionDiffuseComponent;
in vec2  v_TexCoords;
in float v_PosHeight;
in float v_DiffuseComponent;
in vec3  v_Normal;
in vec3  v_ProjectedCoords;
in float v_FlatBlend;

uniform sampler2D u_flat_diffuse;
uniform sampler2D u_flat_diffuse2;
uniform sampler2D u_sand_diffuse;
uniform sampler2D u_sand_diffuse2;
uniform sampler2D u_diffuse_mix_map;
uniform float     u_mapDimension;
uniform sampler2D u_normal_map;
uniform sampler2D u_shadowMap;
uniform vec3      u_lightDir;
uniform bool      u_shadowEnable;
uniform bool      u_debugRenderMode;

const float SHADOW_INFLUENCE = 0.5;
const float MAX_DESATURATING_VALUE = 0.4;
const vec2 TEXEL_SIZE = 1.0 / textureSize(u_shadowMap, 0);

float SampleShadowMap(sampler2D shadowMap, vec2 coords, float compare)
{
    return step(texture2D(shadowMap, coords.xy).r, compare);
}

float SampleShadowMapLinear(sampler2D shadowMap, vec2 coords, float compare, vec2 texelSize)
{
    vec2 pixelPos = coords/texelSize + vec2(0.5);
    vec2 fracPart = fract(pixelPos);
    vec2 startTexel = (pixelPos - fracPart) * texelSize;

    float blTexel = SampleShadowMap(shadowMap, startTexel, compare);
    float brTexel = SampleShadowMap(shadowMap, startTexel + vec2(texelSize.x, 0.0), compare);
    float tlTexel = SampleShadowMap(shadowMap, startTexel + vec2(0.0, texelSize.y), compare);
    float trTexel = SampleShadowMap(shadowMap, startTexel + texelSize, compare);

    float mixA = mix(blTexel, tlTexel, fracPart.y);
    float mixB = mix(brTexel, trTexel, fracPart.y);

    return mix(mixA, mixB, fracPart.x);
}

float calculateLuminosity(vec3 normal)
{
    float currentDepth = v_ProjectedCoords.z;
    float shadow = 0.0f;
    float bias = 6.0 / 8192;

    //PCF filtering
    const int NUM_SAMPLES = 3;
    const int SAMPLE_START = (NUM_SAMPLES - 1) / 2;
    const int NUM_SAMPLES_SQUARED = NUM_SAMPLES * NUM_SAMPLES;
    for (int x = -SAMPLE_START; x <= SAMPLE_START; ++x)
    {
        for (int y = -SAMPLE_START; y <= SAMPLE_START; ++y)
        {
            vec2 coordsOffset = vec2(x,y) * TEXEL_SIZE;
            shadow += SampleShadowMapLinear(u_shadowMap, v_ProjectedCoords.xy + coordsOffset, currentDepth - bias, TEXEL_SIZE);
        }
    }

    return 1.0 - (shadow / NUM_SAMPLES_SQUARED * SHADOW_INFLUENCE);
}

vec4 desaturate(vec4 fragColor, float desaturatingValue)
{
    float colorMedian = (fragColor.r + fragColor.g + fragColor.b) * 0.333;
    vec3 gray = vec3(colorMedian);
    vec4 desaturated = vec4(mix(fragColor.rgb, gray, desaturatingValue), fragColor.a);
    return desaturated;
}

void main()
{
    if (u_debugRenderMode)
        o_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    else
    {
        float DiffuseTextureMix = texture(u_diffuse_mix_map, v_FragPos.xz * u_mapDimension + 0.5).r;
        vec4 sampledDiffuse =
                    mix(mix(texture(u_sand_diffuse, v_TexCoords), texture(u_sand_diffuse2, v_TexCoords), DiffuseTextureMix),
                        mix(texture(u_flat_diffuse, v_TexCoords), texture(u_flat_diffuse2, v_TexCoords), DiffuseTextureMix),
                        clamp(v_PosHeight + 1.0, 0.0, 1.0));

        vec3 ambientColor = 0.08 * sampledDiffuse.rgb;
        vec3 diffuseColor;
        vec3 resultColor;

        //swizzle z and y to rotate Z-aligned normal map 90 degrees around X axis, as like we look at it upside down
        //also scale up texture mapping a bit
        vec3 ShadingNormal = texture(u_normal_map, v_FragPos.xz * 0.0625).xzy;
        ShadingNormal.z *= -1;
        ShadingNormal.x = ShadingNormal.x * 2.0 - 0.5;

        vec3 ShadingNormalFlat = normalize(vec3(0.0, 1.0, 0.0) + 0.5 * ShadingNormal);
        vec3 ShadingNormalShore = normalize(v_Normal + 0.5 * ShadingNormal);

        float DiffuseComponentShore = max(dot(ShadingNormalShore, u_lightDir), 0.0);
        float DiffuseComponentFlat = max(dot(ShadingNormalFlat, u_lightDir), 0.0);
        float diffuseComponent = mix(DiffuseComponentFlat, DiffuseComponentShore, clamp(v_PosHeight, 0.0, 1.0))
                                * mix(0.0, 1.0, clamp(u_lightDir.y * 10, 0.0, 1.0));

        if (u_shadowEnable)
        {
            float luminosity = calculateLuminosity(ShadingNormalShore);
            diffuseColor = luminosity * sampledDiffuse.rgb * diffuseComponent * v_PositionDiffuseComponent;
            resultColor = ambientColor + diffuseColor;
            o_FragColor = vec4(resultColor, sampledDiffuse.a);
            float desaturatingValue = mix(0.0, MAX_DESATURATING_VALUE, luminosity);
            o_FragColor = desaturate(o_FragColor, desaturatingValue);
        }
        else
        {
            diffuseColor = sampledDiffuse.rgb * diffuseComponent * v_PositionDiffuseComponent;
            resultColor = ambientColor + diffuseColor;
            o_FragColor = vec4(resultColor, sampledDiffuse.a);
        }

        float flatBlend = clamp(v_FlatBlend, 0.0, 1.0);
        o_FragColor.a = mix(0.0, 1.0, flatBlend);
    }
}
