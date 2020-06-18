//  Group 47
//  2016B3A70562P   Akash Kabra
//  2016B3A70523P   Ayush Mungad
//  2016B4A70520P   Kumar Anant Raj
//  2016B4A70533P   Mustansir Mama
//  2016B5A70715P   Bhavesh Chand

#ifndef _TYPECHECK_H_
#define _TYPECHECK_H_

#include "ast.h"
#include "langSpec.h"
#include <stdlib.h>
#include<stdio.h>
#include<stdbool.h>
#include<time.h> 
#include "lexer.h"
#include "mapping.h"
#include "parser.h"

bool isLoopVar(ASTNode* id,symTable* tab);
symTable* lookup_var(ASTNode* id, symTable* tab);
symEntry* lookup_entry(ASTNode* id, symTable* tab);



bool populate_drivermodule(ASTNode* D, baseEntry** table);
bool populate_othermodules(ASTNode* O, baseEntry** table);
bool populate_statements(ASTNode* stmts);
bool populate_casestmts(ASTNode* node, bool def, int lineno);
bool populate_default(ASTNode* node, bool def, int lineno);
bool populate_stmt(ASTNode* stmt, symTable* table);
datatype get_expression_type(ASTNode* node, symTable* tab);
datatype get_AOB_expression_type(ASTNode* node, symTable* tab);
bool is_equal_dtype(datatype t1, datatype t2);
bool check_id_which(ASTNode* child, bool isStmt);
bool populate_moduledeclarations(ASTNode* node, baseEntry** table);

#endif
