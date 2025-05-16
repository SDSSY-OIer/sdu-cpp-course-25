#pragma once
#include <string>
#include <vector>

// 32 字节 AES‑256 密钥；16 字节 IV
struct KeyMaterial {
    std::vector<unsigned char> key; // 32
    std::vector<unsigned char> iv;  // 16
};

// 从 master password + salt 派生密钥
KeyMaterial deriveKey(const std::string& master,
                      const std::vector<unsigned char>& salt);

// 加密 / 解密
std::vector<unsigned char> aes256Encrypt(
        const std::vector<unsigned char>& plaintext,
        const KeyMaterial& km);

std::vector<unsigned char> aes256Decrypt(
        const std::vector<unsigned char>& ciphertext,
        const KeyMaterial& km);

