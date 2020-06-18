//  Group 47
//  2016B3A70562P   Akash Kabra
//  2016B3A70523P   Ayush Mungad
//  2016B4A70520P   Kumar Anant Raj
//  2016B4A70533P   Mustansir Mama
//  2016B5A70715P   Bhavesh Chand

#ifndef _MAPPING_H_
#define _MAPPING_H_

#include "langSpec.h"

#define NUM_SLOTS 3547
#define PRIME 31

#define ULL unsigned long long 


typedef union symbol{
    terminal t; 
    non_terminal nt; 
}symbol; 

typedef struct{
    symbol s; 
    char str[25]; 
    bool isterminal; 
}mapping_table;

typedef struct map_node{
    int mapping_position; 
    struct map_node* next; 
}map_node; 


#define BUFFER_LENGTH 1000

typedef struct Grammar{
    bool isterminal;
    symbol s;  
    struct Grammar* next; 
}Grammar; 

typedef struct grammar_index{
    int rule_num; 
    struct grammar_index* next; 
}grammar_index;

typedef struct{
    unsigned long long first;
    unsigned long long follow; 
}FirstAndFollow; 

int get_hash(char* str, int length);
map_node** make_hash_table();
void insert_hash(int hash_index, int mapping_pos, map_node** hash_table);
mapping_table* populate_mapping_table();
map_node** populate_hash_table();
Grammar** make_grammar();
int get_pos_mapping_table(char* token, map_node** mapping_hash_table, mapping_table* map_table);
void insert_rule(Grammar** grammar, map_node** mapping_hash_table, mapping_table* map_table);
FirstAndFollow* initialize_first_follow();
grammar_index** get_grammar_idx(Grammar** grammar);
unsigned long long computefirst(Grammar **G, FirstAndFollow* F, grammar_index** idx_mapping, symbol s, bool isterminal);
grammar_index** get_grammar_follow_idx(Grammar** grammar);
unsigned long long computefollow(Grammar **G, FirstAndFollow* F, grammar_index** idx_mapping_follow, symbol s);
void printFollowSet(FirstAndFollow* F);
void printFirstSet(FirstAndFollow* F);
void print_grammar(Grammar** grammar);
void print_hash(map_node** hash);
void print_index_mapping(grammar_index** idx_mapping);
#endif