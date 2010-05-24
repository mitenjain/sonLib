/*
 * eTree.c
 *
 *  Created on: 21 May 2010
 *      Author: benedictpaten
 */

#include "sonLibGlobalsPrivate.h"

ETree *eTree_construct() {
	ETree *eTree = mallocLocal(sizeof(ETree));
	eTree->branchLength = INFINITY;
	eTree->nodes = constructEmptyList(0, (void (*)(void *))eTree_destruct);
	eTree->label = NULL;
	eTree->parent = NULL;
	return eTree;
}

void eTree_destruct(ETree *eTree) {
	destructList(eTree->nodes);
	if(eTree->label != NULL) {
		free(eTree->label);
	}
	free(eTree);
}

ETree *eTree_getParent(ETree *eTree) {
	return eTree->parent;
}

void eTree_setParent(ETree *eTree, ETree *parent) {
	if(eTree_getParent(eTree) != NULL) {
		listRemove(eTree_getParent(eTree)->nodes, eTree);
	}
	eTree->parent = parent;
	if(parent != NULL) {
		listAppend(parent->nodes, parent);
	}
}

int32_t eTree_getChildNumber(ETree *eTree) {
	return eTree->nodes->length;
}

ETree *eTree_getChild(ETree *eTree, int32_t i) {
	assert(i >= 0);
	assert(i < eTree_getChildNumber(eTree));
	return eTree->nodes->list[i];
}

double eTree_getBranchLength(ETree *eTree) {
	return eTree->branchLength;
}

void eTree_setBranchLength(ETree *eTree, double distance) {
	eTree->branchLength = distance;
}

const char *eTree_getLabel(ETree *eTree) {
	return eTree->label;
}

void eTree_setLabel(ETree *eTree, const char *label) {
	if(eTree->label != NULL) {
		free(eTree->label);
	}
	eTree->label = label == NULL ? NULL : stringCopy(label);
}

/////////////////////////////
//Newick tree parser
/////////////////////////////

static void eTree_parseNewickString_getLabel(char **newickTreeString, ETree *eTree) {
	if(**newickTreeString != ':' && **newickTreeString != ',' && **newickTreeString != ';' && **newickTreeString != ')' && **newickTreeString != '\0') {
	    char *cA;
		eatString(*newickTreeString, &cA);
		eTree_setLabel(eTree, cA);
		free(cA);
	}
}

static void eTree_parseNewickString_getBranchLength(char **newickTreeString, ETree *eTree) {
    if(**newickTreeString != '\0') {
        if (**newickTreeString == ':') {
            (*newickTreeString)++;
            float distance;
            _parseFloat(eatWhiteSpace(newickTreeString), &distance);
            eTree_setBranchLength(eTree, distance);
        }
    }
}

static ETree *eTree_parseNewickStringP(char **newickTreeString) {
    ETree *eTree = eTree_construct();
    if(**newickTreeString == '(') {
        eatWhiteSpace(++(*newickTreeString));
        while(1) {
        	eTree_setParent(eTree_parseNewickStringP(newickTreeString), eTree);
            if(**newickTreeString == ',') {
                newickTreeString = eatWhiteSpace(++(*newickTreeString));
            }
            else {
            	assert(**newickTreeString == ')');
            	break;
            }
        }
        newickTreeString = eatWhiteSpace(++(*newickTreeString));
    }
    eTree_parseNewickString_getLabel(newickTreeString, eTree);
    eTree_parseNewickString_getBranchLength(newickTreeString, eTree);
    assert(**newickTreeString == ',' || **newickTreeString == ';' || **newickTreeString == ')' || **newickTreeString == '\0');
    return eTree;
}

ETree *eTree_parseNewickString(const char *string) {
	//lax newick tree parser
	char *newickTreeString = replaceString(string, '(', " ( ", 3);
	newickTreeString = replaceAndFreeString(newickTreeString, ')', " ) ", 3);
	newickTreeString = replaceAndFreeString(newickTreeString, ':', " : ", 3);
	newickTreeString = replaceAndFreeString(newickTreeString, ',', " , ", 3);
	newickTreeString = replaceAndFreeString(newickTreeString, ';', " ; ", 3);
	char *cA = newickTreeString;
	newickTreeString = eatWhiteSpace(newickTreeString);
	ETree *eTree = newickTreeParser_fn2(&newickTreeString);
	free(cA);
	return eTree;
}

/////////////////////////////
//Newick tree writer
/////////////////////////////

static char *eTree_getNewickTreeStringP(ETree *eTree) {
	char *cA, *cA2;
	if(eTree_getChildNumber(eTree) > 0) {
		int32_t i;
		cA = stringCopy("(");
		for(i=0; i<eTree_getChildNumber(eTree); i++) {
			cA2 = eTree_getNewickTreeStringP(eTree_getChild(eTree, i));
			char *cA3 = stringPrint((i+1 < eTree_getChildNumber(eTree) ? "%s%s," : "%s%s"), cA, cA2);
			free(cA);
			free(cA2);
			cA = cA3;
		}
		cA2 = stringPrint("%s)", cA);
		free(cA);
		cA = cA2;
	}
	else {
		cA = stringCopy("");
	}
	if(eTree_getLabel(eTree) != NULL) {
		cA2 = stringPrint("%s%s", cA, eTree_getLabel(eTree));
		free(cA);
		cA = cA2;
	}
	if(eTree_getBranchLength(eTree) != INFINITY) {
		char *cA2 = stringPrint("%s:%f", cA, eTree_getBranchLength(eTree));
		free(cA);
		cA = cA2;
	}
	return cA;
}

char *eTree_getNewickTreeString(ETree *eTree) {
	char *cA = eTree_getNewickTreeStringP(eTree), *cA2;
	cA2 = stringPrint("%s;", cA);
	free(cA);
	return cA2;
}
