#include <stdio.h>
#include "hmm.h"
#include <string.h>
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
	for(int i = 0; i < MAX_L; i++){
		memset(seq[i], 0, sizeof(char) * MAX_SEQ);
	}
	
	FILE* fin;
	fin = fopen(argv[3], "r");
	int seq_num = 0;
	//printf("read %s\n", argv[3]);
	while(fscanf(fin, "%s", seq[seq_num]) != EOF){
		//printf("%d\n", seq_num);
		//printf("%s\n", seq[seq_num]);
		seq_num++;
	}
	//printf("reading is over.\n");
	fclose(fin);
	HMM model;

	//load initial model
	loadHMM(&model, argv[2]);
	int iteration = 0;
	iteration = atoi(argv[1]);
	while(iteration--){
		printf("%d\n", iteration);
		//char seq[MAX_SEQ] = {};
		double accumulate_initial[MAX_STATE];
		double accumulate_gemma[MAX_STATE];
		double accumulate_epsilon[MAX_STATE][MAX_STATE];
		double accumulate_observation_gemma[MAX_OBSERV][MAX_STATE];

		for(int i = 0; i < MAX_STATE; i++){
			accumulate_initial[i] = 0.0;
		}

		for(int i = 0; i < MAX_STATE; i++){
			accumulate_gemma[i] = 0.0;
		}

		for(int i = 0; i < MAX_STATE; i++){
			for(int j = 0; j < MAX_STATE; j++){
				accumulate_epsilon[i][j] = 0.0;
			}
		}

		for(int i = 0; i < MAX_OBSERV; i++){
			for(int j = 0; j < MAX_STATE; j++){
				accumulate_observation_gemma[i][j] = 0.0;
			}
		}

		//memset(seq, 0, sizeof(char)*MAX_SEQ);
		//int samples_num = 0;

		//file open
		//FILE* fp;
		//fp = fopen(argv[3], "r");
		//read seq from seq_model_#.txt
		for(int t = 0; t < seq_num; t++){
			int len = 0;
			len = strlen(seq[t]);
			double alpha[MAX_SEQ][MAX_STATE] = {};
			double beta[MAX_SEQ][MAX_STATE] = {};
			double gemma[MAX_SEQ][MAX_STATE] = {};
			double epsilon[MAX_SEQ][MAX_STATE][MAX_STATE] = {};
			//for(int i = 0; i < MAX_SEQ; i++){  //for each char of seq
			//alpha initial
			for(int i = 0; i < model.state_num; i++){
				alpha[0][i] = model.initial[i] * model.observation[seq[t][0]-'A'][i];
			}
			//}
			// alpha induction
			for(int i = 1; i < len; i++){ //which seq
				for(int j = 0; j < model.state_num; j++){  //which state
					double tmp = 0.0;
					for(int k = 0; k < model.state_num; k++){	 //sigma sum of alpha and transitions' product
						tmp += alpha[i-1][k] * model.transition[k][j];
					}
					alpha[i][j] = tmp * model.observation[seq[t][i]-'A'][j];
				}
			}
			//alpha termination

			//beta initial
			for(int i = 0; i < model.state_num; i++){
				beta[len-1][i] = 1.0;
			}

			//beta induction
			for(int i = len-2; i >= 0; i--){ //which seq
				for(int j = 0; j < model.state_num; j++){ //which state
					double tmp = 0.0;
					for(int k = 0; k < model.state_num; k++){
						tmp += model.transition[j][k] * model.observation[seq[t][i+1]-'A'][k] * beta[i+1][k];
					}
					beta[i][j] = tmp;
				}
			}

			//calculate gemma
			double normalize_gemma[MAX_SEQ];
			for(int i = 0; i < MAX_SEQ; i++){ //initial zero
				normalize_gemma[i] = 0.0;
			}

			for(int i = 0; i < len; i++){
				for(int j = 0; j < model.state_num; j++){
					gemma[i][j] = alpha[i][j] * beta[i][j];
					normalize_gemma[i] += gemma[i][j];
				}
			}

			//normalize gemma
			for(int i = 0; i < len; i++){
				for(int j = 0; j < model.state_num; j++){
					gemma[i][j] = gemma[i][j]/normalize_gemma[i];
				}
			}

			//calculate epsilon
			double normalize_epsilon[MAX_SEQ];
			for(int i = 0; i < MAX_SEQ; i++){
				normalize_epsilon[i] = 0.0;
			}

			for(int i = 0; i < len-1; i++){
				for(int j = 0; j < model.state_num; j++){
					for(int k = 0; k < model.state_num; k++){
						epsilon[i][j][k] = alpha[i][j] * model.transition[j][k] * model.observation[seq[t][i+1]-'A'][k] * beta[i+1][k];
						normalize_epsilon[i] += epsilon[i][j][k];
					}
				}
			}

			//normalize epsilon
			for(int i = 0; i < len-1; i++){
				for(int j = 0; j < model.state_num; j++){
					for(int k = 0; k < model.state_num; k++){
						epsilon[i][j][k] = epsilon[i][j][k] / normalize_epsilon[i];
					}
				}
			}

			//accumulate gemma
			for(int i = 0; i < model.state_num; i++){
				accumulate_initial[i] += gemma[0][i];
			}

			for(int i = 0; i < model.state_num; i++){
				for(int j = 0; j < len-1; j++){
					accumulate_gemma[i] += gemma[j][i];
				}
			}

			for(int i = 0; i < model.state_num; i++){
				for(int j = 0; j < model.state_num; j++){
					for(int k = 0; k < len-1; k++){
						accumulate_epsilon[i][j] += epsilon[k][i][j];
					}
				}
			}

			for(int i = 0; i < len; i++){
				for(int j = 0; j < model.state_num; j++){
					accumulate_observation_gemma[seq[t][i]-'A'][j] += gemma[i][j];
				}
			}

		}
		//all samples are over
		//file close
		//fclose(fp);

		//Re-estimate Model Parameters
		//pi, of initial
		for(int i = 0; i < model.state_num; i++){
			model.initial[i] = accumulate_initial[i]/seq_num;
		}

		//a, or transition
		for(int i = 0; i < model.state_num; i++){
			for(int j = 0; j < model.state_num; j++){
				model.transition[i][j] = accumulate_epsilon[i][j] / accumulate_gemma[i];
			}
		}

		//b, or observation
		for(int i = 0; i < model.observ_num; i++){
			for(int j = 0; j < model.state_num; j++){
				model.observation[i][j] = accumulate_observation_gemma[i][j]/accumulate_gemma[j];
			}
		}

	}

	FILE* fout;
	fout = fopen(argv[4], "w");
	dumpHMM(fout, &model);
	fclose(fout);

	return 0;
}