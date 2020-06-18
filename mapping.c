//  Group Number 47
//  2016B3A70562P   Akash Kabra
//  2016B3A70523P   Ayush Mungad
//  2016B4A70520P   Kumar Anant Raj
//  2016B4A70533P   Mustansir Mama
//  2016B5A70715P   Bhavesh Chand

#include <stdio.h>
#include "lexerDef.h"
#include "lexer.h" 
#include "mapping.h"

// This function allocates space for the hash table data structure
// This would be called directly by populate_hash_table() function to initialize hash table
map_node** make_hash_table(){
    map_node** hash_table = (map_node**)malloc(sizeof(map_node*) * NUM_SLOTS);
    return hash_table;
}

// This function returns the hash value for every input string
// It follows the hash function: summation over all characters' ascii value * prime^position. (Also Defined in preforma file) 
// Would be used for hashing mapping_table strings in this file and for hashing keywords in isKeyword.c file
int get_hash(char* str, int length){
    int hash = 0; 
    long long power = 1;
    for(int i= 0; i < length; i++){
        int tmp = str[i];
        hash = (hash + (((tmp)*(power))%NUM_SLOTS)%NUM_SLOTS)%NUM_SLOTS;
        power = (power*PRIME)%NUM_SLOTS;
    }
    return hash; 
}

// This function inserts a map node at a index in hash table
// This would be called directly by populate_hash_table() function to insert a single map_node in hash table for mapping_table
void insert_hash(int hash_index, int mapping_pos, map_node** hash_table)
{
    map_node* tmp = hash_table[hash_index]; 
    map_node* new_node = (map_node*)malloc(sizeof(map_node)); 
    new_node-> mapping_position = mapping_pos;  
    hash_table[hash_index] = new_node; 
    new_node -> next = tmp;
}

// This is an auxiliary function for printing the entire hash table for debugging purposes
void print_hash(map_node** hash){
    for(int i=0; i < NUM_SLOTS; i++){
        if(hash[i] != NULL)
            printf("%d --> ", i);
        map_node* tmp = hash[i]; 
        while(tmp!= NULL){
            printf("%d ", tmp -> mapping_position); 
            tmp = tmp -> next; 
        }
        if(hash[i] != NULL)
        printf("\n");
    }
}


// This function populates mapping table with string vs symbol pairs with a corresponding tag 
// This would be called directly by the driver.c before reading the grammar from input file
mapping_table* populate_mapping_table(){
    mapping_table* map_table = (mapping_table*)malloc(sizeof(mapping_table)*1000);
    symbol sym[NUM_TERMINALS + NUM_NON_TERMINALS]; 
    for(int i=0; i < NUM_TERMINALS; i++){
        sym[i].t = (terminal)i;
        map_table[i].s = sym[i];
        for(int j=0; j < 25; j++){
            map_table[i].str[j] = terminal_list[i][j]; 
        }
        map_table[i].isterminal = true; 
    }
    
    // SAME CODE FOR NON TERMINALS
    for(int i=0; i < NUM_NON_TERMINALS; i++){
        sym[i+NUM_TERMINALS].nt = (non_terminal)i;
        map_table[i+NUM_TERMINALS].s = sym[i+NUM_TERMINALS];
        for(int j=0; j < 25; j++){
            map_table[i+NUM_TERMINALS].str[j] = non_terminal_list[i][j]; 
        }
        map_table[i+NUM_TERMINALS].isterminal = false; 
    }
    return map_table;
}

// This function populates hash table by all terminals and non terminals
// This would be called directly by the driver.c for populating hash table for all strings of mapping_table
map_node** populate_hash_table(){
    map_node** hash_table = make_hash_table(); 
    for(int i=0; i < NUM_TERMINALS; i++){
        int idx = get_hash(terminal_list[i], strlen(terminal_list[i])); 
        insert_hash(idx, i, hash_table);
    }

    for(int i=0; i < NUM_NON_TERMINALS; i++){
        int idx = get_hash(non_terminal_list[i], strlen(non_terminal_list[i]));
        insert_hash(idx, i+NUM_TERMINALS, hash_table);
    }
    //print_hash(hash_table);
    return hash_table;
}

