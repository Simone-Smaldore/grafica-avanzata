#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

void main()
{
    vec4 textColor = texture(texture_diffuse1, TexCoords);
    if(textColor.a < 0.3) 
        discard;
    FragColor = textColor;
}