#include <stdio.h>
#include "hmm.h"
#include <string.h>
//#include <math.h>
/*
typedef struct{
   char *model_name;
   int state_num;					//number of state
   int observ_num;					//number of observation
   double initial[MAX_STATE];			//initial prob.
   double transition[MAX_STATE][MAX_STATE];	//transition prob.
   double observation[MAX_OBSERV][MAX_STATE];	//observation prob.
} HMM;
*/
#define MAX_L 12000
char seq[MAX_L][MAX_SEQ];

int main(int argc, char * argv[]){
	//printf("0\n");
	for(int i = 0; i < MAX_L; i++){
		memset(seq[i], 0, sizeof(char)*MAX_SEQ);
	}
	//printf("1\n");
	FILE* fin;
	fin = fopen(argv[2], "r");
	int seq_num = 0;
	//printf("%d\n", seq_num);
	while(fscanf(fin, "%s", seq[seq_num]) != EOF){
		//printf("%d\n", seq_num);
		//printf("%s\n", seq[seq_num]);
		seq_num++;
	}
	fclose(fin);
	//printf("2\n");
	HMM models[5];
	load_models(argv[1], models, 5);

	//FILE* fp;
	FILE* fout;
	//fp = fopen(argv[2], "r");
	fout = fopen(argv[3], "w");
	//char seq[MAX_SEQ];
	//memset(seq, 0, sizeof(char)*MAX_SEQ);

	//printf("4\n");
	for(int t = 0; t < seq_num; t++){
		int len = 0;
		len = strlen(seq[t]);
		double prob[5];
		for(int i = 0; i < 5; i++){
			prob[i] = 0.0;
		}
		for(int i = 0; i < 5; i++){
			
			double delta[MAX_SEQ][MAX_STATE];
			//printf("3\n");
			for(int j = 0; j < MAX_SEQ; j++){
				for(int k = 0; k < MAX_STATE; k++){
					delta[j][k] = 0.0;
				}	
			}
			//delta initial
			for(int j = 0; j < models[i].state_num; j++){
				delta[0][j] = models[i].initial[j] * models[i].observation[seq[t][0]-'A'][j];
			}

			//delta recursion
			for(int j = 1; j < len; j++){
				for(int k = 0; k < models[i].state_num; k++){
					double tmp = -1;
					for(int l = 0; l < models[i].state_num; l++){
						double cmp_t = delta[j-1][l] * models[i].transition[l][k];
						if(cmp_t > tmp){
							tmp = cmp_t;
						}
					}
					delta[j][k] = tmp * models[i].observation[seq[t][j]-'A'][k];
				}
			}
			// find delta's highest prob
			double tmp = -1;
			for(int j = 0; j < models[i].state_num; j++){
				double cmp_t = delta[len-1][j];
				if(cmp_t > tmp){
					tmp = cmp_t;
				}
			}
			prob[i] = tmp;
		}

		// find models' highest prob's index
		int idx = 0;
		double max_v = -1;
		for(int i = 0; i < 5; i++){
			if(prob[i] > max_v){
				idx = i;
				max_v = prob[i];
			}
		}

		//output to result.txt
		fprintf(fout, "model_0%d.txt %e\n", idx+1, max_v);

		//memset(seq, 0, sizeof(char)*MAX_SEQ);
	}

	//fclose(fp);
	fclose(fout);

	return 0;
}