// This functions allocates space for Grammar data structure
// This would be called directly by the driver.c
Grammar** make_grammar(){
    Grammar** grammar = (Grammar**)malloc(sizeof(Grammar*) * NUM_RULES);
    return grammar;
}

// Auxillary function for insert_rule 
// it returns the exact position of the string in the mapping_table by completely scanning the hash table (at the given hash index), in case of collision.  
int get_pos_mapping_table(char* token, map_node** mapping_hash_table, mapping_table* map_table){
    int idx = get_hash(token, strlen(token));
    map_node* tmp = mapping_hash_table[idx];
    int pos; 
    while(tmp != NULL){
        pos = tmp -> mapping_position; 
        if(strcmp(map_table[pos].str, token) != 0){
            tmp = tmp -> next;
        }
        else{
            break;
        }
    }
    return pos; 
}

// This function reads input_grammar.txt and populates the grammar data structure
// This would be called directly by the driver.c after allocating space to grammar
void insert_rule(Grammar** grammar, map_node** mapping_hash_table, mapping_table* map_table){
    FILE* fp = fopen(GRAMMAR_PATH, "r");
    if(fp==NULL) {
        printf("Can't open %s\n",GRAMMAR_PATH);
        exit(1);
    }
    char buffer[BUFFER_LENGTH]; 
    int rule_num = 0;

    while(fgets(buffer, BUFFER_LENGTH, fp)) {
        buffer[strcspn(buffer, "\n")] = 0;
        int len = strlen(buffer);
        //printf("%s", buffer);
        char * token = strtok(buffer, " ");
        //printf("%s\n", token);
        int pos = get_pos_mapping_table(token, mapping_hash_table, map_table); 
        Grammar* node = (Grammar*)malloc(sizeof(Grammar)); 
        node -> s = map_table[pos].s;
        grammar[rule_num] = node;
        Grammar* prev_node = node; 
        while(token != NULL){
            token = strtok(NULL, " ");
            if(token == NULL){
                break;
            }
            pos = get_pos_mapping_table(token, mapping_hash_table, map_table);
            // if(pos < 0 || pos >= NUM_SLOTS){
            //     break;
            // }
            Grammar* node2 = (Grammar*)malloc(sizeof(Grammar)); 
            node2 -> s = map_table[pos].s;       
            node2 -> isterminal = map_table[pos].isterminal; 
            prev_node -> next = node2; 
            prev_node = node2;  
        
        }
        rule_num++;
    }
}

// This is an auxiliary function for printing the entire grammar data structure for debugging purposes
void print_grammar(Grammar** grammar){
    int cnt = 0; 
    for(int i=0; i< NUM_RULES; i++){
        Grammar* tmp = grammar[i]; 
        if(tmp == NULL){
            break;
        }
        cnt = i+1; 
        printf("%s -> ", non_terminal_list[tmp -> s.nt]);
        Grammar* node = tmp -> next; 
        while(node!= NULL){
            if(node -> isterminal == true)
                printf("%s ", terminal_list[(node -> s).t]); 
            else{
                printf("%s ",  non_terminal_list[(node -> s).nt]);
            }
            node = node -> next;
        }
        printf("\n");
        cnt++; 
        
    }
}


// This function allocates space to First and Follow data structure. 
// This would be called directly by the driver.c
FirstAndFollow* initialize_first_follow(){
    FirstAndFollow* F = (FirstAndFollow*)malloc(sizeof(FirstAndFollow)*NUM_NON_TERMINALS);
    return F;
}

// This is an auxillary function for computefirst method which gives an inverse index as follows:
// For first set, it specifies all the rule numbers which have a given non terminals on LHS
grammar_index** get_grammar_idx(Grammar** grammar){
    grammar_index** rule_table = (grammar_index**)malloc(sizeof(grammar_index*) * NUM_RULES);
    for(int i=0; i< NUM_RULES; i++){
        Grammar* tmp = grammar[i]; 
        if(tmp == NULL){
            break;
        }
        grammar_index* swap = rule_table[tmp->s.nt]; 
        grammar_index* new_node = (grammar_index*)malloc(sizeof(grammar_index)); 
        new_node -> rule_num = i; 
        new_node ->next = swap; 
        rule_table[tmp->s.nt] = new_node;         
    }
    return rule_table; 
}

