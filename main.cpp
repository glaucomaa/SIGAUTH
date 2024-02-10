#include "signature.h"
#include <fstream>
#include <iostream>
#include <string>

void copyFile(const std::string &inputPath, const std::string &outputPath) {
  if (inputPath == outputPath) {
    return;
  }

  std::ifstream inputFile(inputPath); // no need with this code structure, but
                                      // maybe will be useful after refactoring
  /* if (!inputFile) { */
  /*     std::cerr << "Ошибка: Не удается открыть входной файл:" << inputPath <<
   * std::endl; */
  /*     return; */
  /* } */

  std::ofstream outputFile(outputPath);
  if (!outputFile) {
    std::cerr << "Ошибка: Не удается открыть выходной файл: " << outputPath
              << std::endl;
    return;
  }

  std::string line;
  while (std::getline(inputFile, line)) {
    outputFile << line << std::endl;
  }

  inputFile.close();
  outputFile.close();
}
bool endsWith(const std::string &str, const std::string &suffix) {
  if (str.length() < suffix.length()) {
    return false;
  }
  return str.compare(str.length() - suffix.length(), suffix.length(), suffix) ==
         0;
}
int main(int argc, char *argv[]) {
  std::string inputFilePath, outputFilePath;

  if (argc > 1) {
    inputFilePath = argv[1];
  } else {
    std::cout << "Введите путь к входному файлу: ";
    std::cin >> inputFilePath;
  }
  std::cout << inputFilePath << std::endl;

  if (inputFilePath.substr(inputFilePath.find_last_of(".") + 1) != "txt") {
    std::cerr << "Ошибка: Входной файл должен быть в формате .txt."
              << std::endl;
    return 1;
  }

  std::ifstream inputFile(inputFilePath);
  if (!inputFile) {
    std::cerr << "Ошибка: Входной файл не существует." << std::endl;
    return 1;
  }
  inputFile.close();
  containsDigitalSignature(inputFilePath);
  char option;

  if (argc > 2) {
    outputFilePath = argv[2];
  } else {
    outputFilePath = "sign_" + inputFilePath;
    if (!endsWith(outputFilePath, ".txt")) {
      std::cerr << "Ошибка: Выходной файл должен быть в формате .txt."
                << std::endl;
      return 1;
    }
  }
  std::cout << "Выберите опцию:" << std::endl;
  std::cout << "1. Добавить подпись" << std::endl;
  std::cout << "2. Удалить подпись" << std::endl;
  std::cout << "Введите свой выбор: ";
  std::cin >> option;
  if (option == '1') {
    copyFile(inputFilePath, outputFilePath);

    addDigitalSignature(outputFilePath);
  } else if (option == '2') {
    copyFile(inputFilePath, outputFilePath);
    removeDigitalSignature(outputFilePath);
  } else {
    std::cout << "Выбран недопустимый параметр." << std::endl;
  }

  return 0;
}
