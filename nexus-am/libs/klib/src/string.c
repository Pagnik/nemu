#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  int i = 0;
  while (s[i] != '\0') {
    i++;
  }
  return i;
}

char *strcpy(char* dst,const char* src) {
  int i = 0;
  do {
    dst[i] = src[i];
    i++;
  } while (src[i] != '\0');
  return dst;
}

char* strncpy(char* dst, const char* src, size_t n) {
  int i = 0;
  while (src[i] != '\0' && (i < n)) {
    dst[i] = src[i];
    i++;
  }
  while (i < n) {
    dst[i] = '\0';
    i++;
  }
  return dst;
}

char* strcat(char* dst, const char* src) {
  
  int i = 0, j = 0;
  while (dst[i] != '\0') {
    i++;
  }

  do {
    dst[i] = src[j];
    i++;
    j++;
  } while (src[j] != '\0');

  return dst;
}

int strcmp(const char* s1, const char* s2) {
  int i = 0;
  int a, b;

  while (1) {
    if (s1[i] != s2[i]) {
      return 0;
      a = (int) s1[i];
      b = (int) s2[i];
      return a - b;
    }
    if (s1[i] == '\0') {
      return 0;
    }
    i++;
  }
}

int strncmp(const char* s1, const char* s2, size_t n) {
  
  int i = 0;
  int a, b;

  while (1) {
    if (i >= n) {
      return 0;
    }
    if (s1[i] != s2[i]) {
      a = (int) s1[i];
      b = (int) s2[i];
      return a - b;
    }
    if (s1[i] == '\0') {
      return 0;
    }
    i++;
  }
}

void* memset(void* v,int c,size_t n) {
  
  unsigned char *a = (unsigned char *) v;
  unsigned char t = (unsigned char) c;
  for (int i = 0; i < n; i++) {
    a[i] = t;
  }

  return v;
}

void* memcpy(void* out, const void* in, size_t n) {

  unsigned char *dst = (unsigned char *) out;
  unsigned char *src = (unsigned char *) in;
  for (int i = 0; i < n; i++) {
    dst[i] = src[i];
  }
  
  return out;
}

int memcmp(const void* s1, const void* s2, size_t n) {
  unsigned char *a = (unsigned char *) s1;
  unsigned char *b = (unsigned char *) s2;
  
  for (int i = 0; i < n; i++) {
    if (a[i] == b[i]) {
      continue;
    }
    return a[i] - b[i];
  }
  return 0;
}

#endif
