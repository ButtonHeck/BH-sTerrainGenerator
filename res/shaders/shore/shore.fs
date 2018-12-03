#version 450

out vec4 o_FragColor;

in vec3  v_FragPos;
in vec2  v_TexCoords;
in float v_TerrainTypeMix;
in vec3  v_Normal;
in float v_AlphaBlend;

uniform sampler2D u_land_diffuse[2];
uniform sampler2D u_sand_diffuse[2];
uniform sampler2D u_diffuse_mix_map;
uniform float     u_mapDimension;
uniform sampler2D u_normal_map;
uniform vec3      u_lightDir;
uniform bool      u_shadowEnable;
uniform bool      u_debugRenderMode;
uniform float     u_ambientDay;
uniform float     u_ambientNight;
uniform vec3      u_viewPosition;
uniform mat4      u_lightSpaceMatrix[3];

const vec3  NORMAL = vec3(0.0, 1.0, 0.0);
const float MAX_DESATURATING_VALUE_LAND = 0.5;
const float MAX_DESATURATING_VALUE_SHORE = 0.7;

@include shadowSampling.ifs
@include desaturationFunc.ifs

void main()
{
    if (u_debugRenderMode)
        o_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    else
    {
        float DiffuseTextureMix = texture(u_diffuse_mix_map, v_FragPos.xz * u_mapDimension + 0.5).r;
        float TerrainTypeMixClamped = clamp(v_TerrainTypeMix, 0.0, 1.0);
        vec4 sampledDiffuse = mix(mix(texture(u_sand_diffuse[0], v_TexCoords),
                                      texture(u_sand_diffuse[1], v_TexCoords),
                                      DiffuseTextureMix),
                                  mix(texture(u_land_diffuse[0], v_TexCoords),
                                      texture(u_land_diffuse[1], v_TexCoords),
                                      DiffuseTextureMix),
                                  TerrainTypeMixClamped);

        @include shadingVariables.ifs

        vec3 ShadingNormal = clamp((texture(u_normal_map, v_FragPos.xz * 0.125).xzy) * 2.2, vec3(0.0), vec3(1.0));
        ShadingNormal.xyz -= vec3(0.5);

        vec3 ShadingNormalLand = normalize(NORMAL + 1.5 * ShadingNormal);
        vec3 ShadingNormalShore = normalize(v_Normal + 0.25 * ShadingNormal);

        float DiffuseComponentShore = max(dot(ShadingNormalShore, u_lightDir), 0.0);
        float DiffuseComponentLand = max(dot(ShadingNormalLand, u_lightDir), 0.0);
        float sunPositionAttenuation = mix(0.0, 1.0, clamp(u_lightDir.y * 10, 0.0, 1.0));
        float diffuseComponent = mix(DiffuseComponentShore, DiffuseComponentLand, TerrainTypeMixClamped) * sunPositionAttenuation;

        ambientColor = mix(ambientColorNightSelf, ambientColorDaySelf, sunPositionAttenuation);
        ambientColor += nightAmbientColor * (1.0 - sunPositionAttenuation);

        if (u_shadowEnable)
        {
            int shadowMapIndex;
            vec3 projectedCoords;
            vec4 fragPosLightSpaceNear = u_lightSpaceMatrix[0] * vec4(v_FragPos, 1.0);
            vec3 projectedCoordsNear = fragPosLightSpaceNear.xyz * 0.5 + 0.5; //transform from [-1;1] to [0;1]
            if (projectedCoordsNear.x > 0.0  && projectedCoordsNear.x < 1.0 &&
                projectedCoordsNear.y > 0.0  && projectedCoordsNear.y < 1.0 &&
                projectedCoordsNear.z > 0.02 && projectedCoordsNear.z < 1.0)
            {
                shadowMapIndex = 0;
                projectedCoords = projectedCoordsNear;
            }
            else
            {
                vec4 fragPosLightSpaceMiddle = u_lightSpaceMatrix[1] * vec4(v_FragPos, 1.0);
                vec3 projectedCoordsMiddle = fragPosLightSpaceMiddle.xyz * 0.5 + 0.5; //transform from [-1;1] to [0;1]
                if (projectedCoordsMiddle.x > 0.0  && projectedCoordsMiddle.x < 1.0 &&
                    projectedCoordsMiddle.y > 0.0  && projectedCoordsMiddle.y < 1.0 &&
                    projectedCoordsMiddle.z > 0.02 && projectedCoordsMiddle.z < 1.0)
                {
                    shadowMapIndex = 1;
                    projectedCoords = projectedCoordsMiddle;
                }
                else
                {
                    vec4 fragPosLightSpaceFar = u_lightSpaceMatrix[2] * vec4(v_FragPos, 1.0);
                    vec3 projectedCoordsFar = fragPosLightSpaceFar.xyz * 0.5 + 0.5; //transform from [-1;1] to [0;1]
                    shadowMapIndex = 2;
                    projectedCoords = projectedCoordsFar;
                }
            }
            float luminosity = ext_calculateLuminosity(shadowMapIndex, projectedCoords);

            diffuseColor = luminosity * sampledDiffuse.rgb * diffuseComponent;
            resultColor = ambientColor + diffuseColor;
            o_FragColor = vec4(resultColor, sampledDiffuse.a);
            float desaturatingValue = mix(0.0,
                                          mix(MAX_DESATURATING_VALUE_SHORE, MAX_DESATURATING_VALUE_LAND, TerrainTypeMixClamped),
                                          min(luminosity * sunPositionAttenuation, diffuseComponent + SHADOW_INFLUENCE));
            ext_desaturate(o_FragColor, desaturatingValue);
        }
        else
        {
            diffuseColor = sampledDiffuse.rgb * diffuseComponent;
            resultColor = ambientColor + diffuseColor;
            o_FragColor = vec4(resultColor, sampledDiffuse.a);
        }

        float alphaBlend = clamp(v_AlphaBlend, 0.0, 1.0);
        o_FragColor.a = mix(0.0, 1.0, alphaBlend);
    }
}
