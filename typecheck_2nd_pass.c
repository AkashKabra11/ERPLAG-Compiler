//  Group 47
//  2016B3A70562P   Akash Kabra
//  2016B3A70523P   Ayush Mungad
//  2016B4A70520P   Kumar Anant Raj
//  2016B4A70533P   Mustansir Mama
//  2016B5A70715P   Bhavesh Chand

#include "ast.h"
#include<stdlib.h>
#include<time.h>
#include<stdio.h>
#include "ast.h"
#include "langSpec.h"
#include <stdlib.h>
#include<stdio.h>
#include<stdbool.h>
#include<time.h> 
#include "lexer.h"
#include "mapping.h"
#include "parser.h"

bool populate_moduleReuseStmt(ASTNode*, baseEntry**);
bool populate_sec_stmts(ASTNode*, baseEntry**);

bool is_assigned_plist(ASTNode* output_plist, int lineno){
    bool valid = true; 
    output_plist = output_plist -> childL;
    // printf("1111\n");  
    while(output_plist -> isLeaf == 1 && output_plist -> s.t != EPS){
        // printf("2222 \n");
        symTable* scope = output_plist -> scope; 
        if(scope == NULL){
            // printf("1111111111111\n");
            return false;
        }
        if(lookup_entry(output_plist, scope) -> assigned == false){
            printf("Line %d: ERROR Return variable %s of this function is unassigned\n", lineno, output_plist -> lexeme);
            valid = false; 
        }
        // else{
        //     printf("%s **", output_plist -> lexeme );
        // }
        output_plist = output_plist -> next -> next; 
    }
    // printf("\n");
    return valid; 
}

bool checkParameters(ASTNode* list1, param* list2){
    ASTNode* node = list1->childL;
    // printf("<<<<<<<<<<<<<< %s ", node -> lexeme); 
    // if(list2 != NULL && list2 -> next != NULL)
    // printf("%s\n", list2 -> next -> id);
    while(node-> s.t != EPS && list2 != NULL){
        // if(node -> scope == NULL){
        //     printf("NULL SCOPE 2 WITH ID = %s\n", node -> lexeme ); 
        // }
        if(node -> scope == NULL){
            // that variable was undeclared
            // printf("222222222222\n");
            node = node -> next; 
            list2 = list2 -> next;
            continue; 
            //return false; 
        }
        datatype t1 = lookup_entry(node, node -> scope) -> type; 
        datatype t2 = list2-> type;
        if(is_equal_dtype(t1, t2)==false){
            
            return false;
        }
        node = node->next;
        list2 = list2->next;
    }
    if(node->s.t!=EPS || list2 !=NULL)
        return false;
    else
        return true;
}

bool populate_sec_stmts(ASTNode* stmts, baseEntry** ftable){
    bool valid = true;
    ASTNode* stmt = stmts -> childL;
    while(stmt -> isLeaf == false){
        valid = populate_moduleReuseStmt(stmt, ftable) && valid; 
        stmt = stmt -> next;
    }
    return valid;
}


bool populate_moduleReuseStmt(ASTNode* stmt, baseEntry** table){

    //navigate to statement
    ASTNode* node = stmt -> childL; 
    if(node -> isLeaf && node -> s.t == USE){
        int lineno = node -> lineno; 
        ASTNode* opt = node -> next;
        ASTNode* mod = opt -> next;
        ASTNode* idList = mod -> next;
        baseEntry* curr = lookup_fn(mod -> lexeme , table); 
        // printf("%s -> \n", curr -> id); 
        // param* x = curr -> input_plist; 
        // while(x != NULL){
        //     printf("%s ", x -> id); 
        //     x = x -> next; 
        // }
        // printf("\n Output List: ");
        // param* y = curr -> output_plist; 
        // while(y != NULL){
        //     printf("%s ", y -> id); 
        //     y = y -> next; 
        // }

        if(curr->defined == 0){
            printf("Line %d: ERROR: Undefined function called\n", lineno);
            return false; 
        }
        bool valid = checkParameters(opt, curr->output_plist);
        if(valid == false){
            printf("Line %d: ERROR: Output Parameters of Function don't match\n", lineno); 
        } 
        bool v2 = true; 
        v2 = checkParameters(idList, curr->input_plist);
        valid = v2 && valid;
        if(v2 == false){
            printf("Line %d: ERROR: Actual and Formal Parameters of Function don't match\n", lineno); 
        }
        if(valid==false)
            return valid;
    }
    else if(node -> isLeaf && node -> s.t == FOR){
        ASTNode* stmts = node->next->next->next;
        return populate_sec_stmts(stmts, table);
    }
    else if(node -> isLeaf && node -> s.t == SWITCH){
        ASTNode* caseStmts = node->next->next;
        ASTNode* stmts = caseStmts->childL->next;
        bool valid = 1; 
        while(stmts!=NULL){
            valid = populate_sec_stmts(stmts, table) && valid;
            stmts = stmts ->next->next;
        }
        ASTNode* default1 = caseStmts->next;
            
        if(default1->childL->isLeaf == 0){
            valid = populate_sec_stmts(default1, table) && valid;
        }
        return valid;
    }
    else if(node -> isLeaf && node -> s.t == WHILE){
        ASTNode* stmts = node->next->next;
        return populate_sec_stmts(stmts, table);
    }
    return true;
}




bool sec_pass_othermodules(ASTNode* O, baseEntry** table){

    bool valid = true;
    ASTNode* mod = O -> childL; 
    while(mod -> isLeaf == 0){
        baseEntry* curr = lookup_fn(mod -> childL -> lexeme , table); 
        int lineno = curr -> lineno;

        ASTNode* output_plist = mod -> childL -> next -> next;
        // printf(">>>>> %s\n", terminal_list[output_plist -> childL -> s.t]);
        valid = is_assigned_plist(output_plist, lineno) && valid;

        ASTNode* moduledef = mod -> childL -> next -> next -> next;
        valid =  populate_sec_stmts(moduledef, table) && valid;
        mod = mod -> next;
    }
    return valid;
}

bool sec_pass_drivermodule(ASTNode* D, baseEntry** table){
    ASTNode* moduledef = D -> childL;
    return populate_sec_stmts(moduledef, table);
}

bool second_pass(ASTNode* head, baseEntry** table){
    // head -> program
    ASTNode* M = head -> childL; 
    ASTNode* O1 = M -> next; 
    ASTNode* D = O1 -> next; 
    ASTNode* O2 = D -> next; 
    bool valid = true;
    valid = sec_pass_othermodules(O1, table) && valid;
    valid = sec_pass_drivermodule(D, table) && valid;
    valid = sec_pass_othermodules(O2, table) && valid;
    return valid;
}

 