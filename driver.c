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
#include "parserDef.h"
#include "parser.h"
#include "codegen.h"

int action[NUM_RULES][MAX_RHS + 1]; 

void get_action_array(){
    FILE* fp = fopen("action.txt" , "r"); 
    //int action[NUM_RULES][MAX_RHS]; 
    int n; 
    int i = 0; 
    int j = 0; 
    char str[120];
    while(fscanf(fp, "%s\n", str) != EOF){
        if(strcmp(str,"\n") == 0){
            continue; 
        }
        action[i/(MAX_RHS+1)][i%(MAX_RHS+1)] = atoi(str); 
        i++; 
    }
    return;
}

int countParseTreeNodes(ptnode* node){
    if(node==NULL) 
        return 0;
    return countParseTreeNodes(node->child) + countParseTreeNodes(node->sibling) + 1;
}

int countASTNodes(ASTNode* node){
    if(node==NULL) 
        return 0;
    return countASTNodes(node->childL) + countASTNodes(node->next) + 1;
}

void printSymbolTable(symTable* st, FILE *outfile, bool inpList, bool arrays){
    if(st->printed)
        return;
    for(int i=0; i < SYMSIZE; i++){
        symEntry* e = st -> table[i];
        int p; 
        if(e != NULL && (!(e->id[0]=='_' && e->id[1]=='_')||arrays==0)){
            if(arrays==false && e->type.b == BOOL){
                p = fprintf(outfile, "%s", e -> id);
                for(int i=p+1; i < NUM_SPACES; i++){
                    fprintf(outfile," ");
                }
                p = fprintf(outfile, "%s", st->moduleName);
                for(int i=p+1; i < NUM_SPACES; i++){
                    fprintf(outfile," ");
                }
                p = fprintf(outfile, "%d", 1);
                for(int i=p+1; i < NUM_SPACES; i++){
                    fprintf(outfile," ");
                }
                p = fprintf(outfile, "no");
                for(int i=p+1; i < NUM_SPACES; i++){
                    fprintf(outfile," ");
                }
                p = fprintf(outfile, "---");
                for(int i=p+1; i < NUM_SPACES; i++){
                    fprintf(outfile," ");
                }
                p = fprintf(outfile, "---");
                for(int i=p+1; i < NUM_SPACES; i++){
                    fprintf(outfile," ");
                }
                p = fprintf(outfile, "boolean");
                for(int i=p+1; i < NUM_SPACES; i++){
                    fprintf(outfile," ");
                }
                p = fprintf(outfile, "%d", e->offset);
                for(int i=p+1; i < NUM_SPACES; i++){
                    fprintf(outfile," ");
                }
                p = fprintf(outfile, "%d", st->nestingLevel);
                fprintf(outfile, "\n"); 
            }
            else if(arrays==false && e->type.b == INT){
                p = fprintf(outfile, "%s", e -> id);
                for(int i=p+1; i < NUM_SPACES; i++){
                    fprintf(outfile," ");
                }
                p = fprintf(outfile, "%s", st->moduleName);
                for(int i=p+1; i < NUM_SPACES; i++){
                    fprintf(outfile," ");
                }
                p = fprintf(outfile, "%d", 2);
                for(int i=p+1; i < NUM_SPACES; i++){
                    fprintf(outfile," ");
                }
                p = fprintf(outfile, "no");
                for(int i=p+1; i < NUM_SPACES; i++){
                    fprintf(outfile," ");
                }
                p = fprintf(outfile, "---");
                for(int i=p+1; i < NUM_SPACES; i++){
                    fprintf(outfile," ");
                }
                p = fprintf(outfile, "---");
                for(int i=p+1; i < NUM_SPACES; i++){
                    fprintf(outfile," ");
                }
                p = fprintf(outfile, "integer");
                for(int i=p+1; i < NUM_SPACES; i++){
                    fprintf(outfile," ");
                }
                p = fprintf(outfile, "%d", e->offset);
                for(int i=p+1; i < NUM_SPACES; i++){
                    fprintf(outfile," ");
                }
                p = fprintf(outfile, "%d", st->nestingLevel);
                fprintf(outfile, "\n"); 
            }
            else if(arrays==false && e->type.b == RL){
                p = fprintf(outfile, "%s", e -> id);
                for(int i=p+1; i < NUM_SPACES; i++){
                    fprintf(outfile," ");
                }
                p = fprintf(outfile, "%s", st->moduleName);
                for(int i=p+1; i < NUM_SPACES; i++){
                    fprintf(outfile," ");
                }
                p = fprintf(outfile, "%d", 4);
                for(int i=p+1; i < NUM_SPACES; i++){
                    fprintf(outfile," ");
                }
                p = fprintf(outfile, "no");
                for(int i=p+1; i < NUM_SPACES; i++){
                    fprintf(outfile," ");
                }
                p = fprintf(outfile, "---");
                for(int i=p+1; i < NUM_SPACES; i++){
                    fprintf(outfile," ");
                }
                p = fprintf(outfile, "---");
                for(int i=p+1; i < NUM_SPACES; i++){
                    fprintf(outfile," ");
                }
                p = fprintf(outfile, "real");
                for(int i=p+1; i < NUM_SPACES; i++){
                    fprintf(outfile," ");
                }
                p = fprintf(outfile, "%d", e->offset);
                for(int i=p+1; i < NUM_SPACES; i++){
                    fprintf(outfile," ");
                }
                p = fprintf(outfile, "%d", st->nestingLevel);
                fprintf(outfile, "\n"); 
            }
            else if(e->type.b == ARR){
                if(arrays==0){
                    p = fprintf(outfile, "%s", e -> id);
                    for(int i=p+1; i < NUM_SPACES; i++){
                        fprintf(outfile," ");
                    }
                }
                p = fprintf(outfile, "%s", st->moduleName);
                for(int i=p+1; i < NUM_SPACES; i++){
                    fprintf(outfile," ");
                }
                if(arrays==1){
                    p = fprintf(outfile, "%s", e -> id);
                    for(int i=p+1; i < NUM_SPACES; i++){
                        fprintf(outfile," ");
                    }
                }
                int el;
                if(e->type.elem_type == INT)
                    el = 2;
                else if(e->type.elem_type == RL)
                    el = 4;
                else if(e->type.elem_type == BOOL)
                    el = 4;
                if(arrays==0){
                    if(inpList==1)
                        p = fprintf(outfile, "%d", 5);
                    else{
                        if(e->type.isHighStatic && e->type.isLowStatic)
                            p = fprintf(outfile, "%d", (e->type.high.stat-e->type.low.stat+1)*el+1);
                        else
                            p = fprintf(outfile, "%d", 1);
                    }
                    for(int i=p+1; i < NUM_SPACES; i++){
                        fprintf(outfile," ");
                    }
                    p = fprintf(outfile, "yes");
                    for(int i=p+1; i < NUM_SPACES; i++){
                        fprintf(outfile," ");
                    }
                }
                if(e->type.isHighStatic && e->type.isLowStatic)
                    p = fprintf(outfile, "static");
                else
                    p = fprintf(outfile, "dynamic");
                for(int i=p+1; i < NUM_SPACES; i++){
                    fprintf(outfile," ");
                }
                if(e->type.isLowStatic)
                    p = fprintf(outfile, "[%d, ", e->type.low.stat);
                else
                    p = fprintf(outfile, "[%s, ", e->type.low.dyn->lexeme);
                if(e->type.isHighStatic)
                    p += fprintf(outfile, "%d]", e->type.high.stat);
                else
                    p += fprintf(outfile, "%s]", e->type.high.dyn->lexeme);      
                for(int i=p+1; i < NUM_SPACES; i++){
                    fprintf(outfile," ");
                }
                if(el==1)
                    p = fprintf(outfile, "boolean");
                else if(el==2)
                    p = fprintf(outfile, "integer");
                else
                    p = fprintf(outfile, "real");
                if(arrays==0){
                    for(int i=p+1; i < NUM_SPACES; i++){
                        fprintf(outfile," ");
                    }
                    p = fprintf(outfile, "%d", e->offset);
                    for(int i=p+1; i < NUM_SPACES; i++){
                        fprintf(outfile," ");
                    }
                    p = fprintf(outfile, "%d", st->nestingLevel);
                }
                fprintf(outfile, "\n"); 
            }
        }
        st->printed = 1;
    }   
}


