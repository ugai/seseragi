#ifndef SESERAGI_ALEMBIC_SCAN_H
#define SESERAGI_ALEMBIC_SCAN_H

#include <expected>
#include <filesystem>
#include <memory>
#include <string>

#include <Alembic/Abc/All.h>
#include <Alembic/Abc/ErrorHandler.h>
#include <Alembic/Util/Exception.h>

#include "alembic_node.hpp"

namespace fs = std::filesystem;

namespace seseragi {

class AbcReader {
public:
  static inline std::expected<std::shared_ptr<AbcNode>, std::string>
  read_alembic_file(const std::string abc_file_path) {
    std::cout << abc_file_path << std::endl;
    if (!fs::exists(abc_file_path)) {
      return std::unexpected{
          std::format("file not found: '{}'", abc_file_path)};
    }

    try {
      Alembic::AbcCoreOgawa::ReadArchive reader;
      Alembic::Abc::IArchive archive(reader, abc_file_path,
                                     Alembic::Abc::ErrorHandler::kThrowPolicy);

      const auto obj = archive.getTop();
      auto root_node = std::make_shared<AbcNode>();
      scan_alembic_tree_recursively(root_node, obj);
      return root_node;
    } catch (Alembic::Util::Exception e) {
      return std::unexpected{e};
    }
  };

private:
  static constexpr unsigned int MaxRecursionDepth = 9999;

  static inline void
  scan_alembic_tree_recursively(std::shared_ptr<AbcNode> node,
                                const Alembic::Abc::IObject &obj,
                                const unsigned int depth = 0) {
    if (depth > MaxRecursionDepth)
      return;

    node->name = obj.getName();
    node->full_name = obj.getFullName();
    const auto meta_data = obj.getMetaData();
    for (const auto &[k, v] : meta_data) {
      node->meta_data[k] = v;
    }

    for (int i = 0; i < obj.getNumChildren(); ++i) {
      const Alembic::Abc::IObject child_obj = obj.getChild(i);
      auto child_node = std::make_shared<AbcNode>();
      scan_alembic_tree_recursively(child_node, child_obj, depth + 1);
      node->children.push_back(child_node);
    }
  };
};

} // namespace seseragi
#endif // SESERAGI_ALEMBIC_SCAN_H
