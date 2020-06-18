//  Group 47
//  2016B3A70562P   Akash Kabra
//  2016B3A70523P   Ayush Mungad
//  2016B4A70520P   Kumar Anant Raj
//  2016B4A70533P   Mustansir Mama
//  2016B5A70715P   Bhavesh Chand

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "lexerDef.h"

FILE* infile;
FILE* tokstr;
int lineno=1;
int charno=0;
int prevcharno;
char in[2*BUF_LEN];
int pos=BUF_LEN;

//  To reset some global variables used in this file for subsequent runs made by the driver without exiting
void reset_globals(){
    fseek(infile, 0, SEEK_SET);
    pos=BUF_LEN;
    lineno=1;
    charno=0;
    return;
}

//  Implement Twin Input Buffer for source code
FILE *getStream(FILE *fp){
    if(ftell(fp)==0)
        fread(in+BUF_LEN,1,BUF_LEN,infile);
    if(pos==2*BUF_LEN-1){
        for(int i=0;i<BUF_LEN;i++){
            in[i]=in[i+BUF_LEN];
            in[i+BUF_LEN]='\0';
        }
        fread(in+BUF_LEN,1,BUF_LEN,infile);
        if(ferror(infile) ) {
            printf("Error in reading from file\n");
        }
        pos=pos-BUF_LEN;
    }
    return infile;
}

//  Read from input buffer and return char to lexer on demand
char read(){
    infile=getStream(infile);
    charno++;
    if(in[pos]=='\n'){
        lineno++;
        prevcharno = charno;
        charno=0;
    }
    return in[pos++];
}

//  Handle retraction
void retract(int n){
    pos--;
    if(in[pos]=='\n'){
        lineno--;
        charno = prevcharno;
    }
    pos-=n-1;
    charno-=n;
}

//  Additional Module to clean file. Used in Driver Case 1
void removeComments(FILE* testcaseFile){
    int state=1;
    int i=0;
    char out[BUF_LEN+1];
    for(int i=0;i<BUF_LEN+1;i++) out[i]='\0';
    char c = '#';    
    while(c!='\0'){
        if(i==BUF_LEN||i==BUF_LEN+1){
            printf("%s", out);
            for(int i=0;i<BUF_LEN+1;i++) out[i]='\0';
            i=0;
        }
        switch(state){
            case 1:
                c=read();
                if(c=='*') state=19;
                else out[i++]=c;
                break;
            case 19:
                c=read();
                if(c=='*') state=20;
                else{
                    out[i++]='*';
                    out[i++]=c;
                    state = 1;
                }
                break;
            case 20:
                c=read();
                if(c=='*') state=21;
                else if(c == '\n') out[i++] = '\n';
                //Do nothing because we are into a comment stage
                break;
            case 21:
                c=read();
                if(c=='*') state=1;
                else{ 
                    if(c == '\n') out[i++] = '\n';
                    state=20;
                }
                break;

        }
    }
    printf("%s", out);
    //fwrite(out,1,i,cleanFile);

}

