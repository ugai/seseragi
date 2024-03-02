#ifndef SESERAGI_ALEMBIC_NODE_H
#define SESERAGI_ALEMBIC_NODE_H

#include <format>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <Alembic/AbcCoreFactory/IFactory.h>
#include <nlohmann/json.hpp>

#include "../utils.hpp"

namespace seseragi::alembic {

class Node : public std::enable_shared_from_this<Node> {
public:
  using Ptr = std::shared_ptr<Node>;

  unsigned int depth;
  std::string name;
  std::string full_name;
  std::map<std::string, std::string> meta_data;
  std::vector<std::shared_ptr<Node>> children;

  inline void as_list(std::vector<std::shared_ptr<Node>> &list,
                      unsigned int depth = 0) {
    spdlog::info("'{}'", this->name);
    list.push_back(this->shared_from_this());
    for (const auto &child : this->children) {
      child->as_list(list, depth + 1);
    }
  }

  inline void to_json(nlohmann::ordered_json &j) {
    j["name"] = name;
    j["fullName"] = full_name;
    j["metadata"] = meta_data;

    if (children.size() == 0)
      return;

    j["children"] = nlohmann::ordered_json::array();
    for (const auto child : children) {
      auto cj = nlohmann::ordered_json::object();
      child->to_json(cj);
      j["children"].push_back(cj);
    }
  };
};

} // namespace seseragi::alembic
#endif // SESERAGI_ALEMBIC_NODE_H
