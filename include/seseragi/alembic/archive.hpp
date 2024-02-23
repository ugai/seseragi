#ifndef SESERAGI_ALEMBIC_ARCHIVE_H
#define SESERAGI_ALEMBIC_ARCHIVE_H

#include <format>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <Alembic/AbcCoreFactory/IFactory.h>
#include <nlohmann/json.hpp>

#include "../utils.hpp"
#include "node.hpp"

namespace seseragi::alembic {

struct Archive {
  std::string core_type_name;
  std::vector<std::string> time_samplings;
  double start_time;
  double end_time;

  std::shared_ptr<Node> top_node = std::make_shared<Node>();
  std::vector<std::shared_ptr<Node>> all_nodes;

  static inline std::string
  get_core_type_name(Alembic::AbcCoreFactory::IFactory::CoreType core_type) {
    // clang-format off
    switch (core_type) {
      case Alembic::AbcCoreFactory::IFactory::CoreType::kHDF5: return "HDF5";
      case Alembic::AbcCoreFactory::IFactory::CoreType::kOgawa: return "Ogawa";
      case Alembic::AbcCoreFactory::IFactory::CoreType::kLayer: return "Layer";
      default: return "Unknown";
    }
    // clang-format on
  };

  inline void to_json(nlohmann::ordered_json &j) {
    j["core_type_name"] = core_type_name;
    j["time_samplings"] = time_samplings;
    j["start_time"] = start_time;
    j["end_time"] = end_time;
    auto tj = nlohmann::ordered_json::object();
    top_node->to_json(tj);
    j["top"] = tj;
  };
};

} // namespace seseragi::alembic
#endif // SESERAGI_ALEMBIC_ARCHIVE_H