//  Read and return tokens to parser on demand
//  Implemented using switch on DFA state
tokenInfo* getNextToken(keymap_node** key_hash){
    int state=1;
    int idlen=0;    //identifier & number length
    char buf[MAX_TK_LEN*5+1];
    buf[0]='\0';
    char c;
    tokenInfo *r=malloc(sizeof(tokenInfo));
    r->line=lineno;

    while(1){
        // printf("%d\n",state);
        switch(state){
            case 0: 
                buf[idlen]='\0';
                r->t=LEXERR;
                printf( "\nLexical Error at line %d : column %d : lexeme %s\n",lineno,charno,buf);
                r->line=lineno;
                return r; 
            case 1:
                c=read();
                switch(c){
                    case '\0':
                        r->t=DOLLAR;
                        r->line=lineno;
                        return r;
                    case ' ':
                    case '\t':
                    case '\r':
                    case '\n':
                        break;
                    case '+':
                        state=2;
                        break;
                    case '-':
                        state=3;
                        break;
                    case '/':
                        state=4;
                        break;
                    case '[':
                        state=5;
                        break;
                    case ']':
                        state=6;
                        break;
                    case ')':
                        state=7;
                        break;
                    case '(':
                        state=8;
                        break;
                    case ';':
                        state=9;
                        break;
                    case ',':
                        state=10;
                        break;
                    case '.':
                        state=11;
                        break;
                    case '=':
                        state=13;
                        break;
                    case '!':
                        state=15;
                        break;
                    case ':':
                        state=17;
                        break;
                    case '*':
                        state=19;
                        break;
                    case '<':
                        state=23;
                        break;
                    case '>':
                        state=26;
                        break;
                    default:
                        buf[idlen++]=c;
                        if(isalpha(c)) state=29;
                        else if(isdigit(c)) state=30;
                        else state=0;
                }
                break;
            case 2:
                r->t=PLUS;
                r->lexeme="+";
                r->line=lineno;
                return r;
            case 3:
                r->t=MINUS;
                r->lexeme="-";
                r->line=lineno;
                return r;
            case 4:
                r->t=DIV;
                r->lexeme="/";
                r->line=lineno;
                return r;
            case 5:
                r->t=SQBO;
                r->lexeme="[";
                r->line=lineno;
                return r;
            case 6:
                r->t=SQBC;
                r->lexeme="]";
                r->line=lineno;
                return r;
            case 7:
                r->t=BC;
                r->lexeme=")";
                r->line=lineno;
                return r;
            case 8:
                r->t=BO;
                r->lexeme="(";
                r->line=lineno;
                return r;
            case 9:
                r->t=SEMICOL;
                r->lexeme=";";
                r->line=lineno;
                return r;
            case 10:
                r->t=COMMA;
                r->lexeme=",";
                r->line=lineno;
                return r;
            case 11:
                c=read();
                if(c=='.') state=12;
                else{
                    r->t=LEXERR;
                    r->lexeme=".";
                    r->line=lineno;
                    retract(1);
                    printf( "\nLexical Error at line %d : column %d : lexeme %s\n",lineno,charno,r->lexeme);
                    return r;
                } 
                break;
            case 12:
                r->t=RANGEOP;
                r->lexeme="..";
                r->line=lineno;
                return r;
            case 13:
                c=read();
                if(c=='=') state=14;
                else{
                    r->t=LEXERR;
                    r->lexeme = "=";
                   
                    retract(1);
                    r->line=lineno;
                    printf( "\nLexical Error at line %d : column %d : lexeme %s\n",lineno,charno,r->lexeme);
                    return r;
                } 
                break;
            case 14:
                r->t=EQ;
                r->lexeme="==";
                r->line=lineno;
                return r;
            case 15:
                c=read();
                if(c=='=') state=16;
                else{
                    r->t=LEXERR;
                    r->lexeme = "!";
                    retract(1);
                    r->line=lineno;
                    printf( "\nLexical Error at line %d : column %d : lexeme %s\n",lineno,charno,r->lexeme);
                    return r;
                } 
                break;
            case 16:
                r->t=NE;
                r->lexeme="!=";
                r->line=lineno;
                return r;
            case 17:
                c=read();
                if(c=='=') state=18;
                else{
                    r->t = COLON;
                    r->lexeme = ":";
                    retract(1);
                    r->line=lineno;
                    return r;
                } 
                break;
            case 18:
                r->t=ASSIGNOP;
                r->lexeme=":=";
                r->line=lineno;
                return r;
            case 19:
                c=read();
                if(c=='*') state=20;
                else{
                    retract(1);
                    r->t=MUL;
                    r->lexeme="*";
                    r->line=lineno;
                    return r;
                }
                break;
            case 20:
                c=read();
                if(c=='*') state=21;
                else if(c=='\0'){
                    retract(1);
                    r->t=LEXERR;
                    r->line=lineno;
                    printf( "\nLexical Error at line %d : column %d : Comment did not terminate\n",lineno,charno);
                    return r;
                }
                //Do nothing because we are into a comment stage
                break;
            case 21:
                c=read();
                if(c=='*') state=22;
                else if(c=='\0'){
                    retract(1);
                    r->t=LEXERR;
                    r->line=lineno;
                    printf( "\nLexical Error at line %d : column %d : Comment did not terminate\n",lineno,charno);
                    return r;
                }
                else state=20; 
                break;
            case 22:
                state=1;
                break;
            case 23:
                c=read();
                if(c=='=') state=24;
                else if(c=='<') state=25;
                else{
                    retract(1);
                    r->t=LT;
                    r->lexeme="<";
                    r->line=lineno;
                    return r;
                }
                break;
            case 24:
                r->t=LE;
                r->lexeme="<=";
                r->line=lineno;
                return r;
            case 25:
                c=read();
                if(c=='<') state=37;
                else{
                    retract(1);
                    r->line=lineno;
                    r->t=DEF;
                    r->lexeme="<<";
                    return r;
                }
                break;
            case 37:
                r->t=DRIVERDEF;
                r->lexeme="<<<";
                r->line=lineno;
                return r;
            case 26:
                c=read();
                if(c=='=') state=27;
                else if(c=='>') state=28;
                else{
                    retract(1);
                    r->t=GT;
                    r->line=lineno;
                    r->lexeme=">";
                    return r;
                }
                break;
            case 27:
                r->t=GE;
                r->lexeme=">=";
                r->line=lineno;
                return r;
            case 28:
                c=read();
                if(c=='>') state=38;
                else{
                    retract(1);
                    r->t=ENDDEF;
                    r->lexeme=">>";
                    r->line=lineno;
                    return r;
                }
                break;
            case 38:
                r->t=DRIVERENDDEF;
                r->lexeme=">>>";
                r->line=lineno;
                return r;
            case 29:
                c=read();
                if(isalnum(c)||c=='_') buf[idlen++]=c;
                else{
                    buf[idlen]='\0';
                    retract(1);
                    int x = lookup_keyword(buf,key_hash);
                    if(x==-1){  //NOT A KEYWORD
                        if(idlen<=MAX_TK_LEN){
                            r->t=ID;
                            r->lexeme=malloc(sizeof(strlen(buf)+1));
                            strcpy(r -> lexeme, buf);
                            r->line=lineno;
                            return r;
                        }
                        else{
                            r->t=LEXERR;
                            r->line=lineno;
                            printf( "\nLexical Error at line %d : column %d:  lexeme %s : Identifier too long\n",lineno,charno,buf);
                            return r;
                        }
                    }
                    else{
                        r->t=x;
                        r->lexeme=malloc(sizeof(strlen(buf)+1));
                        strcpy(r -> lexeme, buf);
                        r->line=lineno;
                        return r;
                    }
                }
                break;
            case 30:
                c=read();
                if(c=='.'){
                    buf[idlen++]=c;
                    state=31;
                }
                /* 1e7 is lexical error?
                else if(c=='e'||c=='E'){
                    buf[idlen++]=c;
                    state=33;
                }*/
                else if(isdigit(c)) buf[idlen++]=c;
                else{
                    buf[idlen]='\0';
                    retract(1);
                    r->t=NUM;
                    r->lexeme=malloc(sizeof(strlen(buf)+1));
                    strcpy(r -> lexeme, buf);
                    r->line=lineno;
                    return r;
                }
                break;
            case 31:
                c = read();
                if(isdigit(c)){
                    buf[idlen++]=c;
                    state=32;
                }
                else if(c=='.'){
                    buf[idlen-1]='\0';
                    retract(2);
                    r->t=NUM;
                    r->lexeme=malloc(sizeof(strlen(buf)+1));
                    strcpy(r -> lexeme, buf);
                    r->line=lineno;
                    return r;
                }
                else{
                    buf[idlen]='\0';
                    retract(1);
                    r->t=LEXERR;
                    r->line=lineno;
                    printf("\nLexical Error at line %d : column %d : lexeme %s\n",lineno,charno,buf);
                    return r;
                }
                break;
            case 32:
                c = read();
                if(c=='e'||c=='E'){
                    buf[idlen++]=c;
                    state=33;
                }
                else if(isdigit(c)) buf[idlen++]=c;
                else{
                    buf[idlen]='\0';
                    r->t=RNUM;
                    r->lexeme=malloc(sizeof(strlen(buf)+1));
                    strcpy(r -> lexeme, buf);
                    retract(1);
                    r->line=lineno;
                    return r;
                }
                break;
            case 33:
                c = read();
                if(c=='+'||c=='-'){
                    buf[idlen++]=c;
                    state=34;
                }
                else if(isdigit(c)){
                    buf[idlen++]=c;
                    state=35;
                }
                else{
                    buf[idlen]='\0';
                    r->t=LEXERR;
                    printf( "\nLexical Error at line %d : column %d : lexeme %s\n",lineno,charno,buf);
                    retract(1);
                    r->line=lineno;
                    return r;
                }
                break;
            case 34:
                c = read();
                if(isdigit(c)){
                    buf[idlen++]=c;
                    state=35;
                }
                else{
                    buf[idlen]='\0';
                    r->t=LEXERR;
                    printf( "\nLexical Error at line %d : column %d : lexeme %s\n",lineno,charno,buf);
                    retract(1);
                    r->line=lineno;
                    return r;
                }
                break;
            case 35:
                c = read();
                if(isdigit(c)) buf[idlen++]=c;
                else{
                    buf[idlen]='\0';
                    r->t=RNUM;
                    r->lexeme=malloc(sizeof(strlen(buf)+1));
                    strcpy(r -> lexeme, buf);
                    retract(1);
                    r->line=lineno;
                    return r;
                }
                break;
        }
    }
}

