//  Group 47
//  2016B3A70562P   Akash Kabra
//  2016B3A70523P   Ayush Mungad
//  2016B4A70520P   Kumar Anant Raj
//  2016B4A70533P   Mustansir Mama
//  2016B5A70715P   Bhavesh Chand

#include "lexerDef.h"
#include "lexer.h"
#include "mapping.h"

//  isKeyword.c implements Hash Table & lookup to help lexer to identify keywords

//  Init. empty hash_table
keymap_node** make_keyword_hash_table(){
    keymap_node** hash_table = (keymap_node**)malloc(sizeof(keymap_node*) * NUM_SLOTS);
    return hash_table; 
}

//  Insert a given keyword in the hash table
void insert_keyword_hash(int hash_index, keymap_node* node , keymap_node** hash_table)
{
    keymap_node* tmp = hash_table[hash_index]; 
    hash_table[hash_index] = node; 
    node -> next = tmp;
}

//  Populate Hash Table with all keywords
keymap_node** populate_keyword_hash_table(){ 
    keymap_node** hash_table = make_keyword_hash_table(); 
    int n = NUM_KEYWORDS;
    for(int i=0; i < n; i++){
        int idx = get_hash(keyword_mapping[i].l, strlen(keyword_mapping[i].l)); 
        keymap_node* tmp = (keymap_node*)malloc(sizeof(keymap_node)); 
        tmp -> k = keyword_mapping[i]; 
        insert_keyword_hash(idx, tmp , hash_table);
    }

    return hash_table;
}

// Search in Hash Table
int lookup_keyword(char* token, keymap_node** hash_table){
    int idx = get_hash(token, strlen(token));
    keymap_node* tmp = hash_table[idx];
    //int pos = 0; 
    while(tmp != NULL){
        //char* str = ; 
        if(strcmp(tmp ->k.l, token) != 0){
            tmp = tmp -> next;
        }
        else{
            return tmp ->k.t; 
            break;
        }
       
    }
    return -1; 
}

// int main(){
//     keymap_node** hash_tab = populate_keyword_hash_table();
//     int x = lookup_keyword("default", hash_tab);
//     //if(x != -1){
//         printf("%d \n", x);
//     //}
// }






