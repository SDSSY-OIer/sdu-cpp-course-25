#include "../include/Crypto.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <stdexcept>

static constexpr int KEY_LEN = 32;   // 256 bit
static constexpr int IV_LEN  = 16;   // 128 bit
static constexpr int SALT_LEN = 16;
static constexpr int PBKDF_ITER = 100'000;

/* 派生密钥 */
KeyMaterial deriveKey(const std::string& master,
                      const std::vector<unsigned char>& salt)
{
    static constexpr int KEY_LEN = 32;
    static constexpr int IV_LEN  = 16;

    /* 先把 32+16 bytes 派生到临时缓冲区 */
    std::vector<unsigned char> buf(KEY_LEN + IV_LEN);

    if (PKCS5_PBKDF2_HMAC(master.c_str(), master.size(),
                          salt.data(), salt.size(),
                          PBKDF_ITER, EVP_sha256(),
                          buf.size(),
                          buf.data()) != 1)
        throw std::runtime_error("PBKDF2 failed");

    /* 拆分填充到 KeyMaterial */
    KeyMaterial km;
    km.key.assign(buf.begin(),            buf.begin() + KEY_LEN);
    km.iv .assign(buf.begin() + KEY_LEN,  buf.end());
    return km;
}
/* 加密 */
std::vector<unsigned char> aes256Encrypt(
        const std::vector<unsigned char>& plaintext,
        const KeyMaterial& km)
{
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    std::vector<unsigned char> out(plaintext.size()+IV_LEN);
    int len=0, out_len=0;

    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr,
                       km.key.data(), km.iv.data());
    EVP_EncryptUpdate(ctx, out.data(), &len,
                      plaintext.data(), plaintext.size());
    out_len = len;
    EVP_EncryptFinal_ex(ctx, out.data()+len, &len);
    out_len += len;
    out.resize(out_len);
    EVP_CIPHER_CTX_free(ctx);
    return out;
}

/* 解密 */
std::vector<unsigned char> aes256Decrypt(
        const std::vector<unsigned char>& ciphertext,
        const KeyMaterial& km)
{
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    std::vector<unsigned char> out(ciphertext.size());
    int len=0, out_len=0;

    EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), nullptr,
                       km.key.data(), km.iv.data());
    EVP_DecryptUpdate(ctx, out.data(), &len,
                      ciphertext.data(), ciphertext.size());
    out_len = len;
    if (EVP_DecryptFinal_ex(ctx, out.data()+len, &len) != 1)
        throw std::runtime_error("Bad master password or data corrupt");
    out_len += len;
    out.resize(out_len);
    EVP_CIPHER_CTX_free(ctx);
    return out;
}