// This function recursively computes the first set for a given symbol
// isterminal is required as while recursing, a rule may yield a terminal, which has to be returned back as a base case without recursing further.
// It takes idx_mapping as input, to access the rules where symbol occours as LHS
unsigned long long computefirst(Grammar **G, FirstAndFollow* F, grammar_index** idx_mapping, symbol s, bool isterminal){
    // base case
    if(isterminal){
        return ((unsigned long long)1)<<s.t; 
    }
    grammar_index* tmp = idx_mapping[s.nt];
    non_terminal curr_nt1 = G[tmp->rule_num]->s.nt;
    // tmp is a linked list which would yield all rule numbers which have symbol as LHS
    while(tmp!=NULL){
        
        int rule_num = tmp->rule_num;
        Grammar* next_node = G[rule_num]; // lhs_nt
        non_terminal curr_nt = next_node -> s.nt; 
        next_node = next_node -> next;
        
        while(next_node != NULL){
            // lookup on the next node to computerfirst of the next node
            unsigned long long next_term_first_set; 
            if(next_node -> isterminal == true){
                next_term_first_set = computefirst(G, F, idx_mapping, next_node->s,true);
            }
            else if(F[next_node -> s.nt].first == 0){
                next_term_first_set = computefirst(G, F, idx_mapping, next_node->s,false);
            }
            else{
                next_term_first_set = F[next_node -> s.nt].first;
            }

            bool eps_set = (next_term_first_set & (((unsigned long long)1)<<EPS));
            if(eps_set == false ){
                F[curr_nt].first = F[curr_nt].first | next_term_first_set; 
                break; 
            }
            else{
                if(next_node -> next == NULL){
                    F[curr_nt].first = F[curr_nt].first | next_term_first_set; 
                    break;
                }
                else{
                    unsigned long long tmp1 = next_term_first_set; 
                    tmp1 = tmp1 & ~(unsigned long long)(((unsigned long long)1) << EPS);
                    F[curr_nt].first = F[curr_nt].first | tmp1;
                }
            }
            next_node = next_node -> next; 
        }
        tmp = tmp->next;
    }
    return F[curr_nt1].first;
}

// This is an auxillary function for computefollow method which gives an inverse index as follows:
// For first set, it specifies all the rule numbers which have a given non terminals on RHS
// This would be called by driver.c to get the aforementioned inverse index mapping to be passed to computeFirstAndFollowSets function
grammar_index** get_grammar_follow_idx(Grammar** grammar){
    grammar_index** rule_table = (grammar_index**)malloc(sizeof(grammar_index*) * NUM_RULES);
    for(int i=0; i< NUM_RULES; i++){
        Grammar* tmp = grammar[i];
        tmp = tmp -> next; 
        while(tmp != NULL){
            if(tmp -> isterminal == false){
                if(rule_table[tmp->s.nt] != NULL && rule_table[tmp->s.nt]->rule_num == i){
                    // do nothing 
                    // We are considering this case when list is being populated with the rule number that already exists in the list
                }
                else{     
                    grammar_index* swap = rule_table[tmp->s.nt]; 
                    grammar_index* new_node = (grammar_index*)malloc(sizeof(grammar_index)); 
                    new_node -> rule_num = i;
                    new_node ->next = swap;
                    rule_table[tmp->s.nt] = new_node;
                }
                
            }
            tmp = tmp -> next;
        }       
    }
    return rule_table; 
}

