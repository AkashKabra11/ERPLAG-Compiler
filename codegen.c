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
#include "codegen.h"

#define C 24

int LABEL_NUM = 1; 
void fileprint_program_code(ASTNode* program, FILE* out);
void print_program_code(ASTNode* program);
void check_bound_fetch_val(ASTNode* node, ASTNode* program, bool isstmt);
void codegen_statements(ASTNode* stmts,  baseEntry** table, ASTNode* program);

code append_code(code code1, code code2){
    if(code1.head == NULL) return code2;
    if(code2.head == NULL) return code1; 
    code1.tail -> next = code2.head;
    code1.tail = code2.tail;
    return code1; 
}

void append_codeline_code(ASTNode* program, codeline* cline){
    if(program ->code.head == NULL){
        program ->code.head = cline; 
        program ->code.tail = cline;
    }
    else{
        program -> code.tail -> next = cline; 
        program->code.tail = cline; 
    }
}

void generate_codeline(ASTNode* a, char line[]){
    codeline* c = (codeline*)calloc(sizeof(codeline), 1);
    strcpy(c ->line,line);
    append_codeline_code(a, c); 
    return; 
}

char* label_generator(){
    char* str = (char*)malloc(sizeof(char)*30) ;
    sprintf(str, "l%d", LABEL_NUM); 
    LABEL_NUM++;
    return str; 
}

void generate_AOB_expr_code(ASTNode* expr, ASTNode* program){
    char str[120];
    symEntry* e =  lookup_entry(expr, expr -> scope); 
    int offset =  expr -> offset; //e -> offset;
     // getting wrong offsets in lookup_entry, why? 
    if(expr -> childL -> isLeaf == 1){
        // not op1/op2/logicalop  X = something
        
        if(expr -> childL -> s.t == NUM){
            // t1 := NUM
            // mov [rbp - C - offset], num 
            sprintf(str,"\tmov qword[rbp - %d], %d", offset + C ,expr -> childL -> val.num); 
            generate_codeline(expr, str);
        }
        else if(expr -> childL -> s.t == RNUM){
            sprintf(str,"\tmov qword[rbp - %d], %f", offset + C ,expr -> childL -> val.rnum); 
            generate_codeline(expr, str);
        }
        else if(expr -> childL -> s.t == TRUE){
            sprintf(str,"\tmov qword[rbp - %d], 1", offset + C); 
            generate_codeline(expr, str);
        }
        else if(expr -> childL -> s.t == FALSE){
            sprintf(str,"\tmov qword[rbp - %d], 0", offset + C); 
            generate_codeline(expr, str);
        }
        else{
            // t1 := a || t1 := A[5] 
            if(expr -> childL -> next -> childL -> s.t == EPS){
                // t1 := a
                int off1 = lookup_entry(expr -> childL, expr -> childL -> scope) -> offset; 
                generate_codeline(expr, "\tpush r8");
                sprintf(str,"\tmov r8, [rbp-%d]", off1 + C);
                generate_codeline(expr, str);
                sprintf(str,"\tmov qword[rbp-%d], r8", offset + C);
                generate_codeline(expr, str);                
                generate_codeline(expr, "\tpop r8");
            }
            else{
                check_bound_fetch_val(expr -> childL, program, 0);
                // r11 -> addr of 
                expr -> code = append_code(expr -> code, expr -> childL -> code);
                generate_codeline(expr, "\tpush r8");
                generate_codeline(expr, "\tmov r8, [r11]");
                sprintf(str,"\tmov qword[rbp-%d], r8", offset + C);
                generate_codeline(expr, str);                
                generate_codeline(expr, "\tpop r8");
            }
        }
        return;
    }
    terminal operation = expr -> childL -> childL -> s.t; 
    ASTNode* X = expr -> childL -> next; 
    ASTNode* F = X -> next; 
    generate_AOB_expr_code(X,program);
    expr -> code = append_code(expr -> code, X -> code);

    generate_AOB_expr_code(F,program);
    expr -> code = append_code(expr -> code, F -> code);

    symEntry* e1 = lookup_entry(X, X -> scope); 
    int offset1 =  X -> offset; //e1 -> offset; 
    symEntry* e2 = lookup_entry(F, F -> scope); 
    int offset2 = F -> offset; // e2 -> offset; 

    generate_codeline(expr, "\tpush r8");
    generate_codeline(expr, "\tpush r9");
    
    sprintf(str,"\tmov r8, qword[rbp - %d]", C + offset1); 
    generate_codeline(expr, str);

    sprintf(str,"\tmov r9, qword[rbp - %d]", C + offset2); 
    generate_codeline(expr, str);

    if(operation == PLUS){
        generate_codeline(expr, "\tadd r8, r9");
        sprintf(str,"\tmov qword[rbp - %d], r8", C + offset); 
        generate_codeline(expr, str);
    }
    else if(operation == MINUS){
        generate_codeline(expr, "\tsub r8, r9");
        sprintf(str,"\tmov qword[rbp - %d], r8", C + offset); 
        generate_codeline(expr, str);
    }
    else if(operation == MUL){
        generate_codeline(expr, "\tmov rax, r9");
        generate_codeline(expr, "\tmul r8");
        sprintf(str,"\tmov qword[rbp - %d], rax", C + offset); 
        generate_codeline(expr, str);
    }
    else if(operation == DIV){
        generate_codeline(expr, "\tmov rax, r8");
        generate_codeline(expr, "\tdiv r9");
        sprintf(str,"\tmov qword[rbp - %d], rax", C + offset); 
        generate_codeline(expr, str);
    }
    else if(operation == AND){
        generate_codeline(expr, "\tand r8, r9");
        sprintf(str,"\tmov qword[rbp - %d], r8", C + offset); 
        generate_codeline(expr, str);
    }
    else if(operation == OR){
        generate_codeline(expr, "\tor r8, r9");
        sprintf(str,"\tmov qword[rbp - %d], r8", C + offset); 
        generate_codeline(expr, str);
    }
    else if(operation == LT){
        generate_codeline(expr, "\tcmp r8, r9");
        char* isless = label_generator();
        char* resume = label_generator(); 
        sprintf(str, "\tjl %s", isless);
        generate_codeline(expr, str);
        sprintf(str,"\tmov qword[rbp - %d], 0", C + offset);
        generate_codeline(expr, str);
        sprintf(str, "\tjmp %s", resume);
        generate_codeline(expr, str);
        sprintf(str, "%s:", isless); 
        generate_codeline(expr, str);
        sprintf(str,"\tmov qword[rbp - %d], 1", C + offset);
        generate_codeline(expr, str);
        sprintf(str, "%s:", resume); 
        generate_codeline(expr, str);
    }
    else if(operation == LE){
        generate_codeline(expr, "\tcmp r8, r9");
        char* islesseq = label_generator();
        char* resume = label_generator(); 
        sprintf(str, "\tjle %s", islesseq);
        generate_codeline(expr, str);
        sprintf(str,"\tmov qword[rbp - %d], 0", C + offset);
        generate_codeline(expr, str);
        sprintf(str, "\tjmp %s", resume);
        generate_codeline(expr, str);
        sprintf(str, "%s:", islesseq); 
        generate_codeline(expr, str);
        sprintf(str,"\tmov qword[rbp - %d], 1", C + offset);
        generate_codeline(expr, str);
        sprintf(str, "%s:", resume); 
        generate_codeline(expr, str);
    }
    else if(operation == GT){
        generate_codeline(expr, "\tcmp r8, r9");
        char* isgreat = label_generator();
        char* resume = label_generator(); 
        sprintf(str, "\tjg %s", isgreat);
        generate_codeline(expr, str);
        sprintf(str,"\tmov qword[rbp - %d], 0", C + offset);
        generate_codeline(expr, str);
        sprintf(str, "\tjmp %s", resume);
        generate_codeline(expr, str);
        sprintf(str, "%s:", isgreat); 
        generate_codeline(expr, str);
        sprintf(str,"\tmov qword[rbp - %d], 1", C + offset);
        generate_codeline(expr, str);
        sprintf(str, "%s:", resume); 
        generate_codeline(expr, str);
    }
    else if(operation == GE){
        generate_codeline(expr, "\tcmp r8, r9");
        char* isgreateq = label_generator();
        char* resume = label_generator(); 
        sprintf(str, "\tjge %s", isgreateq);
        generate_codeline(expr, str);
        sprintf(str,"\tmov qword[rbp - %d], 0", C + offset);
        generate_codeline(expr, str);
        sprintf(str, "\tjmp %s", resume);
        generate_codeline(expr, str);
        sprintf(str, "%s:", isgreateq); 
        generate_codeline(expr, str);
        sprintf(str,"\tmov qword[rbp - %d], 1", C + offset);
        generate_codeline(expr, str);
        sprintf(str, "%s:", resume); 
        generate_codeline(expr, str);
    }
    else if(operation == EQ){
        generate_codeline(expr, "\tcmp r8, r9");
        char* isequal = label_generator();
        char* resume = label_generator(); 
        sprintf(str, "\tje %s", isequal);
        generate_codeline(expr, str);
        sprintf(str,"\tmov qword[rbp - %d], 0", C + offset);
        generate_codeline(expr, str);
        sprintf(str, "\tjmp %s", resume);
        generate_codeline(expr, str);
        sprintf(str, "%s:", isequal); 
        generate_codeline(expr, str);
        sprintf(str,"\tmov qword[rbp - %d], 1", C + offset);
        generate_codeline(expr, str);
        sprintf(str, "%s:", resume); 
        generate_codeline(expr, str);
    }
    else if(operation == NE){
        generate_codeline(expr, "\tcmp r8, r9");
        char* isnoteq = label_generator();
        char* resume = label_generator(); 
        sprintf(str, "\tjne %s", isnoteq);
        generate_codeline(expr, str);
        sprintf(str,"\tmov qword[rbp - %d], 0", C + offset);
        generate_codeline(expr, str);
        sprintf(str, "\tjmp %s", resume);
        generate_codeline(expr, str);
        sprintf(str, "%s:", isnoteq); 
        generate_codeline(expr, str);
        sprintf(str,"\tmov qword[rbp - %d], 1", C + offset);
        generate_codeline(expr, str);
        sprintf(str, "%s:", resume); 
        generate_codeline(expr, str);
    }
    generate_codeline(expr, "\tpop r9");
    generate_codeline(expr, "\tpop r8");
    return;
}


