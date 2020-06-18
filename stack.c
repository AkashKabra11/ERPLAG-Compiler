//  Group 47
//  2016B3A70562P   Akash Kabra
//  2016B3A70523P   Ayush Mungad
//  2016B4A70520P   Kumar Anant Raj
//  2016B4A70533P   Mustansir Mama
//  2016B5A70715P   Bhavesh Chand

#include "parser.h"
#include "parserDef.h"

// Implements Stack used by Parser


stack_str* create_stack(){
    stack_str* st=calloc(1, sizeof(stack_str));
    st->top=-1;
}

bool isFull(stack_str* st){
    return (st->top==STACK_SIZE);
}

bool isEmpty(stack_str* st){
    return (st->top==-1);
}

void push(stack_str* st,stack_entry s){
    if(isFull(st))
        printf("Error: Stack overflow\n");
    else
        st->stack[++st->top] = s;
}

void pop(stack_str* st){
    if(!isEmpty(st))
        --st->top;
}

stack_entry peek(stack_str* st){
    return st->stack[st->top];
}

void print_stack(stack_str* st){
    printf("Printing top of Stack\n");
    for(int i=0; i < st -> top + 1;i++){
        //if(st->stack[i].s.t != 0){
            if(st -> stack[i].ist){
                printf("%s ", terminal_list[st ->stack[i].s.t]);
            }
            else{
                printf("%s ", non_terminal_list[st ->stack[i].s.nt]);
            }
        //}
    }
    printf("\n");
}
