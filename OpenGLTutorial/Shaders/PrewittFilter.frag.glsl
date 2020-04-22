#version 330
in vec2 FragPos;

out vec4 FragColor;

uniform sampler2D imageTexture;
uniform sampler2D prewittVerticalTexture;
uniform sampler2D prewittHorizontalTexture;
uniform int kernelSize;
uniform float imageVerticalScale = 1.f;
uniform float imageHorizontalScale = 1.f;

void main() {
    vec2 normalizedPos = (FragPos + vec2(1.f))/2.f;
    vec2 imageStep = vec2(1.f/imageHorizontalScale, 1.f/imageVerticalScale);
    vec2 kernelStep = vec2(1.f/float(kernelSize), 1.f/float(kernelSize));

    int offsetLeft = kernelSize/2;
    vec3 result_x = vec3(0);
    vec3 result_y = vec3(0);
    for(int x=-offsetLeft; x<=offsetLeft; x++) {
        for(int y=-offsetLeft; y<=offsetLeft; y++) {

            vec2 imageFetchOffset = vec2(float(x), float(y)) * imageStep;
            vec3 color = vec3(texture(imageTexture, normalizedPos + imageFetchOffset));
            float linear_depth = (2.0 * 0.1f) / (100.f + 0.1f - color.r * (100.f - 0.1f));
            color = vec3(linear_depth);

            // float linear_depth = (2.0 * near) / (far + near - texture2D(depthtex, texcoord).r * (far - near));

            float verticalValue = vec3(texture(prewittVerticalTexture, vec2(0.5f) + vec2(float(x), float(y)) * kernelStep)).r;
            float horizontalValue = vec3(texture(prewittHorizontalTexture, vec2(0.5f) + vec2(float(x), float(y)) * kernelStep)).r;

            result_x += horizontalValue * color;
            result_y += verticalValue * color;
        }
    }

    // 0.1f, 100.f

    vec3 outputColor = vec3(step(0.015, length(result_x)) + step(0.015, length(result_y)));

    FragColor = vec4(vec3(1.f) - outputColor, length(outputColor));
}