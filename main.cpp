
#include <thread>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>

#define N 100
#define MAX_NUM_LEN 11  // 最大数字长度(10位)加换行符(1位)

char inputData[N * MAX_NUM_LEN];
unsigned inputSize;

inline unsigned char readHex1(char* p){
  unsigned char b = *(p - 1) - '0';
  unsigned char a = *(p - 2) - '0';
  return (a << 4) | b;
}

long readHex(char* start, char* end){
  char arr[16] = {'0','0','0','0','0','0','0','0','0','0','0','0','0','0','0','0'};
  unsigned len = end - start;
  memcpy(arr + 16 - len, start, len);
  long r = 0;
  unsigned char* buffer = (unsigned char*)&r;

  for(unsigned i = 0; i < 8; i++){
    buffer[i] = readHex1(arr + 16 - i * 2);
  }
  return r;
}

void readFile(const char *filename) {
  FILE* file = fopen(filename, "r");
  struct stat st;
  fstat(fileno(file), &st);
  inputSize = st.st_size;
  fread(inputData, inputSize, 1, file);
  fclose(file);
}

void parseSection(char* start, char* end){
  while(start < end){
    char* p = strchr(start, '\n');
    unsigned long hex = readHex(start, p);
  }
}

void parseInput(unsigned threadNum) {
  unsigned len0 = inputSize / threadNum - 3;
  char* start = inputData;
  char* inputEnd = inputData + inputSize;
  std::vector<std::thread*> threads;

  while(start < inputEnd){
    char* end = start + len0 < inputEnd ? strchr(start + len0, '\n') + 1 : inputEnd;
    // printf("%u %u %u\n", start, end, end - start);
    std::thread* t = new std::thread(parseSection, start, end);
    threads.push_back(t);
    start = end;
  }
  for(std::thread* t : threads){
    t->join();
    delete t;
  }
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
    return 1;
  }

  readFile(argv[1]);

  printf("size: %u\n", inputSize);

  unsigned threadNum = std::thread::hardware_concurrency();
  parseInput(threadNum);


  return 0;
}
