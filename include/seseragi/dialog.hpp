#ifndef SESERAGI_DIALOG_H
#define SESERAGI_DIALOG_H

#include <expected>
#include <optional>
#include <string>

namespace seseragi {

/// @brief Show file dialog.
/// @return
std::expected<std::optional<std::string>, std::string> show_file_dialog() {
  NFD::Guard nfdGuard; // initialize NFD
  NFD::UniquePath outPath;
  constexpr size_t FilterSize = 1;
  nfdfilteritem_t filterItem[FilterSize] = {{"Alembic", "abc"}};

  const nfdresult_t result = NFD::OpenDialog(outPath, filterItem, FilterSize);
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

} // namespace seseragi
#endif // SESERAGI_DIALOG_H
