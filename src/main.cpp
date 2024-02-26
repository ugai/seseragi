#include <expected>
#include <filesystem>
#include <format>
#include <iostream>
#include <optional>
#include <string>

#include <Alembic/Abc/All.h>
#include <Alembic/AbcCoreOgawa/All.h>
#include <Alembic/AbcGeom/All.h>
#include <cxxopts.hpp>
#include <nfd.hpp>
#include <spdlog/spdlog.h>

#include <seseragi/alembic/archive.hpp>
#include <seseragi/alembic/node.hpp>
#include <seseragi/alembic/reader.hpp>
#include <seseragi/app.hpp>
#include <seseragi/dialog.hpp>
#include <seseragi/utils.hpp>
#ifdef _WIN32
#include <seseragi/win32.hpp>
#endif

// Slint generated header
#include "app_window.h"

namespace fs = std::filesystem;
typedef slint::SharedString SStr;
using namespace seseragi;

int main(int argc, char *argv[]) {
#ifdef _WIN32
  win32::CodePageEnforcer code_page_enforcer;
#endif

  cxxopts::Options options(AppName.data(), AppDescription.data());

  // clang-format off
  options.add_options()
      ("file", "Path to .abc file", cxxopts::value<std::string>())
      ("h,help", "Print usage");
  // clang-format on

  options.parse_positional({"file"});
  const auto args = options.parse(argc, argv);

  // Show help and exit
  if (args.count("help")) {
    std::cout << options.help() << std::endl;
    return EXIT_SUCCESS;
  }

  auto ui = AppWindow::create();

  // UI bindings
  {
    AppInfo app_info;
    app_info.name = AppName;
    app_info.description = AppDescription;
    app_info.version = AppVersion;
    app_info.url = AppUrl;
    ui->set_app_info(app_info);

    const auto handle_open_error = [&ui](const std::string &error_message) {
      spdlog::error(error_message);
      ui->set_error_message(error_message.c_str());
      ui->set_has_error(true);

      ui->set_abc_list_view_archive_items({});
      ui->set_abc_list_view_nodes({});
      ui->set_abc_json_view_text("");
    };

    ui->on_open_button_clicked([&ui, &handle_open_error] {
      const auto &dialog_result = show_file_dialog();
      if (dialog_result) {
        if (const auto &path = dialog_result.value(); !path.empty()) {
          ui->set_file_path(SStr(path.string()));
          ui->invoke_reload_file();
        }
      } else {
        handle_open_error(dialog_result.error().c_str());
      }
    });

    // FIXME: 💀 The most hardest part of this app.
    //        This section converts the Seseragi internal
    //        data model into the Slint data model.
    ui->on_reload_file([&ui, &handle_open_error] {
      const auto abc_path = std::string(ui->get_file_path());

      // shorthands
      typedef std::shared_ptr<alembic::Node> NodePtr;
      typedef slint::VectorModel<AbcNode> AbcNodeVecModel;
      typedef slint::VectorModel<AbcKvEntry> AbcKvVecModel;

      const auto &archive_result = alembic::Reader::read_alembic_file(abc_path);
      if (archive_result) {
        const auto archive = archive_result.value();

        // Alembic toplevel metadata
        std::vector<AbcKvEntry> archive_items;
        {
          // clang-format off
          archive_items.push_back(AbcKvEntry{"coreType", SStr(archive->core_type_name)});
          for (size_t i = 0; const auto& ts : archive->time_samplings) {
            auto k = SStr(std::format("timeSampling[{}]", i++));
            archive_items.push_back(AbcKvEntry{k, SStr(ts)});
          }
          archive_items.push_back(AbcKvEntry{"startTime", SStr(std::to_string(archive->start_time))});
          archive_items.push_back(AbcKvEntry{"endTime", SStr(std::to_string(archive->end_time))});
          // clang-format on
        }
        ui->set_abc_list_view_archive_items(
            std::make_shared<AbcKvVecModel>(archive_items));

        // Alembic object hierarchies
        std::vector<AbcNode> dst_nodes;
        {
          for (const auto &src_node : archive->all_nodes) {
            AbcNode dst_node;
            dst_node.indentation = src_node->depth;
            dst_node.name = src_node->name;

            std::vector<AbcKvEntry> dst_meta_items;
            for (const auto &[k, v] : src_node->meta_data) {
              AbcKvEntry dst_meta_item;
              dst_meta_item.key = k;
              dst_meta_item.value = v;
              dst_meta_items.push_back(dst_meta_item);
            }
            dst_node.metadata =
                (std::make_shared<AbcKvVecModel>(dst_meta_items));

            dst_nodes.push_back(dst_node);
          }
        }
        ui->set_abc_list_view_nodes(
            std::make_shared<AbcNodeVecModel>(dst_nodes));

        auto json = nlohmann::ordered_json::object();
        archive->to_json(json);
        auto jc = json.dump(4);
        spdlog::info(jc);
        ui->set_abc_json_view_text(jc.c_str());

        ui->set_has_error(false);
      } else {
        handle_open_error(archive_result.error().c_str());
      }
    });
  }

  if (args.count("file") > 0) {
    const auto &abc_path = args["file"].as<std::string>();
    ui->set_file_path(abc_path.c_str());

    ui->invoke_reload_file();
  }

  ui->run();

  return EXIT_SUCCESS;
}