#include <cstring>
#include <fstream>
#include <string>
int ComputeHash(const std::string &str) {
  int hash = 0;

  for (char ch : str) {
    hash = (hash * 31) + ch;
  }

  return hash;
}
int GenerateFileHash(const std::string &filepath) {
  std::ifstream file(filepath);

  if (!file.is_open()) {
    throw std::runtime_error("Не удалось открыть файл: " + filepath);
  }

  std::string content((std::istreambuf_iterator<char>(file)),
                      std::istreambuf_iterator<char>());
  file.close();

  return ComputeHash(content);
}
int generateFileHash(const std::string &filepath) {
  std::ifstream file(filepath);
  if (!file) {
    throw std::runtime_error("Не удалось открыть файл: " + filepath);
  }

  int hash = 0;
  char ch;
  while (file.get(ch)) {
    hash += static_cast<int>(ch);
  }

  file.close();

  return hash;
}

static constexpr unsigned int S[64] = {
    7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
    5, 9,  14, 20, 5, 9,  14, 20, 5, 9,  14, 20, 5, 9,  14, 20,
    4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
    6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21};

static constexpr unsigned int K[64] = {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a,
    0xa8304613, 0xfd469501, 0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be,
    0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821, 0xf61e2562, 0xc040b340,
    0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8,
    0x676f02d9, 0x8d2a4c8a, 0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c,
    0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70, 0x289b7ec6, 0xeaa127fa,
    0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92,
    0xffeff47d, 0x85845dd1, 0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1,
    0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391};

unsigned int leftRotate(unsigned int value, unsigned int bits) {
  return (value << bits) | (value >> (32 - bits));
}

std::string toHex(unsigned char value) {
  const char hexChars[] = "0123456789abcdef";
  std::string result;
  result += hexChars[(value >> 4) & 0xF];
  result += hexChars[value & 0xF];
  return result;
}

std::string ComputeMD5Hash(const unsigned char *input, size_t size) {
  unsigned int a0 = 0x67452301;
  unsigned int b0 = 0xefcdab89;
  unsigned int c0 = 0x98badcfe;
  unsigned int d0 = 0x10325476;

  size_t paddedSize = ((size + 8) / 64 + 1) * 64;
  unsigned char *paddedInput = new unsigned char[paddedSize];
  memcpy(paddedInput, input, size);
  paddedInput[size] = 0x80;
  memset(paddedInput + size + 1, 0, paddedSize - size - 1);
  uint64_t bitSize = size * 8;
  memcpy(paddedInput + paddedSize - 8, &bitSize, 8);

  for (size_t i = 0; i < paddedSize; i += 64) {
    unsigned int *chunk = reinterpret_cast<unsigned int *>(paddedInput + i);

    unsigned int A = a0;
    unsigned int B = b0;
    unsigned int C = c0;
    unsigned int D = d0;

    for (size_t j = 0; j < 64; ++j) {
      unsigned int F, g;

      if (j < 16) {
        F = (B & C) | ((~B) & D);
        g = j;
      } else if (j < 32) {
        F = (D & B) | ((~D) & C);
        g = (5 * j + 1) % 16;
      } else if (j < 48) {
        F = B ^ C ^ D;
        g = (3 * j + 5) % 16;
      } else {
        F = C ^ (B | (~D));
        g = (7 * j) % 16;
      }

      unsigned int dTemp = D;
      D = C;
      C = B;
      B = B + leftRotate((A + F + K[j] + chunk[g]), S[j]);
      A = dTemp;
    }

    a0 += A;
    b0 += B;
    c0 += C;
    d0 += D;
  }

  delete[] paddedInput;

  unsigned char hash[16];
  memcpy(hash, &a0, 4);
  memcpy(hash + 4, &b0, 4);
  memcpy(hash + 8, &c0, 4);
  memcpy(hash + 12, &d0, 4);

  std::string result;
  for (size_t i = 0; i < 16; ++i) {
    result += toHex(hash[i]);
  }

  return result;
}

std::string ComputeFileHashMD5(const std::string &filepath) {
  std::ifstream file(filepath, std::ios::binary);
  if (!file) {
    throw std::runtime_error("Не удалось открыть файл: " + filepath);
  }

  file.seekg(0, std::ios::end);
  size_t fileSize = file.tellg();
  file.seekg(0, std::ios::beg);

  unsigned char *buffer = new unsigned char[fileSize];
  file.read(reinterpret_cast<char *>(buffer), fileSize);

  std::string hash = ComputeMD5Hash(buffer, fileSize);

  delete[] buffer;
  file.close();

  return hash;
}
