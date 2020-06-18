//  Group 47
//  2016B3A70562P   Akash Kabra
//  2016B3A70523P   Ayush Mungad
//  2016B4A70520P   Kumar Anant Raj
//  2016B4A70533P   Mustansir Mama
//  2016B5A70715P   Bhavesh Chand

#include "parserDef.h"
#include "lexer.h"
#include "mapping.h"
#include <stdbool.h>

ULL one = 1;
bool correct;
//  Function to compute First and Follow Sets. Uses functions defind in mapping.c. Used by driver
void ComputeFirstAndFollowSets(Grammar** G, FirstAndFollow* F, grammar_index** idx_mapping, grammar_index** idx_mapping_follow){
    for(int i=0; i < NUM_NON_TERMINALS; i++){
        F[i].first = 0; 
        F[i].follow = 0; 
    }
    for(int i=0; i < NUM_NON_TERMINALS; i++){
        if(F[i].first == 0){
            //printf("here");
            symbol s; 
            s.nt = (non_terminal)i; 
            computefirst(G,F, idx_mapping, s, false); 
        }
    }

    F[program].follow = ((unsigned long long)1) << DOLLAR; 
    for(int i=0; i < NUM_NON_TERMINALS; i++){
        if(F[i].follow == 0){
            //printf("here");
            symbol s; 
            s.nt = (non_terminal)i; 
            computefollow(G,F, idx_mapping_follow, s); 
        }
    }
}

//  Init. & populate Parse Table. Used by Driver
int** createParseTable(Grammar** G,FirstAndFollow* F){
    int** ptab = malloc(NUM_NON_TERMINALS*sizeof(int*)); 
    for(int i=0; i < NUM_NON_TERMINALS; i++){
        ptab[i] = malloc(NUM_TERMINALS*(sizeof(int)));

    }
    
    for(int i=0;i<NUM_NON_TERMINALS;i++)
        for(int j=0;j<NUM_TERMINALS;j++)
            ptab[i][j]=ERROR;
    
    for(int i=0;i<NUM_RULES;i++){
        Grammar* curr = G[i];
        non_terminal lhs = curr->s.nt;
        curr = curr->next;
        ULL first=0;
        while(curr!=NULL){
            symbol s=curr->s;
            if(curr->isterminal){
                first |= one<<(s.t);
                if(s.t!=EPS) first &= ~(one<<EPS);
                break;
            }
            else{
                first |= F[s.nt].first;
                if((F[s.nt].first & (one<<EPS))==0){
                    first &= ~(one<<EPS);
                    break;
                }
            }
            curr=curr->next;
        }
        if((first & (one<<EPS))!=0){
            first |= F[lhs].follow;
            first &= ~(one<<EPS);
        }

        for(int j=0;j<64;j++){
            int set = first%2;
            if(set) ptab[lhs][j]=i;
            first=first>>1;
        }
    }

    //FILL SYN for Panic Mode
    // for(int i=0;i<NUM_NON_TERMINALS;i++){
    //     ULL follow= F[i].follow;
    //     for(int j=0;j<64;j++){
    //         int set = follow%2;
    //         if(set && ptab[i][j]<0) ptab[i][j]=SYN;
    //         follow=follow>>1;
    //     }
    // }

    return ptab;
}

//  Initialize empty Parse Tree Node
ptnode* init_ptnode(){
    ptnode* p=calloc(1,sizeof(ptnode));
    p->child=NULL;
    p->sibling=NULL;
    return p;
}

//  Initialize Parse Tree with Start Symbol node
ptnode* init_pt(){
    ptnode *p=init_ptnode();
    p->isLeaf=false;
    p->nodeSymbol=non_terminal_list[program];
    p->parentNodeSymbol = ""; 
    return p;
}

//  Initialize Stack with DOLLAR and Start Symbol
stack_str* init_stack(){
    stack_str* st=create_stack();
    stack_entry start;
    start.ist=true;
    start.s.t=DOLLAR;
    start.ptr = NULL;
    push(st,start);

    start.ist=false;
    start.s.nt=program;
    start.ptr=init_pt();
    push(st,start);
    return st;
}