void generate_expr_code(ASTNode* expr, ASTNode* program){
    if(expr -> childL -> isLeaf == false && expr -> childL -> s.nt == unary_op){
        generate_AOB_expr_code(expr -> childL -> next, program); 
        expr -> code = append_code(expr -> code, expr -> childL -> next -> code); 
        if(expr -> childL -> childL -> s.t == MINUS){
            int off1 = expr -> childL -> next -> offset;  //lookup_entry(expr -> childL -> next, expr -> childL -> next -> scope) -> offset; 
            char str[120];
            //expr -> code = append_code(expr -> code, expr -> childL -> next -> code);
            sprintf(str,"\tmov rax, [rbp-%d]", off1 + C);
            generate_codeline(expr, str);
            generate_codeline(expr, "\tmov r8, -1");
            generate_codeline(expr, "\tmul r8");

            int off = expr -> offset;  //lookup_entry(expr, expr -> scope) -> offset; 
            sprintf(str,"\tmov qword[rbp-%d], rax", off + C);
            generate_codeline(expr, str);                
        }
    }
    else{
        
        generate_AOB_expr_code(expr, program);

    }
}

void check_bound_fetch_val(ASTNode* node, ASTNode* program, bool isStmt){
    char str1[30];
    char str2[30];  
    // print(A[5]);
    // All code stored in "A" node
    generate_codeline(node, "\t;BOUND CHECK");
    generate_codeline(node, "\tpush r8"); 
    generate_codeline(node, "\tpush r9");
    generate_codeline(node, "\tpush r10");
    // printf("%s =======\n", node -> lexeme);
    // if(node -> scope == NULL){
    //     printf("scope null\n"); 
    //     return;
    // }
    node -> scope = lookup_var(node, node -> scope);
    symEntry* e = lookup_entry(node, node -> scope);
    // if(e == NULL){
    //     printf("ITS NULL\n"); 
    //     return;
    // }
    int offset =  e -> offset; 
    bool  isStatic = e -> type.isHighStatic && e -> type.isLowStatic; 
    bool flag = 1; 
    sprintf(str1, "\tmov r11, qword[rbp - %d]", C + offset); 
    generate_codeline(node, str1);
    ASTNode* tmp = node -> next -> childL; 
    if(isStmt){
        tmp = tmp -> childL; 
    }         
    if(tmp -> s.t == ID){
        // A[m]; 
        int off1 = lookup_entry(tmp, tmp -> scope) -> offset;
        sprintf(str1, "\tmov r10, qword[rbp - %d]", C + off1); 
        generate_codeline(node, str1); 
    }
    else{
        //A[5]
        flag = 0;  
        sprintf(str1, "\tmov r10, %d", tmp -> val.num); 
        generate_codeline(node, str1); 
    }
    sprintf(str1, "\tmov r8, qword[rbp - %d]", C + offset + 8); 
    generate_codeline(node, str1); 
    
    if(flag == 1 || (flag == 0 && isStatic == 0)){
        sprintf(str1, "\tmov r9, qword[rbp - %d]", C + offset + 16); 
        generate_codeline(node, str1); 

        generate_codeline(node, "cmp r10, r8");

        char* err = label_generator(); 
        sprintf(str1, "\tjl %s", err); 
        generate_codeline(node, str1);

        generate_codeline(node, "cmp r10, r9");

        //char* err = label_generator(); 
        sprintf(str1, "\tjg %s", err); 
        generate_codeline(node, str1);

        char* corr = label_generator(); 
        sprintf(str1, "\tjmp %s", corr); 
        generate_codeline(node, str1);
        
        sprintf(str1, "\t%s:", err); 
        generate_codeline(node, str1);

        generate_codeline(node, "\tmov rdi, error");
        generate_codeline(node, "\tmov rax, 0");
        generate_codeline(node, "\tcall printf");
        generate_codeline(node, "\t;Exit from program");
        generate_codeline(node, "\tmov rsp, [initst]");
        generate_codeline(node, "\tmov rbx, 0");
        generate_codeline(node, "\tmov rax, 1h");
        generate_codeline(node, "\tint 0x80");

        sprintf(str1, "%s:", corr); 
        generate_codeline(node, str1);
    }
    generate_codeline(node, "\tsub r10,r8");
    generate_codeline(node, "\tmov rax, 8");
    generate_codeline(node, "\tmul r10");
    generate_codeline(node, "\tsub r11, rax");

    // temporary 
    // generate_codeline(node, "\tmov rsi,[r11]");
    // generate_codeline(node, "\tmov rdi,lld");
    // generate_codeline(node, "\tcall printf");
    // generate_codeline(node, "\t;Nonsense");
    // printf("wwwwwwwwwwwwwwwwwwwwwwwwww\n");
    //

    generate_codeline(node, "\tpop r10");
    generate_codeline(node, "\tpop r9");
    generate_codeline(node, "\tpop r8");        

}

