//  Group 47
//  2016B3A70562P   Akash Kabra
//  2016B3A70523P   Ayush Mungad
//  2016B4A70520P   Kumar Anant Raj
//  2016B4A70533P   Mustansir Mama
//  2016B5A70715P   Bhavesh Chand

#include "ast.h"
#include "langSpec.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h> 
#include "lexer.h"
#include "mapping.h"
#include "parser.h"

bool isLoopVar(ASTNode* id,symTable* tab);
symTable* lookup_var(ASTNode* id, symTable* tab);
symEntry* lookup_entry(ASTNode* id, symTable* tab);


int get_sym_hash(char* str){
    int length = strlen(str); 
    int hash = 0; 
    long long power = 1;
    for(int i= 0; i < length; i++){
        int tmp = str[i];
        hash = (hash + (((tmp)*(power))%SYMSIZE)%SYMSIZE)%SYMSIZE;
        power = (power*PRIME)%SYMSIZE;
    }
    return hash; 
}

bool insert_sym_table(ASTNode* id, datatype t, symTable* tab, int isDecl){
    
    if(isLoopVar(id,tab)){
        printf("Line %d: ERROR: %s is a For Loop Variable. Can't be redeclared.\n",id->lineno,id->lexeme);
        return false; 
    }
    int hash = get_sym_hash(id -> lexeme);
    symEntry* curr = tab->table[hash];
    symEntry* prev = NULL;
    while(curr != NULL){
        if(strcmp(id -> lexeme,curr -> id) == 0){
            printf("Line %d: ERROR: Variable %s already declared at line %d.\n",id->lineno, id-> lexeme, curr-> lineno);
            return false;
        }
        prev = curr;
        curr = curr -> next;
    }
    curr = calloc(1,sizeof(symEntry));
    if(prev != NULL) prev -> next = curr;
    else tab->table[hash] = curr;
    curr -> id = id-> lexeme;
    curr -> lineno = id -> lineno;
    curr -> type = t;
    curr -> offset = id -> offset; 
    if(t.b == INT)
        curr -> width = INTW; 
    else if(t.b == RL)
        curr -> width = RLW;
    else if(t.b == BOOL)
        curr -> width = BOOLW;
    else if(t.b == ARR)
        curr -> width = ARRW;
    id -> width = curr -> width;  
    //To check if low and high bounds of array declared, of type int, and defined
    if(isDecl){
        if(t.b == ARR){
            bool validLow = 1;
            bool validHigh = 1;
            if(t.isLowStatic==0){ 
                ASTNode* low = t.low.dyn;
                low->scope = lookup_var(low, id->scope);
                if(low->scope == NULL){
                    printf("Line %d: ERROR: Lower bound for array %s not declared.\n",id->lineno, id-> lexeme);
                    validLow = 0;
                }
                if(validLow==1){
                    symEntry* lower = lookup_entry(low, low->scope);
                    if(lower->type.b != INT){
                        printf("Line %d: ERROR: Type of lower array bound %s must be NUM.\n",id->lineno, low-> lexeme);
                        validLow = 0;
                    }
                    else if(lower->assigned == 0){
                        printf("Line %d: ERROR: Lower array bound %s must be defined.\n",id->lineno, low->lexeme);
                        validLow = 0;
                    }
                }
            }
            if(t.isHighStatic==0){
                ASTNode* high = t.high.dyn;
                high->scope = lookup_var(high, id->scope);
                if(high->scope == NULL){
                    printf("Line %d: ERROR: Upper bound for array %s not declared.\n",id->lineno, id-> lexeme);
                    validHigh = 0;
                }
                if(validHigh==1){
                    symEntry* higher = lookup_entry(high, high->scope);
                    if(higher->type.b != INT){
                        printf("Line %d: ERROR: Type of upper array bound %s must be NUM.\n",id->lineno, high->lexeme);
                        validHigh = 0;
                    }
                    else if(higher->assigned == 0){
                        printf("Line %d: ERROR: Upper array bound %s must be defined.\n",id->lineno, high->lexeme);
                        validHigh = 0;
                    }
                }
            }
            return validHigh && validLow;
        }   
    }
        return true;
    
}

void print_sym_table(symTable* st){
    for(int i=0; i < SYMSIZE; i++){
        symEntry* e = st -> table[0]; 
        if(e != NULL){
            printf("%d -> ",i);
            while(e != NULL){
                printf("%s, ", e -> id); 
            }
            printf("\n");
        }
    }
}

baseEntry** make_base_table(){
    baseEntry** fn_table = (baseEntry**)calloc(SYMSIZE,sizeof(baseEntry*));
    return fn_table;
}

