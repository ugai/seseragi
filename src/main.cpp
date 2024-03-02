#include <expected>
#include <filesystem>
#include <format>
#include <iostream>
#include <optional>
#include <string>
#include <thread>

#include <Alembic/Abc/All.h>
#include <Alembic/AbcCoreOgawa/All.h>
#include <Alembic/AbcGeom/All.h>
#include <clip.h>
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
    ui->set_app_info(AppInfo{
        AppName,
        AppDescription,
        AppVersion,
        AppUrl,
    });

    slint::ComponentWeakHandle<AppWindow> weak_ui_handle(ui);
    alembic::Archive::Ptr archive_ptr;

    const auto dispatch_ui_update_for_archive_data = [&weak_ui_handle,
                                                      &archive_ptr] {
      if (!archive_ptr)
        return;

      // FIXME: 💀 The most hardest part of this app.
      //        This section converts the Seseragi internal
      //        data model into the Slint data model.
      slint::invoke_from_event_loop([=] {
        // Alembic toplevel metadata
        const auto items = std::make_shared<slint::VectorModel<AbcKvEntry>>();
        items->push_back({"coreType", archive_ptr->core_type_name.c_str()});
        for (size_t i = 0; const auto &ts : archive_ptr->time_samplings) {
          const auto name = std::format("timeSampling[{}]", i++);
          items->push_back({name.c_str(), ts.c_str()});
        }
        items->push_back(
            {"startTime", SStr::from_number(archive_ptr->start_time)});
        items->push_back({"endTime", SStr::from_number(archive_ptr->end_time)});

        // Alembic object hierarchies
        const auto nodes = std::make_shared<slint::VectorModel<AbcNode>>();
        for (const auto &src_node : archive_ptr->all_nodes) {
          const auto dst_meta_items =
              std::make_shared<slint::VectorModel<AbcKvEntry>>();
          for (const auto &[k, v] : src_node->meta_data) {
            dst_meta_items->push_back({k.c_str(), v.c_str()});
          }

          AbcNode dst_node;
          dst_node.indentation = src_node->depth;
          dst_node.name = src_node->name;
          dst_node.metadata = dst_meta_items;
          nodes->push_back(dst_node);
        }

        if (const auto ui = weak_ui_handle.lock(); ui) {
          (*ui)->set_abc_list_view_archive_items(items);
          (*ui)->set_abc_list_view_nodes(nodes);
          (*ui)->set_error_message("");
        }
      });
    };

    const auto dispatch_ui_update_for_error =
        [&weak_ui_handle](const std::string error_message) {
          slint::invoke_from_event_loop([=]() {
            if (const auto ui = weak_ui_handle.lock(); ui) {
              (*ui)->set_abc_list_view_archive_items({});
              (*ui)->set_abc_list_view_nodes({});
              (*ui)->set_error_message(error_message.c_str());
            }
          });
        };

    const auto dispatch_read_abc_file = [&](std::string path) {
      std::thread t([&, path] {
        const auto &result = alembic::Reader::read_alembic_file(path);
        if (result) {
          archive_ptr = result.value();
          dispatch_ui_update_for_archive_data();
        } else {
          archive_ptr = nullptr;
          spdlog::error(result.error());
          dispatch_ui_update_for_error(result.error());
        }
      });
      t.detach();
    };

    ui->on_open_button_clicked([&] {
      const auto &dialog_result = show_file_dialog();
      if (dialog_result) {
        if (const auto &path = dialog_result.value(); !path.empty()) {
          ui->set_file_path(path.string().c_str());
          dispatch_read_abc_file(path.string());
        }
      } else {
        spdlog::error(dialog_result.error());
        dispatch_ui_update_for_error(dialog_result.error());
      }
    });

    ui->on_reload_button_clicked(
        [&] { dispatch_read_abc_file(ui->get_file_path().data()); });

    ui->on_copy_json_button_clicked([&] {
      if (!archive_ptr)
        return;

      // Create JSON text
      auto json = nlohmann::ordered_json::object();
      archive_ptr->to_json(json);
      const auto s = json.dump(2);
      clip::set_text(s);
    });

    if (args.count("file") > 0) {
      const auto &abc_path = args["file"].as<std::string>();
      ui->set_file_path(abc_path.c_str());
      dispatch_read_abc_file(abc_path);
    }
  }

  ui->run();

  return EXIT_SUCCESS;
}