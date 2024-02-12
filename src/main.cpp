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

#include "app_window.h"

namespace fs = std::filesystem;
using namespace seseragi;

int main(int argc, char *argv[]) {
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
    ui->set_app_name(AppName);
    ui->set_app_description(AppDescription);
    ui->set_app_version(AppVersion);
    ui->set_app_url(AppUrl);

    ui->on_open_button_clicked([&ui] {
      const auto &dialog_result = show_file_dialog();
      if (dialog_result) {
        const auto &abc_path = dialog_result.value();
        if (abc_path) {
          ui->set_file_path(abc_path.value().c_str());

          ui->invoke_reload_file();
        }
      } else {
        spdlog::error(dialog_result.error());
        ui->set_tree_text(dialog_result.error().c_str());
      }
    });

    ui->on_reload_button_clicked([&ui] { ui->invoke_reload_file(); });

    ui->on_reload_file([&ui] {
      auto abc_path = std::string(ui->get_file_path());

      const auto &root_node = AbcReader::read_alembic_file(abc_path);
      if (root_node) {
        const auto &tree_text = root_node.value()->as_tree_text();
        ui->set_tree_text(tree_text.c_str());
      } else {
        spdlog::error(root_node.error());
        ui->set_tree_text(root_node.error().c_str());
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