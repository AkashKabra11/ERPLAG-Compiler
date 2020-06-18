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

int TEMP_NUM = 0; 



bool isLoopVar(ASTNode* id,symTable* tab){
    symTable* t = lookup_var(id,tab);
    if(t==NULL) return false;
    return lookup_entry(id,t) -> isLoopVar;
}


// moduleDeclations
bool populate_moduledeclarations(ASTNode* node, baseEntry** table){
    bool valid = true;
    ASTNode* moduleDeclaration = node -> childL; 
    while(moduleDeclaration -> isLeaf == false){
        // till the EPS Node
        // moduleDeclaration -> scope = table;
        valid = insert_fn(moduleDeclaration,table) && valid;
    
        // if(insert_fn(moduleDeclaration,table) == true){
        //     // printf("Correctly inserted module: %s in symbol table\n", moduleDeclaration -> childL -> lexeme); 
        // }
        
        moduleDeclaration = moduleDeclaration -> next; 
    }
    return valid;
}

bool check_id_which(ASTNode* child, bool isStmt){
    // DECLARED HAI, ARRAY ELEMENT YA NORMAL VARIABLE HAI TO BOUND CHECK KIYA. 
    symTable* tmpscope = child -> scope;  
    child -> scope = lookup_var(child, child -> scope); 
    // printf("---------------- %d\n", child -> scope == NULL); 
    if(child -> scope == NULL){
        printf("Line %d: ERROR: Trying to use an undefined variable %s\n", child -> lineno,child -> lexeme); 
        return false; 
    }
    else{
        ASTNode* whichID_child;
        if(!isStmt){
            child -> next -> scope = tmpscope;
            whichID_child = child -> next -> childL; 
            whichID_child -> scope = tmpscope;
        }
        else{
            child -> next -> scope = tmpscope;
            child -> next -> childL -> scope = tmpscope;
            whichID_child = child -> next -> childL -> childL; 
            whichID_child -> scope = tmpscope;
        }
        if(whichID_child -> isLeaf && whichID_child -> s.t == ID){
            whichID_child -> scope = lookup_var(whichID_child, whichID_child -> scope); 
            if(whichID_child -> scope == NULL){
                printf("Line %d: ERROR: Trying to use an undefined variable %s\n", child -> lineno, whichID_child -> lexeme); 
                return false; 
            }
            else{
                // is Assigned A[m] -> m declared but is it assigned
            }
        }
        else if(whichID_child -> isLeaf && whichID_child -> s.t == NUM){
            // bound checking
            datatype t = lookup_entry(child, child -> scope) -> type;
            if(t.isLowStatic == 1 && t.isHighStatic == 1){
                int a = t.low.stat; 
                int b = t.high.stat;
                int val = whichID_child -> val.num; 
                // printf("-----> %d %d %d\n", a, val, b); 
                if(a <= val && val <= b){
                    // okay
                }
                else{
                    printf("Line %d: ERROR: Index %d out of bounds\n", child -> lineno, whichID_child -> val.num);
                    return false;  
                }
            }
        }
    }
    return true; 
}

bool is_equal_dtype(datatype t1, datatype t2){
    if(t1.b < 0 || t2.b < 0 )
        return false; 
    basictype b1, b2; 
    b1 = t1.b; 
    b2 = t2.b; 
    if(t1.b == ARR && t2.b == ARR){
        if(t1.elem_type != t2.elem_type){
            return false; 
        }
        if(t1.isHighStatic && t1.isLowStatic && t2.isHighStatic && t2.isLowStatic){
            int l1 = t1.low.stat; 
            int l2 = t2.low.stat; 
            int h1 = t1.high.stat; 
            int h2 = t2.high.stat; 
            if(l1 == l2 && h1 == h2){
                return true; 
            }
            else{
                return false; 
            }

        }
        else{
            // some par is dynamic, cant check
            return true; 
        }
    }
        
    if(b1 == b2){
        return true; 
    }
    else{
        return false; 
    }
}

