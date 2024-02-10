#include "hash.h"
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <tuple>

std::pair<int, int> getRandomNumbers(const std::vector<int> &numbers) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, numbers.size() - 1);

  int p = numbers[dis(gen)];
  int q;
  do {
    q = numbers[dis(gen)];
  } while (q == p);

  return std::make_pair(p, q);
}

int hexStringToLL(const std::string &hexString) {
  std::stringstream ss;
  ss << std::hex << hexString;
  int result;
  ss >> result;
  return result;
}

struct KeyPair {
  int publicKey;
  int privateKey;
  int n;
};

bool is_prime(int num) {
  if (num <= 1)
    return false;
  if (num <= 3)
    return true;
  if (num % 2 == 0 || num % 3 == 0)
    return false;
  for (int i = 5; i * i <= num; i += 6) {
    if (num % i == 0 || num % (i + 2) == 0)
      return false;
  }
  return true;
}

int gcd(int a, int b) {
  while (b != 0) {
    int temp = b;
    b = a % b;
    a = temp;
  }
  return a;
}

int mod_inverse(int a, int m) {
  int m0 = m;
  int y = 0, x = 1;

  if (m == 1)
    return 0;

  while (a > 1) {
    int q = a / m;
    int t = m;

    m = a % m;
    a = t;
    t = y;

    y = x - q * y;
    x = t;
  }

  if (x < 0)
    x += m0;

  return x;
}

KeyPair generate_key_pair(int p, int q) {
  KeyPair keyPair;
  keyPair.n = p * q;
  int phi = (p - 1) * (q - 1);
  int e = 65537; // 2^16+1 instead of 2 < e < λ(n) and gcd(e, λ(n)) = 1; that
                 // is, e and λ(n) are coprime
  while (e < phi) {
    if (gcd(e, phi) == 1)
      break;
    else
      e++;
  }

  int d = mod_inverse(e, phi);

  keyPair.publicKey = e;
  keyPair.privateKey = d;

  return keyPair;
}

void write_keys_to_file(const KeyPair &keyPair, const std::string &filename) {
  std::ofstream file(filename);
  if (file.is_open()) {
    file << "Публичный ключ: " << keyPair.publicKey << "\n";// simply(e)
    file << "Приватный ключ: " << keyPair.privateKey << "\n";// simply(d)
    file << "n: " << keyPair.n << "\n";
    file.close();
    std::cout << "Ключи записанны в файл:" << filename << std::endl;
  } else {
    std::cerr << "Не удается открыть файл для записи" << std::endl;
  }
}
KeyPair read_keys_from_file(const std::string &filename) {
  KeyPair keyPair;
  std::ifstream file(filename);
  if (file.is_open()) {
    std::string line;
    std::getline(file, line);
    size_t pos = line.find(": ");
    keyPair.publicKey = std::stoi(line.substr(pos + 2));

    std::getline(file, line);
    pos = line.find(": ");
    keyPair.privateKey = std::stoi(line.substr(pos + 2));

    std::getline(file, line);
    pos = line.find(": ");
    keyPair.n = std::stoi(line.substr(pos + 2));

    file.close();
  } else {
    std::cerr << "Не удается открыть файл для чтения" << std::endl;
  }

  return keyPair;
}
int encrypt(int message, int publicKey, int n) {
  int encrypted_message = 1;
  for (int i = 0; i < publicKey; i++) {
    encrypted_message = (encrypted_message * message) % n;
  }
  return encrypted_message;
}

int decrypt(int encrypted_message, int privateKey, int n) {
  int decrypted_message = 1;
  for (int i = 0; i < privateKey; i++) {
    decrypted_message = (decrypted_message * encrypted_message) % n;
  }
  return decrypted_message;
}
const std::string separator = "#####";

void addDigitalSignature(const std::string &filename) {
  std::ofstream file(filename, std::ios::app);
  if (file.is_open()) {
    KeyPair key_pair;
    if (std::ifstream("./keys.txt")) {
      key_pair = read_keys_from_file("./keys.txt");
    } else {
      std::vector<int> numbers = {
          1229, 1231, 1237, 1249, 1259, 1277, 1279, 1283, 1289, 1291,
          1297, 1301, 1303, 1307, 1319, 1321, 1327, 1361, 1367, 1373,
          1381, 1399, 1409, 1423, 1427, 1429, 1433, 1439, 1447, 1451,
          1453, 1459, 1471, 1481, 1483, 1487, 1489, 1493, 1499, 1511,
          1523, 1531, 1543, 1549, 1553, 1559};//extend with another prime numbers

      int p, q;
      std::tie(p, q) = getRandomNumbers(numbers);
      std::cout << p << " " << q << std::endl;
      key_pair = generate_key_pair(p, q);
      write_keys_to_file(key_pair, "./keys.txt");
    }
    int signature = encrypt(hexStringToLL(ComputeFileHashMD5(filename)),
                            key_pair.privateKey, key_pair.n);
    /* std::cout<<signature<<std::endl; */ // to check that signature is unique
                                           // for the different keys and same
                                           // for the same keys
    file << separator << "Цифровая подпись: " << signature << std::endl;
    file.close();
    std::cout << "Цифровая подпись добавлена в " << filename << std::endl;
  } else {
    std::cerr << "Не удаётся открыть файл: " << filename << std::endl;
  }
}

bool containsDigitalSignature(const std::string &filename) {
  std::ifstream file(filename);
  std::string line;
  while (getline(file, line)) {
    if (line.find(separator) != std::string::npos) {
      std::cout << "Цифровая подпись найдена в " << filename << std::endl;
      return true;
    }
  }
  std::cout << "Цифровая подпись не найдена в " << filename << std::endl;
  return false;
}

void removeDigitalSignature(const std::string &filename) {
  std::ifstream fin(filename);
  std::ofstream temp("temp.txt");

  std::string line;
  while (getline(fin, line)) {
    if (line.find(separator) == std::string::npos) {
      temp << line << std::endl;
    }
  }

  fin.close();
  temp.close();

  remove(filename.c_str());
  rename("temp.txt", filename.c_str());

  std::cout << "Цифровая подпись удалена из " << filename << std::endl;
}
