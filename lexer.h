//  Group 47
//  2016B3A70562P   Akash Kabra
//  2016B3A70523P   Ayush Mungad
//  2016B4A70520P   Kumar Anant Raj
//  2016B4A70533P   Mustansir Mama
//  2016B5A70715P   Bhavesh Chand

#ifndef _LEXER_H_
#define _LEXER_H_

#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
#include<string.h>

#include "lexerDef.h"

void reset_globals();
tokenInfo* getNextToken(keymap_node** key_hash);
void removeComments(FILE* testcaseFile);
void lexer(keymap_node** key_hash);

#endif