void printModuleSymbolTable(ASTNode* node, FILE* outfile, bool arrays){
    if(node==NULL)
        return;
    if(node->isLeaf && node->s.t==ID){
        if(node->scope!=NULL)
            printSymbolTable(node->scope, outfile, 0, arrays);
    }
    printModuleSymbolTable(node -> childL, outfile, arrays);
    printModuleSymbolTable(node -> next, outfile, arrays);
}


void printSymbolTables(ASTNode* head, FILE *outfile){ 
    ASTNode* M = head -> childL; 
    ASTNode* O1 = M -> next; 
    ASTNode* D = O1 -> next; 
    ASTNode* O2 = D -> next; 
    ASTNode* curr = O1->childL;
    while(curr->isLeaf==0){
        ASTNode* input_plist = curr->childL->next;
        ASTNode* output_plist = curr->childL->next->next;
        printSymbolTable(input_plist->scope, outfile, 1, 0);
        printSymbolTable(output_plist->scope, outfile, 0, 0);
        ASTNode* moduleDef = curr->childL->next->next->next;
        printModuleSymbolTable(moduleDef, outfile, 0);
        curr = curr->next;
    }
    printModuleSymbolTable(D, outfile, 0);
    curr = O2 -> childL;
    while(curr->isLeaf==0){
        ASTNode* input_plist = curr->childL->next;
        ASTNode* output_plist = curr->childL->next->next;
        printSymbolTable(input_plist->scope, outfile, 1, 0);
        printSymbolTable(output_plist->scope, outfile, 0, 0);
        ASTNode* moduleDef = curr->childL->next->next->next;
        printModuleSymbolTable(moduleDef, outfile, 0);
        curr = curr->next;
    }
}