datatype get_AOB_expression_type(ASTNode* node, symTable* tab){
    node = node -> childL; 
    node -> scope = tab; 
    datatype junk;
    junk.b = -1; 
    if(node -> isLeaf == true){
        // is it declared
        if(node -> s.t == ID){
            if(check_id_which(node, 0) == false){
                junk.b = -2; // not found
                return junk;
            }
            else{
               // array ; well formed; 
                datatype t = lookup_entry(node, node -> scope) -> type; 
                if(node -> next -> childL -> isLeaf && node -> next -> childL -> s.t == EPS){
                    return t; // A
                }
                else{
                        // A[5] 
                    datatype t1; 
                    t1.b = t.elem_type; 
                    return t1;
                }
            }
        }
        else if(node -> s.t == NUM){
            datatype d; 
            d.b = INT; 
            return d;
        }
        else if(node -> s.t == RNUM){
            datatype d; 
            d.b = RL; 
            return d;
        }
        else if(node -> s.t == BOOLEAN || node -> s.t == TRUE || node -> s.t == FALSE){
            datatype d; 
            d.b = BOOL; 
            return d;
        }
    }
    // if(node -> isLeaf == false && (node -> s.nt == op1 || node -> s.nt == op2)){
    ASTNode* op = node; 
    op -> offset = op -> parent -> offset; 
    symTable* sc = node -> scope; 
    node = node -> next; 
    node -> scope = sc; 
    node -> offset = node -> parent -> offset; 

    datatype t1 = get_AOB_expression_type(node, node -> scope); 
    
    // if(node -> ruleNum == X_node){
        //insert_sym_table()
        char* name = (char*)malloc(sizeof(char)*MAX_TK_LEN);
        sprintf(name, "__TEMP%d", TEMP_NUM);
        node -> lexeme = name; 
        TEMP_NUM++; 
        while(insert_sym_table(node, t1, node -> scope, 1) == false){
            sprintf(name, "__TEMP%d", TEMP_NUM);
            TEMP_NUM++; 
            node -> lexeme = name;
        }
        node -> parent -> offset = (node -> offset) + (node -> width);
        // symEntry* e1 = lookup_entry(node -> parent, node -> parent -> scope); 
        // e1 -> offset = node -> parent -> offset;  
    // }
    node = node -> next;
    node -> scope = sc; 
    node -> offset = node -> parent -> offset; 
    datatype t2 = get_AOB_expression_type(node, node -> scope);
    // printf("t1 -> %d, t2 -> %d\n", t1.b, t2.b);
    // if(node -> ruleNum == X_node){
        //char* name = (char*)malloc(sizeof(char)*MAX_TK_LEN);
        sprintf(name, "__TEMP%d", TEMP_NUM);
        node -> lexeme = name; 
        TEMP_NUM++; 
        while(insert_sym_table(node, t2, node -> scope, 1) == false){
            sprintf(name, "__TEMP%d", TEMP_NUM);
            TEMP_NUM++;
            node -> lexeme = name;
        }  
        node -> parent -> offset = (node -> offset) + (node -> width);
    // }
        // symEntry* e2 = lookup_entry(node -> parent, node -> parent -> scope); 
        // e2 -> offset = node -> parent -> offset; 

    if(t1.b == -2 || t2.b == -2){
        junk.b = -2;
        return junk;
    }
    if(is_equal_dtype(t1, t2) == true){
        if(op -> isLeaf == false ){
            
            if(op -> s.nt == op1 || op-> s.nt == op2){
                if(t1.b == INT || t1.b == RL )
                    return t1;
                
                // int + int , real + real
            }
            if(op -> s.nt == relationalOp)
                if(t1.b == INT || t1.b == RL ){
                    datatype dbool; 
                    dbool.b = BOOL;
                    return dbool;
                }
            if(op -> s.nt == logicalOp){
                if(t1.b == BOOL){
                    return t1;
                }
            }
        }
    }
        return junk; 
} 