void generate_print_code(ASTNode* node, ASTNode* program){
    // print(x) => r11 is the address of x
    int basic = 0; 
    if(node -> s.t == ID){
        symEntry* e = lookup_entry(node, node -> scope);
        if(e -> type.b == INT || e-> type.b == ARR && e -> type.elem_type == INT){
            basic = 1; 
        } 
        else if(e -> type.b == RL || e-> type.b == ARR && e -> type.elem_type == RL){
            basic = 2; 
        }
        else  if(e -> type.b == BOOL || e-> type.b == ARR && e -> type.elem_type == BOOL){
            basic = 3; 
        }
    }
    generate_codeline(node, "\tpush rbp");
    generate_codeline(node, "\tpush rsp");
    generate_codeline(node, "\tpush r11");
    generate_codeline(node, "\tpush r13");
    generate_codeline(node, "\tpush r14");
    generate_codeline(node, "\tpush rax");
    generate_codeline(node, "\tpush rcx");
    
    if(basic == 1){
        generate_codeline(node, "\tmov rdi, lld");
        generate_codeline(node, "\tmov rsi, qword[r11]");
    } 
    else if(node -> s.t == NUM){
        generate_codeline(node, "\tmov rdi, lld");
        char str[120];
        sprintf(str, "\tmov rsi, %d", node -> val.num);
        generate_codeline(node, str);
    } 
    else if(node -> s.t == RNUM || basic == 2){
        // NOT SURE ABOUT THIS YET
    }
    else if(node -> s.t == TRUE){
        generate_codeline(node, "\tmov rdi, true");
        // rsi dont care
    }
    else if(node -> s.t == FALSE){
        generate_codeline(node, "\tmov rdi, false");
        // rsi dont care
    }
    else{
        //cmp r11, 0
        generate_codeline(node, "\tmov r15, [r11]");
        generate_codeline(node, "\tcmp r15, 0");
        //je fl
        char* false_label = label_generator(); 
        char str1[30]; 
        sprintf(str1,"\tje %s",false_label);
        generate_codeline(node, str1);
        //jmp l
        generate_codeline(node, str1); 
        generate_codeline(node, "\tmov rdi, true");
        
        char* r_label = label_generator(); 
        sprintf(str1,"\tjmp %s",r_label);
        generate_codeline(node, str1); 
        //fl:
        sprintf(str1,"\t%s:",false_label);
        generate_codeline(node, str1); 
        generate_codeline(node, "\tmov rdi, false");

        sprintf(str1,"\t%s:",r_label);
        generate_codeline(node, str1); 

    }
    generate_codeline(node, "\txor rax, rax");
    generate_codeline(node, "\tcall printf");
    generate_codeline(node, "\tpop rcx");
    generate_codeline(node, "\tpop rax");
    generate_codeline(node, "\tpop r14");
    generate_codeline(node, "\tpop r13");
    generate_codeline(node, "\tpop r11");
    generate_codeline(node, "\tpop rsp");
    generate_codeline(node, "\tpop rbp");
}


void generate_scan_code(ASTNode* node, ASTNode* program){
    // print(x) => r11 is the address of x
    int basic = 0; 
    if(node -> s.t == ID){
        symEntry* e = lookup_entry(node, node -> scope);
        if(e -> type.b == INT || e-> type.b == ARR && e -> type.elem_type == INT){
            basic = 1; 
        } 
        else if(e -> type.b == RL || e-> type.b == ARR && e -> type.elem_type == RL){
            basic = 2; 
        }
        else  if(e -> type.b == BOOL || e-> type.b == ARR && e -> type.elem_type == BOOL){
            basic = 3; 
        }
    }
    generate_codeline(node, "\tpush rbp");
    generate_codeline(node, "\tpush rsp");
    generate_codeline(node, "\tpush r11");
    generate_codeline(node, "\tpush r13");
    generate_codeline(node, "\tpush r14");
    generate_codeline(node, "\tpush rax");
    generate_codeline(node, "\tpush rcx");
    generate_codeline(node, "\tmov [stack],rsp");
    generate_codeline(node, "\tand rsp, 0xFFFFFFFFFFFFFFF0");

    if(basic == 1 || basic ==3){
        generate_codeline(node, "\tmov rdi, lldin");
        generate_codeline(node, "\tmov rsi, r11");
    } 
    else if(basic == 2){
        // NOT SURE ABOUT THIS YET
    }

    generate_codeline(node, "\txor rax, rax");
    generate_codeline(node, "\tcall scanf");
    generate_codeline(node, "\tmov rsp,[stack]");
    generate_codeline(node, "\tpop rcx");
    generate_codeline(node, "\tpop rax");
    generate_codeline(node, "\tpop r14");
    generate_codeline(node, "\tpop r13");
    generate_codeline(node, "\tpop r11");
    generate_codeline(node, "\tpop rsp");
    generate_codeline(node, "\tpop rbp");
}

