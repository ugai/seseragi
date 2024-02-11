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
#include <seseragi/utils.hpp>

#include "app_window.h"

namespace fs = std::filesystem;
using namespace seseragi;

constexpr char AppName[] = "Seseragi";
constexpr char AppDescription[] =
    "A simple GUI tool for checking the hierarchy information of Alembic files.";
constexpr char AppVersion[] = "0.1.0";
constexpr char AppUrl[] = "https://github.com/ugai/seseragi";
constexpr char AppLicense[] = "MIT License";

std::expected<std::optional<std::string>, std::string> show_file_dialog() {
  NFD::Guard nfdGuard; // initialize NFD
  NFD::UniquePath outPath;
  constexpr size_t FilterSize = 1;
  nfdfilteritem_t filterItem[FilterSize] = {{"Alembic", "abc"}};

  nfdresult_t result = NFD::OpenDialog(outPath, filterItem, FilterSize);

  switch (result) {
  case NFD_OKAY:
    return outPath.get();
  case NFD_CANCEL:
    return std::nullopt;
  default:
    const auto msg = std::format("Error: {}", NFD::GetError());
    return std::unexpected{msg};
  }
}

int main(int argc, char *argv[]) {
  cxxopts::Options options(AppName, AppDescription);

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
    ui->set_app_license(AppLicense);

    ui->on_open_button_clicked([&ui] {
      const auto &dialog_result = show_file_dialog();
      if (dialog_result) {
        const auto &abc_path = dialog_result.value();
        if (abc_path) {
          ui->set_file_path(slint::SharedString(abc_path.value()));

          ui->invoke_reload_file();
        }
      }
    });

    ui->on_reload_button_clicked([&ui] { ui->invoke_reload_file(); });

    ui->on_reload_file([&ui] {
      auto abc_path = std::string(ui->get_file_path());
      abc_path = utils::convert_mbcs_win(abc_path);

      const auto &root_node = AbcReader::read_alembic_file(abc_path);
      if (root_node) {
        auto tree_text = root_node.value()->as_tree_text();
        tree_text = utils::convert_utf8(tree_text);
        ui->set_tree_text(slint::SharedString(tree_text));
      }
    });

    if (args.count("file") > 0) {
      auto abc_path = args["file"].as<std::string>();
      abc_path = utils::convert_utf8(abc_path);
      ui->set_file_path(slint::SharedString(abc_path));

      ui->invoke_reload_file();
    }
  }

  ui->run();

  return EXIT_SUCCESS;
}