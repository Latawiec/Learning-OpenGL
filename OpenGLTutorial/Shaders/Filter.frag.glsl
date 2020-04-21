#version 330
in vec2 FragPos;

out vec4 FragColor;

uniform sampler2D imageTexture;
uniform sampler2D kernelTexture;
uniform int kernelSize;
uniform float imageVerticalScale = 1.f;
uniform float imageHorizontalScale = 1.f;

void main() {
    vec2 normalizedPos = (FragPos + vec2(1.f))/2.f;
    vec2 imageStep = vec2(1.f/imageHorizontalScale, 1.f/imageVerticalScale);
    vec2 kernelStep = vec2(1.f/float(kernelSize), 1.f/float(kernelSize));

    int offsetLeft = kernelSize/2;
    vec3 result = vec3(0);
    for(int x=-offsetLeft; x<=offsetLeft; x++) {
        for(int y=-offsetLeft; y<=offsetLeft; y++) {

            vec2 imageFetchOffset = vec2(float(x), float(y)) * imageStep;
            vec3 color = vec3(texture(imageTexture, normalizedPos + imageFetchOffset));
            color = vec3(color.x + color.y + color.z / 3.f);

            vec3 kernelValue = vec3(texture(kernelTexture, vec2(0.5f) + vec2(float(x), float(y)) * kernelStep));

            result += kernelValue * color;
        }
    }

    FragColor = vec4(vec3(round(length(result))), 1.f);
    //FragColor = vec4(texture(kernelTexture, normalizedPos*imageVerticalScale/float(kernelSize)));
}