void codegen_stmt(ASTNode* stmt,  baseEntry** table, ASTNode* program){
    ASTNode* node = stmt -> childL; 
    if(node -> isLeaf && node -> s.t == GET_VALUE){
        node = node -> next;
        generate_codeline(node, "\t; Scan line code");
        
        if(node -> s.t == ID){
            symEntry* e = lookup_entry(node, node -> scope);
            int off = e -> offset;
            datatype t = e -> type; 
            char str[120]; 
            if(t.b == ARR){
                sprintf(str,"\tmov r11, [rbp - %d]",C + off);
                generate_codeline(node, str); 

                sprintf(str,"\tmov r13, [rbp - %d]",C + off+8);
                generate_codeline(node, str); 

                sprintf(str,"\tmov r14, [rbp - %d]",C + off+16);
                generate_codeline(node, str); 

                generate_codeline(node, "\tinc r14");

                generate_codeline(node, "\tmov [reg],r11");
                generate_codeline(node, "\tpush rbp");
                generate_codeline(node, "\tpush rsp");
                generate_codeline(node, "\tpush r8");
                generate_codeline(node, "\tpush r11");
                generate_codeline(node, "\tpush r13");
                generate_codeline(node, "\tpush r14");
                generate_codeline(node, "\tpush rdx");
                generate_codeline(node, "\tpush rax");
                generate_codeline(node, "\tpush rcx");
                generate_codeline(node, "\tmov rdi, inarr");
                generate_codeline(node, "\tmov rsi, r14");
                generate_codeline(node, "\tsub rsi, r13");
                if(t.elem_type == INT)
                    generate_codeline(node, "\tmov rdx, int");
                else if(t.elem_type == RL)
                    generate_codeline(node, "\tmov rdx, rl");
                else if(t.elem_type == BOOL)
                    generate_codeline(node, "\tmov rdx, bool");
                generate_codeline(node, "\tmov rcx, r13");
                generate_codeline(node, "\tmov r8, r14");
                generate_codeline(node, "\tdec r8");
                generate_codeline(node, "\txor rax, rax");
                generate_codeline(node, "\tcall printf");
                generate_codeline(node, "\tpush rcx");
                generate_codeline(node, "\tpush rax");
                generate_codeline(node, "\tpush rdx");
                generate_codeline(node, "\tpush r14");
                generate_codeline(node, "\tpush r13");
                generate_codeline(node, "\tpush r11");
                generate_codeline(node, "\tpush r8");
                generate_codeline(node, "\tpush rsp");
                generate_codeline(node, "\tpush rbp");
                generate_codeline(node, "\tmov r11,[reg]");

                char* loop = label_generator();
                sprintf(str,"\t%s:",loop);
                generate_codeline(node, str); 

                generate_scan_code(node, program); 

                generate_codeline(node, "\tsub r11, 8"); 
                generate_codeline(node,"\tinc r13"); 
                generate_codeline(node, "\tcmp r13,r14"); 
                sprintf(str,"\tjne %s",loop);
                generate_codeline(node, str); 
                            
            }
            else{
                // int/ rnum/ bool
                generate_codeline(node, "\tpush rbp");
                generate_codeline(node, "\tpush rsp");
                generate_codeline(node, "\tpush rax");
                generate_codeline(node, "\tpush rcx");
                generate_codeline(node, "\tmov rdi, inline");
                if(t.b == INT)
                    generate_codeline(node, "\tmov rsi, int");
                else if(t.b == RL)
                    generate_codeline(node, "\tmov rsi, rl");
                else if(t.b == BOOL)
                    generate_codeline(node, "\tmov rsi, bool");
                generate_codeline(node, "\txor rax, rax");
                generate_codeline(node, "\tcall printf");
                generate_codeline(node, "\tpop rcx");
                generate_codeline(node, "\tpop rax");
                generate_codeline(node, "\tpop rsp");
                generate_codeline(node, "\tpop rbp");
                // mov r11, ebp
                // sub r11, c+off
                // mov r11, rbp - C - off
                generate_codeline(node, "\tmov r11, rbp");
                sprintf(str,"\tsub r11, %d",C + off);
                generate_codeline(node, str);
                generate_scan_code(node, program);
            }

        }
        node -> parent -> code = append_code(node -> parent -> code, node -> code);
        
    }

    else if(node -> isLeaf && node -> s.t == PRINT){
        
        node = node -> next -> childL;
        generate_codeline(node, "\t; Print line code");
        generate_codeline(node, "\tpush rbp");
        generate_codeline(node, "\tpush rsp");
        generate_codeline(node, "\tpush rax");
        generate_codeline(node, "\tpush rcx");
        generate_codeline(node, "\tmov rdi, outline");
        generate_codeline(node, "\txor rax, rax");
        generate_codeline(node, "\tcall printf");
        generate_codeline(node, "\tpop rcx");
        generate_codeline(node, "\tpop rax");
        generate_codeline(node, "\tpop rsp");
        generate_codeline(node, "\tpop rbp");

        if(node -> s.t == ID){
            symEntry* e = lookup_entry(node, node -> scope);
            int off = e -> offset;
            datatype t = e -> type; 
            char str[120]; 
            if(t.b == ARR){
                if(node -> next -> childL -> s.t != EPS){
                    check_bound_fetch_val(node, program, 0);
                    //node -> code = append_code(node -> code, node -> next -> childL -> code);
                    // bound check A[m] or A[5], r11 is ptr to the value  
                    generate_print_code(node, program); 
                }
                else{
                    // print the entire array 
                     
                    // mov r11, [rbp - %d], C + off
                    // mov r13, [rbp - %d], C + off + 8
                    // mov r14, [rbp - %d], C + off + 16
                    // inc r14
                    // L: 
                    // generate_print_code(node, program); 
                    // sub r11, 8
                    // inc r13
                    // cmp r13,r14
                    // jne L
                    sprintf(str,"\tmov r11, [rbp - %d]",C + off);
                    generate_codeline(node, str); 

                    sprintf(str,"\tmov r13, [rbp - %d]",C + off+8);
                    generate_codeline(node, str); 

                    sprintf(str,"\tmov r14, [rbp - %d]",C + off+16);
                    generate_codeline(node, str); 

                    generate_codeline(node, "\tinc r14");

                    char* loop = label_generator();
                    sprintf(str,"\t%s:",loop);
                    generate_codeline(node, str); 

                    generate_print_code(node, program); 

                    generate_codeline(node, "\tsub r11, 8"); 
                    generate_codeline(node,"\tinc r13"); 
                    generate_codeline(node, "\tcmp r13,r14"); 
                    sprintf(str,"\tjne %s",loop);
                    generate_codeline(node, str); 
                }
                                
            }
            else{
                // int/ rnum/ bool
                // mov r11, ebp
                // sub r11, c+off
                // mov r11, rbp - C - off
                generate_codeline(node, "\tmov r11, rbp");
                // printf("---%d\n", C+off);
                sprintf(str,"\tsub r11, %d",C + off);
                generate_codeline(node, str);
                generate_print_code(node, program);
            }
        }
        else{
            // print(5), print(true) , print(rnum)
            generate_print_code(node, program); 
        }
        node -> parent -> code = append_code(node -> parent -> code, node -> code);
        ASTNode* var = node -> parent; 
        var -> parent -> code =  append_code(var -> parent -> code, var -> code);

    }

    else if(node -> isLeaf && node -> s.t == DECLARE){

        ASTNode* idList = node -> next; 
        node = node -> next -> childL; 
        
        while( node -> s.t == ID){
            symEntry* e = lookup_entry(node, node -> scope); 
            datatype t = e -> type; 
            int offset = e -> offset; 
            char str1[120]; 
            if(t.b == ARR){
                //printf("hii\n");
                // append in idlist -> code
                // mov [rbp- C -offset], esp
                // sprintf(str1,"",offset + C); 
                generate_codeline(idList, "\t;Array declaration");
                generate_codeline(idList, "\tmov r8, rsp");
                generate_codeline(idList, "\tsub r8, 8");
                sprintf(str1,"\tmov [rbp - %d], r8",offset + C); 
                generate_codeline(idList, str1);
                if(t.isLowStatic){
                    // mov r8, num
                    // mov [rbp -C -offset -8], r8
                    //int tmp = t.low.stat; 
                    sprintf(str1,"\tmov r8, %d",t.low.stat); 
                    generate_codeline(idList, str1);
                    sprintf(str1,"\tmov [rbp - %d], r8",offset + C + 8); 
                    generate_codeline(idList, str1);
                }
                else{
                    symEntry *e1 = lookup_entry(t.low.dyn, t.low.dyn -> scope);
                    int offset1 = e1 -> offset;
                    sprintf(str1, "\tmov r8, [rbp - %d]", C + offset1); 
                    generate_codeline(idList, str1);
                    sprintf(str1, "\tmov [rbp - %d], r8", C + offset + 8);
                    generate_codeline(idList, str1);
                    //mov r8, [rbp - C -offset1]
                    // mov [rbp - C - offset - 8], r8
                }
                if(t.isHighStatic){
                    // mov r9, num
                    // mov [rbp -C -offset -16], r9
                    sprintf(str1,"\tmov r9, %d",t.high.stat); 
                    generate_codeline(idList, str1);
                    sprintf(str1,"\tmov [rbp - %d], r9",offset + C + 16); 
                    generate_codeline(idList, str1);
                }
                else{
                    symEntry *e2 = lookup_entry(t.high.dyn, t.high.dyn -> scope);
                    int offset2 = e2 -> offset;
                    sprintf(str1, "\tmov r9, [rbp - %d]", C + offset2); 
                    generate_codeline(idList, str1);
                    sprintf(str1, "\tmov [rbp - %d], r9", C + offset + 16);
                    generate_codeline(idList, str1);
                    // mov [rbp - C - offset - 16],[rbp - C -offset2]
                }
                //sub r9,r8
                generate_codeline(idList, "\tsub r9, r8");
                // inc r9
                generate_codeline(idList, "\tinc r9");
                //mul r9,8
                generate_codeline(idList, "\tmov rax, 8d");
                generate_codeline(idList, "\tmul r9");
                // sub esp,r9
                generate_codeline(idList, "\tsub rsp, rax");
            }
            node = node -> next;
        }
        idList -> parent -> code = idList-> code;  
    }

    else if(node -> isLeaf && node -> s.t == ASSIGNOP){
        ASTNode* ID= node -> next; 
        ASTNode* whichstmt = ID -> next;
        char str[120]; 
        if(whichstmt -> childL -> s.nt == index1){
            // lvalueARRStmt
            // A[5] = expression
            ASTNode* expr = whichstmt -> childL -> next; 
            int off2 = expr -> offset; 
            generate_expr_code(expr, program); 
            
            ID -> code = append_code(ID -> code, expr -> code); 
            check_bound_fetch_val(ID, program, 1); 
            // r1 => reqd address 
            
            generate_codeline(ID, "\tpush r8");
            sprintf(str,"\tmov r8, [rbp-%d]", off2 + C);
            generate_codeline(ID, str);

            // sprintf(str,"\tmov qword[r11], r8");
            generate_codeline(ID, "\tmov qword[r11], r8");
            generate_codeline(ID, "\tpop r8");
        }
        else{
            // lvalueIDstmt
           
            symEntry* e = lookup_entry(ID, ID -> scope); 
            if(e -> type.b == ARR){
                // A := B
                // dynamic type checking 
                // then assignment
                ASTNode* B = ID -> next -> childL; 
                symEntry* e2 = lookup_entry(B, B -> scope); 
                int offB = e2 -> offset; 
                int offA = e -> offset; 

                generate_codeline(ID, "\tpush r8");
                generate_codeline(ID, "\tpush r9");
                generate_codeline(ID, "\tpush r10");
                generate_codeline(ID, "\tpush r11");
                bool isAstat = e -> type.isLowStatic && e -> type.isHighStatic; 
                bool isBstat = e2 -> type.isLowStatic && e2 -> type.isHighStatic; 
                
                if(isAstat == 0 || isBstat == 0){

                    sprintf(str,"\tmov r8, [rbp-%d]", offA + C + 8);
                    generate_codeline(ID, str);

                    sprintf(str,"\tmov r9, [rbp-%d]", offA + C + 16);
                    generate_codeline(ID, str);

                    sprintf(str,"\tmov r10, [rbp-%d]", offB + C + 8);
                    generate_codeline(ID, str);

                    sprintf(str,"\tmov r11, [rbp-%d]", offB + C + 16);
                    generate_codeline(ID, str);

                    generate_codeline(ID, "\tcmp r8, r10");

                    char* errorlabel = label_generator(); 
                    sprintf(str, "\tjne %s", errorlabel); 
                    generate_codeline(ID, str);

                    generate_codeline(ID, "\tcmp r9, r11");

                    //char* errorlabel = label_generator(); 
                    sprintf(str, "\tjne %s", errorlabel); 
                    generate_codeline(ID, str);

                    char* resumelabel = label_generator(); 
                    sprintf(str, "\tjmp %s", resumelabel); 
                    generate_codeline(ID, str);

                    sprintf(str, "%s:", errorlabel); 
                    generate_codeline(ID, str);

                    generate_codeline(ID, "\tmov rdi, typmmerror");
                    generate_codeline(ID, "\tmov rax, 0");
                    generate_codeline(ID, "\tcall printf");
                    generate_codeline(ID, "\t;Exit from program");
                    generate_codeline(ID, "\tmov rsp, [initst]"); 
                    generate_codeline(ID, "\tmov rbx, 0");
                    generate_codeline(ID, "\tmov rax, 1h");
                    generate_codeline(ID, "\tint 0x80");

                    sprintf(str, "%s:", resumelabel); 
                    generate_codeline(ID, str);
                }

                //mov r8, [rbp - C - offB]
                //mov [rbp - C - offA], r8


                sprintf(str, "\tmov r8, [rbp - %d]", offB + C); 
                generate_codeline(ID, str);

                sprintf(str, "\tmov [rbp - %d], r8", offA + C); 
                generate_codeline(ID, str);

                generate_codeline(ID, "\tpop r11");
                generate_codeline(ID, "\tpop r10");
                generate_codeline(ID, "\tpop r9");
                generate_codeline(ID, "\tpop r8");

                ID -> parent -> code = append_code(ID -> parent -> code, ID -> code); 


            }
            else{
                // a = expression
                int off = e -> offset;
                
                int off2 = lookup_entry(whichstmt, whichstmt -> scope) -> offset; 
                generate_expr_code(whichstmt, program); 
                ID -> code = append_code(ID -> code, whichstmt -> code); 

                generate_codeline(ID, "\tpush r8");
                sprintf(str,"\tmov r8, [rbp-%d]", off2 + C);
                generate_codeline(ID, str);

                sprintf(str,"\tmov qword[rbp-%d], r8", off + C);
                generate_codeline(ID, str);
                generate_codeline(ID, "\tpop r8"); 
            }          
        }
        node -> code = append_code(node -> code, ID -> code); 
        node -> parent -> code =  append_code(node -> parent-> code, ID -> code);

    }
    else if(node -> isLeaf && node -> s.t == FOR){
        // all code in FOR node
        ASTNode* id = node -> next; 
        ASTNode* range = node -> next -> next; 
        ASTNode* stmts = range -> next; 
        int lowr = range -> childL -> val.num; 
        int highr = range -> childL -> next -> val.num; 

        symEntry* e=  lookup_entry(id, id -> scope);
        int off = e -> offset;
        char str[120]; 
        sprintf(str,"\tmov qword[rbp - %d], %d", off + C, lowr);
        generate_codeline(node, str); 

        sprintf(str,"\tmov r8, %d", lowr);
        generate_codeline(node, str); 
        
        sprintf(str,"\tmov r9, %d", highr);
        generate_codeline(node, str); 

        char* loop = label_generator(); 
        sprintf(str,"%s:", loop);
        generate_codeline(node, str); 

        generate_codeline(node, "\tpush r8");
        generate_codeline(node, "\tpush r9");

        codegen_statements(stmts, table, program);
        node -> code = append_code(node -> code, stmts -> code); 

        generate_codeline(node, "\tpop r9");
        generate_codeline(node, "\tpop r8"); 

        generate_codeline(node, "\tcmp r9, r8");

        char* brk = label_generator(); 
        sprintf(str, "\tjle %s", brk);
        generate_codeline(node, str);  

        generate_codeline(node, "\tinc r8");

        sprintf(str, "\tmov qword[rbp - %d], r8", off + C);
        generate_codeline(node, str);

        sprintf(str, "\tjmp %s", loop);
        generate_codeline(node, str);

        sprintf(str, "%s:", brk);
        generate_codeline(node, str);

        node -> parent -> code = append_code(node -> parent -> code, node -> code);
    }
    else if(node -> isLeaf && node -> s.t == WHILE){
        ASTNode* expr = node -> next;
        ASTNode* stmts = expr -> next;

        char str[120];
        char *brk = label_generator();
        char *whl = label_generator();

        sprintf(str, "%s:", whl);
        generate_codeline(node, str);

        generate_expr_code(expr, program);
        code c1 = expr -> code;
        node -> code = append_code(node -> code, c1);
        int off = expr -> offset ; 

        sprintf(str,"\tmov r9, qword[rbp - %d]", off + C);
        generate_codeline(node, str);
        generate_codeline(node, "\tcmp r9, 0");
        sprintf(str, "\tje %s", brk);
        generate_codeline(node, str);        


        codegen_statements(stmts, table, program);
        // print_program_code(stmts);
        node -> code = append_code(node -> code, stmts -> code);

        sprintf(str, "jmp %s", whl);
        generate_codeline(node, str);

        sprintf(str, "%s:", brk);
        generate_codeline(node, str);

  

        node -> parent -> code = append_code(node -> parent -> code, node -> code);
    }
    else if(node -> isLeaf && node -> s.t == SWITCH){
        ASTNode* ID = node -> next;
        int off = lookup_entry(ID, ID -> scope) -> offset; 
        char str[120]; 
        sprintf(str, "\tmov r8, [rbp - %d]", C + off); 
        generate_codeline(node, str);

        char* resume_label = label_generator(); 
        ASTNode* value = ID -> next -> childL;
        ASTNode* stmt = value -> next; 
        while(stmt != NULL){
            char* label = label_generator(); 
            // value = value -> childL;
            // printf("%s\n", terminal_list[value -> childL -> s.t]);
            if(value -> childL->  s.t == NUM){
                sprintf(str, "\tcmp r8, %d", value -> childL -> val.num); 
                generate_codeline(node, str);
            }
            else if(value -> childL -> s.t == TRUE){
                generate_codeline(node, "\tcmp r8,1");
            }
            else if(value -> childL -> s.t == FALSE){
                generate_codeline(node, "\tcmp r8,0");
            }
            // else{
            //     printf("%s\n", terminal_list[value ->s.t]);
            // }
            sprintf(str, "\tjne %s", label); 
            generate_codeline(node, str);

            generate_codeline(node, "\tpush r8");

            codegen_statements(stmt, table, program);
            node -> code = append_code(node -> code, stmt -> code); 
            generate_codeline(node, "\tpop r8");

            sprintf(str, "\tjmp %s", resume_label); 
            generate_codeline(node, str);

            sprintf(str, "%s:", label); 
            generate_codeline(node, str);

            value = value -> next -> next; 
            stmt = stmt -> next -> next; 
        } 
        ASTNode* default1 = ID -> next -> next; 
        ASTNode* defstmts = default1 -> childL; 
        codegen_statements(default1, table, program);
        node -> code  = append_code(node -> code, default1 -> code); 

        sprintf(str, "%s:", resume_label); 
        generate_codeline(node, str);

        node -> parent -> code = append_code(node -> parent -> code, node -> code);
    }
    else if(node -> isLeaf && node -> s.t == USE){
        char str[120]; 
        ASTNode* oplist = node -> next -> childL;  
        // ID ID ID EPS
        ASTNode* fun = node -> next -> next; 
        ASTNode* iplist = fun -> next -> childL; 
        // ID ID ID EPS
        
        while(iplist -> s.t != EPS){
            iplist = iplist -> next; 
        }
        iplist = iplist -> prev; 
        int insize = 0;
        // now iplist is at last param; 
        while(iplist != NULL){
            symEntry* e = lookup_entry(iplist, iplist -> scope) ; 
            datatype t = e -> type; 
            int off = e-> offset;
            if(t.b != ARR){
                sprintf(str,"\tmov r8, [rbp - %d]", off + C);
                generate_codeline(node, str);
                generate_codeline(node, "\tpush r8");
                insize += 8; 
            }
            else{
                // order: high -> low -> ptr
                sprintf(str,"\tmov r8, [rbp - %d]", off + 16 + C);
                generate_codeline(node, str);
                generate_codeline(node, "\tpush r8");
                sprintf(str,"\tmov r8, [rbp - %d]", off + 8 + C);
                generate_codeline(node, str);
                generate_codeline(node, "\tpush r8");
                sprintf(str,"\tmov r8, [rbp - %d]", off + C);
                generate_codeline(node, str);
                generate_codeline(node, "\tpush r8");
                insize += 24;
            }
            iplist = iplist -> prev; 
        }

        int cnt = 0; 
        ASTNode* op = oplist; 
        while(oplist -> s.t != EPS){
            oplist = oplist -> next; 
            cnt += 8; 
        }
        sprintf(str,"\tsub rsp, %d", cnt);
        generate_codeline(node, str);

        generate_codeline(node, "\tpush rbp");

        sprintf(str,"\tcall %s", fun -> lexeme);
        generate_codeline(node, str);

        generate_codeline(node, "\tpop rbp");        
        generate_codeline(node, "\t; Done with fun call");

        while(op -> s.t != EPS){
            symEntry* e = lookup_entry(op, op -> scope); 
            int off = e -> offset; 
            generate_codeline(node, "\tpop r8");
            sprintf(str,"\tmov [rbp - %d], r8", off + C);
            generate_codeline(node, str);
            op = op -> next; 
        }

        sprintf(str,"\tadd rsp, %d", insize);
        generate_codeline(node, str);
        node -> parent -> code = append_code(node -> parent -> code, node -> code);
    }
}