datatype get_expression_type(ASTNode* node, symTable* tab){
    node -> offset = node -> parent -> offset; 
    if(node -> childL -> isLeaf == false && node -> childL -> s.nt == unary_op){

        symTable* sc = node -> scope; 
        node = node -> childL; 
        node -> offset = node -> parent -> offset; 
        node -> scope = sc; 
        node = node -> next; 
        node -> offset = node -> parent -> offset; 
        node -> scope = sc; 
    }
    node -> offset = node -> parent -> offset;
    datatype t1 = get_AOB_expression_type(node, node -> scope); 
    // node -> offset = node -> parent -> offset;
    char* name = (char*)malloc(sizeof(char)*MAX_TK_LEN);

    //if(t1.b >= 0){
    
        sprintf(name, "__TEMP%d", TEMP_NUM);
            node -> lexeme = name; 
            TEMP_NUM++; 
            // node -> offset += node -> width;  // ?? 
        
            while(insert_sym_table(node, t1, node -> scope,1) == false){
                sprintf(name, "__TEMP%d", TEMP_NUM);
                TEMP_NUM++;
                node -> lexeme = name;
            }
            // printf("__TEMP%d %d %d\n", TEMP_NUM, t1.b, node -> offset); 
            // node -> offset += node -> width; 
        // printf(">>>>>>>>%s\n", non_terminal_list[node -> parent -> s.nt]);
        node -> parent -> offset = (node -> offset) + (node -> width);
        // printf("%s ------------> %d\n", non_terminal_list[node -> parent -> s.nt], node -> parent -> offset);
        // symEntry* e2 = lookup_entry(node -> parent, node -> parent -> scope); 

        // e2 -> offset = node -> parent -> offset; 
    //}
    return t1; 
}

bool check_recursion(ASTNode* a, int lineno){
    bool flag = 0; 
    char* lex1 = a -> lexeme; 
    while(a -> s.nt != module){
        if(a -> s.nt == driverModule){
            flag = 1; 
            break; 
        }
        a = a -> parent; 
    }
    if(flag == 0){
        a = a -> childL; 
        if(strcmp(a -> lexeme , lex1) == 0){
            printf("Line %d: ERROR: Recursion not allowed\n", lineno); 
            return false; 
        }
    }
    return true; 
}

int varnum ; 
void get_num_assign(ASTNode* a, int NumAssign[]){
    if(a == NULL) return;     
    if(a -> isLeaf && a -> s.t == ID){
        // if(a -> scope == NULL){
        //     printf("%s <<<<<\n", a -> lexeme);
        //     return; 
        // }
        symEntry* e = lookup_entry(a, a -> scope);
        // if(e == NULL){
        //     printf("%s <<<\n", a -> lexeme);
        //     return; 
        // }
        NumAssign[varnum] = e -> assigned; 
       
        varnum++; 
        return; 
    }
     get_num_assign(a -> childL, NumAssign);
     get_num_assign(a -> next, NumAssign); 
     return; 
    
}

