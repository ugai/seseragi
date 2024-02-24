#ifndef SESERAGI_ALEMBIC_READER_H
#define SESERAGI_ALEMBIC_READER_H

#include <expected>
#include <filesystem>
#include <memory>
#include <string>

#include <Alembic/Abc/All.h>
#include <Alembic/Abc/ErrorHandler.h>
#include <Alembic/AbcCoreFactory/IFactory.h>
#include <Alembic/Util/Exception.h>

#include "archive.hpp"
#include "node.hpp"

namespace fs = std::filesystem;

namespace seseragi::alembic {

class Reader {
public:
  static inline std::expected<std::shared_ptr<Archive>, std::string>
  read_alembic_file(const std::string abc_file_path) {
    std::cout << abc_file_path << std::endl;
    if (!fs::exists(abc_file_path)) {
      return std::unexpected{
          std::format("file not found: '{}'", abc_file_path)};
    }

    try {
      Alembic::AbcCoreFactory::IFactory factory;
      Alembic::AbcCoreFactory::IFactory::CoreType core_type;
      Alembic::Abc::IArchive archive =
          factory.getArchive(abc_file_path, core_type);
      if (!archive.valid()) {
        return std::unexpected{
            std::format("invalid archive: '{}'", abc_file_path)};
      }

      const auto archive_info = std::make_shared<Archive>();
      archive_info->core_type_name = Archive::get_core_type_name(core_type);

      double start_time, end_time;
      Alembic::Abc::GetArchiveStartAndEndTime(archive, start_time, end_time);
      archive_info->start_time = start_time;
      archive_info->end_time = end_time;

      for (int i = 0; i < archive.getNumTimeSamplings(); i++) {
        const auto ts = archive.getTimeSampling(i);
        const auto tst = ts->getTimeSamplingType();
        const auto time_per_cycle = tst.getTimePerCycle();
        std::stringstream ss;
        if (tst.isAcyclic())
          ss << "Acyclic";
        else if (tst.isCyclic())
          ss << "Cyclic";
        else if (tst.isUniform())
          ss << "Uniform";
        ss << std::format(", numStoredTimes={}, cyclesPerTime={}, "
                          "timePerCycle={}, samplesPerCycle={}",
                          ts->getNumStoredTimes(), 1.0 / time_per_cycle,
                          time_per_cycle, tst.getNumSamplesPerCycle());
        archive_info->time_samplings.push_back(ss.str());
      }

      scan_alembic_tree_recursively(archive_info->all_nodes,
                                    archive_info->top_node, archive.getTop());
      return archive_info;
    } catch (Alembic::Util::Exception e) {
      return std::unexpected{e};
    }
  };

private:
  static constexpr unsigned int MaxRecursionDepth = 9999;

  static inline void scan_alembic_tree_recursively(
      std::vector<std::shared_ptr<Node>> &node_list, std::shared_ptr<Node> node,
      const Alembic::Abc::IObject &obj, const unsigned int depth = 0) {
    if (depth > MaxRecursionDepth)
      return;

    node->depth = depth;
    node->name = obj.getName();
    node->full_name = obj.getFullName();
    const auto meta_data = obj.getMetaData();
    for (const auto &[k, v] : meta_data) {
      node->meta_data[k] = v;
    }
    node_list.push_back(node);

    for (int i = 0; i < obj.getNumChildren(); ++i) {
      const Alembic::Abc::IObject child_obj = obj.getChild(i);
      auto child_node = std::make_shared<Node>();
      scan_alembic_tree_recursively(node_list, child_node, child_obj,
                                    depth + 1);
      node->children.push_back(child_node);
    }
  };
};

} // namespace seseragi::alembic
#endif // SESERAGI_ALEMBIC_READER_H
