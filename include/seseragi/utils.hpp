#ifndef SESERAGI_UTILS_H
#define SESERAGI_UTILS_H

#include <string>

namespace seseragi::utils {

/// @brief Add space characers to stringstream.
/// @param ss
/// @param count
static inline void indent(std::stringstream &ss, unsigned char count = 0) {
  for (int i = 0; i < count; ++i)
    ss << ' ';
}

} // namespace seseragi::utils
#endif // SESERAGI_UTILS_H
