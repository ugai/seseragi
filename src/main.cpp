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

#include <seseragi/alembic_node.hpp>
#include <seseragi/alembic_scan.hpp>
#include <seseragi/app.hpp>
#include <seseragi/dialog.hpp>
#include <seseragi/utils.hpp>
#ifdef _WIN32
#include <seseragi/win32.hpp>
#endif

// Slint
#include "app_window.h"

namespace fs = std::filesystem;
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

    ui->on_open_button_clicked([&ui] {
      const auto &dialog_result = show_file_dialog();
      if (dialog_result) {
        ui->set_file_opened(false);
        const auto &abc_path = dialog_result.value();
        if (abc_path) {
          ui->set_file_path(abc_path.value().c_str());
          ui->invoke_reload_file();
        }
      } else {
        // TODO: Display error message to user
        spdlog::error(dialog_result.error());
        ui->invoke_clear_file();
      }
    });

    ui->on_reload_button_clicked([&ui] { ui->invoke_reload_file(); });

    ui->on_clear_file([&ui] {
      ui->set_tree_list({}); // clear
      ui->set_file_opened(false);
    });

    ui->on_reload_file([&ui] {
      auto abc_path = std::string(ui->get_file_path());

      // FIXME: 💀 The most hardest part of this app.
      const auto &root_node = AbcReader::read_alembic_file(abc_path);
      if (root_node) {
        std::vector<std::shared_ptr<AbcNode>> src_nodes{};
        root_node.value()->as_list(src_nodes);

        std::vector<SlintAlembicNodeListItem> dst_nodes{};
        for (const auto &src_node : src_nodes) {
          SlintAlembicNodeListItem dst_node;
          dst_node.indentation = src_node->depth;
          dst_node.name = src_node->name;

          std::vector<SlintAlembicMetadataItem> dst_meta_items{};
          for (const auto &[k, v] : src_node->meta_data) {
            SlintAlembicMetadataItem dst_meta_item;
            dst_meta_item.key = k;
            dst_meta_item.value = v;
            dst_meta_items.push_back(dst_meta_item);
          }
          dst_node.metadata =
              (std::make_shared<slint::VectorModel<SlintAlembicMetadataItem>>(
                  dst_meta_items));

          dst_nodes.push_back(dst_node);
        }

        ui->set_tree_list(
            std::make_shared<slint::VectorModel<SlintAlembicNodeListItem>>(
                dst_nodes));
        ui->set_file_opened(true);
      } else {
        // TODO: Display error message to user
        spdlog::error(root_node.error());
        ui->invoke_clear_file();
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