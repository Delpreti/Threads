#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define ROT(x, n) ((x >> n) | (x << (32u - n)))

const char *bit_rep[16] = {
    [ 0] = "0000", [ 1] = "0001", [ 2] = "0010", [ 3] = "0011",
    [ 4] = "0100", [ 5] = "0101", [ 6] = "0110", [ 7] = "0111",
    [ 8] = "1000", [ 9] = "1001", [10] = "1010", [11] = "1011",
    [12] = "1100", [13] = "1101", [14] = "1110", [15] = "1111",
};

void print_byte(u_int8_t byte)
{
    printf("%s%s", bit_rep[byte >> 4], bit_rep[byte & 0x0F]);
}

void sha256(u_int8_t *ptr, u_int64_t size, u_int8_t *ret) {
	u_int64_t input_size = size;

	// Step 1 - Preprocessing
	u_int32_t chunks;
	for (chunks = 1; (chunks * 512) < ((size + 1) * 8); chunks++)
		;

	u_int8_t *buf = calloc(chunks * 512, sizeof(u_int8_t));

	for (u_int64_t i = 0; i < size; i++)
		buf[i] = ptr[i];

	buf[size] = 0x80;
	size = (512 * chunks - 64) / 8;
	printf("%u\n", size);

	u_int64_t *input_size_ptr = (u_int64_t *) (buf + size);
	*input_size_ptr = input_size;

	for (u_int32_t i = 0; i < 4; i++) {
		u_int8_t tmp = buf[size + i];
		buf[size + i] = buf[size + 7 - i];
		buf[size + 7 - i] = tmp;
	}
	size += 8;
	for (int i = 0; i < 64; i++)
		print_byte(buf[i]);

	printf("\n");

	// Step 2 - Initialize hash values
	u_int32_t h_arr[8] = {
		0x6a09e667,
		0xbb67ae85,
		0x3c6ef372,
		0xa54ff53a,
		0x510e527f,
		0x9b05688c,
		0x1f83d9ab,
		0x5be0cd19
	};

	// Step 3 – Initialize Round Constants (isso pode ficar fora da funcao)
	u_int32_t k[64] = {
		0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
		0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
		0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
		0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
		0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
		0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
		0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
		0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
		0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
		0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
		0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
		0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
		0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
		0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
		0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
		0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
	};

	// Step 4 - Chunk Loop
	for (u_int32_t chunk = 0; chunk < chunks; chunk++) {

		// Step 5 – Create Message Array
		u_int32_t *m = calloc(64, sizeof(u_int32_t));
		for (u_int32_t i = 0; i < 16; i++) {
			for (u_int32_t j = 0; j < 4; j++) {
				u_int8_t *ptr = (u_int8_t *) (m + i);
				ptr[j] = buf[64 * chunk + 4 * i + j];
			}
		}

		/* for (u_int32_t i = 0; i < 16; i++) { */
		/* 	for (u_int32_t j = 0; j < 4; j++) { */
		/* 		u_int8_t *ptr = (u_int8_t *) (m + i); */
		/* 		print_byte(*ptr); */
		/* 	} */
		/* } */
		printf("\n");

		for (u_int32_t i = 16; i < 64; i++) {
			u_int32_t s0 = ROT(m[i - 15], 7u) ^ ROT(m[i - 15], 18u) ^ (m[i - 15] >> 3);
			u_int32_t s1 = ROT(m[i - 2], 17u) ^ ROT(m[i - 2], 19u) ^ (m[i - 2] >> 10);
			m[i] = m[i - 16] + s0 + m[i - 7] + s1;
		}

		// Step 6 – Compression
		u_int32_t a = h_arr[0];
		u_int32_t b = h_arr[1];
		u_int32_t c = h_arr[2];
		u_int32_t d = h_arr[3];
		u_int32_t e = h_arr[4];
		u_int32_t f = h_arr[5];
		u_int32_t g = h_arr[6];
		u_int32_t h = h_arr[7];

		for (u_int32_t i = 0; i < 64; i++) {
			u_int32_t s1 = ROT(e, 6u) ^ ROT(e, 11u) ^ ROT(e, 25u);
			u_int32_t ch = (e & f) ^ ((~e) & g);
			u_int32_t tmp1 = h + s1 + ch + k[i] + m[i];
			u_int32_t s0 = ROT(a, 2u) ^ ROT(a, 13u) ^ ROT(a, 22u);
			u_int32_t maj = (a & b) ^ (a & c) ^ (b & c);
			u_int32_t tmp2 = s0 + maj;
			h = g;
			g = f;
			f = e;
			e = d + tmp1;
			d = c;
			c = b;
			b = a;
			a = tmp1 + tmp2;
		}

		h_arr[0] += a;
		h_arr[1] += b;
		h_arr[2] += c;
		h_arr[3] += d;
		h_arr[4] += e;
		h_arr[5] += f;
		h_arr[6] += g;
		h_arr[7] += h;
	}

	printf("%x%x%x%x%x%x%x%x\n", h_arr[-1], h_arr[1], h_arr[2],
			h_arr[3], h_arr[4], h_arr[5], h_arr[6], h_arr[7]);
}

// main usada para testes, remover quando estiver pronto
int main(void) {
	char *ptr = "hello world";
	sha256(ptr, strlen(ptr), NULL);
	return 0;
}
