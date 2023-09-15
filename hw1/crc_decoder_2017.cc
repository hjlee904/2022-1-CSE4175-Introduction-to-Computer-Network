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
int mod_div(int* data_ext, char* generator, int d_len, int g_len) {
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
	int kk = 1;
	int gg = 0;
	for (int j = 1; j < g_len; j++) {
		if (temp[j] == '0') gg = 1;
		else gg = 0;
		kk *= gg;
	}	
	
	return kk;

}

// crc_decoder
int main(int argc, char* argv[]) {
	// 인자의 수가 맞지 않은 경우
	if (argc != 6) {
		printf("usage: ./crc_decoder input_file output_flie result_file generator dataword_size\n");
		exit(0);
	}
	char input_file[100];
	strcpy(input_file, argv[1]);
	char output_file[100];
	strcpy(output_file, argv[2]);
	char result_file[100];
	strcpy(result_file, argv[3]);
	char generator[20];
	strcpy(generator, argv[4]);
	int dataword_size = atoi(argv[5]);
	int g_len = strlen(argv[4]);

	FILE* fp;
	fp = fopen(input_file, "r");
	FILE* fp2;
	fp2 = fopen(output_file, "w");
	FILE* fp3;
	fp3 = fopen(result_file, "w");
	
	int num = 0;
	int err = 0;

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
	// result_file을 open하지 못할 경우
	if (fp3 == NULL) {
		printf("result file open error.\n");
		exit(0);
	}
	// dataword_size가 4 또는 8이 아닌 경우
	if (!((dataword_size == 4) || (dataword_size == 8))) {
		printf("dataword size must be 4 or 8.\n");
		exit(0);
	}

	else {
		// input_file로부터 데이터를 읽어 들인다.
		int cw[32], cw2[32];
		char data;
		char pdn[9];
		int kk = 1;
		int h = 0; // padding bit 수
		for (int j = 0; j < 8; j++) pdn[j] = fgetc(fp);
		for (int j = 0; j < 8; j++) {
			if (pdn[7 - j] == '1') h += kk;
			kk *= 2;
		}
		int hh = dataword_size + g_len - 1; // codeword의 길이
		
		// dataword의 크기가 4인 경우
		if (dataword_size == 4) {
			while (1) {
				// 읽어 들인 데이터에서 패딩비트를 제거한다.
				int flag = 0;
				for (int j = 0; j < h; j++) {
					data = fgetc(fp);
					if (data == EOF) flag = 1;
				}
				if (flag == 1) break;

				int dw[32] = { 0, };
				int dw2 = 0;
				int k = 1;

				// 읽어 들인 데이터를 codeword 크기로 나눈다.
				for (int j = 0; j < hh; j++) {
					data = fgetc(fp);
					if (data == EOF) flag = 1;
					if (data == '0') cw[j] = 0;
					else cw[j] = 1;
					if (j == (hh - 1)) num++;
				}
				if (flag == 1) break;
				for (int j = 0; j < hh; j++) {
					data = fgetc(fp);
					if (data == EOF) flag = 1;
					if (data == '0') cw2[j] = 0;
					else cw2[j] = 1;
					if (j == (hh - 1)) num++;
				}
				if (flag == 1) break;

				// 에러 체크
				if (mod_div(cw, generator, dataword_size, g_len) != 1) err++;
				if (mod_div(cw2, generator, dataword_size, g_len) != 1) err++;

				for (int j = 0; j < 4; j++) {
					dw[j] = cw[j];
				}
				for (int j = 4; j < 8; j++) {
					dw[j] = cw2[j - 4];
				}

				// codeword -> dataword
				for (int j = 0; j < 8; j++) {
					dw2 += dw[7 - j] * k;
					k *= 2;
				}

				// out_file에 write
				fprintf(fp2, "%c", dw2);
			}
		}
		// dataword의 크기가 8인 경우
		else if (dataword_size == 8) {
			while (1) {
				// 읽어 들인 데이터에서 패딩비트를 제거한다.
				int flag = 0;
				for (int j = 0; j < h; j++) {
					data = fgetc(fp);
					if (data == EOF) flag = 1;
				}
				if (flag == 1) break;

				int dw[32] = { 0, };
				int dw2 = 0;
				int k = 1;
				
				// 읽어 들인 데이터를 codeword 크기로 나눈다.
				for (int j = 0; j < hh; j++) {
					data = fgetc(fp);
					if (data == EOF) flag = 1;
					if (data == '0') cw[j] = 0;
					else cw[j] = 1;
					if (j == (hh - 1)) num++;
				}
				if (flag == 1) break;

				// 에러 체크
				if (mod_div(cw, generator, dataword_size, g_len) != 1) err++;

				for (int j = 0; j < 8; j++) {
					dw[j] = cw[j];
				}

				// codeword -> dataword
				for (int j = 0; j < 8; j++) {
					dw2 += dw[7 - j] * k;
					k *= 2;
				}

				// out_file에 write
				fprintf(fp2, "%c", dw2);
			}
		}
		fprintf(fp3, "%d %d", num, err);

	}
	fclose(fp);
	fclose(fp2);
	fclose(fp3);

	return 0;
}
