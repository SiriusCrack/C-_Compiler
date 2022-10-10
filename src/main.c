#include "ASTNode.h"
#include "SymbolTable.h"
#include "SemanticAnalysis.h"
#include "parser.tab.h"
#include <stdio.h>

extern FILE * yyin;
extern int yydebug;

int NodeUID;
Node * AST;
ScopeTable * SymbolTable;

int PrintDebugFlag;
int PrintTreeFlag;
int PrintAnnotatedTreeFlag;
int PrintSymTblFlag;
int warns;
int errs;

void parseArgs (int, char * []);

int main (int argc, char * argv[]) {
    NodeUID = 0;
    PrintDebugFlag = 0;
    PrintTreeFlag = 0;
    PrintAnnotatedTreeFlag = 0;
    PrintSymTblFlag = 0;
    warns = 0;
    errs = 0;
    if(argc > 1) {
        parseArgs(argc, argv);
    }
    SymbolTable = NewGlobalScope();
    yyparse();
    PrintTree(AST, 0);
    WriteScopes(AST, SymbolTable);
    WriteRefs(AST, SymbolTable);
    PrintSymbolTable(SymbolTable);
    if(errs < 1) PrintAnnotatedTree(AST, 0);
    printf("Number of warnings: %d\n", warns);
    printf("Number of errors: %d\n", errs);
    return 0;
}

void parseArgs (int argc, char * argv[]) {
    FILE * fp = fopen(argv[argc-1], "r");
    if(fp) {
        yyin = fp;
    } else {
        printf("Need file to parse.\n");
    }
    int i;
    for(i = 1; i < argc-1; i++) {
        switch (argv[i][1]) {
            case 'd':
                yydebug = 1;
                break;
            case 'D':
                break;
            case 'h':
                printf("usage: c- [options] [sourcefile]\n");
                printf("options:\n");
                printf("-d \t- turn on parser debugging\n");
                printf("-D \t- turn on symbol table debugging\n");
                printf("-h \t- print this usage message\n");
                printf("-p \t- print the abstract syntax tree\n");
                printf("-P \t- print the abstract syntax tree plus type information\n");
                break;
            case 'p':
                PrintTreeFlag = 1;
                break;
            case 'P':
                PrintAnnotatedTreeFlag = 1;
                break;
            case 'T':
                PrintSymTblFlag = 1;
                break;
            case 'Z':
                PrintDebugFlag = 1;
                break;
            default:
                printf("\"-%s\" is not a recognized argument.", argv[i]);
                break;
        }
    }
}