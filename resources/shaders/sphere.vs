 #version 330 core
layout (location = 0) in vec3 vertex;
layout (location = 2) in vec3 normals;
layout (location = 1) in vec2 textureCoordinates;

out vec2 TextureCoordinates;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

void main() {
	gl_Position = projection * view * model * vec4(vertex, 1.0f);
	TextureCoordinates = textureCoordinates;
}