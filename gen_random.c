#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <string.h>

#define MAX_NUM_LEN 11  // 最大数字长度(10位)加换行符(1位)

int main(int argc, char** argv) {
  char filePath[32];
  unsigned NUM_INTEGERS;
  sscanf(argv[1], "%u", &NUM_INTEGERS);
  sprintf(filePath, "random%s", argv[1]);

  unsigned FILE_SIZE = NUM_INTEGERS * MAX_NUM_LEN;

  int fd = open(filePath, O_RDWR | O_CREAT | O_TRUNC, 0644);
  ftruncate(fd, FILE_SIZE);
  char *map = mmap(NULL, FILE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  srand(time(NULL));

  char *current_pos = map;
  size_t total_bytes = 0;

  // 生成随机数并写入映射的内存
  for (int i = 0; i < NUM_INTEGERS; i++) {
    unsigned num = rand() & ((1u << 31) - 1);
    int written = sprintf(current_pos, "%u\n", num);
    current_pos += written;
    total_bytes += written;
  }

  ftruncate(fd, total_bytes);
  msync(map, total_bytes, MS_SYNC);
  munmap(map, FILE_SIZE);
  close(fd);
  return 0;
}