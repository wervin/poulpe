#version 450

layout(location = 0) out vec4 outFragColor;

layout(binding = 0) uniform UniformBufferObject {
    vec3 resolution;
    float time;
} ubo;

vec3 palette( float t ) {
    vec3 a = vec3(0.5, 0.5, 0.5);
    vec3 b = vec3(0.5, 0.5, 0.5);
    vec3 c = vec3(1.0, 1.0, 1.0);
    vec3 d = vec3(0.263,0.416,0.557);

    return a + b*cos( 6.28318*(c*t+d) );
}

void main() {
    vec2 uv = (gl_FragCoord.xy * 2.0 - ubo.resolution.xy) / ubo.resolution.y;
    vec3 finalColor = vec3(0.0);
    vec2 uv0 = uv;
    float d0 = length(uv0);

    for (float i = 0.0; i < 4.0; i++)
    {
        uv = fract(uv * 1.5) - 0.5;

        float d = length(uv) * exp(-d0);

        vec3 color = palette(d0 + i * 0.4 + ubo.time * 0.4);

        d = sin(d * 8.0 + ubo.time) / 8.0;
        d = abs(d);
        d = pow(0.01 / d, 1.2);

        finalColor += color * d;
    }

    outFragColor = vec4(finalColor, 1.0);
}