extern FILE* infile;

//  Fill Relevant info. in Parse Tree Node. Called after node is popped from stack, beacuse only then is this info. available.
ptnode* populatePTNode(Grammar* G,ptnode* par_node){
    ptnode* node=calloc(1,sizeof(ptnode));
    node->s = G->s;
    node->isLeaf = G->isterminal;
    node->parentNodeSymbol = par_node->nodeSymbol;
    if(node->isLeaf){
        node->tokenName = terminal_list[G->s.t];
    }
    else{
        node->nodeSymbol = non_terminal_list[G->s.nt];
    }
    return node;
}

terminal is_in_synset(terminal t){
    switch(t){
        /*case(SEMICOL):
            return SEMICOL;
        case(END):
            return END;
        case(ENDDEF):
            return ENDDEF;
        case(DRIVERENDDEF):
            return DRIVERENDDEF;
        case(BC):
            return BC;*/
        default:
            return -1; 
    }    
}

// Parser. Returns Root of constructed Parse Tree 
ptnode* parseInputSourceCode(int** ptab,Grammar** G,FILE* in, FirstAndFollow* F,keymap_node** key_hash){
    stack_str* st=init_stack();
    stack_str* rev=create_stack();
    
    ptnode* pthead=peek(st).ptr;
    infile=in;
    int ruleno;
    tokenInfo* tok;
    tok = getNextToken(key_hash);
    bool print_once = 0; 
    correct = true;

    while(1){
        //print_stack(st); 
        if(tok->t==LEXERR){ // Lexer returns error.
            tok = getNextToken(key_hash);
            correct = false;
            continue;
        }
        else if(tok ->t == DOLLAR){ // Lexer returns DOLLAR on end of source file.
            stack_entry s_top = peek(st); 
            if(s_top.ist && s_top.s.t==DOLLAR){ // If matching Dollar on Stack
                if(correct) printf("************Code syntactically correct***************\n");
                return pthead;
            }
            else if(s_top.ist == false && (F[s_top.s.nt].follow & (one << DOLLAR))) //If Stack top has a non-terminal whose follow contains DOLLAR
            {   
                // do nothing
                // Will be handled normally below.
            }
            else{   //If stack top can neither be matched nor be expanded to EPS
                if(print_once  == 0){
                    if(peek(st).ist){
                        printf("\nActual top of Stack is %s\n", terminal_list[peek(st).s.t]);
                    }
                    else{
                        printf("\nActual top of Stack is %s\n", non_terminal_list[peek(st).s.nt]);
                    }
                    printf("Syntactic error at line %d\nCode ended prematurely\n",tok->line);
                    print_once = 1;
                    correct = false;
                }
            } 
        }
        if(isEmpty(st)){    //DOLLAR popped before code completion
            printf("\nSyntactic Error: Stack Underflow\n");
            return pthead;
        }
    
        stack_entry top=peek(st);
        if(top.ist){    //If stack top  is a Terminal

            if(top.s.t==tok->t){    //If this terminal matches with next token
                ptnode* node = top.ptr;
                node->lexeme = tok->lexeme;
                node->lineno = tok->line;
                node -> ruleNum = 0; 
                if(node->isLeaf && node->s.t==NUM) node->val.num=atoi(tok->lexeme);
                if(node->isLeaf && node->s.t==RNUM) node->val.rnum=atof(tok->lexeme);
                pop(st);
                tok = getNextToken(key_hash);
            }
            else{   //Does not match
                printf("\nSyntactic error at line %d: %s\n",tok->line,tok->lexeme);
                printf("Actual top of Stack is %s\n",terminal_list[top.s.t]);
                correct = false;

                //Error recovery
                bool follow_tag = 0;
                // Pop the Top of Stack in this case
                pop(st);
            }
        }
        else{   // If stack top is a non-terminal
            ruleno=ptab[top.s.nt][tok->t];
            if(ruleno>=0){  //Rule exists in Parse Table
                //printf("%d\n", ruleno);
                ptnode* curr_node,*par_node;
                par_node = top.ptr;
                pop(st);
                par_node -> ruleNum = ruleno + 1; 
                //print_stack(st);
                Grammar* curr = G[ruleno];
                non_terminal lhs = curr->s.nt;
                curr = curr->next;

                if(!(curr->isterminal && curr -> s.t==EPS)) {
                    curr_node = populatePTNode(curr,par_node);
                    par_node -> child = curr_node; 
                    par_node -> isEps = (curr->isterminal && curr->s.t==EPS);
                }
                else{   // this is done so that eps is not pushed in the stack and it's ptnode is made.
                    // it is eps 
                    //pop(st);
                    curr_node = populatePTNode(curr,par_node);
                    par_node -> child = curr_node; 
                    par_node -> isEps = (curr->isterminal && curr->s.t==EPS);
                    curr = curr -> next; 
                }
                ptnode* prev_node = curr_node;
                while(curr!=NULL){  //  Create child nodes in Parse Tree for all RHS terms of Grammar Rule
                    stack_entry t;
                    t.ist=curr->isterminal;
                    t.s=curr->s;
                    t.ptr = curr_node;
                    curr=curr->next;
                    push(rev,t);
                    if(curr != NULL){
                        curr_node = populatePTNode(curr,par_node);
                        prev_node -> sibling = curr_node; 
                        prev_node = curr_node;
                    }
                }
                while(!isEmpty(rev)){   //Required because push in stack needs to be in reverse order 
                    push(st,peek(rev));
                    pop(rev);
                }
            }
            else{   //No matching rule in Parse Table
                correct=false;
                printf("\nSyntactic error at line %d: %s\n",tok->line,tok->lexeme);
                // printf("Unexpected token %s\n",terminal_list[tok->t]);
                printf("Actual top of Stack is %s\n", non_terminal_list[top.s.nt]);

                //Error recovery
                bool follow_tag = 0; 
                bool temp=true;
                while(is_in_synset(tok->t) == -1){
                    while(tok -> t == LEXERR){
                        tok = getNextToken(key_hash);
                    }
                    if(!top.ist && (F[top.s.t].follow & (ULL)(one << tok->t))){ //handles LEXERR and follow =DOLLAR
                        // dont get next token
                        follow_tag = 1; 
                        break; 
                    }
                    else{
                        // get next token
                        tok=getNextToken(key_hash);
                        if(tok -> t == DOLLAR){ 
                            follow_tag = 1; 
                            break;
                        }
                    }
                }

                terminal term = tok -> t; 
                if(follow_tag == 1){
                    //if(temp == true)
                        pop(st); 
                    follow_tag = 0; 
                }
                else{
                    while(!top.ist || top.s.t != term ){
                        pop(st);
                        top=peek(st);
                        if(isEmpty(st)){
                            printf("\nSyntactic Error: Stack Underflow\n");
                            return pthead;
                        }
                    }
                }
            // }
            // else if(ruleno==SYN){
            //     printf("Expected a %s\n",non_terminal_list[top.s.nt]);
            //     pop(st);
            // }
            }
        }
    }
    return pthead;
}

