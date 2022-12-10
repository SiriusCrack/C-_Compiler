#include "codegen.h"
#include <stdio.h>
#include <string.h>
#include "emitcode.h"

extern Node *AST;
extern FILE * code;

int toffset = 0;
int returnAddress = 0;
int mainAddress = 0;

static void walkAST (Node * node);
static void generateCode (Node *node);
static void generateInit ();
static void generateIO ();

void CodeGen() {
    generateIO();
    walkAST(AST);
    generateInit();
}

void walkAST(Node * node) {
    if(node == NULL) { return; }
    if(node->isIO) { walkAST(node->sibling); return; }
    generateCode(node);
    walkAST(node->sibling);
}

void generateCode(Node * node) {
    switch(node->nodeType) {
        case ntFunc:
            fprintf(code, "* FUNCTION %s\n", node->literal);
            toffset = node->size;
            fprintf(code, "* TOFF set: %d\n", toffset);
            returnAddress = emitWhereAmI();
            if(strcmp("main", node->literal) == 0) { mainAddress = returnAddress; }
            emitRM("ST", 3, -1, 1, "Store return address");
            for(int child = 0; child < AST_MAX_CHILDREN; child++) {
                walkAST(node->child[child]);
            }
            fprintf(code, "* Add standard closing in case there is no return statement\n");
            emitRM("LDC", 2, 0, 6, "Set return value to 0");
            emitRM("LD", 3, -1, 1, "Load return address");
            emitRM("LD", 1, 0, 1, "Adjust fp");
            emitRM("JMP", 7, 0, 3, "Return");
            fprintf(code, "* END FUNCTION %s\n", node->literal);
            toffset = 0;
            break;
        case ntCompound:
        case ntCompoundwFunc:
            fprintf(code, "* COMPOUND\n");
            toffset = node->size;
            fprintf(code, "* TOFF set: %d\n", toffset);
            fprintf(code, "* Compound Body\n");
            fprintf(code, "* TOFF set: %d\n", toffset);
            for(int child = 0; child < AST_MAX_CHILDREN; child++) {
                walkAST(node->child[child]);
            }
            fprintf(code, "* END COMPOUND\n");
            toffset = 0;
            break;
    }
}

void generateInit() {
    fprintf(code, "%3d:  %5s  %lld,%lld(%lld)\t%s %s\n", 0, "JMP", 7, emitWhereAmI()-1, 7, "Jump to init [backpatch]", "");
    fprintf(code, "* INIT\n");
    emitRM("LDA", 1, 0, 0, "set first frame at end of globals");
    emitRM("ST", 1, 0, 1, "store old fp (point to self)");
    fprintf(code, "* INIT GLOBALS AND STATICS\n");
    fprintf(code, "* END INIT GLOBALS AND STATICS\n");
    emitRM("LDA", 3, 1, 7, "Return address in ac");
    emitRM("JMP", 7, mainAddress-(emitWhereAmI()+1), 7, "Jump to main");
    emitRO("HALT", 0, 0, 0, "DONE!");
    fprintf(code, "* END INIT\n");
}

void generateIO() {
    emitSkip(1);
    fprintf(code, "* ** ** ** ** ** ** ** ** ** ** ** **\n");
    fprintf(code, "* FUNCTION input\n");
    emitRM("ST", 3, -1, 1, "Store return address");
    emitRO("IN", 2, 2, 2, "Grab int input");
    emitRM("LD", 3, -1, 1, "Load return address");
    emitRM("LD", 1, 0, 1, "Adjust fp");
    emitRM("JMP", 7, 0, 3, "Return");
    fprintf(code, "* END FUNCTION input\n");
    fprintf(code, "* \n");
    fprintf(code, "* ** ** ** ** ** ** ** ** ** ** ** **\n");
    fprintf(code, "* FUNCTION output\n");
    emitRM("ST", 3, -1, 1, "Store return address");
    emitRM("LD", 3, -2, 1, "Load parameter");
    emitRO("OUT", 3, 3, 3, "Output integer");
    emitRM("LD", 3, -1, 1, "Load return address");
    emitRM("LD", 1, 0, 1, "Adjust fp");
    emitRM("JMP", 7, 0, 3, "Return");
    fprintf(code, "* END FUNCTION output\n");
    fprintf(code, "* \n");
    fprintf(code, "* ** ** ** ** ** ** ** ** ** ** ** **\n");
    fprintf(code, "* FUNCTION inputb\n");
    emitRM("ST", 3, -1, 1, "Store return address");
    emitRO("INB", 2, 2, 2, "Grab bool input");
    emitRM("LD", 3, -1, 1, "Load return address");
    emitRM("LD", 1, 0, 1, "Adjust fp");
    emitRM("JMP", 7, 0, 3, "Return");
    fprintf(code, "* END FUNCTION inputb\n");
    fprintf(code, "* \n");
    fprintf(code, "* ** ** ** ** ** ** ** ** ** ** ** **\n");
    fprintf(code, "* FUNCTION outputb\n");
    emitRM("ST", 3, -1, 1, "Store return address");
    emitRM("LD", 3, -2, 1, "Load parameter");
    emitRO("OUTB", 3, 3, 3, "Output bool");
    emitRM("LD", 3, -1, 1, "Load return address");
    emitRM("LD", 1, 0, 1, "Adjust fp");
    emitRM("JMP", 7, 0, 3, "Return");
    fprintf(code, "* END FUNCTION outputb\n");
    fprintf(code, "* \n");
    fprintf(code, "* ** ** ** ** ** ** ** ** ** ** ** **\n");
    fprintf(code, "* FUNCTION inputc\n");
    emitRM("ST", 3, -1, 1, "Store return address");
    emitRO("INC", 2, 2, 2, "Grab char input");
    emitRM("LD", 3, -1, 1, "Load return address");
    emitRM("LD", 1, 0, 1, "Adjust fp");
    emitRM("JMP", 7, 0, 3, "Return");
    fprintf(code, "* END FUNCTION inputc\n");
    fprintf(code, "* \n");
    fprintf(code, "* ** ** ** ** ** ** ** ** ** ** ** **\n");
    fprintf(code, "* FUNCTION outputc\n");
    emitRM("ST", 3, -1, 1, "Store return address");
    emitRM("LD", 3, -2, 1, "Load parameter");
    emitRO("OUTC", 3, 3, 3, "Output char");
    emitRM("LD", 3, -1, 1, "Load return address");
    emitRM("LD", 1, 0, 1, "Adjust fp");
    emitRM("JMP", 7, 0, 3, "Return");
    fprintf(code, "* END FUNCTION outputc\n");
    fprintf(code, "* \n");
    fprintf(code, "* ** ** ** ** ** ** ** ** ** ** ** **\n");
    fprintf(code, "* FUNCTION outnl\n");
    emitRM("ST", 3, -1, 1, "Store return address");
    emitRO("OUTNL", 3, 3, 3, "Output a newline");
    emitRM("LD", 3, -1, 1, "Load return address");
    emitRM("LD", 1, 0, 1, "Adjust fp");
    emitRM("JMP", 7, 0, 3, "Return");
    fprintf(code, "* END FUNCTION outnl\n");
    fprintf(code, "* \n");
    fprintf(code, "* ** ** ** ** ** ** ** ** ** ** ** **\n");
}