bool populate_stmt(ASTNode* stmt, symTable* table, baseEntry** ftable){
/*
ioStmt GET_VALUE BO ID BC SEMICOL
ioStmt PRINT BO var BC SEMICOL
var/ID declared or not 

simpleStmt
1. assignmentStmt:
ID declared? 
ID -> basetype = expression -> type
lalurARRstmt -> index1 -> ID declared or not? 
if(basetype == ARRAY){further checking
    dynamic array?? 
}
modulereusestmt ->
defined declared usage wala
assigned = True karna hai 1st pass me.
*/
    ASTNode* node = stmt -> childL; 
    node -> scope = stmt -> scope; 
    node -> offset = stmt -> offset; 
    if(node -> isLeaf && node -> s.t == GET_VALUE){
        ASTNode* ID = node -> next; 
        ID -> scope = node -> scope; 
        if(isLoopVar(ID ,ID ->scope)){
            printf("Line %d: ERROR: %s is a For Loop Variable. Can't be assigned\n",ID->lineno,ID->lexeme);
            return false; 
        }
        ID -> scope = lookup_var(ID, ID -> scope); 
        if(ID -> scope == NULL){
            printf("Line %d: ERROR: Trying to initialize an undefined variable %s\n", ID -> lineno, ID -> lexeme); 
            return false; 
        }
        symEntry* ent = lookup_entry(ID, ID-> scope);
        ent -> assigned += 1;
        return true;
    }
    else if(node -> isLeaf && node -> s.t == PRINT){
        ASTNode* var =  node -> next;
        var -> scope = node -> scope; 
        ASTNode* child = var -> childL;

        child -> scope = lookup_var(child, var -> scope);
        if(child -> scope == NULL){
            printf("NO SCOPE\n");
        }
        if(child -> isLeaf && child -> s.t == ID){
            if(check_id_which(child, 0) == false){
                return false; 
            } 
        }
        return true; 
    }
    else if(node -> isLeaf && node -> s.t == ASSIGNOP){
        ASTNode* a =  node -> next;
        a -> scope = node -> scope; 
        symTable* tmpscope = a -> scope; 
        //a -> offset = a -> parent -> offset;
        
        bool valid = true;
        if(isLoopVar(a ,a ->scope)){
            printf("Line %d: ERROR: %s is a For Loop Variable. Can't be assigned.\n",a->lineno,a->lexeme);
            valid = false; 
        }

        a -> scope = lookup_var(a, a -> scope); 
        ASTNode* b = a -> next; 
        
        b -> scope = tmpscope; 
        if(a -> scope == NULL){
            printf("Line %d: ERROR: Trying to assign undeclared variable %s\n",  a -> lineno, a -> lexeme); 
            valid = false;
        }
        else{
            symEntry* ee = lookup_entry(a, a-> scope); 
            a -> offset = ee -> offset;
            b -> offset = b -> parent -> offset; 
            
            if(a -> next -> childL -> isLeaf == false && a -> next -> childL -> s.nt == index1){
                // LvalueARRStmt 
                if(check_id_which(a, 1) == false){
                    return false; 
                }
                symEntry *e = lookup_entry(a, a->scope);
                e -> assigned += 1;
                datatype t1 = e->type;
                datatype tmp; 
                tmp.b = t1.elem_type; 
                t1 = tmp; 
                b -> childL -> next -> scope = b -> scope;
                ASTNode* c = b -> childL -> next;
                c -> offset = b -> offset; 
                datatype t2 = get_expression_type(c, c -> scope);
                //b -> offset = c -> offset; 
                b -> parent -> offset = b -> offset; 
                if(is_equal_dtype(t1, t2) == false){
                    // printf("dgfh %d %d \n", t1.elem_type, t2.b);
                    // printf("%d %d\n >>>>>>>>>>>>>\n", t1.b, t2.b); 
                    if(t2.b == -2){
                        // do nothing, some variable was undefined 
                    }
                    else if(t2.b == -1){
                        printf("Line %d: ERROR: Some type mismatch in RHS\n", a -> lineno); 
                    }
                    else if(t1.b == -1){
                         // do nothing, some variable was undefined 
                    }
                    else{
                        printf("Line %d: ERROR: Type mismatch in LHS and RHS\n", a -> lineno); 
                    }
                    valid = false; 
                }
                
            }
            else{
                // LvalueIDstmt
                //ID = EXPR 
                // if(check_id_which(a, 0) == false){
                //     return false; 
                // }
                symEntry* e =  lookup_entry(a, a -> scope); 
                e -> assigned += 1;
                datatype t1 = e -> type; 

                datatype t2 = get_expression_type(b, b -> scope); 
                // b -> parent -> offset = b -> offset; 
                // printf(" -------------- %d %d \n", t1.b, t2.b);
                if(is_equal_dtype(t1, t2) == false){
                    // printf("dgfh %d %d \n", t1.b, t2.b);
                    if(t2.b == -2){
                        // do nothing, some variable was undefined 
                    }
                    else if(t2.b == -1){
                        printf("Line %d: ERROR: Some type mismatch in RHS \n", a -> lineno); 
                    }
                    else if(t1.b == -1){
                         // do nothing, some variable was undefined 
                    }
                    else{
                        printf("Line %d: ERROR: Type mismatch in LHS and RHS\n", a -> lineno); 
                    }
                    valid =  false; 
                }

            }
            // is Assigned ? 
        }
        return valid;
    }
    else if(node -> isLeaf && node -> s.t == DECLARE){
        ASTNode* a =  node -> next;
        a -> scope = node -> scope; 
        a -> offset = node -> offset; 
        bool v =  insert_vars(a,a -> scope);
        a -> parent -> offset = a -> offset;
        // if(a -> parent -> ruleNum != X_node)
        // printf("%s ^^^^^^^^^^^^^\n", non_terminal_list[a -> parent -> s.nt]);
        // stmt -> offset = a -> offset; 
        return v; 
    }
    else if(node -> isLeaf && node -> s.t == FOR){
        bool valid1 = 1; 
        symTable* tab = node -> scope;
        ASTNode* a =  node -> next;
        a -> scope = lookup_var(a,tab);
        if(a -> scope == NULL){
            printf("Line %d: ERROR: Loop variable %s not declared.\n",a->lineno,a->lexeme);
            return false;
            valid1 = 0; 
        }
        symEntry* entry = lookup_entry(a,a -> scope);
        if(entry -> type.b != INT){
            printf("Line %d: ERROR: Loop variable %s must be integer.\n",a->lineno,a->lexeme);
            return false;
            valid1 = 0; 
        }
        entry ->isLoopVar = true;
        
        a = a -> next;
        a -> scope = tab;
        if(a -> childL -> val.num > a -> childL -> next -> val.num){
            printf("Line %d: ERROR: Loop lower bound must be <= upper bound.\n",a->childL->lineno);
            // return false;
            valid1 = 0; 
        }

        a = a -> next;
        a -> scope = (symTable*)calloc(1,sizeof(symTable));
        a ->  scope -> parent = tab; 
        a -> childL -> scope = a -> scope;
        a -> offset = a -> parent -> offset;
        a -> scope -> moduleName = a -> scope -> parent -> moduleName;
        a -> scope -> nestingLevel = a -> scope -> parent -> nestingLevel + 1; 
        bool valid = populate_statements(a, ftable) && valid1;
        a -> parent -> offset = a -> offset; 
        entry ->isLoopVar = false;
        return valid;
    }
    else if(node -> isLeaf && node -> s.t == WHILE){
        int lineno = node ->lineno;
        symTable* tab = node -> scope;
        ASTNode* a =  node -> next;
        a -> scope = tab;
        a-> offset = a -> parent -> offset;
        datatype t = get_expression_type(a,tab);
        ASTNode* expr = a; 
        int NumAssign[1000]; 
        varnum = 0; 
        get_num_assign(expr -> childL, NumAssign); 
        varnum =0; 
        a -> parent -> offset = a -> offset; 
        bool valid = 1; 
        if(t.b != BOOL){
            printf("Line %d: ERROR: Loop condition must be boolean.\n",lineno);
            valid = false; 
        }

        a = a -> next;
        a -> scope = (symTable*)calloc(1,sizeof(symTable));
        a -> scope -> parent = tab;
        a -> scope -> moduleName = a -> scope -> parent -> moduleName;
        a -> scope -> nestingLevel = a -> scope -> parent -> nestingLevel + 1;
        a -> childL -> scope = a -> scope;
        a -> offset = a -> parent -> offset; 
        bool v1  = populate_statements(a, ftable);
        a -> parent -> offset = a -> offset; 
        int NumAssign2[1000]; 

        varnum = 0; 
        get_num_assign(expr -> childL, NumAssign2); 
        bool isassn = 0; 
        for(int i=0; i < varnum; i++){
            if(NumAssign[i] != NumAssign2[i]){
                isassn = 1; 
                break; 
            }
        }
        if(isassn == 0){
            printf("Line %d: ERROR: None of the while loop expression variables are assigned.\n",lineno);
            valid = 0; 
        }
        varnum =0; 

        return v1 && valid; 
    }
    else if(node -> isLeaf && node -> s.t == SWITCH){
        int lineno = node -> lineno; 
        symTable* tab = node -> scope; 
        ASTNode* a = node -> next; 
        a -> scope = lookup_var(a,tab);
        bool valid = 1; 
        if(a -> scope == NULL){
            printf("Line %d: ERROR: Switch variable %s not declared.\n",a->lineno,a->lexeme);
            valid = 0; 
        }
        symEntry* entry = lookup_entry(a,a -> scope);
        if(entry -> type.b == RL || entry -> type.b == ARR){
            printf("Line %d: ERROR: Switch variable %s cannot be of type real.\n",a->lineno,a->lexeme);
            return false;  // as per last notice: Stage 2: Some updates and explanations Point 7
            valid = 0; 
        }
        bool def = 0; 
        if(entry -> type.b == INT){
            def = 1; 
        }
        else if(entry -> type.b == BOOL){
            def = 0; 
        }
        // printf("def = %d\n", def);
        // bool valid = true;
        a = a -> next;  // casestmts
        a -> scope = (symTable*)calloc(1,sizeof(symTable));
        symTable* caseScope = a -> scope; 
        a ->  scope -> parent = tab; 
        a -> childL -> scope = a -> scope;
        // a -> childL -> scope -> parent = tab;
        // If value in case stmts repeat, do we have to handle that ??
        a -> offset = a -> parent -> offset;
        a -> scope -> moduleName = a -> scope -> parent -> moduleName;
        a -> scope -> nestingLevel = a -> scope -> parent -> nestingLevel + 1; 
        a -> childL -> offset = a -> offset; 
        if(populate_casestmts(a -> childL, def, lineno, ftable) == false){
            valid = false; 
        }
        // a -> offset = a -> childL -> offset; 
        a -> parent -> offset = a -> offset; 

        a = a -> next;
        a -> scope = caseScope;
        a -> offset = a -> parent -> offset; 
        // printf("s\n"); 
        if(populate_default(a, def, lineno, ftable) == false){
            valid = false; 
        }
        a -> parent -> offset = a -> offset;
        // printf("ss\n"); 
        return valid; 
    }
    else if(node -> isLeaf && node -> s.t == USE){
        int lineno = node -> lineno;  

        ASTNode* a =  node -> next;
        a -> scope = node -> scope;
        
        symTable* tmpscope = a -> scope; 
        ASTNode* b = a -> childL;
        bool valid = true;
        while(b -> s.t != EPS){
            b -> scope = lookup_var(b, tmpscope);
            
            if(b->scope == NULL){
                printf("Line %d: ERROR: Trying to assign undeclared variable %s\n", b->lineno, b->lexeme); 
                valid = false;
            }
            else if(isLoopVar(b ,b ->scope)){
                printf("Line %d: ERROR: %s is a For Loop Variable. Can't be reassigned.\n",b->lineno,b->lexeme);
                valid = false; 
            }
            else{
                // printf("33333333333\n");                
                symEntry* ent = lookup_entry(b, b->scope);
                ent->assigned += 1;
            }
            b = b -> next; 
        }
        a = a -> next;
        a -> scope = tmpscope;
        if(check_recursion(a, lineno) == false){
            valid = false; 
        }
        //To check if function is declared or defined but not both.
        baseEntry* curr = lookup_fn(a->lexeme, ftable);
        if(curr==NULL){
            printf("Line %d: ERROR: Module %s neither declared nor defined\n", a -> lineno, a -> lexeme);
            valid = false; 

        }
        else if(curr -> defined && curr -> declared && !curr->called){
            printf("Line %d: ERROR: Module %s both declared and defined\n", a -> lineno, a -> lexeme);  
            valid = false;   
        }
        else
            curr->called = true;
        
        a = a->next;
        a -> scope = tmpscope;
        b = a -> childL;
        while(b->s.t != EPS){
            b -> scope = lookup_var(b, tmpscope);
            if(b->scope == NULL){
                printf("Line %d: ERROR: Trying to pass undeclared variable %s\n", b->lineno, b->lexeme); 
                valid = false;
            }
            else{
                // symEntry* en = lookup_entry(b, b->scope);
                // if(en->assigned == 0 && en -> type.b != ARR){
                //     printf("Line %d: ERROR: Trying to pass unassigned variable %s\n", b->lineno, b->lexeme); 
                //     valid = false; 
                // }
            }
            b = b->next;
        }
        return valid;
    }
}


