#version 330

in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragPosition;

uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform vec4 fogColor;
uniform float fogStart;
uniform float fogEnd;
uniform vec3 cameraPos;

void main()
{
    vec4 texelColor = texture(texture0, fragTexCoord) * colDiffuse * fragColor;
    float dist = length(fragPosition - cameraPos);
    float fogIntensity = clamp((dist - fogStart) / (fogEnd - fogStart), 0.0, 1.0);
    gl_FragColor = mix(texelColor, fogColor, fogIntensity);
}