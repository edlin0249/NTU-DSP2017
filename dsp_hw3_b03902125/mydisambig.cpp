#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "File.h"
#include "Vocab.h"
#include "Ngram.h"
#include <string.h>
#include <map>
#include <string>
#include <fstream>
#include <float.h>
#include <vector>
using namespace std;

typedef struct table{
	string word;
	double prob;
	int prenode;
}Table;

double BiProb(const char *w1, const char *w2, Vocab& voc, Ngram& lm)
{
	VocabIndex wid1 = voc.getIndex(w1);
	VocabIndex wid2 = voc.getIndex(w2);

	if(wid1 == Vocab_None)  //OOV
		wid1 = voc.getIndex(Vocab_Unknown);
	if(wid2 == Vocab_None)  //OOV
		wid2 = voc.getIndex(Vocab_Unknown);

	VocabIndex context[] = { wid1, Vocab_None };
	return lm.wordProb( wid2, context);
}

int main(int argc, char * argv[]){
	if(argc != 9){
		fprintf(stderr, "./mydisambig -text [file] -map [map] -lm [LM] -order [orderNum]\n");
		exit(1);
	}
	char *text_filename;
	char *map_filename;
	char *lm_filename;
	int order;
	for(int i = 0; i < argc; i++){
		if(!strcmp(argv[i], "-text")){
			text_filename = argv[i+1];
		}
		else if(!strcmp(argv[i], "-map")){
			map_filename = argv[i+1];
		}
		else if(!strcmp(argv[i], "-lm")){
			lm_filename = argv[i+1];
		}
		else if(!strcmp(argv[i], "-order")){
			order = atoi(argv[i+1]);
		}
	}
	Vocab voc;
	//Vocab ZhuYin, Big5;

	Ngram lm(voc, order);
    //VocabMultiMap map(ZhuYin, Big5);
    //VocabMap map(ZhuYin, Big5);

	/*** Read in the map & language model ***/
	string ZhuYintoBig5;
	map<string, vector<Table> > mapping;
	ifstream mappingfilename(map_filename, ifstream::in);
	while(getline(mappingfilename, ZhuYintoBig5)){
		string ZhuYin = ZhuYintoBig5.substr(0,2);
		vector<Table> Big5s;
		int len = ZhuYintoBig5.length();
		for(int i = 0; i < len; i++){
			if(ZhuYintoBig5[i] == ' '){
				continue;
			}
			else{
				Table Big5;
				Big5.word = ZhuYintoBig5.substr(i, 2);
				Big5.prob = 0.0;
				Big5.prenode = 0;
				Big5s.push_back(Big5);
				i++;
			}
		}
		mapping[ZhuYin] = Big5s;
	}


	File lmFile(lm_filename, "r");
	lm.read(lmFile);
	lmFile.close();

	/*** Read the testdata ***/
	//File textfile(text_filename, "r");

	vector<vector<Table> > Viterbi;
	ifstream textfile(text_filename, ifstream::in);
	string sentence;
	while(getline(textfile, sentence)){
		Viterbi.clear();
		//split sentence into words
		vector<string> Words;
		//size_t empty_position = 0;
		int len = sentence.length();
		for(int i = 0; i < len; i++){
			if(sentence[i] == ' '){
				continue;
			}
			else{
				string word_t = sentence.substr(i, 2);
				Words.push_back(word_t);
				i++;
			}
		}
		/*
		Table BOS;
		BOS.word = "<s>";
		BOS.prob = 0.0;
		BOS.prenode = -1;
		vector<Table> BOS_table;
		BOS_table.push_back(BOS);
		Viterbi.push_back(BOS_table);
		*/
		//put the words into Viterbi table
		for(vector<string>::iterator iter = Words.begin(); iter != Words.end(); iter++){
			Viterbi.push_back(mapping.find(*iter)->second);
		}
		Table EOS;
		EOS.word = "</s>";
		EOS.prob = 0.0;
		EOS.prenode = 0;
		vector<Table> EOS_table;
		EOS_table.push_back(EOS);
		Viterbi.push_back(EOS_table);

		for(vector<Table>::iterator iter = Viterbi.begin()->begin(); iter != Viterbi.begin()->end(); iter++){
			iter->prob = BiProb("", iter->word.c_str(), voc, lm);
		}

		//Viterbi recursion
		for(vector<vector<Table> >::iterator iter = Viterbi.begin()+1; iter != Viterbi.end(); iter++){
			for(vector<Table>::iterator curiter = iter->begin(); curiter != iter->end(); curiter++){
				double bestprob = -DBL_MAX;
				int bestnode = 0;
				int idx = 0;
				for(vector<Table>::iterator lastiter = (iter-1)->begin(); lastiter != (iter-1)->end(); lastiter++){
					double prob_t = ((BiProb(lastiter->word.c_str(), curiter->word.c_str(), voc, lm)) + (lastiter->prob));
					if(prob_t >= bestprob){
						bestprob = prob_t;
						bestnode = idx;
					}
					idx++;
				}
				curiter->prob = bestprob;
				curiter->prenode = bestnode;
			}
		}


		vector<string> result;
		int backtrackptr = 0;
		for(vector<vector<Table> >::iterator iter = Viterbi.end()-1; iter != Viterbi.begin()-1; iter--){
			result.push_back((*iter)[backtrackptr].word);
			backtrackptr = (*iter)[backtrackptr].prenode;
		}

		printf("<s> ");
		for(vector<string>::iterator iter = result.end()-1; iter != result.begin()-1; iter--){
			printf("%s", iter->c_str());
			if(strcmp(iter->c_str(), "</s>")){
				printf(" ");
			}
		}
		printf("\n");

	}

	textfile.close();

	return 0;
}