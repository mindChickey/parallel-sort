
#include <stdio.h>
#include <string.h>

inline unsigned char readHex1(char* hex){
  unsigned char b = hex[1] - '0';
  unsigned char a = hex[0] - '0';
  return (a << 4) | b;
}

template<unsigned N>
long readHexEven(char* hex){
  long r = 0;
  unsigned char* buffer = (unsigned char*)&r;

  for(unsigned i = 0; i < N / 2; i++){
    buffer[i] = readHex1(hex + N - 2 - 2 * i);
  }
  return r;
}

template<unsigned N>
long readHex(char* hex){
  if(N % 2 == 0){
    return readHexEven<N>(hex);
  } else {
    long a = hex[0] - '0';
    long b = readHexEven<N-1>(hex + 1);
    return (a << ((N - 1) * 4)) | b;
  }
}
 
typedef long (*readHexFT)(char*);

readHexFT readHexFS[16] = {
  readHex<1>, readHex<2>, readHex<3>, readHex<4>, readHex<5>, readHex<6>, readHex<7>, readHex<8>, 
  readHex<9>, readHex<10>, readHex<11>, readHex<12>, readHex<13>, readHex<14>, readHex<15>, readHex<16>
};

inline void writeHex1(char* hex, unsigned char x){
  hex[1] = (x & 0xF) + '0';
  hex[0] = (x >> 4) + '0';
}

template<unsigned N>
void writeHexEven(char* hex, long x){
  unsigned char* buffer = (unsigned char*)&x;

  for(unsigned i = 0; i < N / 2; i++){
    writeHex1(hex + N - 2 - 2 * i, buffer[i]);
  }
}

template<unsigned N>
void writeHex(char* hex, long x){
  if(N % 2 == 0){
    writeHexEven<N>(hex ,x);
  } else {
    writeHexEven<N-1>(hex + 1, x);
    hex[0] = (char)((unsigned long)x >> 4*(N-1)) + '0';
  }
}

typedef void (*writeHexFT)(char*, long);

writeHexFT writeHexFS[16] = {
  writeHex<1>, writeHex<2>, writeHex<3>, writeHex<4>, writeHex<5>, writeHex<6>, writeHex<7>, writeHex<8>, 
  writeHex<9>, writeHex<10>, writeHex<11>, writeHex<12>, writeHex<13>, writeHex<14>, writeHex<15>, writeHex<16>
};

// int main(){
//   long x = 0x1301785258;
//   char hex[17] = "                ";
//   writeHexFS[9](hex, x);
//   puts(hex);
//   return 0;
// }