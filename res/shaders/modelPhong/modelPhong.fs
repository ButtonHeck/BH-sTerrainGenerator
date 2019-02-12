#version 450
#extension GL_ARB_bindless_texture : enable
#extension GL_ARB_gpu_shader_int64 : enable

out vec4 o_FragColor;

in vec2         v_TexCoords;
in vec3         v_Normal;
in float        v_AlphaValue;
in vec3         v_FragPos;
flat in uvec2   v_TexIndices;

//arrays of bindless texture handlers
uniform uint64_t  u_textureDiffuse[200];
uniform uint64_t  u_textureSpecular[150];
uniform bool      u_shadow;
uniform vec3      u_lightDir;
uniform bool      u_shadowEnable;
uniform bool      u_useLandBlending;
uniform bool      u_isGrass;
uniform float     u_ambientDay;
uniform float     u_ambientNight;
uniform vec3      u_viewPosition;
uniform bool      u_isLowPoly;

const float MAX_DESATURATING_VALUE = 0.5;
const float SPECULAR_SHININESS = 4.0;

@include shadowSampling.ifs
@include desaturationFunc.ifs

void main()
{
    vec4 sampledDiffuse = texture(sampler2D(u_textureDiffuse[v_TexIndices.x]), v_TexCoords);
    vec4 sampledSpecular = sampledDiffuse * texture(sampler2D(u_textureSpecular[v_TexIndices.y]), v_TexCoords).r;

    @include shadingVariables.ifs

    float sunPositionAttenuation = mix(0.0, 1.0, clamp(u_lightDir.y * 3, 0.0, 1.0));
    vec3 shadingNormal = normalize(v_Normal);

    float diffuseComponent = max(dot(shadingNormal, u_lightDir), 0.0) * sunPositionAttenuation * (1.0 - u_ambientDay);
    if (u_isGrass)
    {
        /*
        make grass diffuse lighting look more natural by calculating diffuse component for its reversed normal
        because there are no backfaces for the grass. Then make it no less than defined minimum (calculated empirically)
        in cases a leaf is almost vertical (horizontally oriented normal) and the sun is at zenith (vertical light direction),
        otherwise grass coloring would look unnatural too.
        */
        float diffuseComponentNegative = max(dot(-shadingNormal, u_lightDir), 0.0) * sunPositionAttenuation * (1.0 - u_ambientDay);
        diffuseComponent = max(diffuseComponent, diffuseComponentNegative);
        diffuseComponent = mix(diffuseComponent, max(diffuseComponent, 0.33), sunPositionAttenuation);
    }

    vec3 lightDirectionReflected = reflect(-u_lightDir, shadingNormal);
    vec3 viewDirection = normalize(u_viewPosition - v_FragPos);
    float specularComponent = pow(max(dot(lightDirectionReflected, viewDirection), 0.0), SPECULAR_SHININESS) * sunPositionAttenuation;

    ambientColor = mix(ambientColorNightSelf, ambientColorDaySelf, sunPositionAttenuation);
    //add a bit of red in the night time
    ambientColor += nightAmbientColor * (1.0 - sunPositionAttenuation);

    if (u_shadowEnable)
    {
        int shadowMapIndex;
        vec3 projectedCoords;
        float luminosity;
        if (!u_isLowPoly)
        {
            //use more precise algorithms for nearby fragments
            ext_calculateShadowMapIndexAndProjectedCoords(shadowMapIndex, projectedCoords);
            luminosity = ext_calculateLuminosity5(shadowMapIndex, projectedCoords, u_bias);
        }
        else
        {
            //low poly models could not be caught in the nearest layer of shadow map
            ext_calculateShadowMapIndexAndProjectedCoordsLowp(shadowMapIndex, projectedCoords);
            luminosity = ext_calculateLuminosity3Lowp(shadowMapIndex, projectedCoords, u_bias);
        }

        diffuseColor = luminosity * sampledDiffuse.rgb * diffuseComponent;
        //make sure that shadowed fragment do not have any specular reflection
        float specularLuminosityInfluence = 1.0 - ((1.0 - luminosity) * SHADOW_INFLUENCE_RECIPROCAL);
        specularColor = specularLuminosityInfluence * luminosity * specularComponent * sampledSpecular.rgb;
        resultColor = ambientColor + diffuseColor + specularColor;
        o_FragColor = vec4(resultColor, sampledDiffuse.a);

        //apply no desaturation if there is no sun lit surface, or little if diffuse component is low enough
        float desaturatingValue = mix(0.0,
                                      MAX_DESATURATING_VALUE,
                                      min(luminosity * sunPositionAttenuation, diffuseComponent + SHADOW_INFLUENCE));
        ext_desaturate(o_FragColor, desaturatingValue);
        //diminish Y negative oriented surfaces a bit (just a visual flavour)
        o_FragColor += clamp(o_FragColor * shadingNormal.y * 0.5, -0.04, 0.0);
    }
    else
    {
        diffuseColor = sampledDiffuse.rgb * diffuseComponent;
        specularColor = sampledSpecular.rgb * specularComponent;
        resultColor = ambientColor + diffuseColor + specularColor;
        o_FragColor = vec4(resultColor, sampledDiffuse.a);
        //no desaturation here as it requires luminosity calculation
        o_FragColor += clamp(o_FragColor * shadingNormal.y * 0.5, -0.04, 0.0);
    }

    //make closer to ground fragment color mix a bit with land texture (just a visual flavour)
    if(u_useLandBlending)
        o_FragColor.a = mix(0.0, 1.0, v_AlphaValue);
}