bool populate_default(ASTNode* node, bool def, int lineno, baseEntry** ftable){
    // printf(" --> %s\n", non_terminal_list[node -> s.nt]);
    if(node -> childL -> isLeaf == 1 && node -> childL -> s.t == EPS && def == 1){
        printf("Line %d: ERROR: Switch construct should have a default construct for integer switch variable\n", lineno);
        return false;
    }
    if(def == 0 && node -> childL -> isLeaf == 0 && node -> childL -> s.nt == statement){
        ASTNode* firstStmt = node->childL;
        printf("Line %d: ERROR: Switch construct should not have a default construct for boolean switch variable\n", firstStmt->childL->lineno);
        return false;
    }

    if(populate_statements(node, ftable) == false){
        return false; 
    }
    // node -> parent -> offset = 
    return true; 
}

bool populate_casestmts(ASTNode* node, bool def, int lineno, baseEntry** ftable){
// node -> value
    ASTNode* stmts = node -> next; 
    symTable* sc = node -> scope; 
    stmts -> scope = node -> scope;
    stmts -> offset = node -> offset;  
    bool valid = 1; 
    bool true_present = 0; 
    bool false_present = 0; 
    while(stmts != NULL){
        node -> scope = sc; 
        stmts -> scope = sc; 
        ASTNode* ch = node -> childL; 
        ch -> scope = sc; 

        if(ch -> s.t == NUM && def == 0){
            printf("Line %d: ERROR: %d should be of type BOOLEAN.\n",ch->lineno,ch->val.num);
            return false;
        }
        else if((ch -> s.t == TRUE || ch -> s.t == FALSE) && def == 1){
            printf("Line %d: ERROR: %s should be of type INTEGER.\n",ch->lineno,ch->lexeme);
            return false;
        }
        if(ch -> s.t == TRUE){
            true_present = 1; 
        }
        if(ch -> s.t == FALSE){
            false_present = 1; 
        }
        stmts -> offset = stmts -> parent -> offset; 
        valid = populate_statements(stmts, ftable) && valid;
        stmts -> parent -> offset = stmts -> offset; 
        node -> offset = stmts -> offset; 
        // if(valid == 0){
        //     printf("Invalid: %d\n", stmts -> childL -> childL -> lineno); 
        // }
        node = stmts -> next; 
        stmts = node -> next; 
    }
    if(true_present == 1 && false_present == 0){
        printf("Line %d: ERROR: Switch construct should have case FALSE should be of type INTEGER.\n", lineno);
        return false;
    }
    if(true_present == 0 && false_present == 1){
        printf("Line %d: ERROR: Switch construct should have case TRUE should be of type INTEGER.\n", lineno);
        return false;
    }
    return valid; 
    
}

