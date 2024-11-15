
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