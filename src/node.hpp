#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "material.hpp"
#include "geometry.hpp"

class node {
public:
    node (const std::string& node_name = "");
    ~node();

    void set_transformation(const glm::mat4& transformation);

    glm::mat4 get_global_transformation() const;
    void set_global_transformation(glm::mat4 global_transformation) const;

    void add_child(node* child);
    void add_geometry(const mesh_t& geometry);
    void update(float delta_time);
private:
    std::string m_name;
    node* m_parent;
    std::vector<node*> m_children;
    glm::mat4 m_transformation;
    std::vector<mesh_t> m_geometries;

    // Calculate global transformation matrix
    void calculate_global_transformation() const;

    mutable glm::mat4 m_global_transformation;
};

node::node(const std::string& node_name)
    : m_name(node_name), m_parent(nullptr), m_transformation(glm::mat4(1.0f)), m_global_transformation(glm::mat4(1.0f)) {}

node::~node() {
    for (node* child : m_children) {
        delete child;
    }
}

void node::set_transformation(const glm::mat4& transformation) {
    m_transformation = transformation;
    calculate_global_transformation();
}

glm::mat4 node::get_global_transformation() const {
    return m_global_transformation;
}

void node::set_global_transformation(glm::mat4 global_transformation) const {
    m_global_transformation = global_transformation;
}

void node::add_child(node* child) {
    child->m_parent = this;
    m_children.push_back(child);
}

void node::add_geometry(const mesh_t& geometry) {
    m_geometries.push_back(geometry);
}

void node::update(float delta_time) {
    for (node* child : m_children) {
        child->update(delta_time);
    }
}

void node::calculate_global_transformation() const {
    if (m_parent) {
        m_global_transformation = m_parent->get_global_transformation() * m_transformation;
    } else {
        m_global_transformation = m_transformation;
    }
    for (node* child : m_children) {
        child->calculate_global_transformation();
    }
}
