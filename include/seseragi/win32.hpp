#ifndef SESERAGI_WIN32_H
#define SESERAGI_WIN32_H

#include <string>

#ifdef _WIN32
#include <Windows.h>

namespace seseragi::win32 {

class CodePageEnforcer {
public:
  inline CodePageEnforcer(unsigned int code_page = CP_UTF8) {
    this->last_code_page = GetConsoleOutputCP();
    SetConsoleOutputCP(code_page);
  }
  inline ~CodePageEnforcer() { SetConsoleOutputCP(this->last_code_page); }

private:
  unsigned int last_code_page;
};
#endif // _WIN32

} // namespace seseragi::win32
#endif // SESERAGI_WIN32_H
