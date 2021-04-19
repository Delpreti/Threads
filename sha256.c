#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void sha256(unsigned char *ptr, size_t size, unsigned char *ret) {
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
}

int main(void) {
	char *ptr = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
	sha256(ptr, strlen(ptr), NULL);
	return 0;
}
