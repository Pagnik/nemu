#include "fs.h"

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

extern size_t ramdisk_read(void *buf, size_t offset, size_t len);
extern size_t ramdisk_write(void *buf, size_t offset, size_t len);
extern size_t dispinfo_read(void *buf, size_t offset, size_t len);
extern size_t fb_write(const void *buf, size_t offset, size_t len);
typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  
  ReadFn read;
  WriteFn write;
  size_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};


int fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, const void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd);
size_t fs_filesz(int fd);

size_t serial_write(const void *buf, size_t offset, size_t len);

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  {"stdin", 0, 0, invalid_read, invalid_write},
  {"stdout", 0, 0, invalid_read, serial_write},
  {"stderr", 0, 0, invalid_read, serial_write},
  {"/proc/dispinfo", 0, 0, dispinfo_read}, 
  {"/dev/fb", 0, 0, NULL, fb_write},
#include "files.h"
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))
#define NR_FB 4
void init_fs() {
  // TODO: initialize the size of /dev/fb
  assert(strcmp(file_table[NR_FB].name, "/dev/fb") == 0);
  file_table[NR_FB].size = screen_height() * 4 * screen_width() * 4;

  printf("fb size: %d\n", file_table[NR_FB].size);
}



int fs_open(const char *pathname, int flags, int mode) {
  for (int i = 0; i < NR_FILES; i++) {
    if (strcmp(file_table[i].name, pathname) == 0) {
      return i;
    }
  }
  panic("no such file\n");
}

int fs_close(int fd) {
  return 0;
}


size_t fs_lseek(int fd, size_t offset, int whence) {
  size_t new_offset = file_table[fd].open_offset;
  switch (whence) {
    case SEEK_SET: {
      /*assert((file_table[fd].size >= offset));
      file_table[fd].open_offset = offset;*/
      new_offset = offset;
      break;
    }
    case SEEK_CUR: {
      new_offset = offset + file_table[fd].open_offset;
      break;
    }
    case SEEK_END: {
      //file_table[fd].open_offset = file_table[fd].size;
      new_offset = offset + file_table[fd].size;
      break;
    }
    default: {
      panic("shouldn't reach here");
    }
  }

  // the size of files is fixed
  assert((new_offset <= file_table[fd].size));
  file_table[fd].open_offset = new_offset;
  return new_offset;

}

size_t fs_read(int fd, void *buf, size_t len) {
  size_t res;
  if (file_table[fd].read != NULL) {
    res = file_table[fd].read(buf, file_table[fd].open_offset, len);
  } else {
    assert(file_table[fd].open_offset + len <= file_table[fd].size);
  
    res = ramdisk_read(buf, 
                file_table[fd].disk_offset + file_table[fd].open_offset, 
                len);
  }
  file_table[fd].open_offset += res;
  return res;
}

size_t fs_write(int fd, const void *buf, size_t len) {
  size_t res;
  if (file_table[fd].write != NULL) {
    res = file_table[fd].write(buf, file_table[fd].open_offset, len);
  } else {
    assert(file_table[fd].open_offset + len <= file_table[fd].size);
  
    res = ramdisk_write(buf, 
                file_table[fd].disk_offset + file_table[fd].open_offset, 
                len);
  }
  file_table[fd].open_offset += res;
  return res;
}


size_t fs_filesz(int fd) {
  return file_table[fd].size;
}