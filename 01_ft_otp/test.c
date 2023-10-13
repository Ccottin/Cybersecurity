#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>

int main() {
    const char *message = "Hello, SHA-1!";
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1((unsigned char *)message, strlen(message), hash);

    printf("SHA-1 Hash: ");
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
        printf("%02x", hash[i]);
    }
    printf("\n");

    return 0;
}
