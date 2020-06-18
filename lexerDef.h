//  Group 47
//  2016B3A70562P   Akash Kabra
//  2016B3A70523P   Ayush Mungad
//  2016B4A70520P   Kumar Anant Raj
//  2016B4A70533P   Mustansir Mama
//  2016B5A70715P   Bhavesh Chand

#ifndef _LEXERDEF_H_
#define _LEXERDEF_H_

#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
#include<string.h>

#include "langSpec.h"

#define MAX_TK_LEN 20
#define BUF_LEN 512
#define LEXERR -1
#define NUM_SPACES 25

extern FILE* infile;
extern int pos; 
extern int lineno;
extern int charno;

int lookup_keyword(char* token, keymap_node** hash_table);

keymap_node** populate_keyword_hash_table();

#endif