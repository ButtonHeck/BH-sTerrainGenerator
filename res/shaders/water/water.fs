#version 450

out vec4 o_FragColor;

in vec3  v_FragPos;
in vec3  v_Normal;

uniform samplerCube u_skybox;
uniform sampler2D   u_specular_map;
uniform sampler2D   u_normal_map;
uniform vec3        u_lightDir;
uniform vec3        u_viewPosition;
uniform bool        u_debugRenderMode;
uniform float       u_ambientDay;
uniform float       u_ambientNight;
uniform mat4        u_lightSpaceMatrix[2];

const float MAX_DESATURATING_VALUE = 0.5;
const vec3  KISSEL_COLOR = vec3(107.0, 30.0, 15.0) / 255.0;
const float KISSEL_ALPHA_MIN = 0.7;
const float REFLECTION_MIX_DAY = 0.25;
const float REFLECTION_MIX_NIGHT = 0.025;

@include shadowSampling.ifs
@include desaturationFunc.ifs

void main()
{
    if (u_debugRenderMode)
        o_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    else
    {
        vec3 sampledDiffuse = KISSEL_COLOR;

        @include shadingVariables.ifs

        vec3 ShadingNormal = clamp((texture(u_normal_map, v_FragPos.xz * 0.125).xzy) * 2.2, vec3(0.0), vec3(1.0));
        ShadingNormal.xyz -= vec3(0.5);
        ShadingNormal = normalize(v_Normal + ShadingNormal);

        vec3 ViewDir = normalize(u_viewPosition - v_FragPos);
        float sunPositionAttenuation = mix(0.0, 1.0, clamp(u_lightDir.y * 10, 0.0, 1.0));

        //fresnel
        float fresnelEffect = (1.0 - clamp(dot(ViewDir, ShadingNormal) + 0.2, 0.0, 1.0))
                               * mix(REFLECTION_MIX_NIGHT, REFLECTION_MIX_DAY, clamp(u_lightDir.y * 10, 0.0, 1.0));

        //diffuse
        float diffuseComponent = max(dot(ShadingNormal, u_lightDir), 0.0) * sunPositionAttenuation;

        ambientColor = mix(ambientColorNightSelf, ambientColorDaySelf, sunPositionAttenuation);
        ambientColor += nightAmbientColor * (1.0 - sunPositionAttenuation);

        //specular
        vec3 Reflect = reflect(-u_lightDir, ShadingNormal);
        float specularComponent = pow(max(dot(Reflect, ViewDir), 0.0), 32.0) * 8 * fresnelEffect;
        vec3 sampledSpecular = texture(u_specular_map, v_FragPos.zx * 0.125).rgb * sunPositionAttenuation;

        //reflect skybox component
        vec3 skyboxCoords = reflect(-ViewDir, ShadingNormal);
        vec4 sampledDiffuseSkybox = vec4(texture(u_skybox, skyboxCoords).rgb, 1.0);

        //shadowing
        vec4 fragPosLightSpaceNear = u_lightSpaceMatrix[0] * vec4(v_FragPos, 1.0);
        vec3 projectedCoordsNear = fragPosLightSpaceNear.xyz * 0.5 + 0.5; //transform from [-1;1] to [0;1]
        int shadowMapIndex;
        vec3 projectedCoords;
        if (projectedCoordsNear.x > 0.0 && projectedCoordsNear.x < 1.0 &&
            projectedCoordsNear.y > 0.0 && projectedCoordsNear.y < 1.0 &&
            projectedCoordsNear.z > 0.02 && projectedCoordsNear.z < 1.0)
        {
            shadowMapIndex = 0;
            projectedCoords = projectedCoordsNear;
        }
        else
        {
            shadowMapIndex = 1;
            vec4 fragPosLightSpaceFar = u_lightSpaceMatrix[1] * vec4(v_FragPos, 1.0);
            vec3 projectedCoordsFar = fragPosLightSpaceFar.xyz * 0.5 + 0.5; //transform from [-1;1] to [0;1]
            projectedCoords = projectedCoordsFar;
        }
        float luminosity = ext_calculateLuminosity(shadowMapIndex, projectedCoords);
        //if we hit the point further than in the farthest shadow map, let it be unshadowed
        if (projectedCoords.z > 1.0)
            luminosity = 1.0;

        diffuseColor = luminosity * sampledDiffuse * diffuseComponent;
        specularColor = specularComponent * sampledSpecular;
        resultColor = ambientColor + diffuseColor + specularColor;

        o_FragColor = vec4(mix(resultColor, sampledDiffuseSkybox.rgb, fresnelEffect), fresnelEffect + KISSEL_ALPHA_MIN);

        float desaturatingValue = mix(0.0,
                                      MAX_DESATURATING_VALUE,
                                      min(luminosity * sunPositionAttenuation, diffuseComponent + SHADOW_INFLUENCE));
        ext_desaturate(o_FragColor, desaturatingValue);
    }
}