void printARSize(ASTNode* head, FILE *outfile){
    ASTNode* M = head -> childL; 
    ASTNode* O1 = M -> next; 
    ASTNode* D = O1 -> next; 
    ASTNode* O2 = D -> next;
    ASTNode* curr = O1->childL; 
    while(curr->isLeaf==0){
        fprintf(outfile, "%s %d\n", curr->childL->lexeme, curr->offset);
        curr = curr->next;
    }
    fprintf(outfile, "driver %d\n", D->offset);
    curr = O2->childL;
    while(curr->isLeaf==0){
        fprintf(outfile, "%s %d\n", curr->childL->lexeme, curr->offset);
        curr = curr->next;
    }
}

void printArrays(ASTNode* head, FILE *outfile){
    ASTNode* M = head -> childL; 
    ASTNode* O1 = M -> next; 
    ASTNode* D = O1 -> next; 
    ASTNode* O2 = D -> next; 
    ASTNode* curr = O1->childL;
    while(curr->isLeaf==0){
        ASTNode* input_plist = curr->childL->next;
        ASTNode* output_plist = curr->childL->next->next;
        printSymbolTable(input_plist->scope, outfile, 1, 1);
        printSymbolTable(output_plist->scope, outfile, 0, 1);
        ASTNode* moduleDef = curr->childL->next->next->next;
        printModuleSymbolTable(moduleDef, outfile, 1);
        curr = curr->next;
    }
    printModuleSymbolTable(D, outfile, 1);
    curr = O2 -> childL;
    while(curr->isLeaf==0){
        ASTNode* input_plist = curr->childL->next;
        ASTNode* output_plist = curr->childL->next->next;
        printSymbolTable(input_plist->scope, outfile, 1, 1);
        printSymbolTable(output_plist->scope, outfile, 0, 1);
        ASTNode* moduleDef = curr->childL->next->next->next;
        printModuleSymbolTable(moduleDef, outfile, 1);
        curr = curr->next;
    }
}

