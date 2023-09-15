#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <cstring>

void conv(int node, int ***arr, int ***path) {

	int* flag = (int*)malloc(sizeof(int) * node);
	for (int i = 0; i < node; i++) {
		flag[i] = 0;
	}
	int idx = 0;
	for (int n1 = 0; n1 < node; n1++) {  //n1 = A
		for (int i = 0; i < node; i++) {
			flag[i] = 0;
		}
		flag[n1] = 1;
		for (int j = 0; j < node - 1; j++) {
			
			int len = 200;
			int len_i = 100;
			for (int n2 = 0; n2 < node; n2++) { //SPT=? node번 반복.
				if ((n1 != n2) && (len > arr[n1][n2][2]) && (arr[n1][n2][2] != -999) && flag[n2] == 0) {
					//path = 경로, path[A][A] = SPT A(0)D(1), path[A][C] = 3(0)E(1)D(2)A(3)
					len = arr[n1][n2][2];
					len_i = n2;
				}
			}
			if (len == 200) break;
			idx = path[n1][n1][0];
			idx++;
			path[n1][n1][idx] = len_i;
			path[n1][n1][0]++;
			flag[len_i] = 1;
			int temp, temp2;

			for (int r = 0; r < node; r++) {
				if (flag[r] == 0 && (arr[r][len_i][2] != -999)) {
					if (arr[n1][r][2] == -999) {
						arr[n1][r][2] = arr[r][len_i][2] + len;
						temp = arr[n1][len_i][1];
						for (int k = 0; k < node; k++) {
							temp2 = temp;
							temp = arr[n1][temp][1];
							if (temp == n1) break;
						}
						arr[n1][r][1] = arr[n1][temp2][1];
					}
					else if (arr[n1][r][2] != -999) {
						if (arr[n1][r][2] == (arr[r][len_i][2] + len)) {
							temp = arr[n1][r][1];
							for (int k = 0; k < node; k++) {
								temp2 = temp;
								temp = arr[n1][temp][1];
								if (temp == r) break;
							}
							if (temp2 > len_i) {
								temp = arr[n1][len_i][1];
								for (int k = 0; k < node; k++) {
									temp2 = temp;
									temp = arr[n1][temp][1];
									if (temp == n1) break;
								}
								arr[n1][r][1] = arr[n1][temp2][1];
							}
						}
						else if (arr[n1][r][2] > (arr[r][len_i][2] + len)) {
							arr[n1][r][2] = arr[r][len_i][2] + len;
							temp = arr[n1][len_i][1];
							for (int k = 0; k < node; k++) {
								temp2 = temp;
								temp = arr[n1][temp][1];
								if (temp == n1) break;
							}
							arr[n1][r][1] = arr[n1][temp2][1];
						}
					}
					
					
				}
			}
			
		}
	}
		
}

void msg(FILE* fp2, int ***arr, int ***path, FILE* fp4, int node) {
	char str[1000];
	int src;
	int dtn;
	for (int i = 0; i < 100; i++) {
		fscanf(fp2, "%d", &src);
		fscanf(fp2, "%d", &dtn);
		if (fgets(str, sizeof(str), fp2) == NULL) break;

		if (arr[dtn][src][2] == -999) {
			fprintf(fp4, "from %d to %d cost infinite hops unreachable message%s", src, dtn, str);
		}
		else {
			fprintf(fp4, "from %d to %d cost %d hops ", src, dtn, arr[src][dtn][2]);
			int temp = arr[src][dtn][1];
			fprintf(fp4, "%d ", src);
			for(int k = 0; k < node; k++) {
				fprintf(fp4, "%d ", temp);
				temp = arr[temp][dtn][1];
				if (temp == dtn) break;
			}
			fprintf(fp4, "message%s", str);
		}
	}
	fprintf(fp4, "\n");
}

void pnt(int node, int ***arr, FILE* fp4) {
	for (int i = 0; i < node; i++) {
		for (int j = 0; j < node; j++) {
			if (arr[i][j][2] != -999) fprintf(fp4, "%d %d %d\n", arr[i][j][0], arr[i][j][1], arr[i][j][2]);
		}
		fprintf(fp4, "\n");
	}
}

