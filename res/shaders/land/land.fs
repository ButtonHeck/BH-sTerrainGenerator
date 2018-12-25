#version 450

out vec4 o_FragColor;

in vec3 v_FragPos;
in vec2 v_TexCoords;

uniform sampler2D u_land_diffuse[2];
uniform sampler2D u_diffuse_mix_map;
uniform sampler2D u_normal_map;
uniform vec3      u_lightDir;
uniform float     u_mapDimension;
uniform bool      u_shadowEnable;
uniform float     u_ambientDay;
uniform float     u_ambientNight;
uniform vec3      u_viewPosition;

const vec3  NORMAL = vec3(0.0, 1.0, 0.0);
const float MAX_DESATURATING_VALUE = 0.5;

@include shadowSampling.ifs
@include desaturationFunc.ifs

void main()
{
    float DiffuseTextureMix = texture(u_diffuse_mix_map, v_FragPos.xz * u_mapDimension + 0.5).r;
    vec4 sampledDiffuse = mix(texture(u_land_diffuse[0], v_TexCoords),
                              texture(u_land_diffuse[1], v_TexCoords),
                              DiffuseTextureMix);

    @include shadingVariables.ifs

    vec3 ShadingNormal = texture(u_normal_map, v_FragPos.xz * 0.125).xzy;
    ShadingNormal.xyz -= vec3(0.5);
    ShadingNormal = normalize(NORMAL + 5.0 * ShadingNormal);

    float sunPositionAttenuation = mix(0.0, 1.0, clamp(u_lightDir.y * 10, 0.0, 1.0));
    float diffuseComponent = max(dot(ShadingNormal, u_lightDir), 0.0) * sunPositionAttenuation;

    ambientColor = mix(ambientColorNightSelf, ambientColorDaySelf, sunPositionAttenuation);
    ambientColor += nightAmbientColor * (1.0 - sunPositionAttenuation);

    if (u_shadowEnable)
    {
        int shadowMapIndex;
        vec3 projectedCoords;
        ext_calculateShadowMapIndexAndProjectedCoords(shadowMapIndex, projectedCoords);
        float luminosity = ext_calculateLuminosity3(shadowMapIndex, projectedCoords, u_bias);

        diffuseColor = luminosity * sampledDiffuse.rgb * diffuseComponent;
        resultColor = ambientColor + diffuseColor;
        o_FragColor = vec4(resultColor, sampledDiffuse.a);
        float desaturatingValue = mix(0.0,
                                      MAX_DESATURATING_VALUE,
                                      min(luminosity * sunPositionAttenuation, diffuseComponent + SHADOW_INFLUENCE));
        ext_desaturate(o_FragColor, desaturatingValue);
    }
    else
    {
        diffuseColor = sampledDiffuse.rgb * diffuseComponent;
        resultColor = ambientColor + diffuseColor;
        o_FragColor = vec4(resultColor, sampledDiffuse.a);
    }
}
