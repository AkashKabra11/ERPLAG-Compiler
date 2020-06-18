//  Group 47
//  2016B3A70562P   Akash Kabra
//  2016B3A70523P   Ayush Mungad
//  2016B4A70520P   Kumar Anant Raj
//  2016B4A70533P   Mustansir Mama
//  2016B5A70715P   Bhavesh Chand

#ifndef _PARSE_H_
#define _PARSE_H_

#include <stdio.h>
#include <stdlib.h>
#include "parserDef.h"

int** createParseTable(Grammar** G,FirstAndFollow* F);
ptnode* init_ptnode();
ptnode* init_pt();
stack_str* init_stack();
ptnode* populatePTNode(Grammar* G,ptnode* par_node);
terminal is_in_synset(terminal t);
ptnode* parseInputSourceCode(int** ptab,Grammar** G,FILE* in, FirstAndFollow* F,keymap_node** key_hash);
void printNode(ptnode* node, FILE* outfile);
void printParseTree(ptnode* node, FILE *outfile);
void PrintParseTable(int** ptab);
void printParseTreePreOrder(ptnode* node, FILE *outfile);
void ComputeFirstAndFollowSets(Grammar** G, FirstAndFollow* F, grammar_index** idx_mapping, grammar_index** idx_mapping_follow);




#endif