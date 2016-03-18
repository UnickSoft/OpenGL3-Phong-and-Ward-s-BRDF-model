#version 330

in vec3 Position;
in vec3 Normal;
in vec3 Tangent;
in vec2 TexCoord;

out vec3 vertexNormal;
out vec4 vertexPosition;
out vec3 vertexTangent;
out vec2 vertexTexCoord;

uniform mat4 ProjectionMatrix, CameraMatrix, ModelMatrix, NormalMatrix;

void main(void)
{
    mat4 viewModelMatrix = CameraMatrix * ModelMatrix;
	gl_Position = ProjectionMatrix *  viewModelMatrix * vec4(Position, 1.0);
    
    vertexNormal   = normalize(mat3(NormalMatrix) * Normal);
    vertexTangent  = normalize(mat3(NormalMatrix) * Tangent);
    vertexPosition = viewModelMatrix * vec4(Position, 1.0);
    vertexTexCoord = TexCoord;
}
