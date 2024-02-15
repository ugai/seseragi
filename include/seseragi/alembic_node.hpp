#ifndef SESERAGI_ALEMBIC_NODE_H
#define SESERAGI_ALEMBIC_NODE_H

#include <format>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <seseragi/utils.hpp>

namespace seseragi {

struct AbcNode {
  std::string name;
  std::string full_name;
  std::map<std::string, std::string> meta_data;

  std::vector<std::shared_ptr<AbcNode>> children;

  inline std::string as_tree_text() const {
    std::stringstream ss{};
    create_tree_text_recursively(ss, this);
    return ss.str();
  }

  inline static void create_tree_text_recursively(std::stringstream &ss,
                                                  const AbcNode *node,
                                                  unsigned int depth = 0) {
    spdlog::info("'{}'", node->name);

    utils::indent(ss, depth);
    ss << node->name << std::endl;

    for (const auto &[k, v] : node->meta_data) {
      utils::indent(ss, depth + 1);
      ss << std::format("- '{}': {}", k, v) << std::endl;
    }

    for (const auto &child : node->children) {
      AbcNode::create_tree_text_recursively(ss, child.get(), depth + 1);
    }
  }

  inline std::vector<std::string> &as_tree_list() const {
    std::vector<std::string> strs{};
    create_tree_list_recursively(strs, this);
    return strs;
  }

  inline static void
  create_tree_list_recursively(std::vector<std::string> &strs,
                               const AbcNode *node, unsigned int depth = 0) {
    spdlog::info("'{}'", node->name);

    std::stringstream ss;
    ss << node->name << std::endl;

    for (const auto &[k, v] : node->meta_data) {
      utils::indent(ss, depth + 1);
      ss << std::format("- '{}': {}", k, v) << std::endl;
    }

    strs.push_back(ss.str());

    for (const auto &child : node->children) {
      AbcNode::create_tree_text_recursively(ss, child.get(), depth + 1);
    }
  }
};

} // namespace seseragi
#endif // SESERAGI_ALEMBIC_NODE_H