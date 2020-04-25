#version 330
in vec2 FragPos;

out vec4 FragColor;

uniform sampler2D imageTexture;
uniform sampler2D bayerMatrixTexture;
uniform float matrixVerticalScale = 1.f;
uniform float matrixHorizontalScale = 1.f;

void main() {
    vec2 normalizedPos = (FragPos + vec2(1.f))/2.f;
    vec2 scaledMatrixPos = vec2(matrixHorizontalScale, matrixVerticalScale) * normalizedPos;
    
    vec3 imageColor = texture(imageTexture, normalizedPos).xyz; 
    //vec3 grayscaleColor = vec3(imageColor.x + imageColor.y + imageColor.z / 3.f);
    //vec3 grayscaleColor = vec3(min(imageColor.x, min(imageColor.y, imageColor.z)));
    vec3 grayscaleColor = vec3(0.21f * imageColor.r + 0.72 * imageColor.g + 0.07f * imageColor.b);
    vec3 matrixValue = texture(bayerMatrixTexture, scaledMatrixPos).xyz;
    
    //FragColor = vec4(round(grayscaleColor + .9f*(matrixValue - 0.45f)), 1.f);

    FragColor = vec4((imageColor + vec3(1.f))/2.f, 1.f);
}