bool populate_statements(ASTNode* stmts, baseEntry** ftable){
    bool valid = true;
    ASTNode* stmt = stmts -> childL;
    while(stmt -> isLeaf == false){
            stmt -> offset = stmts -> offset; 
            stmt -> scope = stmts -> scope; 
            valid = populate_stmt(stmt, stmt -> scope, ftable) && valid;
            stmts -> offset = stmt -> offset; 
            stmt = stmt -> next;             
    }
    return valid;
}

bool populate_othermodules(ASTNode* O, baseEntry** table){
// void populate_moduledeclarations(ASTNode* node, baseEntry** table){
//     ASTNode* moduleDeclaration = node -> childL; 
//     while(moduleDeclaration -> isLeaf == 0){
//         // till the EPS Node
//         moduleDeclaration -> scope = table;
//         if(insert_fn(moduleDeclaration,table) == true){
//             printf("Correctly inserted module: %s in symbol table\n", moduleDeclaration -> childL -> lexeme); 
//         }
//         moduleDeclaration = moduleDeclaration -> next; 
//     }
// }    
    bool valid = true;
    ASTNode* mod = O -> childL; 
    while(mod -> isLeaf == 0){
        // mod -> scope = table;
        mod -> offset = 0;  
        if(insert_fn(mod, table) == true){
            
            // insert fn sets scope of moduledef as well
            ASTNode* moduledef = mod -> childL -> next -> next -> next;
            moduledef -> offset = moduledef -> parent -> offset;  
            valid =  populate_statements(moduledef, table) && valid;
            mod -> offset = moduledef -> offset;
        }
        else{
            // Drop this redefined module
            if(mod -> prev != NULL){
                mod -> prev -> next = mod -> next; 
            }
            if(mod -> next != NULL){
                mod -> next -> prev = mod -> prev; 
            }
            if(mod -> parent -> childL == mod){
                mod -> parent -> childL = mod -> next; 
            }
        }
        
        mod = mod -> next;
    }
    if(mod -> prev != NULL)
    mod -> prev -> parent -> offset = mod -> prev -> offset; 
    return valid;
}

bool populate_drivermodule(ASTNode* D, baseEntry** table){
    D -> offset = 0; 
    if(insert_fn(D, table) == true){
        ASTNode* moduledef = D -> childL ; 
        moduledef -> offset = D -> offset; 
        bool v1 = populate_statements(moduledef, table);
        D -> offset = moduledef -> offset; 
        return v1; 
    }
}


bool populate_symbol_table(ASTNode* head, baseEntry** table){
    // head -> program
    ASTNode* M = head -> childL; 
    ASTNode* O1 = M -> next; 
    ASTNode* D = O1 -> next; 
    ASTNode* O2 = D -> next; 
    bool valid = true;
    valid = populate_moduledeclarations(M, table) && valid; 
    valid = populate_othermodules(O1, table) && valid; 
    // printf("ttt\n");
    valid = populate_drivermodule(D, table) && valid; 
    // printf("wqfw\n");
    valid = populate_othermodules(O2, table) && valid; 
    return valid;
}
