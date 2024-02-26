#ifndef SESERAGI_DIALOG_H
#define SESERAGI_DIALOG_H

#include <expected>
#include <filesystem>
#include <optional>
#include <string>

namespace fs = std::filesystem;

namespace seseragi {

/// @brief Show file dialog.
/// @return
std::expected<fs::path, std::string> show_file_dialog() {
  NFD::Guard nfdGuard; // initialize NFD
  NFD::UniquePath outPath;
  constexpr size_t FilterSize = 1;
  nfdfilteritem_t filterItem[FilterSize] = {{"Alembic", "abc"}};

  try {
    const nfdresult_t result = NFD::OpenDialog(outPath, filterItem, FilterSize);
    switch (result) {
    case NFD_OKAY:
      return fs::path(outPath.get());
    case NFD_CANCEL:
      return fs::path();
    default:
      const auto msg = std::format("Error: {}", NFD::GetError());
      return std::unexpected{msg};
    }
  } catch (std::exception ex) {
    return std::unexpected{ex.what()};
  } catch (...) {
    return std::unexpected{"Unknown error"};
  }
}

} // namespace seseragi
#endif // SESERAGI_DIALOG_H
