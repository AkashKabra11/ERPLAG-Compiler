//  Group 47
//  2016B3A70562P   Akash Kabra
//  2016B3A70523P   Ayush Mungad
//  2016B4A70520P   Kumar Anant Raj
//  2016B4A70533P   Mustansir Mama
//  2016B5A70715P   Bhavesh Chand

#ifndef _LANGSPEC_H_
#define _LANGSPEC_H_


typedef enum terminal{
    PLUS, 
    MINUS, 
    MUL, 
    DIV, 
    LT, 
    LE, 
    GE, 
    GT, 
    EQ, 
    NE, 
    DEF, 
    ENDDEF,
    COLON,
    RANGEOP,
    SEMICOL,
    COMMA,
    ASSIGNOP,
    SQBO,
    SQBC,
    BO,
    BC,
    DRIVERDEF,
    DRIVERENDDEF,
    
    INTEGER,
    REAL,
    BOOLEAN,
    OF,
    ARRAY,
    START,
    END,
    DECLARE,
    MODULE,
    DRIVER,
    PROGRAM,
    RECORD,
    TAGGED,
    UNION,
    GET_VALUE,
    PRINT,
    USE,
    WITH,
    PARAMETERS,
    TRUE,
    FALSE,
    TAKES,
    INPUT,
    RETURNS,
    AND,
    OR,
    FOR,
    IN,
    SWITCH,
    CASE,
    BREAK,
    DEFAULT,
    WHILE,

    ID,
    NUM,
    RNUM,

    EPS,
    DOLLAR
}terminal;

typedef enum non_terminal{
	program,
	moduleDeclarations,
	moduleDeclaration,
	otherModules,
	driverModule,
	module,
	ret,
	input_plist,
	N1,
	output_plist,
	N2,
	dataType,
	range_arrays,
	type,
	moduleDef,
	statements,
	statement,
	ioStmt,
	boolConstt,
	var_id_num,
	var,
	whichId,
	simpleStmt,
	assignmentStmt,
	whichStmt,
	lvalueIDStmt,
	lvalueARRStmt,
	index1,
	moduleReuseStmt,
	optional,
	idList,
	N3,
	expression,
	U,
	new_NT,
	unary_op,
	arithmeticOrBooleanExpr,
	N7,
	AnyTerm,
	N8,
	arithmeticExpr,
	N4,
	term,
	N5,
	factor,
	op1,
	op2,
	logicalOp,
	relationalOp,
	declareStmt,
	condionalStmt,
	caseStmts,
	N9,
	value,
	default1,
	iterativeStmt,
	range
}non_terminal; 


typedef struct keyword{
    char* l;
    terminal t;
} keyword;

//  Struct encapsulating info about token. Returned by lexer to parser
typedef struct{
    terminal t;
    char* lexeme;
    int line;
    //int col;
}tokenInfo;

//  Struct used for Keyword Hash Table Nodes
typedef struct keymap_node{
    keyword k; 
    struct keymap_node* next; 
}keymap_node; 

#define NUM_KEYWORDS (WHILE-INTEGER+1)
#define NUM_TERMINALS (DOLLAR-PLUS+1)
#define NUM_NON_TERMINALS (range-program+1)
    

extern keyword keyword_mapping[NUM_KEYWORDS];
extern char* terminal_list[NUM_TERMINALS];
extern char* non_terminal_list[NUM_NON_TERMINALS];

#define NUM_RULES 105
#define GRAMMAR_PATH "input_grammar.txt"


#define MAX_RHS 13

#endif