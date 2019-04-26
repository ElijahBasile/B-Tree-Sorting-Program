/*
 * btree.h
 *
 *  Created on: Mar 31, 2019
 *      Author: Elijah Basile
 */

#ifndef BTREE_H_
#define BTREE_H_

#include <stdlib.h>
#include <stdbool.h>

// ====================== STRUCTURES =======================

typedef struct {
	int* dataPtr;
	struct node* rightPtr;
} ENTRY;

typedef struct node {
	struct node* firstPtr;
	int numEntries;
	ENTRY entries[2];
} NODE;

typedef struct {
	int count;
	NODE* root;
	int (*compare) (int* argu1,int* argu2);
} BTREE;

// ============== PROTOTYPE DECLARATIONS ====================

// User interfaces
BTREE* 	BTree_Create 	(int (*compare) (int* argu1, int* argu2));
void 	BTree_Traverse 	(BTREE* tree, void (*process)  (int* dataPtr));
BTREE*	BTree_Destroy 	(BTREE* tree);
void	BTree_Insert 	(BTREE* tree, int* dataInPtr);
bool 	BTree_Delete	(BTREE* tree, int *dltKey);
void*	BTree_Search	(BTREE* tree, int* dataPtr);
bool 	BTree_Empty		(BTREE* tree);
bool	BTree_Full 		(BTREE* tree);
int		BTree_Count		(BTREE* tree);


// Internal BTree functions

void* 	_search
					(BTREE* tree, int* targetPtr, NODE* target);
int	 	_searchNode
					(BTREE* tree, NODE* nodePtr, int* target);
bool	 _delete
					(BTREE* tree, NODE* root, int* dltKeyPtr, bool* success);
bool	 _insert
					(BTREE* tree, NODE* root, int* dataInPtr, ENTRY* upEntry);
void  	_traverse
					(NODE* root, void (*process) (int* dataPtr));
void 	_splitNode
					(NODE* root, int entryNdx, int compResult, ENTRY* upEntry);
void  	_insertEntry
					(NODE* root, int entryNdx, ENTRY upEntry);
bool  	_deleteEntry
					(NODE* node, int entryNdx);
bool  	_deleteMid
					(NODE* root, int entryNdx, NODE* leftPtr);
bool	_reflow
					(NODE* root, int entryNdx);
void	_borrowLeft
					(NODE* root, int entryNdx, NODE* leftTree, NODE* rightTree);
void  	_borrowRight
					(NODE* root, int entryNdx, NODE* leftTree, NODE* rightTree);
void  	_combine
					(NODE* root, int entryNdx, NODE* leftTree, NODE* rightTree);

#endif /* BTREE_H_ */
