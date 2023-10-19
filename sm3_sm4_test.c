#include <stdio.h>
#include <openssl/evp.h>

int main() {
    // Initialize OpenSSL library
    OpenSSL_add_all_algorithms();

    // Test SM3
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    const EVP_MD *md = EVP_sm3();
    EVP_DigestInit_ex(mdctx, md, NULL);
    EVP_DigestUpdate(mdctx, "Hello, SM3!", 11);
    unsigned char sm3_hash[EVP_MD_size(md)];
    EVP_DigestFinal_ex(mdctx, sm3_hash, NULL);

    printf("SM3 Hash: ");
    for (int i = 0; i < EVP_MD_size(md); i++) {
        printf("%02x", sm3_hash[i]);
    }
    printf("\n");

    // Test SM4
    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    const EVP_CIPHER *cipher = EVP_sm4_ecb();
    unsigned char sm4_key[EVP_CIPHER_key_length(cipher)];
    unsigned char plaintext[] = "Hello, SM4!";
    unsigned char ciphertext[sizeof(plaintext)];
    unsigned char decrypted[sizeof(plaintext)];

    // Generate a random SM4 key
    if (!RAND_bytes(sm4_key, sizeof(sm4_key)) || !EVP_EncryptInit_ex(ctx, cipher, NULL, sm4_key, NULL)) {
        fprintf(stderr, "SM4 initialization error\n");
        return 1;
    }

    int ciphertext_len = 0;
    if (!EVP_EncryptUpdate(ctx, ciphertext, &ciphertext_len, plaintext, sizeof(plaintext))) {
        fprintf(stderr, "SM4 encryption error\n");
        return 1;
    }

    int final_len = 0;
    if (!EVP_EncryptFinal_ex(ctx, ciphertext + ciphertext_len, &final_len)) {
        fprintf(stderr, "SM4 encryption error\n");
        return 1;
    }
    ciphertext_len += final_len;

    printf("SM4 Ciphertext: ");
    for (int i = 0; i < ciphertext_len; i++) {
        printf("%02x", ciphertext[i]);
    }
    printf("\n");

    // Clean up
    EVP_CIPHER_CTX_free(ctx);
    EVP_MD_CTX_free(mdctx);
    EVP_cleanup();

    return 0;
}