//  Print a Parse Tree node's info. with proper formatting
void printNode(ptnode* node, FILE* outfile){
    int p; 
    if(node->lexeme==NULL){
        p = fprintf(outfile,"----");
        for(int i=p+1; i < NUM_SPACES; i++){
            fprintf(outfile," ");
        }
    } 
    else{
        p = fprintf(outfile,"%s",node->lexeme);
        for(int i=p+1; i < NUM_SPACES; i++){
            fprintf(outfile," ");
        }
    }

    p = fprintf(outfile,"%d",node->lineno);
    for(int i=p+1; i < NUM_SPACES; i++){
            fprintf(outfile," ");
    }

    if(node->tokenName==NULL){
        p = fprintf(outfile,"----");
        for(int i=p+1; i < NUM_SPACES; i++){
            fprintf(outfile," ");
        }
    }
    else{
        p = fprintf(outfile,"%s",node->tokenName);
        for(int i=p+1; i < NUM_SPACES; i++){
            fprintf(outfile," ");
        }
    }

    if(node->isLeaf && node->s.t==NUM){
        p = fprintf(outfile,"%d",node->val.num);
        for(int i=p+1; i < NUM_SPACES; i++){
            fprintf(outfile," ");
        }
    }
    else if(node->isLeaf && node->s.t==RNUM){
        p = fprintf(outfile,"%f",node->val.rnum);
        for(int i=p+1; i < NUM_SPACES; i++){
            fprintf(outfile," ");
        }
    }
    else{
        p = fprintf(outfile,"----");
        for(int i=p+1; i < NUM_SPACES; i++){
            fprintf(outfile," ");
        }
    }

    if(node->parentNodeSymbol==""){
        p = fprintf(outfile,"----");
        for(int i=p+1; i < NUM_SPACES; i++){
            fprintf(outfile," ");
        }
    }
    else{
        p = fprintf(outfile,"%s",node->parentNodeSymbol);
        for(int i=p+1; i < NUM_SPACES; i++){
            fprintf(outfile," ");
        }
    }

    p = fprintf(outfile,"%d",node->isLeaf);
    for(int i=p+1; i < NUM_SPACES; i++){
            fprintf(outfile," ");
    }

    if(node->nodeSymbol==NULL){
        fprintf(outfile,"----");
        for(int i=p+1; i < NUM_SPACES; i++){
            fprintf(outfile," ");
        }
    }
    else{
        fprintf(outfile,"%s",node->nodeSymbol);
        for(int i=p+1; i < NUM_SPACES; i++){
            fprintf(outfile," ");
        }
    }
    // fprintf(outfile, "ruleNum = %d", node -> ruleNum);
    fprintf(outfile, "\n");

}

