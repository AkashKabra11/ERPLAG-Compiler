//  Group 47
//  2016B3A70562P   Akash Kabra
//  2016B3A70523P   Ayush Mungad
//  2016B4A70520P   Kumar Anant Raj
//  2016B4A70533P   Mustansir Mama
//  2016B5A70715P   Bhavesh Chand

#include "ast.h"
#include<stdlib.h>
#include "lexer.h"
#include "mapping.h"
#include "parser.h"
#include <time.h>
#include <stdbool.h>


ASTNode* constructAST(ptnode* pthead, int action[NUM_RULES][MAX_RHS+1], ASTNode* par, ASTNode* inh);
void printASTPreOrder(ASTNode* node, FILE *outfile);
ASTNode* populateASTNode(ptnode* n, ASTNode* par){
    ASTNode* a = (ASTNode*)calloc(1,sizeof(ASTNode)); 
    a -> s = n -> s; 
    a -> lexeme = n -> lexeme; 
    a -> lineno = n -> lineno; 
    a -> val = n -> val;
    a -> ruleNum = n -> ruleNum;
    a -> isLeaf = n -> isLeaf;
	a -> parent = par; 
    return a;
}

ASTNode* appendLL(ASTNode* n1, ASTNode* n2){
    n1 -> next = n2;
    n2 -> prev = n1;
    return n1; 
}

ASTNode* makenode1(ptnode* n1, int action[NUM_RULES][MAX_RHS+1], ASTNode* par){
	ASTNode* a1 = constructAST(n1, action, par, NULL); 
    return a1; 
}

ASTNode* makenode2(ptnode* n1,ptnode* n2, int action[NUM_RULES][MAX_RHS+1], ASTNode* par){
    ASTNode* a1 = constructAST(n1, action, par, NULL); 
    ASTNode* a2 = constructAST(n2, action, par, NULL); 
    a1 = appendLL(a1, a2);
    return a1; 
}

ASTNode* makenode3(ptnode* n1,ptnode* n2,ptnode* n3, int action[NUM_RULES][MAX_RHS+1], ASTNode* par){    
    ASTNode* a1 = constructAST(n1, action, par, NULL); 
    ASTNode* a2 = constructAST(n2, action, par, NULL); 
    ASTNode* a3 = constructAST(n3, action, par, NULL);
    a1 = appendLL(a1, a2);
    a2 = appendLL(a2, a3);
    return a1; 
}

ASTNode* makenode4(ptnode* n1,ptnode* n2,ptnode* n3,ptnode* n4,int action[NUM_RULES][MAX_RHS+1], ASTNode* par){
    ASTNode* a1 = constructAST(n1, action, par, NULL); 
    ASTNode* a2 = constructAST(n2, action, par, NULL); 
    ASTNode* a3 = constructAST(n3, action, par, NULL);
    ASTNode* a4 = constructAST(n4, action, par, NULL); 
    a1 = appendLL(a1, a2);
    a2 = appendLL(a2, a3);
    a3 = appendLL(a3, a4);
    return a1;
}

ASTNode* makeOffsetNode(int num){
    ASTNode* a = (ASTNode*)calloc(1,sizeof(ASTNode)); 
    num_value nm;// = (num_value*)calloc(1, sizeof(num_value));
	nm.num = 0; 
	a -> val = nm; 
    a -> lexeme = "0";
    return a; 
}

