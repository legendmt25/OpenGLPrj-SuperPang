#version 330 core
layout (location = 0) in vec4 vertex;

out vec2 textureCoordinates;

uniform mat4 model;
uniform mat4 projection;

void main() {
	gl_Position = projection * model * vec4(vertex.xy, 0.0f, 1.0f);
	textureCoordinates = vertex.zw;
}