#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void sha256(unsigned char *ptr, size_t size, unsigned char *ret) {

	// Step 1 - Preprocessing
	unsigned char mask = 0;
	unsigned char *buf = calloc(1000, sizeof(unsigned char));
	for (int i = 0; i < size; i++)
		buf[i] = ptr[i];

	buf[size++] = 0x80;

	int multiple;
	for (multiple = 1; (multiple * 512) < (size * 8); multiple++)
		;

	size_t pad_size = (512 * multiple - 64 - 8) / 8;
	size += pad_size;
	/* 1000 0000 */

	printf("%zu\n", pad_size);

	// Step 2 - Initialize hash values
	uint32_t h[8];

	h[0] = 0x6a09e667;
	h[1] = 0xbb67ae85;
	h[2] = 0x3c6ef372;
	h[3] = 0xa54ff53a;
	h[4] = 0x510e527f;
	h[5] = 0x9b05688c;
	h[6] = 0x1f83d9ab;
	h[7] = 0x5be0cd19;

	// Step 3 – Initialize Round Constants (isso pode ficar fora da funcao)

}

// main usada para testes, remover quando estiver pronto
int main(void) {
	char *ptr = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
	sha256(ptr, strlen(ptr), NULL);
	return 0;
}
