#include "ASTNode.h"
#include "Token.h"
#include "NodeVector.h"
#include "parser.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Node * NewNode (Token token) {
    Node * newNode = (Node *) malloc(sizeof(Node));
    if(newNode == NULL) {
        printf("Out of memory error at line %d\n", token.lineNum);
        return NULL;
    } else {
        int i;
        for (i = 0; i < MAX_CHILDREN; i++) {
            newNode->child[i] = NULL;
        }
        newNode->childCount = 0;
        newNode->sibling = NULL;
        newNode->literal = strdup(token.literal);
        free(token.literal);
        newNode->tokenClass = token.tokenClass;
        newNode->lineNum = token.lineNum;
        newNode->siblingLevel = 0;
        switch (token.tokenClass) {
            case NUMCONST:
                newNode->value.integer = token.value.integer;
                break;
            case ID:
                newNode->value.str = strdup(token.value.str);
                free(token.value.str);
                break;
            case CHARCONST:
                newNode->value.character = token.value.character;
                break;
            case STRINGCONST:
                newNode->value.str = strdup(token.value.str);
                free(token.value.str);
                break;
            case BOOLCONST:
                newNode->value.integer = token.value.integer;
                break;
            default:
                newNode->value.str = strdup(token.value.str);
                free(token.value.str);
        }
        printf("made node %s\n", newNode->literal);
        return newNode;
    }
}

Node * AddSibling (Node * treePtr, Node * newSibling) {
    if(treePtr == NULL) {
        printf("started row with %s\n", newSibling->literal);
        return newSibling;
    } else {
        Node * cur = treePtr;
        while(cur->sibling != NULL) {
            cur = cur->sibling;
        }
        newSibling->siblingLevel = cur->siblingLevel + 1;
        cur->sibling = newSibling;
        printf("added %s as sibling to %s\n", newSibling->literal, cur->literal);
        return treePtr;
    }
}

Node * AddChild (Node * treePtr, Node * newChild) {
    if(treePtr == NULL) {
        printf("adding child to null, dummy\n");
        return treePtr;
    } else {
        treePtr->child[treePtr->childCount] = newChild;
        treePtr->childCount = treePtr->childCount+1;
        if(newChild == NULL) {
            printf("added null as child to %s\n", treePtr->literal);
        } else {
            printf("added %s as child to %s\n", newChild->literal, treePtr->literal);
        }
        return treePtr;
    }
}

void PrintTree (Node * AST, int level, int flag) {
    if(flag != 1) {
        return;
    } else if(AST == NULL) {
        printf("empty tree\n");
        return;
    } else {
        Node * cur = AST;
        do {
            if(cur->siblingLevel > 0) {
                int i = 0;
                for(i = 0;i < level; i++) {
                    printf(".   ");
                }
                printf("Sibling: %d  ", cur->siblingLevel);
            }
            switch (cur->nodeType) {
                case ntVar:
                    printf("Var: %s of type %s ", cur->value.str, cur->dataType);
                    break;
                case ntFunc:
                    printf("Func: %s returns type %s ", cur->value.str, cur->dataType);
                    break;
                case ntParm:
                    printf("Parm: %s of type %s ", cur->value.str, cur->dataType);
                    break;
                case ntCompound:
                    printf("Compound ");
                    break;
                case ntAssign:
                    printf("Assign: %s ", cur->value.str);
                    break;
                case ntID:
                    printf("Id: %s ", cur->value.str);
                    break;
                case ntConst:
                    printf("Const %d ", cur->value.integer);
                    break;
                case ntReturn:
                    printf("Return ");
                    break;
                case ntOp:
                    printf("Op: %s ", cur->value.str);
                    break;
                case ntCall:
                    printf("Call: %s ", cur->value.str);
                    break;
                case ntIf:
                    printf("If ");
                    break;
                case ntVarArray:
                    printf("Var: %s of array of type %s ", cur->value.str, cur->dataType);
                    break;
                case ntParmArray:
                    printf("Parm: %s of array of type %s ", cur->value.str, cur->dataType);
                    break;
                case ntIter:
                    printf("While ");
                    break;
                default:
                    printf("unknown node\n");
                    break;
                }
            printf("[line: %d]", cur->lineNum);
            printf("\n");
            int c;
            for(c = 0; c < cur->childCount; c++) {
                if(cur->child[c] != NULL) {
                    int i = 0;
                    for(i = 0;i < level+1; i++) {
                        printf(".   ");
                    }
                    printf("Child: %d  ", c);
                    PrintTree(cur->child[c], level+1, 1);
                }
            }
            cur = cur->sibling;
        } while (cur != NULL);
        return;
    }
}