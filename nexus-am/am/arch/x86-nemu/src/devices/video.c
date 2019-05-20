#include <am.h>
#include <x86.h>
#include <amdev.h>
#include <klib.h>

static uint32_t* const fb __attribute__((used)) = (uint32_t *)0x40000;

#define SCREEN_PORT 0x100
#define WIDTH_MASK 0xffff0000
#define HEIGHT_MASK 0x0000ffff



int W;
int H;
size_t video_read(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_INFO: {
      _VideoInfoReg *info = (_VideoInfoReg *)buf;
      int size_info = inl(SCREEN_PORT);
      info->width = (size_info & WIDTH_MASK) >> 16;
      info->height = size_info & HEIGHT_MASK;
      //printf("w: %d, h: %d\n", info->width, info->height);
      return sizeof(_VideoInfoReg);
    }
  }
  return 0;
}

size_t video_write(uintptr_t reg, void *buf, size_t size) {
  switch (reg) {
    case _DEVREG_VIDEO_FBCTL: {
      _FBCtlReg *ctl = (_FBCtlReg *)buf;
      
      //int size = screen_width() * screen_height();
      //for (int i = 0; i < size; i ++) fb[i] = i;
      if (ctl->sync) {
        // do nothing, hardware syncs.
      } else {
        int x = ctl->x, y = ctl->y, w = ctl->w, h = ctl->h;
        uint32_t *pixels = ctl->pixels;
        
        int cp_bytes = sizeof(uint32_t) * ((w < (W - x)) ? (w) : (W - x));
        for (int j = 0; j < h && y + j < H; j ++) {
          memcpy(&fb[(y + j) * W + x], pixels, cp_bytes);
          pixels += w;
        }
        
      }
      return sizeof(_FBCtlReg);
    }
  }
  return 0;
}

void vga_init() {
  W = screen_width();
  H = screen_height();
}
