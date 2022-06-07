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


//#version 330 core
//out vec4 FragColor;
//
//float near = 0.1; 
//float far  = 100.0; 
//  
//float LinearizeDepth(float depth) 
//{
//  float z = depth * 2.0 - 1.0; // back to NDC 
//  return (2.0 * near * far) / (far + near - z * (far - near));
//}
//
//void main()
//{             
//  float depth = LinearizeDepth(gl_FragCoord.z) / far; // divide by far for demonstration
//  FragColor = vec4(vec3(depth), 1.0);
//}



