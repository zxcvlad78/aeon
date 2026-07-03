#include "Systems.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace Systems3D {
    void render(entt::registry& registry, sf::RenderWindow& window) {
        auto camera_view = registry.view<Transform3D, Camera3D>();
        
        glm::mat4 view_matrix;
        glm::mat4 proj_matrix;
        bool has_camera = false;
        
        for (auto [entity, transform, camera] : camera_view.each()) {
            view_matrix = camera.get_view_matrix(transform.position);
            proj_matrix = camera.get_projection_matrix();
            has_camera = true;
            break;
        }
        
        if (!has_camera) {
            return;
        }
        
        
        auto mesh_view = registry.view<Transform3D, Mesh3D>();
        for (auto [entity, transform, mesh] : mesh_view.each()) {
            glm::mat4 model_matrix = transform.get_model_matrix();

            const auto& shader = mesh.shader.handle().get();

            sf::Shader::bind(shader);
            shader->setUniform("view", sf::Glsl::Mat4(glm::value_ptr(view_matrix)));
            shader->setUniform("projection", sf::Glsl::Mat4(glm::value_ptr(proj_matrix)));

            shader->setUniform("model", sf::Glsl::Mat4(glm::value_ptr(model_matrix)));

            sf::RenderStates states;
            states.shader = shader;
            window.draw(*mesh.vertexBuffer, 0, mesh.vertexCount, states);
        }

        sf::Shader::bind(nullptr);
    }
}