ASTNode* constructAST(ptnode* pthead, int action[NUM_RULES][MAX_RHS+1], ASTNode* par, ASTNode* inh){
    // if(pthead -> isLeaf == 0)
    // printf("%s %d\n", non_terminal_list[pthead -> s.nt], pthead -> ruleNum);
    // else
    // {
    //     printf("%s %d\n", terminal_list[pthead -> s.t], pthead -> ruleNum);
    // }
    // if(par == NULL){
    //     printf("Par == NULL\n");
    // }
	int ruleNum = (pthead -> ruleNum) -1;
	// int main_action = action[ruleNum][0];
	ASTNode* curr_node = populateASTNode(pthead, par);
	curr_node -> inh = inh; 
	if(ruleNum == -1){
		// For leaf node
		return populateASTNode(pthead, curr_node);
	}
	int main_action = action[ruleNum][0];
	
	ptnode* p[MAX_RHS]; 
	int cnt = 0; 
	ptnode* child_node = pthead -> child; 
	for(int i=1; i < MAX_RHS; i++){
        if(child_node == NULL)
            break;
		if(action[ruleNum][i] > 0){
			p[cnt] = child_node; 
			cnt++;  
		}
		child_node = child_node -> sibling;
	}
	
	if(main_action == 0 || main_action == 3){
        
        switch(cnt){
            case 0:
                curr_node -> childL = NULL;
                break;
            case 1:  
                curr_node -> childL = makenode1(p[0], action, curr_node);
                // A -> leaf
                // ASTNode* tmp = curr_node; 
                // curr_node = curr_node -> childL; 
                // curr_node -> next = tmp -> next; 
                // curr_node -> prev = tmp -> prev; 
                // curr_node -> childL = NULL; 
                // curr_node -> parent = tmp -> parent;
                // if(curr_node -> parent != NULL){ 
                //     if(curr_node -> parent -> childL == tmp)                  
                //         curr_node -> parent -> childL = curr_node; 
                // }
                // if(curr_node -> next != NULL){
                //     curr_node -> next -> prev = curr_node; 
                // }
                // if(curr_node -> prev != NULL){
                //     curr_node -> prev -> next = curr_node; 
                // }
                // free(tmp);
                break;				
            case 2:
                curr_node -> childL = makenode2(p[0], p[1], action, curr_node);
                
                break;
            case 3:
                curr_node -> childL = makenode3(p[0], p[1], p[2], action, curr_node);
                break;
            case 4:
                curr_node -> childL = makenode4(p[0], p[1], p[2], p[3], action, curr_node);
                break;
        }
        //M->mM
        ASTNode* tmp = curr_node -> childL; 
        for(int i=1; i < MAX_RHS; i++){
            if(tmp == NULL){
                break;
            }
            // printf("%d ", action[ruleNum][i]);
            if(action[ruleNum][i] == 2){
                // printf("%s %s %d------------------\n", non_terminal_list[tmp -> s.nt], terminal_list[tmp ->childL -> s.t], ruleNum); 
                if(tmp -> prev != NULL)
                    tmp -> prev -> next = tmp -> childL; 
                if(tmp -> childL != NULL){					
                    tmp -> childL -> prev = tmp -> prev; 
                    ASTNode* childtmp = tmp -> childL; 
                    while(childtmp != NULL){
                        childtmp -> parent = tmp -> parent; 
                        childtmp = childtmp -> next; 
                    }
                    ASTNode* tmp2 = tmp -> childL; 
                    free(tmp); 
                    tmp = tmp2; 
                }					
            }
            if(action[ruleNum][i] > 0)
                tmp = tmp -> next; 
        }
        // printf("\n");
	}
	else if(main_action == 1){
        // A -> B  
        // printf("1\n");
        curr_node -> childL = makenode1(p[0], action, curr_node);
        //if(curr_node -> childL != NULL && curr_node -> childL -> isLeaf == 0){
        ASTNode* tmp = curr_node -> childL;	
        curr_node -> childL = curr_node -> childL -> childL;
        // curr_node -> childL -> childL -> parent = curr_node;  
        if(curr_node -> childL != NULL){
            ASTNode* nd = curr_node -> childL; 
            while(nd != NULL){
            nd -> parent = curr_node; 
            nd = nd -> next; 
            }
        }
        free(tmp);
        //}
	}
	else if(main_action == 2){
        // inh and syn ones
        //printf("2\n");
		switch(ruleNum + 1){
			case 46:
                {
				ASTNode* a = (ASTNode*)calloc(1,sizeof(ASTNode)); 
				a -> s.t = ASSIGNOP; 
    			a -> lexeme = ":="; 
    			a -> lineno = p[0]->lineno; 
    			a -> ruleNum = 0; 
				a -> isLeaf = true;
				a -> parent = curr_node; 
				ASTNode* a2 = constructAST(p[0], action, curr_node, NULL);
				ASTNode* a3 = constructAST(p[1], action, curr_node, NULL);
				a = appendLL(a,a2);
				a2 = appendLL(a2,a3);
				curr_node -> childL = a;
				break;
                }
			case 53:
				curr_node -> childL = makenode4(p[1], p[0], p[2], p[3], action, curr_node);
				break;

			case 66:
			case 69:
			case 73:
			case 76:
                ;
				ASTNode* a0 = constructAST(p[0] , action, NULL, NULL);
				// ASTNode* a1 = constructAST(p[1] , action, curr_node);
				ASTNode* a1 = constructAST(p[1] , action, curr_node, a0  -> childL);
				// a0 = appendLL(a0, a1);
				curr_node -> childL = a1 -> childL;
                ASTNode* ax = a1 -> childL; 
                while(ax != NULL){
                    ax -> parent = curr_node;
                    ax = ax ->  next; 
                }

				free(a0);
				free(a1);
				break;	
			
			case 67:
			case 74:
			case 77:
            {
				ASTNode* a1 = constructAST(p[0], action, curr_node, NULL);
                // ASTNode* tmp = a1; 
                // a1 = a1 -> childL; 
                // free(tmp); 
				ASTNode* a_inh = (ASTNode*)malloc(sizeof(ASTNode));  
                // POPULATE AS NEEDED
                a_inh -> ruleNum = X_node; 
                // a_inh -> parent = curr_node;
                a_inh -> childL = curr_node -> inh; 
				ASTNode* a2 = constructAST(p[1], action, curr_node, NULL);
				a1 = appendLL(a1, a_inh);
				a_inh = appendLL(a_inh, a2);
                // a1 -> next = a_inh; 
                // a_inh -> next = a2; 
				
                

				ASTNode* a3 =  constructAST(p[2], action, curr_node, a1);	
                curr_node -> childL = a3-> childL;
                // a3 -> childL -> parent = curr_node; 
                free(a3);
				// a1 -> parent = curr_node; 
                // a2 -> parent = curr_node; 
                // a3 -> parent = curr_node; 
            }
            break;

			case 68:
			case 72:
			case 75:
			case 78:
				curr_node -> childL = curr_node -> inh;
                break;
                
			case 71:{
				ASTNode* a1 = constructAST(p[0], action, curr_node, NULL);
                ASTNode* a_inh = (ASTNode*)malloc(sizeof(ASTNode));  // X node
                a_inh -> ruleNum = X_node; 
                a_inh -> childL = curr_node -> inh; 
				ASTNode* a2 = curr_node -> inh; 
				ASTNode* a3 = constructAST(p[1], action, curr_node, NULL);
				a1 = appendLL(a1, a_inh);
				a_inh = appendLL(a_inh, a3);
				curr_node -> childL = a1;
            }
            break;
		}
	}
    // else if(main_action == 3){
    //     // printf("3\n");
    //     ASTNode* a1 = populateASTNode(pthead -> child, par);
        
    //     par -> childL = a1;
    //     a1 -> next = curr_node -> next; 
    //     a1 -> prev = curr_node -> prev; 
    //     if(curr_node -> next != NULL){
    //         curr_node -> next -> prev = a1; 
    //     }
    //     if(curr_node -> prev != NULL){
    //         curr_node -> prev -> next = a1; 
    //     }
    //     ASTNode* tmp = curr_node; 
    //     curr_node = a1; 
    //     // printf("%s\n",terminal_list[curr_node-> s.t]);
    //     free(tmp);
    //     // return a1; 
    // }
	return curr_node;
}