//  Independent lexer. Used in Driver Case 2
void lexer(keymap_node** key_hash){
    while(1){
        tokenInfo* r = getNextToken(key_hash);
        if(r->t==LEXERR)
            continue;
        else if(r->t==DOLLAR)
            break;
        else{

            int p;
            p = printf("%d", r -> line); 
            for(int i=p+1; i< NUM_SPACES; i++){
                printf(" ");
            }
            p = printf("%s", r -> lexeme); 
            for(int i=p+1; i< NUM_SPACES; i++){
                printf(" ");
            }
            p = printf("%s", terminal_list[r->t]); 
            for(int i=p+1; i< NUM_SPACES; i++){
                printf(" ");
            }
            printf("\n");
        }
    }
    int p;
    p = printf("%d", lineno); 
    for(int i=p+1; i< NUM_SPACES; i++){
        printf(" ");
    }
    p = printf("$"); 
    for(int i=p+1; i< NUM_SPACES; i++){
        printf(" ");
    }
    p = printf("%s", terminal_list[DOLLAR]); 
    for(int i=p+1; i< NUM_SPACES; i++){
        printf(" ");
    }
    printf("\n");
}

// int main(int argc, char** argv){
//     if(argc!=3){
//         printf("Enter source code file and output file.");
//         return -1;
//     }
//     FILE* fp1=fopen(argv[1],"r"); 
//     FILE* fp2=fopen(argv[2],"w");
//     infile = fp1;
//     tokstr = fp2;

//     keymap_node** key_hash = populate_keyword_hash_table();
//     lexer(key_hash);
//     infile=fopen(argv[1],"r"); 
//     pos=BUF_LEN;
//     removeComments(fp1);
//     //fclose(fp2);
//     //fclose(tokstr);
// } 