void codegen_statements(ASTNode* stmts,  baseEntry** table, ASTNode* program){
    ASTNode* stmt = stmts -> childL;
    // generate_codeline(stmts, "\n");
    while(stmt -> isLeaf == false){
        codegen_stmt(stmt, table, program);
        stmts -> code = append_code(stmts -> code, stmt -> code); 
        // print_program_code(stmt);
        stmt = stmt -> next;             
    }   
}

void codegen_othermodules(ASTNode* O, baseEntry** table, ASTNode* program){
    ASTNode* module = O -> childL; 
    char str[120];
    while(module -> isLeaf == false){
        sprintf(str, "%s: ", module -> childL -> lexeme);
        generate_codeline(module, str);
        generate_codeline(module, "\tmov rbp,rsp");
        generate_codeline(module, "\tadd rbp,16");
        
        sprintf(str, "\tsub rsp, %d", module -> offset); 
        generate_codeline(module, str);

        ASTNode* iplist = module -> childL -> next -> childL; // ID dtype ID dtype
        ASTNode* outlist = module -> childL -> next -> next -> childL; // id type id type
        ASTNode* stmts = module -> childL -> next -> next -> next; 
        ASTNode* temp = outlist;
        int outsize = 0;
        while(temp -> s.t != EPS){
            outsize += 8;
            temp = temp -> next -> next;
        }
        int var = 0; 
        while(iplist -> s.t != EPS){
            symEntry* e = lookup_entry(iplist, iplist -> scope) ; 
            datatype t = e -> type; 
            int off = e-> offset;
            // printf("%d , %d, %d, %d", t.b, t.elem_type, t.isLowStatic, t.isHighStatic);
            if(t.b != ARR){
                sprintf(str,"\tmov r8, [rbp + %d]", outsize + var);
                generate_codeline(module, str);
                sprintf(str,"\tmov [rbp - %d], r8", off + C);
                generate_codeline(module, str);
                var += 8; 
            }
            else{
                // order: high -> low -> ptr
                sprintf(str,"\tmov r8, [rbp + %d]", outsize + var);
                generate_codeline(module, str);
                sprintf(str,"\tmov [rbp - %d], r8", off + C);
                generate_codeline(module, str);
                var += 8; 
                if(t.isLowStatic == 0){
                    sprintf(str,"\tmov r8, [rbp + %d]", outsize + var);
                    generate_codeline(module, str);
                    sprintf(str,"\tmov [rbp - %d], r8", off + C + 8);
                    generate_codeline(module, str);
                }
                else{
                    // printf("here\n");
                    
                    sprintf(str,"\tmov r8, [rbp + %d]", outsize + var);
                    generate_codeline(module, str);
                    sprintf(str,"\tmov r9, %d", t.low.stat);
                    generate_codeline(module, str);
                    generate_codeline(module, "\tcmp r8, r9");
                    char* res_lab = label_generator();
                    sprintf(str,"\tje %s", res_lab); 
                    generate_codeline(module, str);

                    generate_codeline(module, "\tmov rdi, typmmerror");
                    generate_codeline(module, "\tmov rax, 0");
                    generate_codeline(module, "\tcall printf");
                    generate_codeline(module, "\t;Exit from program");
                    generate_codeline(module, "\tmov rsp, [initst]"); 
                    generate_codeline(module, "\tmov rbx, 0");
                    generate_codeline(module, "\tmov rax, 1h");
                    generate_codeline(module, "\tint 0x80");

                    sprintf(str,"%s:", res_lab);
                    generate_codeline(module, str);   

                    sprintf(str,"\tmov [rbp - %d], r8", off + C + 8);
                    generate_codeline(module, str);
                }
                var += 8; 
                if(t.isHighStatic == 0){
                    sprintf(str,"\tmov r8, [rbp + %d]", outsize + var);
                    generate_codeline(module, str);
                    sprintf(str,"\tmov [rbp - %d], r8", off + C + 16);
                    generate_codeline(module, str);
                }
                else{
                    sprintf(str,"\tmov r8, [rbp + %d]", outsize + var);
                    generate_codeline(module, str);
                    sprintf(str,"\tmov r9, %d", t.high.stat);
                    generate_codeline(module, str);
                    generate_codeline(module, "\tcmp r8, r9");
                    char* res_lab = label_generator();
                    sprintf(str,"\tje %s", res_lab);
                    generate_codeline(module, str);

                    generate_codeline(module, "\tmov rdi, typmmerror");
                    generate_codeline(module, "\tmov rax, 0");
                    generate_codeline(module, "\tcall printf");
                    generate_codeline(module, "\t;Exit from program");
                    generate_codeline(module, "\tmov rsp, [initst]"); 
                    generate_codeline(module, "\tmov rbx, 0");
                    generate_codeline(module, "\tmov rax, 1h");
                    generate_codeline(module, "\tint 0x80");

                    sprintf(str,"%s:", res_lab);
                    generate_codeline(module, str);   

                    sprintf(str,"\tmov [rbp - %d], r8", off + C + 16);
                    generate_codeline(module, str);
                }
                var += 8; 
            }
            iplist = iplist -> next -> next; 
        }

        codegen_statements(stmts, table, program); 
        module -> code = append_code(module -> code, stmts -> code);

        int cnt = 0;
        while(outlist -> s.t != EPS){
            // can not be array 
            int offset = lookup_entry(outlist, outlist -> scope) -> offset;

            sprintf(str,"\tmov r8, [rbp - %d]", offset + C);
            generate_codeline(module, str);

            sprintf(str,"\tmov [rbp + %d], r8", cnt);
            generate_codeline(module, str);

            cnt += 8;  
            outlist = outlist -> next -> next; 
        } 

        generate_codeline(module, "\tsub rbp, 16");
        generate_codeline(module, "\tmov rsp, rbp");
        generate_codeline(module, "ret"); 
        O -> code = append_code(O-> code, module -> code);
        module = module -> next;
    }


}

