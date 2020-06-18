//  Group 47
//  2016B3A70562P   Akash Kabra
//  2016B3A70523P   Ayush Mungad
//  2016B4A70520P   Kumar Anant Raj
//  2016B4A70533P   Mustansir Mama
//  2016B5A70715P   Bhavesh Chand

#ifndef _AST_H_
#define _AST_H_

#include "parserDef.h"

#define INTW 8
#define BOOLW 8
#define RLW 8
#define ARRW 24

typedef struct ASTNode ASTNode;

#define X_node -100
#include <stdbool.h>
#include<stdio.h>
#include "mapping.h"
#define SYMSIZE 103


typedef enum basictype{ 
    INT,
    RL,
    BOOL,
    ARR
}basictype;

typedef union bound{
    int stat;
    ASTNode* dyn;
}bound;

typedef struct datatype{
    basictype b;
    bool isLowStatic;
    bool isHighStatic; 
    bound low;
    bound high;
    // bool isref;
    basictype elem_type;
}datatype;

typedef struct symEntry{
    char* id;
    int lineno;
    datatype type;
    int assigned;
    bool isLoopVar;
    bool isWhileVar; 
    // num_value value;
    int width;
    int offset;
    struct symEntry* next;         // For chaining
}symEntry;

typedef struct symTable{
    symEntry* table[SYMSIZE];  
    int nestingLevel;
    char* moduleName;
    // int size;
    bool printed;
    struct symTable* parent;
}symTable;

typedef struct param{
    char* id;
    datatype type;
    struct param* next;
} param;

typedef struct codeline{
    char line[120]; 
    struct codeline* next;
}codeline;

typedef struct code{
    codeline* head;
    codeline* tail;
}code;

typedef struct baseEntry{
    char* id;
    bool defined;   // is it defined in Moduledef
    bool declared;  // is it declared yet?
    bool current;   // Symbolizes if this function is the current function or not. To avoid recursion. 
    bool called; // has it been correct called earlier
    param* input_plist;
    param* output_plist;
    symTable* st;
    int lineno; 
    struct baseEntry* next;
}baseEntry;


typedef struct ASTNode{
    symbol s; 
    char* lexeme;
    int lineno;
    num_value val; 
    bool isLeaf; 
    int ruleNum;
    ASTNode* childL;
    // ASTNode* childR;
    ASTNode* inh;
    ASTNode* prev;
    ASTNode* next;
    ASTNode* parent;
    symTable* scope;
    code code;
    int offset; 
    int width; 
} ASTNode;


ASTNode* constructAST(ptnode* pthead, int action[NUM_RULES][MAX_RHS+1], ASTNode* par, ASTNode* inh);
void printASTPreOrder(ASTNode* node, FILE* outfile);
int get_sym_hash(char* str);
bool insert_sym_table(ASTNode* id, datatype t, symTable* tab,int isDecl);
void print_sym_table(symTable* st);
baseEntry** make_base_table();
datatype get_type(ASTNode* type);
param* get_param_list(ASTNode* ID, ASTNode* dtype, symTable* st);

bool populate_symbol_table(ASTNode* head, baseEntry** table);
void print_basetable(baseEntry** table);
symEntry* lookup_entry(ASTNode* id, symTable* tab);
symTable* lookup_var(ASTNode* id, symTable* tab);
bool insert_vars(ASTNode* idList, symTable* table);
bool insert_fn(ASTNode* fn,baseEntry** table);
baseEntry* lookup_fn(char* id, baseEntry** table);

void ASTPass(ASTNode* node);


bool isLoopVar(ASTNode* id,symTable* tab);
symTable* lookup_var(ASTNode* id, symTable* tab);
symEntry* lookup_entry(ASTNode* id, symTable* tab);
bool populate_drivermodule(ASTNode* D, baseEntry** table);
bool populate_othermodules(ASTNode* O, baseEntry** table);
bool populate_statements(ASTNode* stmts, baseEntry** ftable);
bool populate_casestmts(ASTNode* node, bool def, int lineno, baseEntry** ftable);
bool populate_default(ASTNode* node, bool def, int lineno, baseEntry** ftable);
bool populate_stmt(ASTNode* stmt, symTable* table, baseEntry** ftable);
datatype get_expression_type(ASTNode* node, symTable* tab);
datatype get_AOB_expression_type(ASTNode* node, symTable* tab);
bool is_equal_dtype(datatype t1, datatype t2);
bool check_id_which(ASTNode* child, bool isStmt);
bool populate_moduledeclarations(ASTNode* node, baseEntry** table);


bool second_pass(ASTNode* head, baseEntry** table);






#endif