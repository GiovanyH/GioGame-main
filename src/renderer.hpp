#include "object.hpp"

void render_object(object_t *object, glm::mat4 view, glm::mat4 proj) {
    for (mesh_t *mesh : object->meshes) {
        glUseProgram(mesh->material->shader_program);

        // Set texture if available
        if (mesh->material->texture_id != 0) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, mesh->material->texture_id);
            glUniform1i(glGetUniformLocation(mesh->material->shader_program, "texture_diffuse"), 0);
            glUniform1i(glGetUniformLocation(mesh->material->shader_program, "use_texture"), GL_TRUE);
        } else {
            glUniform1i(glGetUniformLocation(mesh->material->shader_program, "use_texture"), GL_FALSE);
        }

        // Set material properties
        glUniform4fv(glGetUniformLocation(mesh->material->shader_program, "base_color"), 1, &mesh->material->base_color[0]);
        glUniform1f(glGetUniformLocation(mesh->material->shader_program, "roughness"), mesh->material->roughness / 255.0f);
        glUniform1f(glGetUniformLocation(mesh->material->shader_program, "metallic"), mesh->material->metallic / 255.0f);

        // Set matrices
        glUniformMatrix4fv(glGetUniformLocation(mesh->material->shader_program, "model"), 1, GL_FALSE, &object->model[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(mesh->material->shader_program, "view"), 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(glGetUniformLocation(mesh->material->shader_program, "projection"), 1, GL_FALSE, &proj[0][0]);

        // Bind VAO and draw the mesh
        glBindVertexArray(mesh->VAO);
        glDrawArrays(GL_TRIANGLES, 0, mesh->vertices.size() / 8);

        // Unbind VAO
        glBindVertexArray(0);
    }
}