// In Order printing of Parse Tree
void printParseTree(ptnode* node, FILE *outfile){
    if(node==NULL) return;
    
    printParseTree(node->child,outfile);
    printNode(node,outfile);
    if(node-> child!= NULL){
        ptnode* tmp = node -> child -> sibling; 
        while(tmp!= NULL){
            printParseTree(tmp,outfile);
            tmp = tmp -> sibling;
        }
    }
    
}

// Pre Order Printing of Parse Tree. Used for testing puposes only. 
void printParseTreePreOrder(ptnode* node, FILE *outfile){ 
    if(node==NULL) return;
    // if(node -> isLeaf == true)
        printNode(node,outfile);
    printParseTreePreOrder(node->child,outfile);
    printParseTreePreOrder(node->sibling,outfile);
}

// Used for testing puposes only.
void PrintParseTable(int** ptab){

    for(int i=0; i < NUM_NON_TERMINALS; i++){
        printf("%s : ", non_terminal_list[i]);
        for(int j= 0; j < NUM_TERMINALS; j++){
            if(ptab[i][j] != -1){

                printf("(%s, %d) ", terminal_list[j], ptab[i][j]); 
            }
        }
        printf("\n");
    }
}

/*int main(int argc, char* argv[]){
    if(argc!=3){
        printf("Enter source code file and output file.");
        return -1;
    }
    FILE* fp1=fopen(argv[1],"r"); 
    FILE* fp2=fopen(argv[2],"w");
    infile = fp1;
    FILE* parsetree = fp2;

    mapping_table* map_table = populate_mapping_table();
    map_node** mapping_hash_table = populate_hash_table();
    Grammar** grammar = make_grammar();
    insert_rule(grammar, mapping_hash_table, map_table);
    //print_grammar(grammar);

    FirstAndFollow* F = initialize_first_follow(); 
    grammar_index** idx_mapping = get_grammar_idx(grammar); 
    
    grammar_index** idx_mapping_follow = get_grammar_follow_idx(grammar); 
    //print_index_mapping(idx_mapping_follow);
    ComputeFirstAndFollowSets(grammar, F, idx_mapping, idx_mapping_follow);
    //printFirstSet(F); 
    //printFollowSet(F);

    int** ptab = createParseTable(grammar,F);

    //PrintParseTable(ptab);

    ptnode* pt= parseInputSourceCode(ptab,grammar,infile, F);
    printParseTree(pt,parsetree);
}*/