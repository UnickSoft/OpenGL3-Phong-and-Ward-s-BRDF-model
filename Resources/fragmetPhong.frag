#version 330

in vec3 vertexNormal;
in vec4 vertexPosition;
in vec3 vertexBitangent;
in vec2 vertexTexCoord;

uniform vec3 ambientColor;
uniform vec3 diffuseColor;
uniform vec3 specularColor;
uniform float shininess;
uniform vec3 lightPosition;
uniform mat4 CameraMatrix;

out vec4 fragColor;

vec4 PhongShading()
{
    vec3 L = normalize(lightPosition - vertexPosition.xyz);
    vec3 E = normalize(-vertexPosition.xyz); // we are in Eye Coordinates, so EyePos is (0,0,0)
    vec3 R = normalize(-reflect(L.xyz, vertexNormal.xyz));
    
    //calculate Ambient Term:
    vec4 Iamb = vec4(ambientColor, 1.0);
    
    //calculate Diffuse Term:
    vec4 Idiff = vec4(diffuseColor * max(dot(vertexNormal, L), 0.0), 1.0);
    Idiff = clamp(Idiff, 0.0, 1.0);
    
    // calculate Specular Term:
    vec4 Ispec = vec4(specularColor * pow(max(dot(R, E), 0), shininess), 1.0);
    Ispec = clamp(Ispec, 0.0, 1.0);
    
    // write Total Color:
    return Iamb + Idiff + Ispec;
}

void main(void)
{
    // write Total Color:
    fragColor = PhongShading();
}