void codegen_drivermodule(ASTNode* D, baseEntry** table, ASTNode* program){
    int off = D -> offset; 
    generate_codeline(D, "main:");
    generate_codeline(D, "\tmov [initst], rsp");
    generate_codeline(D, "\tmov rbp, rsp"); 
    
    char str1[120]; 
    sprintf(str1,"\tsub rsp, %d",off + C - 8 ); 
    generate_codeline(D, str1);
    
    ASTNode* moduledef = D -> childL; 
    codegen_statements(moduledef, table, program); 
        //mov eax,1
        //xor ebx, ebx
        //int 0x80
    // generate_codeline(moduledef, "\tmov rsp, [initst]"); 
    generate_codeline(moduledef, "\tmov rsp, rbp"); 
    generate_codeline(moduledef,"\tmov eax,1");
    generate_codeline(moduledef,"\txor ebx, ebx");
    generate_codeline(moduledef,"\tint 0x80");

    // print_program_code(D);
    D -> code = append_code( D -> code , moduledef -> code); 
    
}

void codegen_program(ASTNode* program, baseEntry** table){
    // head -> program
    ASTNode* M = program -> childL; 
    ASTNode* O1 = M -> next; 
    ASTNode* D = O1 -> next; 
    ASTNode* O2 = D -> next; 
    generate_codeline(program, "global main"); 
    generate_codeline(program, "extern printf");
    generate_codeline(program, "extern scanf");
    generate_codeline(program, "\n");
    // other externs

    generate_codeline(program, "section .data");
    // add other .data fields
    generate_codeline(program, "error: db \"RUN TIME ERROR: Array Index Out of bounds\",10,0"); 
    generate_codeline(program, "lld: db  \"%lld\",10, 0"); 
    generate_codeline(program, "lldin: db  \"%lld\", 0");
    generate_codeline(program, "flt: db  \"%lf\",10, 0");
    generate_codeline(program, "fltin: db  \"%lf\", 0");
    generate_codeline(program, "true: db \"true\",10,0"); 
    generate_codeline(program, "false: db \"false\",10,0");
    generate_codeline(program, "outline: db \"Output: \",10,0");
    generate_codeline(program, "inline: db \"Input: Enter a %s value\",10,0");
    generate_codeline(program, "inarr: db \"Input: Enter %d array elements of %s type for range %d to %d\",10,0");
    generate_codeline(program, "int: db \"integer\",0");
    generate_codeline(program, "rl: db \"real\",0");
    generate_codeline(program, "bool: db \"boolean\",0");
    generate_codeline(program, "typmmerror: db \" RUN TIME ERROR: Type mismatch Error\",10,0");
    generate_codeline(program, "\n");

    generate_codeline(program, "section .bss");
    generate_codeline(program, "\tstack: resq 1");
    generate_codeline(program, "\treg: resq 1");
    generate_codeline(program, "\tinitst: resq 1");
    generate_codeline(program, "\n");
    


    generate_codeline(program, "section .text");
    codegen_othermodules(O1, table, program);
    codegen_drivermodule(D, table, program);
    codegen_othermodules(O2, table, program);
    
    D->code = append_code(D -> code, O1 -> code); 
    O1->code = append_code(O1 -> code, O2 -> code); 
    program -> code = append_code(program -> code, D -> code);
    // print_program_code(D);
}


