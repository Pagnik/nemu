#include "common.h"
#include <amdev.h>

size_t serial_write(const void *buf, size_t offset, size_t len) {
  for (int i = 0; i < len; i++) {
    _putc(((char *) buf)[i]);
  }
  return len;
}

#define NAME(key) \
  [_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [_KEY_NONE] = "NONE",
  _KEYS(NAME)
};

size_t events_read(void *buf, size_t offset, size_t len) {
  return 0;
}

static char dispinfo[128] __attribute__((used));

size_t dispinfo_read(void *buf, size_t offset, size_t len) {

  assert(len <= 128);
  memcpy(buf, dispinfo, len);
  return len;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  // ????
  // who's gonna use this stupid "rectangle" API?
  int sw = screen_width();
  int sh = screen_height();
  assert(offset / 4 <= sw * sh);
  int x = (offset / 4) / sw;
  int y = (offset / 4) % sw;
  //int w = ()
  size_t d = ((len / 4) < (sw - y)) ? (len / 4) : (sw - y);
  draw_rect(buf, x, y, d, 1);
  len -= d;
  draw_rect(buf + d, x + 1, 0,  (len / 4) % sw, (len / 4 / sw) + 1);
}


void init_device() {
  Log("Initializing devices...");
  _ioe_init();

  // TODO: print the string to array `dispinfo` with the format
  // described in the Navy-apps convention

  int sw = screen_width();
  int sh = screen_height();

  sprintf(dispinfo, "WIDTH:%d\nHEIGHT:%d\n", sw, sh);
}
