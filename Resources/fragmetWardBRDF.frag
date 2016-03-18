#version 330

in vec3 vertexNormal;
in vec4 vertexPosition;
in vec3 vertexTangent;
in vec2 vertexTexCoord;


uniform sampler2D MapTexture;
uniform vec3 ambientColor;
uniform vec3 diffuseColor;
uniform vec3 specularColor;
uniform float shininess;
uniform vec3 lightPosition;
uniform mat4 CameraMatrix;

out vec4 fragColor;


vec4 WardBRDFShading()
{
    vec3 L = normalize(lightPosition - vertexPosition.xyz);
    vec3 E = normalize(-vertexPosition.xyz); // we are in Eye Coordinates, so EyePos is (0,0,0)
    vec3 R = normalize(-reflect(L.xyz, vertexNormal.xyz));
    
    //calculate Ambient Term:
    vec4 Iamb = vec4(ambientColor, 1.0);
    
    //calculate Diffuse Term:
    float dotLN =  max(dot(vertexNormal, L), 0.0);
    vec4 Idiff  = vec4(diffuseColor * dotLN, 1.0);
    Idiff = clamp(Idiff, 0.0, 1.0);
    
    // calculate Specular Term:
    vec4 Ispec = vec4(0.0);
    if (dotLN > 0.0)
    {
        vec3 halfwayVector = normalize(L + E);
        vec3 binormalDirection = cross(vertexNormal, vertexTangent);
        
        float _AlphaX = 0.4;
        float _AlphaY = 0.1;
        float dotHN = dot(halfwayVector, vertexNormal);
        float dotVN = dot(E, vertexNormal);
        float dotHTAlphaX =
        dot(halfwayVector, vertexTangent) / _AlphaX;
        
        float dotHBAlphaY = dot(halfwayVector,
                                binormalDirection) / _AlphaY;
        
        Ispec = vec4(specularColor
                     * sqrt(max(0.0, dotLN / dotVN))
                     * exp(-2.0 * (dotHTAlphaX * dotHTAlphaX
                                   + dotHBAlphaY * dotHBAlphaY) / (1.0 + dotHN)), 1.0);

        Ispec = clamp(Ispec, 0.0, 1.0);
    }
    
    // write Total Color:
    return Iamb + Idiff + Ispec;
}

void main(void)
{
    // write Total Color:
    fragColor = WardBRDFShading();
}