datatype get_type(ASTNode* type){ // here type should be datatype of type
    datatype *return_type = (datatype*)calloc(1,sizeof(datatype)); 
    type = type -> childL;  // type was initially datatype and type, my child is INT, RNUM, ARRAY, etc. 
    if(type -> isLeaf && type -> s.t == INTEGER){
        return_type -> b = INT;         
    }
    else if(type -> isLeaf && type -> s.t == REAL){
        return_type -> b = RL; 
    }
    else if(type -> isLeaf && type -> s.t == BOOLEAN){
        return_type -> b = BOOL; 
    }
    else{
        return_type -> b = ARR;
        ASTNode* index = type -> childL;
        // now at index1 
        bool flag = 0;
        if(index -> childL -> isLeaf && index -> childL -> s.t == NUM){
            return_type -> isLowStatic = true;
            return_type -> low.stat = index -> childL -> val.num;
        }
        else{
            return_type -> isLowStatic = false;
            return_type -> low.dyn = index -> childL; 
        }
        index = index -> next; 
        if(index -> childL -> isLeaf && index -> childL -> s.t == NUM){
            return_type -> isHighStatic = true;
            return_type -> high.stat = index -> childL -> val.num;
        }
        else{
            return_type -> isHighStatic = false;
            return_type -> high.dyn = index -> childL; 
        }
        ASTNode* btype = type -> next -> childL;  // type is at range_arrays
        switch(btype->s.t){
            case INTEGER:
                return_type->elem_type = INT;
                break;
            case REAL:
                return_type->elem_type = RL;
                break;
            case BOOLEAN:
                return_type->elem_type = BOOL;
                break;
        }
        // printf("Type %d\n",return_type -> elem_type);
    }
    return *return_type; 
}

param* get_param_list(ASTNode* ID, ASTNode* dtype, symTable* st){
    param* prev_p = NULL;
    param* list_start = NULL;
    // get input plist  
    while(dtype != NULL){
        param* p = (param*)calloc(1,sizeof(param)); 
        ID -> offset = ID -> parent -> offset; 
        ID -> scope = st; 
        dtype -> scope = st; 
        p -> id = ID -> lexeme; 
        // printf("%s ", p->id);
        p -> type = get_type(dtype);
        insert_sym_table(ID, p -> type, st, 0);
        ID -> parent -> offset = ID -> offset + ID -> width; 
        ID = dtype -> next; 
        dtype = ID -> next; 
        if(prev_p != NULL){
            prev_p -> next = p;
        }
        else{
            list_start = p; 
        }
        prev_p = p;   
        // Symtable stuff
        
    }
    return list_start; 
}

baseEntry* lookup_fn(char* id, baseEntry** table){
    int hash = get_sym_hash(id);
    baseEntry* curr = table[hash];
    while(curr != NULL){
        if(strcmp(id, curr->id) == 0) 
            return curr;
        curr = curr-> next;
    }
    return NULL;
}

