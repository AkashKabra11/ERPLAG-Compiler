//  Group 47
//  2016B3A70562P   Akash Kabra
//  2016B3A70523P   Ayush Mungad
//  2016B4A70520P   Kumar Anant Raj
//  2016B4A70533P   Mustansir Mama
//  2016B5A70715P   Bhavesh Chand

#ifndef _PARSERDEF_H_
#define _PARSERDEF_H_

#include <stdbool.h>

#define ERROR -1
#define SYN -2
#include "mapping.h"

extern bool correct;
extern ULL one;
#define STACK_SIZE 1000

typedef union num_value{
    int num;
    float rnum;
}num_value;

typedef struct ptnode{
    //struct ptnode* parent;
    symbol s; 
    char* lexeme;
    int lineno; 
    char* tokenName; 
    num_value val; 
    char* parentNodeSymbol; 
    bool isLeaf; 
    char* nodeSymbol; 
    bool isEps; 
    int ruleNum; 
    struct ptnode* child;
    struct ptnode* sibling; 
}ptnode;

typedef struct stack_entry{
    bool ist;
    symbol s;
    ptnode* ptr;
}stack_entry;

typedef struct stack_str{
    stack_entry stack[STACK_SIZE];
    int top;
}stack_str;

stack_str* create_stack();
bool isFull(stack_str*);
bool isEmpty(stack_str*);
void push(stack_str*,stack_entry );
void pop(stack_str*);
stack_entry peek(stack_str*);
void print_stack(stack_str* st);

#endif