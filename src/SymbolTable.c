#include "ASTNode.h"
#include "SymbolTable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern int PrintSymTblFlag;

ScopeTable * NewGlobalScope () {
    ScopeTable * newScope = (ScopeTable *) malloc(sizeof(ScopeTable));
    if(newScope == NULL) {
        printf("New scope out of memory\n");
        return NULL;
    } else {
        newScope->parent = NULL;
        int i;
        for(i = 0; i < SCOPE_MAX_CHILDREN; i++) {
            newScope->child[i] = NULL;
        }
        newScope->scopeName = strdup("Global");
        newScope->node = NULL;
        newScope->depth = 0;
        newScope->symbolTable = NULL;
        return newScope;
    }
}

ScopeTable * NewScope (Node * node) {
    ScopeTable * newScope = (ScopeTable *) malloc(sizeof(ScopeTable));
    if(newScope == NULL) {
        printf("New scope out of memory\n");
        return NULL;
    } else {
        newScope->parent = NULL;
        int i;
        for(i = 0; i < SCOPE_MAX_CHILDREN; i++) {
            newScope->child[i] = NULL;
        }
        newScope->scopeName = node->literal;
        newScope->node = node;
        newScope->depth = 0;
        newScope->symbolTable = NULL;
        return newScope;
    }
}

ScopeTable * GetMatchingChildScope (ScopeTable * scopeTable, int nodeUID) {
    int i;
    for(i = 0; i < SCOPE_MAX_CHILDREN; i++) {
        if(scopeTable->child[i]->node->UID == nodeUID) {
            return scopeTable->child[i];
        }
    }
    printf("child with UID%d not found\n", nodeUID);
    return NULL;
}

void AddChildScope (ScopeTable * parentScopeTable, ScopeTable * newScopeTable) {
    newScopeTable->parent = parentScopeTable;
    int i;
    for(i = 0; i < SCOPE_MAX_CHILDREN; i++) {
        if(parentScopeTable->child[i] == NULL) {
            parentScopeTable->child[i] = newScopeTable;
            newScopeTable->depth = parentScopeTable->depth+1;
            return;
        }
    }
}

void PrintSymbolTable (ScopeTable * symbolTable) {
    if(PrintSymTblFlag == 0) return;
    int i;
    for(i = 0; i < symbolTable->depth; i++) {
        printf(".\t");
    }
    printf("%d: %s\n", symbolTable->depth, symbolTable->scopeName);
    SymbolTableEntry * cur = symbolTable->symbolTable;
    while(cur != NULL) {
        for(i = 0; i < symbolTable->depth; i++) {
            printf(".\t");
        }
        printf(" ");
        printf("entry %d: %s\n", cur->node->isDecl, cur->node->literal);
        cur = cur->next;
    }
    for(i = 0; i < SCOPE_MAX_CHILDREN; i++) {
        if(symbolTable->child[i] != NULL) {
            PrintSymbolTable(symbolTable->child[i]);
        }
    }
}

SymbolTableEntry * NewEntry (Node * node) {
    SymbolTableEntry * newEntry = (SymbolTableEntry *) malloc(sizeof(ScopeTable));
    if(newEntry == NULL) {
        printf("New entry out of memory\n");
        return NULL;
    } else {
        newEntry->next = NULL;
        newEntry->node = node;
        newEntry->following = NULL;
        int i;
        for(i = 0; i < ENTRY_MAX_FOLLOWERS; i++) {
            newEntry->followers[i] = NULL;
        }
        newEntry->isDecl = node->isDecl;
        return newEntry;
    }
}

int AddEntryToScope (SymbolTableEntry * entry, ScopeTable * scope) {
    // Connect to decl
    if(!entry->isDecl) {
        SymbolTableEntry * myDecl = FindDecl(entry, scope);
        if(myDecl != NULL) {
            entry->following = myDecl;
            int i;
            for(i = 0; i < ENTRY_MAX_FOLLOWERS; i++) {
                if(myDecl->followers[i] == NULL) {
                    myDecl->followers[i] = entry;
                }
            }
            if(!entry->following->node->isInitialized) {
                printf("%s may not be initialized\n", entry->node->literal);
            }
        } else {
            printf("%s wasn't declared, dumbo\n", entry->node->literal);
            free(entry);
            return 0;
        }
    }
    // Add to scope
    if(scope->symbolTable == NULL) {
        scope->symbolTable = entry;
    } else {
        // Abomination of a traversal if i ever saw one
        SymbolTableEntry * cur = scope->symbolTable;
        while(1) { // removed duplicate check. should probably rework
            if(cur->next != NULL) {
                cur = cur->next;
            } else {
                cur->next = entry;
                return 1;
            }
        }
    }
}

SymbolTableEntry * FindDecl(SymbolTableEntry * entry, ScopeTable * scope) {
    // Search this scope
    if(scope->symbolTable != NULL) {
        SymbolTableEntry * cur = scope->symbolTable;
        while(cur != NULL) {
            if(cur->isDecl) {
                if(strcmp(cur->node->literal, entry->node->literal) == 0) { // found match
                    return cur;
                }
            }
            cur = cur->next;
        }
    }
    // Scope Traversal
    if(scope->parent != NULL) { // traverse up
        return FindDecl(entry, scope->parent);
    } else { // you've reached the top
        return NULL;
    }
}