void fileprint_program_code(ASTNode* program, FILE* outfile){
    code c = program -> code; 
    codeline* lin = c.head; 
    
    while(lin != NULL){
        fprintf(outfile, "%s\n", lin -> line); 
        lin = lin -> next; 
    }
}

void print_program_code(ASTNode* program){
    code c = program -> code; 
    codeline* lin = c.head; 
    
    while(lin != NULL){
        printf("%s\n", lin -> line); 
        lin = lin -> next; 
    }
}


// int main(int argc, char* argv[]){
    
//     if(argc!=3){
//         printf("Enter source code file and output file.\n");
//         return -1;
//     }
//     printf("Status of Our Implementation:\n"); 
//     printf("(a) FIRST and FOLLOW set automated\n");
//     printf("(c) Both lexical and syntax analysis modules implemented\n");
//     FILE* fp1=fopen(argv[1],"r"); 
//     if(fp1==NULL) {
//         printf("Can't open %s\n",argv[1]);
//         return 0; 
//     }
 
//     infile = fp1;
//     FILE* fp2,*parsetree;
//     keymap_node** key_hash = populate_keyword_hash_table();
//     mapping_table* map_table = populate_mapping_table();
//     map_node** mapping_hash_table = populate_hash_table();
//     Grammar** grammar = make_grammar();
//     insert_rule(grammar, mapping_hash_table, map_table);
//     FirstAndFollow* F = initialize_first_follow(); 
//     grammar_index** idx_mapping = get_grammar_idx(grammar); 
//     grammar_index** idx_mapping_follow = get_grammar_follow_idx(grammar); 
//     ComputeFirstAndFollowSets(grammar, F, idx_mapping, idx_mapping_follow);
//     int** ptab = createParseTable(grammar,F);
//     int flag=1;
//     char input = '#';
//     while(1){
        
        
//        	if(input!='\n'){
//        			printf("Select option\n");
//        	}
//        	scanf("%c",&input);
        