bool insert_fn(ASTNode* fn,baseEntry** table){
    int hash;
    char* lexeme;
    baseEntry *curr,*prev;
    prev = NULL; 
    if(!fn->isLeaf && fn->s.nt==moduleDeclaration){
        lexeme = fn -> childL -> lexeme;
        hash = get_sym_hash( lexeme );
        curr = table[hash];
        prev = NULL;
        while(curr != NULL){
            if(strcmp(lexeme, curr-> id) == 0){
                if(curr->declared == true){
                    printf("Line %d: ERROR: Module %s redeclared, previously declared at line %d\n", fn -> childL -> lineno, curr -> id, curr -> lineno);
                    return false;
                }
                if(curr->defined == true){
                    printf("Line %d: ERROR: Module %s already defined at line %d\n", fn -> childL -> lineno, curr -> id, curr -> lineno);
                    return false;
                }
            }
            prev = curr;
            curr = curr -> next;
        }
        curr = calloc(1,sizeof(baseEntry));
        if(prev != NULL) prev -> next = curr;
        else{ table[hash] = curr;
            //printf("Bhavesh Bhavesh\n");
        }
        curr -> declared = true;
        curr -> id = lexeme;
        curr -> lineno = fn -> childL -> lineno;
    }
    else if(!fn->isLeaf && fn->s.nt==module){
        lexeme = fn -> childL -> lexeme;
        hash = get_sym_hash( lexeme );
        curr = table[hash];

        while(curr != NULL){
            if(strcmp(lexeme, curr -> id) == 0){
                if(curr -> defined == true){
                    printf("Line %d: ERROR: Module %s redefined, previously declared at line %d\n", fn -> childL -> lineno,curr -> id, curr -> lineno); 
                    return false;
                }
                else if(curr -> declared == 1){
                    break;
                }
            }
            prev = curr; 
            curr = curr -> next; 
        }
        if(curr == NULL){
            curr = (baseEntry*)calloc(1,sizeof(baseEntry)); 
            if(prev != NULL){
                prev -> next = curr; 
            }
            else{
                table[hash] = curr;
            }
        }
        // defined here
        curr -> defined = 1;  
        curr -> id = lexeme;        
        ASTNode* input_plist = fn -> childL -> next; 
        input_plist -> offset = fn -> offset; 
        ASTNode* ID = input_plist -> childL; 
        ID -> offset = input_plist -> offset; 
        ASTNode* dtype =  ID -> next; 
        dtype -> offset = input_plist -> offset; 
        // ID 
        
        // get output plist
        ASTNode* ret = input_plist -> next; 
         
        ASTNode* ID2 = ret -> childL; 
        ASTNode* type = ID2 -> next; 
        curr -> st = (symTable*)calloc(1,sizeof(symTable));
        curr -> st -> moduleName = curr -> id;
        curr -> st -> nestingLevel = 0;
        symTable* st2 = (symTable*)calloc(1,sizeof(symTable));
        st2 -> parent = curr -> st;
        st2 -> moduleName = st2 -> parent -> moduleName;
        st2 -> nestingLevel = 1;
        input_plist -> scope = curr ->st; 
        ret -> scope = st2;

        // printf("%s \n InputPlist: ", curr -> id); 
        curr -> input_plist = get_param_list(ID, dtype, curr -> st);
        input_plist -> parent -> offset = input_plist -> offset; 
        // printf("\nOutputPlist: ");
        ret -> offset = input_plist -> parent -> offset;
        curr -> output_plist = get_param_list(ID2, type, st2); 
        // printf("\n");
        ret -> parent -> offset = ret -> offset; 

        curr -> lineno = fn -> childL -> lineno;
        
        ASTNode* moduledef = ret -> next;
        moduledef -> offset = ret -> offset;
        moduledef -> scope = ret -> scope;
    }
    else if(!fn->isLeaf && fn->s.nt==driverModule){
        lexeme = "driver";
        hash = get_sym_hash(lexeme);
        curr = table[hash];
        prev = NULL;
        while(curr != NULL){
            prev = curr;
            curr = curr -> next;
        }
        curr = calloc(1,sizeof(baseEntry));
        if(prev != NULL) prev -> next = curr;
        else table[hash] = curr;
        curr -> defined = true;
        curr -> id = lexeme;
        fn -> scope = (symTable*)calloc(1,sizeof(symTable));
        fn -> scope -> moduleName = curr -> id;
        fn -> scope -> nestingLevel = 1;
        ASTNode* moduledef = fn -> childL; 
        moduledef -> scope = fn -> scope; 
    }
    return true;
}

bool insert_vars(ASTNode* idList, symTable* table){   //Called for idlist, datatype
    datatype t = get_type( idList -> next);
    // printf("%d %d -->  ", t.b, t.elem_type);
    idList -> next -> scope = table; 
    ASTNode* curr = idList -> childL;
    bool valid = true;
    while(!(curr -> isLeaf && curr -> s.t == EPS)){
        curr -> scope = table;
        curr -> offset = idList -> offset; 
        valid = insert_sym_table(curr, t, table, 1) && valid;
        idList -> offset = curr-> offset + curr -> width; 
        curr = curr->next;
    }
    return valid; 
}


symTable* lookup_var(ASTNode* id, symTable* tab){
    //Kaunse scope me mila variable
    symEntry* entry = lookup_entry(id,tab);
    if(entry != NULL){
        // id -> scope = tab; 
        return tab;
    }
    if(tab-> parent != NULL) 
        return lookup_var(id,tab->parent);
    else 
        return NULL;
}

symEntry* lookup_entry(ASTNode* id, symTable* tab){
    //Kaunse scope me mila variable
    int hash = get_sym_hash(id -> lexeme);
    symEntry* curr = tab->table[hash];
    while(curr != NULL){
        if(strcmp(id -> lexeme,curr -> id) == 0){
            return curr;
        }
        curr = curr -> next;
    }
    return NULL; 
}

void print_basetable(baseEntry** table){
    
    for(int i=0; i < SYMSIZE; i++){
        baseEntry* t = table[i];
        bool flg = 0; 
        if(t != NULL){
            printf("%d -> ", i);
            flg = 1; 
        }
        while(t != NULL){
            printf("%s ,", t -> id);
            t = t -> next; 
        } 
        if(flg)
        printf("\n"); 
    }

}

