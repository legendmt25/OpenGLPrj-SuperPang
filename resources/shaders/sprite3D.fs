#version 330 core

in vec2 TextureCoordinates;
out vec4 color;

uniform sampler2D image;
uniform vec3 sprite3DColor;

void main() {
    color = vec4(sprite3DColor, 1.0f) * texture(image, TextureCoordinates);
}