#ifndef SESERAGI_UTILS_H
#define SESERAGI_UTILS_H

#include <string>

namespace seseragi::utils {

/// @brief Convert string encoding to UTF-8.
/// @param s
/// @return
static inline std::string convert_utf8(std::string &s) {
  return s;
};

/// @brief Convert string encoding to system MBCS (Multi-Byte Character Set) for
/// Windows.
/// @param s
/// @return
static inline std::string convert_mbcs_win(std::string &s) {
  return s;
};

/// @brief Add space characers to stringstream.
/// @param ss
/// @param count
static inline void indent(std::stringstream &ss, unsigned char count = 0) {
  for (int i = 0; i < count; ++i)
    ss << ' ';
}

} // namespace seseragi::utils
#endif // SESERAGI_UTILS_H