int main(int argc, char* argv[]){
    
    if(argc!=3){
        printf("Enter source code file and output file.\n");
        return -1;
    }
    // printf("Status of Our Implementation:\n"); 
    // printf("(a) FIRST and FOLLOW set automated\n");
    // printf("(c) Both lexical and syntax analysis modules implemented\n");
    FILE* fp1=fopen(argv[1],"r"); 
    if(fp1==NULL) {
        printf("Can't open %s\n",argv[1]);
        return 0; 
    }
 
    infile = fp1;
    FILE* fp2,*parsetree;
    keymap_node** key_hash = populate_keyword_hash_table();
    mapping_table* map_table = populate_mapping_table();
    map_node** mapping_hash_table = populate_hash_table();
    Grammar** grammar = make_grammar();
    insert_rule(grammar, mapping_hash_table, map_table);
    FirstAndFollow* F = initialize_first_follow(); 
    grammar_index** idx_mapping = get_grammar_idx(grammar); 
    grammar_index** idx_mapping_follow = get_grammar_follow_idx(grammar); 
    ComputeFirstAndFollowSets(grammar, F, idx_mapping, idx_mapping_follow);
    int** ptab = createParseTable(grammar,F);
    int flag=1;
    get_action_array(); 
    char input = '#';
    while(1){
        
        
       	if(input!='\n'){
       			printf("Select option\n");
       	}
       	scanf("%c",&input);
        
        switch(input){
            case '0':
                flag = 0;
                break;
            case '1':
                reset_globals();
                lexer(key_hash); 
                break;
            case '2':
                reset_globals();
                ptnode* pt = parseInputSourceCode(ptab,grammar,infile, F,key_hash);
                if(correct==false){
                    printf("************Code syntactically incorrect***************\n");
                    continue;
                }
                printf("************Printing Parse Tree Pre-order***************\n");
                printParseTreePreOrder(pt,stdout);
                break;
            case '3':
                reset_globals();
                fp2=fopen(argv[2],"w");
                if(fp1==NULL) {
                    printf("File %s not found\n",argv[1]);
                    flag = 0; 
                    break;
                }
                pt = parseInputSourceCode(ptab,grammar,infile, F,key_hash);
                if(correct==false){
                    printf("************Code syntactically incorrect***************\n");
                    continue;
                }
				ASTNode* a = constructAST(pt, action, NULL, NULL);
                printf("************Printing AST Pre-order***************\n");
                printASTPreOrder(a, stdout);  
                //fileprint_program_code(a, parsetree);                
                break;
            case '4':
                reset_globals();
                pt = parseInputSourceCode(ptab,grammar,infile, F,key_hash);
                if(correct==false){
                    printf("************Code syntactically incorrect***************\n");
                    continue;
                }
                int numPTNodes = countParseTreeNodes(pt);
                a = constructAST(pt, action, NULL, NULL);
                int numASTNodes = countASTNodes(a);
                int PTSize = sizeof(ptnode)*numPTNodes;
                int ASTSize = sizeof(ASTNode)*numASTNodes;
                // printf("%d %d",sizeof(ptnode) , sizeof(ASTNode));
                printf("Parse tree Number of nodes %d\t", numPTNodes);
                printf("Allocated Memory %d\n", PTSize);
                printf("AST Number of nodes %d\t", numASTNodes);
                printf("Allocated Memory %d\n", ASTSize);
                double compression = (double)(PTSize-ASTSize)/(double)PTSize*100.0;
                printf("Compression = %.2f%%\n", compression);
                break;
            case '5':
                reset_globals();
                // fp2=fopen(argv[2],"w");
                // if(fp1==NULL) {
                //     printf("File %s not found\n",argv[1]);
                //     flag = 0; 
                //     break;
                // }
                pt = parseInputSourceCode(ptab,grammar,infile, F,key_hash);
                if(correct==false){
                    printf("************Code syntactically incorrect***************\n");
                    continue;
                }
                a = constructAST(pt, action, NULL, NULL);
                ASTPass(a);
                baseEntry** bt = make_base_table(); 
                bool valid = populate_symbol_table(a, bt);
                valid = second_pass(a, bt) && valid;
                if(valid==true){
                    printf("************Code semantically correct***************\n");
                    printf("************Printing Symbol Tables***************\n");
                }
                else{
                    printf("************Code semantically incorrect***************\n");
                    printf("************Printing Symbol Tables***************\n");
                }
                printSymbolTables(a, stdout);
                break;
            case '6':
                reset_globals();
                pt = parseInputSourceCode(ptab,grammar,infile, F,key_hash);
                if(correct==false){
                    printf("************Code syntactically incorrect***************\n");
                    continue;
                }
                a = constructAST(pt, action, NULL, NULL);
                ASTPass(a);
                bt = make_base_table(); 
                valid = populate_symbol_table(a, bt);
                valid = second_pass(a, bt) && valid;
                if(valid==true){
                    printf("************Code semantically correct***************\n");
                    printf("************Printing Sizes of Activation Records***************\n");
                }
                else{
                    printf("************Code semantically incorrect***************\n");
                    printf("************Printing Sizes of Activation Records***************\n");
                }
                printARSize(a, stdout);
                break;
            case '7':
                reset_globals();
                pt = parseInputSourceCode(ptab,grammar,infile, F,key_hash);
                if(correct==false){
                    printf("************Code syntactically incorrect***************\n");
                    continue;
                }
                a = constructAST(pt, action, NULL, NULL);
                ASTPass(a);
                bt = make_base_table(); 
                valid = populate_symbol_table(a, bt);
                valid = second_pass(a, bt) && valid;
                if(valid==true){
                    printf("************Code semantically correct***************\n");
                }
                else{
                    printf("************Code semantically incorrect***************\n");
                }
                printArrays(a, stdout);
                break;
            case '8':
                reset_globals();
                clock_t    start_time, end_time;
                double total_CPU_time, total_CPU_time_in_seconds;
                start_time = clock();
                pt = parseInputSourceCode(ptab,grammar,infile, F,key_hash);
                if(correct==false){
                    printf("************Code syntactically incorrect***************\n");
                    continue;
                }
                a = constructAST(pt, action, NULL, NULL);
                ASTPass(a);
                bt = make_base_table(); 
                valid = populate_symbol_table(a, bt);
                valid = second_pass(a, bt) && valid;
                if(valid==true){
                    printf("************Code semantically correct***************\n");
                }
                else{
                    printf("************Code semantically incorrect***************\n");
                }
                end_time = clock();
                total_CPU_time  =  (double) (end_time - start_time);
                total_CPU_time_in_seconds =   total_CPU_time / CLOCKS_PER_SEC;
                
                //Printing both total_CPU_time and total_CPU_time_in_seconds 
                printf("Total CPU time = %f ", total_CPU_time);
                printf("Total CPU time in seconds = %.10f\n", total_CPU_time_in_seconds);
                break;
            case '9':
                reset_globals();
                fp2=fopen(argv[2],"w");
                if(fp1==NULL) {
                    printf("File %s not found\n",argv[1]);
                    flag = 0; 
                    break;
                }
                printf("LEVEL 4\n");
                pt = parseInputSourceCode(ptab,grammar,infile, F,key_hash);
                if(correct==false){
                    printf("************Code syntactically incorrect***************\n");
                    continue;
                }

                a = constructAST(pt, action, NULL, NULL);
                ASTPass(a);
                bt = make_base_table(); 
                valid = 1; 
                valid = populate_symbol_table(a, bt);
                valid = second_pass(a, bt) && valid;
                if(valid==true){
                    printf("************Code semantically correct***************\n");
                    
                }
                else{
                    printf("************Code semantically incorrect***************\n");
                    continue; 
                }
                codegen_program(a, bt);
                printf("************Code compiles successfully***************\n");
                fileprint_program_code(a, fp2);  
                fclose(fp2);
                break;
            case '\n': 
            	break;

            default:
            		printf("Invalid option\n");
            		break;
        }
        if(flag==0){
            fclose(infile);
            break;
        }
    
    }
}