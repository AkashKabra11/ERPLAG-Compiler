//  Group 47
//  2016B3A70562P   Akash Kabra
//  2016B3A70523P   Ayush Mungad
//  2016B4A70520P   Kumar Anant Raj
//  2016B4A70533P   Mustansir Mama
//  2016B5A70715P   Bhavesh Chand

#ifndef _CODEGEN_H_
#define _CODEGEN_H_

void codegen_program(ASTNode* program, baseEntry** table);
void fileprint_program_code(ASTNode* program, FILE* outfile);


#endif