#version 330 core

uniform float time;
uniform vec2 resolution;
uniform float fearFactor;

vec2 center = vec2(0.5, 0.5);
float maxLen = length(center);

float random (vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))*
        43758.5453123);
}

void main() {
    vec2 st = gl_FragCoord.xy / resolution;
    vec2 diff = center - st;
    float len = length(diff);

    float rnd = random( st + vec2(1, time / 5.0) );
	
    float opacity = (fearFactor * len) / maxLen;
    gl_FragColor = vec4(vec3(rnd), opacity);
}