//         switch(input){
//             case '0':
//                 flag = 0;
//                 break;
//             case '1':
//                 //rewind(infile);
//                 reset_globals();
//                 removeComments(infile);
//                 //fclose(infile);
//                 break;
//             case '2':
//                 //rewind(infile);
//                 reset_globals();
//                 lexer(key_hash); 
//                 //fseek(infile, 0, SEEK_SET);
//                 break;
//             case '3':
//                 fp2=fopen(argv[2],"w");
                
//                 if(fp1==NULL) {
//                     printf("Can't open %s\n",argv[1]);
//                     flag = 0; 
//                     break;
//                 }
//                 parsetree = fp2;
//                 reset_globals();
//                 ptnode* pt= parseInputSourceCode(ptab,grammar,infile, F,key_hash);
//                 //printParseTreePreOrder(pt,parsetree);
// 				ASTNode* a = constructAST(pt, action, NULL, NULL); 
//                 ASTPass(a);
//                 baseEntry** bt = make_base_table(); 
//                 bool valid = populate_symbol_table(a, bt);
//                 // printf("===================\n\n");
//                 valid = second_pass(a, bt) && valid;
//                 // print_basetable(bt);
//                 if(valid == 1){
//                     codegen_program(a, bt);
//                 }
//                 // printASTPreOrder(a, parsetree); 
//                 fileprint_program_code(a, parsetree);                
//                 fclose(parsetree);
//                 break;
            
//             case '4':
//                 fp2=fopen(argv[2],"w");
                
//                 if(fp1==NULL) {
//                     printf("Can't open %s\n",argv[1]);
//                     flag = 0; 
//                     break;
//                 }
//                 parsetree = fp2;
//                 reset_globals();
//                 pt= parseInputSourceCode(ptab,grammar,infile, F,key_hash);
//                 //printParseTreePreOrder(pt,parsetree);
// 				a = constructAST(pt, action, NULL, NULL); 
//                 ASTPass(a);
//                 bt = make_base_table(); 
//                 valid = populate_symbol_table(a, bt);
//                 // printf("===================\n\n");
//                 valid = second_pass(a, bt) && valid;
//                 // print_basetable(bt);
//                 if(valid == 1){
//                     codegen_program(a, bt);
//                 }
//                 printASTPreOrder(a, parsetree); 
//                 // fileprint_program_code(a, parsetree);                
//                 fclose(parsetree);
//                 break;
            
//             case '\n': 
//             	break;

//             default:
//             		printf("Invalid option\n");
//             		break;
//         }
//         if(flag==0){
//             fclose(infile);
//             break;
//         }
    
//     }
// }