#version 330

in vec2 fragTexCoord;

out vec4 fragColor;
uniform sampler2D texture0;

void main() {
    vec4 color = texture(texture0, fragTexCoord);
    if (color.r == 1.0 && color.g == 1.0 && color.b = 1.0) 
        fragColor = vec4(1.0, 0.0, 0.0, 1.0);
    else
        fragColor = color;
} 