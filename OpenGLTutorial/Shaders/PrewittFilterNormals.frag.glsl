#version 330
in vec2 FragPos;

out vec4 FragColor;

uniform sampler2D imageTexture;
uniform float imageVerticalScale = 1.f;
uniform float imageHorizontalScale = 1.f;

void main() {
    vec2 normalizedPos = (FragPos + vec2(1.f))/2.f;
    vec2 imageStep = vec2(1.f/imageHorizontalScale, 1.f/imageVerticalScale);

    vec2 top_left_coord = normalizedPos     + vec2(-imageStep.x, imageStep.y);
    vec2 top_mid_coord = normalizedPos      + vec2(0.f,          imageStep.y);
    vec2 top_right_coord = normalizedPos    + vec2(imageStep.x,  imageStep.y);
    vec2 bottom_left_coord = normalizedPos  + vec2(-imageStep.x, -imageStep.y);
    vec2 bottom_mid_coord = normalizedPos   + vec2(0.f,          -imageStep.y);
    vec2 bottom_right_coord = normalizedPos + vec2(imageStep.x,  -imageStep.y);
    vec2 center_left_coord = normalizedPos  + vec2(-imageStep.x, 0.f);
    vec2 center_right_coord = normalizedPos + vec2(imageStep.x,  0.f);

    
    vec3 color_top_left     = vec3(texture(imageTexture, top_left_coord));
    vec3 color_top_mid      = vec3(texture(imageTexture, top_mid_coord));
    vec3 color_top_right    = vec3(texture(imageTexture, top_right_coord));
    vec3 color_bottom_left  = vec3(texture(imageTexture, bottom_left_coord));
    vec3 color_bottom_mid   = vec3(texture(imageTexture, bottom_mid_coord));
    vec3 color_bottom_right = vec3(texture(imageTexture, bottom_right_coord));
    vec3 color_center_left  = vec3(texture(imageTexture, center_left_coord));
    vec3 color_center_right = vec3(texture(imageTexture, center_right_coord));

    // Horizontal
    float result_x = 0.f;
    result_x += 1.f - abs(dot(color_top_left, color_top_right));
    result_x += 1.f - abs(dot(color_center_left, color_center_right));
    result_x += 1.f - abs(dot(color_bottom_left, color_bottom_right));

    // Horizontal
    float result_y = 0.f;
    result_y += 1.f - abs(dot(color_top_left, color_bottom_left));
    result_y += 1.f - abs(dot(color_top_mid, color_bottom_mid));
    result_y += 1.f - abs(dot(color_top_right, color_bottom_right));
    
    //vec3 outputColor = vec3(step(0.015, length(result_x)) + step(0.015, length(result_y)));
    vec3 outputColor = vec3(step(0.4, abs(result_x + result_y)));

    FragColor = vec4(vec3(1.f) - outputColor, 1.f);
    // FragColor = texture(imageTexture, normalizedPos);
}