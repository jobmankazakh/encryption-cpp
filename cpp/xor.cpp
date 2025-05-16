/*
 * Simple File Encryptor/Decryptor
 * 
 * Processes all files in a directory (./raw for encryption, ./encrypted for decryption)
 * using a user-supplied arbitrary-length decimal key.
 * 
 * Usage:
 *   - Place raw files in ./raw directory for encryption.
 *   - Place .enc files in ./encrypted directory for decryption.
 *   - The program will output to ./encrypted (for encryption) or ./decrypted (for decryption).
 * 
 * Author: Sklyar Y. V. (aka JobManKazakh)
 * License: See LICENSE file for details.
 * 
 * Permission is granted to individuals to use, copy, and modify this code for personal purposes,
 * including use at their place of employment, provided such use is not for, or on behalf of, clients
 * or external organizations.
 * 
 * Companies and organizations may not use this code for:
 * - Providing services to clients or customers,
 * - Communication or integration with other companies or external organizations,
 * - Any commercial product, service, or offering to third parties,
 * 
 * without explicit, written permission from the author.
 * 
 * Redistribution or sublicensing of this code is not permitted without explicit written permission
 * from the author.
 * 
 * You may not claim this code as your own or remove this copyright notice.
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <cstdint>
#include <algorithm>
#include <chrono>

namespace fs = std::filesystem;

// Divide decimal string by 256, return quotient and remainder
std::pair<std::string, uint8_t> divmod256(const std::string& decimal) {
    std::string quotient;
    int remainder = 0;
    for (char c : decimal) {
        int digit = c - '0';
        int value = remainder * 10 + digit;
        int q = value / 256;
        remainder = value % 256;
        if (!quotient.empty() || q > 0) {
            quotient.push_back('0' + q);
        }
    }
    if (quotient.empty()) quotient = "0";
    return {quotient, static_cast<uint8_t>(remainder)};
}

// Convert decimal string to vector<uint8_t> base 256 representation
std::vector<uint8_t> decimalStringToBytes(std::string decimal) {
    std::vector<uint8_t> bytes;
    while (decimal != "0") {
        auto [quotient, remainder] = divmod256(decimal);
        bytes.push_back(remainder);
        decimal = quotient;
    }
    if (bytes.empty()) {
        bytes.push_back(0);
    }
    std::reverse(bytes.begin(), bytes.end());
    return bytes;
}

void processFile(const fs::path& inputPath, const std::vector<uint8_t>& key, const std::string& mode) {
    constexpr size_t BUFFER_SIZE = 4 * 1024 * 1024; // 4 MB buffer for SSD
    std::ifstream inFile(inputPath, std::ios::binary);
    if (!inFile) {
        std::cerr << "Failed to open input file: " << inputPath << "\n";
        return;
    }

    fs::path outputDir = (mode == "enc") ? "./encrypted" : "./decrypted";
    fs::create_directories(outputDir);

    fs::path outputPath;
    if (mode == "enc") {
        outputPath = outputDir / (inputPath.filename().string() + ".enc");
    } else if (mode == "dec") {
        std::string filename = inputPath.filename().string();
        if (filename.size() > 4 && filename.substr(filename.size() - 4) == ".enc") {
            filename = filename.substr(0, filename.size() - 4);
        }
        outputPath = outputDir / filename;
    } else {
        std::cerr << "Unknown mode: " << mode << "\n";
        return;
    }

    std::ofstream outFile(outputPath, std::ios::binary);
    if (!outFile) {
        std::cerr << "Failed to open output file: " << outputPath << "\n";
        return;
    }

    std::vector<char> buffer(BUFFER_SIZE);
    size_t step = 0;
    size_t keyLen = key.size();

    while (inFile) {
        inFile.read(buffer.data(), BUFFER_SIZE);
        std::streamsize bytesRead = inFile.gcount();
        if (bytesRead <= 0) break;

        size_t keyIndex = step % keyLen;

        for (size_t i = 0; i < static_cast<size_t>(bytesRead); ++i) {
            uint8_t dataByte = static_cast<uint8_t>(buffer[i]);
            uint8_t keyByte = key[keyIndex];
            uint8_t resultByte = (mode == "enc") ? (dataByte + keyByte) % 256 : (256 + dataByte - keyByte) % 256;
            buffer[i] = static_cast<char>(resultByte);

            keyIndex = (keyIndex + 1) % keyLen;
        }

        outFile.write(buffer.data(), bytesRead);
        step += static_cast<size_t>(bytesRead);
    }

    std::cout << "Processed: " << inputPath.filename() << " -> " << outputPath.filename() << "\n";
}

int main() {
    std::string mode;
    std::cout << "enc/dec: ";
    std::getline(std::cin, mode);

    if (mode != "enc" && mode != "dec") {
        std::cerr << "Invalid mode. Use 'enc' or 'dec'.\n";
        return 1;
    }

    std::string keyInputStr;
    std::cout << "digital key (decimal, arbitrary length): ";
    std::getline(std::cin, keyInputStr);

    // Validate input: only digits allowed
    if (keyInputStr.empty() || keyInputStr.find_first_not_of("0123456789") != std::string::npos) {
        std::cerr << "Invalid key input. Must be a non-empty numeric decimal string.\n";
        return 1;
    }

    auto key = decimalStringToBytes(keyInputStr);

    fs::path inputDir = (mode == "enc") ? "./raw" : "./encrypted";

    if (!fs::exists(inputDir) || !fs::is_directory(inputDir)) {
        std::cerr << "Input directory does not exist: " << inputDir << "\n";
        return 1;
    }

    auto start = std::chrono::steady_clock::now();

    for (const auto& entry : fs::directory_iterator(inputDir)) {
        if (!entry.is_regular_file()) continue;
        const auto& path = entry.path();
        std::string filename = path.filename().string();

        if (filename == "." || filename == "..") continue;

        processFile(path, key, mode);
    }

    auto end = std::chrono::steady_clock::now();
    auto diff = end - start;
    auto diff_ms = std::chrono::duration_cast<std::chrono::milliseconds>(diff).count();

    std::cout << "Total time: " << diff_ms / 1000.0 << " seconds\n";

    return 0;
}
