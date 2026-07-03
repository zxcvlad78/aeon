#pragma once
#include <SFML/Graphics.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <memory>
#include <entt/entt.hpp>

struct Transform3D {
    glm::vec3 position{0.0f};
    glm::vec3 rotation{0.0f};
    glm::vec3 scale{1.0f};

    glm::mat4 get_model_matrix() const {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::rotate(model, glm::radians(rotation.x), {1.0f, 0.0f, 0.0f});
        model = glm::rotate(model, glm::radians(rotation.y), {0.0f, 1.0f, 0.0f});
        model = glm::rotate(model, glm::radians(rotation.z), {0.0f, 0.0f, 1.0f});
        model = glm::scale(model, scale);
        return model;
    }
};

struct Mesh3D {
    entt::resource<sf::Shader> shader;
    std::shared_ptr<sf::VertexBuffer> vertexBuffer;
    unsigned int vertexCount{0};
};

struct Camera3D {
    float fov{45.0f};
    float aspectRatio{4.0f / 3.0f};
    float nearPlane{0.1f};
    float farPlane{100.0f};
    
    glm::vec3 front{0.0f, 0.0f, -1.0f};
    glm::vec3 up{0.0f, 1.0f, 0.0f};

    glm::mat4 get_view_matrix(const glm::vec3& pos) const {
        return glm::lookAt(pos, pos + front, up);
    }
    glm::mat4 get_projection_matrix() const {
        return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
    }
};