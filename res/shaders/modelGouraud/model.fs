#version 450
layout (early_fragment_tests) in;

out vec4 o_FragColor;

in vec2  v_TexCoords;
in float v_DiffuseComponent;
in float v_SpecularComponent;
in float v_AlphaValue;
in float v_SunPositionAttenuation;
in float v_NormalY;
in vec3  v_FragPos;

uniform sampler2D u_texture_diffuse1;
uniform sampler2D u_texture_specular1;
uniform bool      u_shadow;
uniform vec3      u_lightDir;
uniform bool      u_shadowEnable;
uniform bool      u_useLandBlending;
uniform float     u_ambientDay;
uniform float     u_ambientNight;
uniform vec3      u_viewPosition;
uniform mat4      u_lightSpaceMatrix[3];

const float MAX_DESATURATING_VALUE = 0.5;

@include shadowSampling.ifs
@include desaturationFunc.ifs

void main()
{
    vec4 sampledDiffuse = texture(u_texture_diffuse1, v_TexCoords);
    vec4 sampledSpecular = sampledDiffuse * texture(u_texture_specular1, v_TexCoords).r;

    @include shadingVariables.ifs

    ambientColor = mix(ambientColorNightSelf, ambientColorDaySelf, v_SunPositionAttenuation);
    ambientColor += nightAmbientColor * (1.0 - v_SunPositionAttenuation);

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

        diffuseColor = luminosity * sampledDiffuse.rgb * v_DiffuseComponent;
        specularColor = luminosity * sampledSpecular.rgb * v_SpecularComponent;
        resultColor = ambientColor + diffuseColor + specularColor;
        o_FragColor = vec4(resultColor, sampledDiffuse.a);
        float desaturatingValue = mix(0.0,
                                      MAX_DESATURATING_VALUE,
                                      min(luminosity * v_SunPositionAttenuation, v_DiffuseComponent + SHADOW_INFLUENCE));
        ext_desaturate(o_FragColor, desaturatingValue);
        o_FragColor += clamp(o_FragColor * v_NormalY * 0.5, -0.04, 0.0);
    }
    else
    {
        diffuseColor = sampledDiffuse.rgb * v_DiffuseComponent;
        specularColor = sampledSpecular.rgb * v_SpecularComponent;
        resultColor = ambientColor + diffuseColor + specularColor;
        o_FragColor = vec4(resultColor, sampledDiffuse.a);
        o_FragColor += clamp(o_FragColor * v_NormalY * 0.5, -0.04, 0.0);
    }

    if(u_useLandBlending)
        o_FragColor.a = mix(0.0, 1.0, v_AlphaValue);
}
