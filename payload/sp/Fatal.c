// Implements __assert_func

#include <revolution.h>
#include <stdio.h> // snprintf

void __assert_func(const char *filename, int line, const char *assertFunc,
                   const char *expr) {
  char buf[128];
  snprintf(buf, sizeof(buf), "Assertion failed:\n[%s:%i] %s: %s", filename,
           line, assertFunc, expr);

  const GXColor fg = {255, 255, 255, 255};
  const GXColor bg = {0, 0, 0, 255};
  OSFatal(fg, bg, buf);

  __builtin_unreachable();
}