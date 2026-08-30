#pragma once

#include <string>

namespace Barnocraft::Rendering {

    const std::string VERTEX_SHADER_SOURCE = R"(
        #version 330 core
        
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aNormal;
        layout (location = 2) in vec2 aTexCoord;
        
        uniform mat4 uModel;
        uniform mat4 uView;
        uniform mat4 uProjection;
        
        out vec3 Normal;
        out vec3 FragPos;
        out vec2 TexCoord;
        
        void main() {
            FragPos = vec3(uModel * vec4(aPos, 1.0));
            Normal = mat3(transpose(inverse(uModel))) * aNormal;
            TexCoord = aTexCoord;
            gl_Position = uProjection * uView * vec4(FragPos, 1.0);
        }
    )";

    const std::string FRAGMENT_SHADER_SOURCE = R"(
        #version 330 core
        
        in vec3 Normal;
        in vec3 FragPos;
        in vec2 TexCoord;
        
        uniform sampler2D uTexture;
        uniform vec3 uLightPos;
        uniform vec3 uViewPos;
        uniform vec3 uLightColor;
        
        out vec4 FragColor;
        
        void main() {
            // Simple lighting
            vec3 norm = normalize(Normal);
            vec3 lightDir = normalize(uLightPos - FragPos);
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = diff * uLightColor;
            
            // Ambient
            float ambientStrength = 0.1;
            vec3 ambient = ambientStrength * uLightColor;
            
            // Combine
            vec3 result = (ambient + diffuse) * texture(uTexture, TexCoord).rgb;
            FragColor = vec4(result, 1.0);
        }
    )";

} // namespace Barnocraft::Rendering