void printASTNode(ASTNode* node, FILE* outfile){
    int p;
    // if(node -> ruleNum == X_node){
    //     p = fprintf(outfile,"X node here, a temporary node, offset = %d, width = %d\n", node -> offset, node -> width);
    //     return;
    // } 
    bool isX = 0; 
    if(node -> ruleNum == X_node){
        isX = 1; 
    }


    if(isX){
        p = fprintf(outfile,"----");
        for(int i=p+1; i < NUM_SPACES; i++){
            fprintf(outfile," ");
        }
    }
    else if(node->lexeme==NULL){
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

    if(isX == 0)
        p = fprintf(outfile,"%d",node->lineno);
    else
        p = fprintf(outfile,"---");
    for(int i=p+1; i < NUM_SPACES; i++){
        fprintf(outfile," ");
    }

    if(isX)
        p = fprintf(outfile, "X");
    else if(node -> isLeaf == 0){
        p = fprintf(outfile, "%s", non_terminal_list[node -> s.nt]);
    }
    else
    {
        p = fprintf(outfile, "%s", terminal_list[node -> s.t]);
    }
    for(int i=p+1; i < NUM_SPACES; i++){
        fprintf(outfile," ");
    }

    
    p = fprintf(outfile,"%d",node->offset);
    for(int i=p+1; i < NUM_SPACES; i++){
        fprintf(outfile," ");
    }

    p = fprintf(outfile,"%d",node->width);
    for(int i=p+1; i < NUM_SPACES; i++){
        fprintf(outfile," ");
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

    
    p = fprintf(outfile,"%d",node->isLeaf);
    for(int i=p+1; i < NUM_SPACES; i++){
        fprintf(outfile," ");
    }

    
    // p = fprintf(outfile, "%d", node -> ruleNum);
	// for(int i=p+1; i < NUM_SPACES; i++){
    //     fprintf(outfile," ");
    // }
    fprintf(outfile, "\n");

}

void printASTPreOrder(ASTNode* node, FILE *outfile){ 
    if(node==NULL) return;
    printASTNode(node,outfile);
    
    printASTPreOrder(node->childL,outfile);
    printASTPreOrder(node->next,outfile);
}


void ASTPass(ASTNode* node){ 
    if(node==NULL) return;
    if(node -> childL != NULL){
        node -> childL -> parent = node; 
    }
    ASTPass(node->childL);
    if(node -> next != NULL){
        node -> next -> parent = node -> parent; 
    }
    ASTPass(node->next);
}


