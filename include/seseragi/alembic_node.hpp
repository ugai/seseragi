#ifndef SESERAGI_ALEMBIC_NODE_H
#define SESERAGI_ALEMBIC_NODE_H

#include <format>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "utils.hpp"

namespace seseragi {

struct AbcNode : public std::enable_shared_from_this<AbcNode> {
  unsigned int depth;
  std::string name;
  std::string full_name;
  std::map<std::string, std::string> meta_data;
  std::vector<std::shared_ptr<AbcNode>> children;

  inline void as_list(std::vector<std::shared_ptr<AbcNode>> &list,
                      unsigned int depth = 0) {
    spdlog::info("'{}'", this->name);
    list.push_back(this->shared_from_this());
    for (const auto &child : this->children) {
      child->as_list(list, depth + 1);
    }
  }
};

} // namespace seseragi
#endif // SESERAGI_ALEMBIC_NODE_H