// This function recursively computes the follow set for a given symbol
unsigned long long computefollow(Grammar **G, FirstAndFollow* F, grammar_index** idx_mapping_follow, symbol s){
    //printf("%s ", non_terminal_list[s.nt]);
    // if(F[s.nt].follow != 0){
    //     //printf("%s ", non_terminal_list[s.nt]);
    //     return F[s.nt].follow;
    // }
    grammar_index* tmp = idx_mapping_follow[s.nt];

    while(tmp!=NULL){
        int rule_num = tmp->rule_num;
        Grammar* next_node = G[rule_num]; // lhs_nt
        non_terminal curr_nt = next_node -> s.nt;
        next_node = next_node -> next;
        while(next_node!=NULL){
            while(next_node != NULL && ((next_node -> isterminal == true) || next_node->s.nt != s.nt)){
                next_node = next_node->next;
            }
            if(next_node == NULL){
                break;
            }
            else{
                next_node = next_node -> next; 
            }
        
            // A->BC rule form we are finding follow of C
            if(next_node == NULL){
                unsigned long long lhs_follow = 0;
                non_terminal lhs =  G[tmp->rule_num]->s.nt;
                if(lhs != s.nt){ //A->BA
                    lhs_follow = computefollow(G, F, idx_mapping_follow, G[tmp->rule_num]->s);
                }
                F[s.nt].follow |= lhs_follow;
                break;
            }

            //A->Ct finding follow of C
            if(next_node->isterminal){
                F[s.nt].follow |= ((unsigned long long)1)<<next_node->s.t;
            }
            else{
                //A->BCD and we are finding follow of C so first OR it with first(D) except EPS
                unsigned long long next_term_first_set = (ULL)0; 
                Grammar *temp = next_node;
                while(temp != NULL){
                    //printf("%s ", non_terminal_list[temp -> s.nt]);
                    if(temp->isterminal && temp -> s.t != EPS){
                        next_term_first_set |= ((ULL)1) << temp -> s.t; 
                        break;
                    }
                    else{
                        next_term_first_set |= F[temp -> s.nt].first; 
                        bool isEps = next_term_first_set & (((unsigned long long)1) << EPS); 
                        if(isEps){
                            //printf(" -- here -- \n");
                            if(temp -> next != NULL){

                                next_term_first_set &= ~(unsigned long long)(((unsigned long long)1) << EPS); 
                            }
                        }
                        else{
                            break; 
                        }
                    }
                    temp = temp -> next; 
                }

                non_terminal lhs =  G[tmp->rule_num]->s.nt;

                if(next_term_first_set & (((unsigned long long)1) << EPS)){
                    next_term_first_set &=  ~(unsigned long long)(((unsigned long long)1) << EPS); 
                    F[s.nt].follow |= next_term_first_set; 
                    F[s.nt].follow |= computefollow(G, F, idx_mapping_follow, G[tmp->rule_num]->s); 
                }
                else{
                    F[s.nt].follow |= next_term_first_set; 
                }
            }
            next_node = next_node->next;
        }
        tmp = tmp->next;  
    }
    return F[s.nt].follow;   

}

// This is an auxiliary function for printing the entire inverse index mapping of first and follow sets for debugging purposes
void print_index_mapping(grammar_index** idx_mapping){
    for(int i=0; i< NUM_NON_TERMINALS; i++){
        printf("%s --> ", non_terminal_list[i]);
        grammar_index* tmp = idx_mapping[i]; 
        while(tmp != NULL){
            printf( "%d, ",tmp -> rule_num);
            tmp = tmp -> next;  
        }
        printf("\n");
    }
}

// This is an auxiliary function for printing the First set for debugging purposes
void printFirstSet(FirstAndFollow* F){
    for(int i=0; i < NUM_NON_TERMINALS; i++){
        //non_terminal i = unary_op;
        non_terminal nt = (non_terminal)i; 
        printf("%s -> ", non_terminal_list[i]); 
        unsigned long long tmp = F[i].first; 
        int cnt = 0; 
        while(tmp > 0){
            if(tmp%2 == 1){
                printf("%s ", terminal_list[cnt]); 
            }
            tmp /= 2; 
            cnt++; 
        }
        printf("\n");
    }
}

// This is an auxiliary function for printing the Follow set for debugging purposes
void printFollowSet(FirstAndFollow* F){
    for(int i=0; i < NUM_NON_TERMINALS; i++){
        //non_terminal i = unary_op;
        non_terminal nt = (non_terminal)i; 
        printf("%s -> ", non_terminal_list[i]); 
        unsigned long long tmp = F[i].follow; 
        int cnt = 0; 
        while(tmp > 0){
            if(tmp%2 == 1){
                printf("%s ", terminal_list[cnt]); 
            }
            tmp /= 2; 
            cnt++; 
        }
        printf("\n");
    }
}