int main(int argc, char* argv[]) {
	// 인자의 수가 맞지 않은 경우
	if (argc != 4) {
		printf("usage: linkstate topologyfile messagesfile changesfilee\n");
		exit(0);
	}

	char topologyfile[100];
	strcpy(topologyfile, argv[1]);
	char messagefile[100];
	strcpy(messagefile, argv[2]);
	char changesfile[100];
	strcpy(changesfile, argv[3]);
	char outputfile[100];
	strcpy(outputfile, "output_ls.txt");

	FILE* fp1;
	fp1 = fopen(topologyfile, "r");
	FILE* fp2;
	fp2 = fopen(messagefile, "r");
	FILE* fp3;
	fp3 = fopen(changesfile, "r");
	FILE* fp4;
	fp4 = fopen(outputfile, "w");

	int node = 0;

	// topologyfile을 open하지 못할 경우
	if (fp1 == NULL) {
		printf("Error: open input file.\n");
		exit(0);
	}
	// messagefile을 open하지 못할 경우
	if (fp2 == NULL) {
		printf("Error: open input file.\n");
		exit(0);
	}
	// changesfile을 open하지 못할 경우
	if (fp3 == NULL) {
		printf("Error: open input file.\n");
		exit(0);
	}

	else {
		fscanf(fp1, "%d", &node); //int형 변수 node에 node의 개수 저장
		// converging the rounting table
		// create the arr (int[node][node][3])
		int*** arr = (int***)malloc(sizeof(int**) * node);
		for (int i = 0; i < node; i++) {
			arr[i] = (int**)malloc(sizeof(int*) * node);
			for (int j = 0; j < node; j++) {
				arr[i][j] = (int*)malloc(sizeof(int) * 3);
			}
		}

		// initalize the arr
		for (int i = 0; i < node; i++) {
			for (int j = 0; j < node; j++) {
				arr[i][j][0] = j;
				arr[i][j][1] = -999;
				arr[i][j][2] = -999;
			}
		}
		for (int i = 0; i < node; i++) {
			arr[i][i][0] = i;
			arr[i][i][1] = i;
			arr[i][i][2] = 0;
		}
		// 
		int t1 = 0;
		int t2 = 0;
		int t3 = 0;
		while (EOF != fscanf(fp1, "%d", &t1)) {
			fscanf(fp1, "%d", &t2);
			fscanf(fp1, "%d", &t3);
			arr[t1][t2][1] = t2;
			arr[t1][t2][2] = t3;
			arr[t2][t1][1] = t1;
			arr[t2][t1][2] = t3;
		}
		int*** arr_t = (int***)malloc(sizeof(int**) * node);
		for (int i = 0; i < node; i++) {
			arr_t[i] = (int**)malloc(sizeof(int*) * node);
			for (int j = 0; j < node; j++) {
				arr_t[i][j] = (int*)malloc(sizeof(int) * 3);
			}
		}

		for (int i = 0; i < node; i++) {
			for (int j = 0; j < node; j++) {
				for (int k = 0; k < 3; k++) {
					arr_t[i][j][k] = arr[i][j][k];
				}
			}
		}

		int*** path = (int***)malloc(sizeof(int**) * node);
		for (int i = 0; i < node; i++) {
			path[i] = (int**)malloc(sizeof(int*) * node);
			for (int j = 0; j < node; j++) {
				path[i][j] = (int*)malloc(sizeof(int) * 100);
			}
		}
		
		conv(node, arr, path);
		
		
		// print the arr
		pnt(node, arr, fp4);
		
		msg(fp2, arr, path, fp4, node);
		
		fclose(fp2);
		int src = 0;
		int dtn = 0;
		int cost = 0;

		for (int i = 0; i < 100; i++) {
			if (fscanf(fp3, "%d", &src) == EOF) break;
			fscanf(fp3, "%d", &dtn);
			fscanf(fp3, "%d", &cost);

			int*** arr_t2 = (int***)malloc(sizeof(int**) * node);
			for (int i = 0; i < node; i++) {
				arr_t2[i] = (int**)malloc(sizeof(int*) * node);
				for (int j = 0; j < node; j++) {
					arr_t2[i][j] = (int*)malloc(sizeof(int) * 3);
				}
			}

			for (int i = 0; i < node; i++) {
				for (int j = 0; j < node; j++) {
					for (int k = 0; k < 3; k++) {
						arr_t2[i][j][k] = arr_t[i][j][k];
					}
				}
			}

			arr_t2[src][dtn][1] = dtn;
			arr_t2[dtn][src][1] = src;
			arr_t2[src][dtn][2] = cost;
			arr_t2[dtn][src][2] = cost;

			for (int i = 0; i < node; i++) {
				for (int j = 0; j < node; j++) {
					for (int k = 0; k < 3; k++) {
						arr_t[i][j][k] = arr_t2[i][j][k];
					}
				}
			}

			conv(node, arr_t2, path);
			
			pnt(node, arr_t2, fp4);

			FILE* fp2;
			fp2 = fopen(messagefile, "r");
			msg(fp2, arr_t2, path, fp4, node);
		
			fclose(fp2);
			
			for (int i = 0; i < node; i++) {
				for (int j = 0; j < node; j++) {
					free(arr_t2[i][j]);
				}
			}
			for (int i = 0; i < node; i++) {
				free(arr_t2[i]);
			}
			free(arr_t2);
			
		}
		
		

		// free the arr
		for (int i = 0; i < node; i++) {
			for (int j = 0; j < node; j++) {
				free(arr[i][j]);
			}
		}
		for (int i = 0; i < node; i++) {
			free(arr[i]);
		}
		free(arr);

		for (int i = 0; i < node; i++) {
			for (int j = 0; j < node; j++) {
				free(arr_t[i][j]);
			}
		}
		for (int i = 0; i < node; i++) {
			free(arr_t[i]);
		}
		free(arr_t);

		// free the path
		for (int i = 0; i < node; i++) {
			for (int j = 0; j < node; j++) {
				free(path[i][j]);
			}
		}
		for (int i = 0; i < node; i++) {
			free(path[i]);
		}
		free(path);

		printf("Complete. Output file written to output_ls.txt.\n");
	}
	fclose(fp1);
	fclose(fp3);
	fclose(fp4);

	return 0;
}
