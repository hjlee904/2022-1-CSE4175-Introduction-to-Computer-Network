#ifdef _MSC_VER
#define CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <cstring>

// xor 함수
char exor(char a, char b) {
	if (a == b) return '0';
	else return '1';
}

// modulo-2 division 함수
void mod_div(int* data_ext, char* generator, int d_len, int g_len) {
	char data[32];
	char key[16];
	char temp[16] = "000000000000000";
	int k = 0;
	
	for (int j = 0; j < (d_len + g_len - 1); j++) {
		if (data_ext[j] == 0) data[j] = '0';
		else data[j] = '1';
	}
	for (int j = 0; j < g_len; j++) {
		if (generator[j] == '0') key[j] = '0';
		else key[j] = '1';
	}

	for (int j = 0; j < g_len; j++) {
		temp[j] = data[j];
	}
	k++;

	while (temp[0] != '1') {
		for (int j = 0; j < g_len; j++) temp[j] = data[j + k];
		k++;
		if (k > (d_len - 1)) break;
	}

	if (temp[0] == '1') {
		while (1) {
			for (int j = 0; j < g_len; j++) {
				temp[j] = exor(temp[j], key[j]);
			}
			while (1) {
				if ((k > (d_len - 1)) || (temp[0] == '1')) break;
				for (int j = 0; j < (g_len - 1); j++) temp[j] = temp[j + 1];
				temp[(g_len - 1)] = data[k + (g_len - 1)];
				k++;
			}
			if (k > (d_len - 1)) {
				if (temp[0] == '1') {
					for (int j = 0; j < g_len; j++) {
						temp[j] = exor(temp[j], key[j]);
					}
				}
				break;
			}
		}
	}
	for (int j = 1; j < g_len; j++) {
		if (temp[j] == '0') data_ext[j + (d_len - 1)] = 0;
		else data_ext[j + (d_len - 1)] = 1;
	}

}

// crc_encoder
int main(int argc, char* argv[]) {
	// 인자의 수가 맞지 않은 경우
	if (argc != 5) {
		printf("usage: ./crc_encoder input_file output_flie generator dataword_size\n");
		exit(0);
	}
	char input_file[100];
	strcpy(input_file, argv[1]);
	char output_file[100];
	strcpy(output_file, argv[2]);
	char generator[20];
	strcpy(generator, argv[3]);
	int dataword_size = atoi(argv[4]);
	int g_len = strlen(argv[3]);

	FILE* fp;
	fp = fopen(input_file, "r");
	FILE* fp2;
	fp2 = fopen(output_file, "w");

	// input_file을 open하지 못할 경우
	if (fp == NULL) {
		printf("input file open error.\n");
		exit(0);
	}
	// output_file을 open하지 못할 경우
	if (fp2 == NULL) {
		printf("output file open error.\n");
		exit(0);
	}
	// dataword_size가 4 또는 8이 아닌 경우
	if (!((dataword_size == 4) || (dataword_size == 8))) {
		printf("dataword size must be 4 or 8.\n");
		exit(0);
	}

	else {
		// input_file로수부터 데이터를 읽어 들인다.
		int dw[32] = { 0, };
		int dw2[32] = { 0, };
		
		// dataword의 크기가 4인 경우
		if (dataword_size == 4) {
			int h = (dataword_size + g_len - 1) * 2;
			h = h % 8;
			h = 8 - h;
			int h2 = h;

			// padding byte
			int pb[8] = { 0,0,0,0,0,0,0,0 };
			for (int j = 7; j >= 0; j--) {
				pb[j] = h2 % 2;
				h2 = h2 / 2;
			}
			for (int j = 0; j < 8; j++) {
				fprintf(fp2, "%d", pb[j]);
			}
		
			while (1) {
				// 읽어 들인 데이터를 dataword로 나눈다.
				char data = fgetc(fp);
				if (data == EOF) break;
			
				for (int j = (dataword_size - 1); j >= 0; j--) {
					dw2[j] = data % 2;
					data = data / 2;
				}
				for (int j = (dataword_size - 1); j >= 0; j--) {
					dw[j] = data % 2;
					data = data / 2;
				}
			
				// dataword -> codeword (modulo-2 division)
				int data_ext[32] = { 0, };
				int data_ext2[32] = { 0, };
				for (int j = 0; j < dataword_size; j++) {
					data_ext[j] = dw[j];
				}

				// modulo-2 division
				mod_div(data_ext, generator, dataword_size, g_len);

				// dataword -> codeword
				for (int j = 0; j < dataword_size; j++) {
					data_ext2[j] = dw2[j];
				}

				// modulo-2 division
				mod_div(data_ext2, generator, dataword_size, g_len);
	
				// padding
				int pad[32] = { 0, };
				for (int j = h; j < (h + dataword_size + g_len - 1); j++) {
					pad[j] = data_ext[j - h];
					pad[j + dataword_size + g_len - 1] = data_ext2[j - h];
				}

				// out_file에 write
				for (int j = 0; j < h + ((dataword_size + g_len - 1) * 2); j++) {
					fprintf(fp2, "%d", pad[j]);
				}
			}
		}
		// dataword의 크기가 8인 경우
		else if (dataword_size == 8) {
			int dw[32] = { 0, };

			int h = dataword_size + g_len - 1;
			h = h % 8;
			h = 8 - h;
			int h2 = h;

			// padding byte
			int pb[8] = { 0,0,0,0,0,0,0,0 };
			for (int j = 7; j >= 0; j--) {
				pb[j] = h2 % 2;
				h2 = h2 / 2;
			}
			for (int j = 0; j < 8; j++) {
				fprintf(fp2, "%d", pb[j]);
			}
			while (1) {
				// 읽어 들인 데이터를 dataword로 나눈다.
				char data = fgetc(fp);
				if (data == EOF) break;
		
				for (int j = (dataword_size - 1); j >= 0; j--) {
					dw[j] = data % 2;
					data = data / 2;
				}
			
				// dataword -> codeword (modulo-2 division)
				int data_ext[32] = { 0, };
				for (int j = 0; j < dataword_size; j++) {
					data_ext[j] = dw[j];
				}

				// modulo-2 division
				mod_div(data_ext, generator, dataword_size, g_len);

				// padding
				int pad[32] = { 0, };
				for (int j = h; j < (h + dataword_size + g_len - 1); j++) {
					pad[j] = data_ext[j - h];
				}

				// out_file에 write
				for (int j = 0; j < h + (dataword_size + g_len - 1); j++) {
					fprintf(fp2, "%d", pad[j]);
				}
			}
		}
	}
	fclose(fp);
	fclose(fp2);

	return 0;
}
