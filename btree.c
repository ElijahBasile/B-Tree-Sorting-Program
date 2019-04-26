/*
 * btree.c
 *
 *  Created on: Mar 31, 2019
 *      Author: Elijah Basile
 */

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "btree.h"

// ================ CONSTANTS & MACROS =====================

const int ORDER = 3;
const int MIN_ENTRIES = (((ORDER + 1) / 2) - 1);

const int MAX = 1000;


// User interfaces
BTREE* 	BTree_Create 	(int (*compare) (int* argu1,int* argu2)) {
	BTREE* tree = (BTREE*)malloc(sizeof(BTREE));
	if (!tree)
		printf("Overflow error 101\a\n"), exit(100);
	tree->compare = compare;
	tree->root = NULL;
	tree->count = 0;

	return tree;
}

void 	BTree_Traverse 	(BTREE* tree, void (*process) (int* dataPtr)) {
	if (tree->root)
		_traverse(tree->root,process);
	return;
}

BTREE*	BTree_Destroy 	(BTREE* tree) {
	while (tree->count) {
		(tree->count)--;
		BTree_Delete(tree,tree->root->entries[0].dataPtr);
	}
	return tree;
}

void	BTree_Insert 	(BTREE* tree, int* dataInPtr) {
	bool taller;
	NODE* newPtr;
	ENTRY upEntry;

	if (tree->root == NULL) {
		// empty Tree. Insert first node
		if ((newPtr = (NODE*)malloc(sizeof (NODE)))) {
			newPtr->firstPtr = NULL;
			newPtr->numEntries = 1;
			newPtr->entries[0].dataPtr = dataInPtr;
			newPtr->entries[0].rightPtr = NULL;
			tree->root = newPtr;
			(tree->count)++;

			for (int i = 1; i < ORDER - 1; i++) {
				newPtr->entries[i].dataPtr = NULL;
				newPtr->entries[i].rightPtr = NULL;
			}
			return;
		} else {
			printf("Error 100 in BTree_Insert\a\n"), exit(100);
		}
	}
	taller = _insert(tree,tree->root,dataInPtr,&upEntry);

	if (taller) {
		// tree has grown. create new root
		newPtr = (NODE*)malloc(sizeof(NODE));
		if (newPtr) {
			newPtr->entries[0] = upEntry;
			newPtr->firstPtr = tree->root;
			newPtr->numEntries = 1;
			tree->root = newPtr;
		} else {
			printf("Overflow error 101\a\n"), exit(101);
		}
	}
	(tree->count)++;
	return;
}

bool 	BTree_Delete	(BTREE* tree, int *dltKey) {
	bool success;
	NODE* dltPtr;

	if (!tree->root)
		return false;

	_delete(tree,tree->root,dltKey,&success);

	if (success) {
		(tree->count)--;
		if (tree->root->numEntries == 0) {
			dltPtr = tree->root;
			tree->root = tree->root->firstPtr;
			free(dltPtr);
		} // root empty
	} // success
	return success;
}

void*	BTree_Search	(BTREE* tree, int* targetPtr) {
	if (tree->root)
		return _search (tree, targetPtr, tree->root);
	else
		return NULL;
}

bool 	BTree_Empty		(BTREE* tree) {
	if (tree->count == 0)
		return true;
	return false;
}

bool	BTree_Full 		(BTREE* tree) {
	if (tree->count == MAX)
		return true;
	return false;
}

int		BTree_Count		(BTREE* tree) {
	return tree->count;
}


// Internal BTree functions

void* _search (BTREE* tree, int* targetPtr, NODE* root) {
	int entryNo;

	if (!root)
		return NULL;

	if (tree->compare(targetPtr, root->entries[0].dataPtr) < 0)
		return _search(tree, targetPtr, root->firstPtr);

	entryNo = root->numEntries - 1;
	while (tree->compare(targetPtr,root->entries[entryNo].dataPtr) < 0)
		entryNo--;

	if (tree->compare(targetPtr, root->entries[entryNo].dataPtr) == 0)
		return (root->entries[entryNo].dataPtr);

	return (_search (tree, targetPtr, root->entries[entryNo].rightPtr));
} // _search

int	 _searchNode (BTREE* tree, NODE* nodePtr, int* target) {
	int walker;

	if (tree->compare(target,nodePtr->entries[0].dataPtr) < 0)
		return 0;

	walker = nodePtr->numEntries - 1;
	while (tree->compare(target,nodePtr->entries[walker].dataPtr) < 0)
		walker--;
	return walker;
}

bool	 _delete (BTREE* tree, NODE* root, int* dltKeyPtr, bool* success) {
	NODE* leftPtr;
	NODE* subtreePtr;
	int entryNdx;
	int underflow;

	if (!root) {
		*success = false;
		return false;
	} // null tree

	entryNdx = _searchNode (tree, root,dltKeyPtr);
	if (tree->compare(dltKeyPtr,root->entries[entryNdx].dataPtr) == 0) {
		// found entry to be deleted
		*success = true;
		if (root->entries[entryNdx].rightPtr == NULL)
			// entry is a leaf node
			underflow = _deleteEntry (root,entryNdx);
		else {
			if (entryNdx > 0)
				leftPtr = root->entries[entryNdx - 1].rightPtr;
			else
				leftPtr = root->firstPtr;
			underflow = _deleteMid (root, entryNdx, leftPtr);
			if (underflow)
				underflow = _reflow(root, entryNdx);
		} // else internal node
	} // else found entry
	else {
		if (tree->compare (dltKeyPtr,root->entries[0].dataPtr) < 0)
			// delete key < first entry
			subtreePtr = root->firstPtr;
		else
			// delete key in right subtree
			subtreePtr = root->entries[entryNdx].rightPtr;

		underflow = _delete (tree, subtreePtr, dltKeyPtr, success);

		if (underflow)
			underflow = _reflow (root, entryNdx);
	} // else not found

	return underflow;
} // _delete

bool	 _insert (BTREE* tree, NODE* root, int* dataInPtr, ENTRY* upEntry) {
	int compResult;
	int entryNdx;
	bool taller;

	NODE* subtreePtr;

	if (!root) {
		//leaf found -- build new entry
		(*upEntry).dataPtr = dataInPtr;
		(*upEntry).rightPtr = NULL;
		return true; // tree taller
	} // if null tree

	entryNdx = _searchNode (tree, root, dataInPtr);
	compResult = tree->compare(dataInPtr, root->entries[entryNdx].dataPtr);
	if (entryNdx <= 0 && compResult < 0)
		subtreePtr = root->firstPtr;
	else
		// in entry's right subtree
		subtreePtr = root->entries[entryNdx].rightPtr;
	taller = _insert(tree, subtreePtr, dataInPtr, upEntry);

	if (taller) {
		// need to create new node
		if (root->numEntries >= ORDER - 1) {
			_splitNode (root, entryNdx, compResult, upEntry);
			taller = true;
		} // node full
		else {
			if (compResult >= 0) { // new data >= current entry -- insert after
				_insertEntry(root, entryNdx + 1, *upEntry);
			} else {
				// insert before current entry
				_insertEntry (root, entryNdx, *upEntry);
			}
			(root->numEntries)++;
			taller = false;
		} // else
	} // if taller

	return taller;
}


void  _traverse (NODE* root, void (*process) (int* dataPtr)) {
	int scanCount;
	NODE* ptr;

	if (!root)
		return;

	scanCount = 0;
	ptr = root->firstPtr;

	while (scanCount <= root->numEntries) {
		if (ptr)
			_traverse (ptr, process);

		if (scanCount < root->numEntries) {
			process (root->entries[scanCount].dataPtr);
			ptr = root->entries[scanCount].rightPtr;
		}
		scanCount++;
	}
	return;
} // _traverse


void  _splitNode (NODE* node, int entryNdx, int compResult, ENTRY* upEntry) {
	int fromNdx;
	int toNdx;
	NODE* rightPtr;

	rightPtr = (NODE*)malloc(sizeof(NODE));
	if (!rightPtr)
		printf("Overflow error 101 in _splitNode\a\n"),exit(100);

	// build right subtree node
	if (entryNdx < MIN_ENTRIES)
		fromNdx = MIN_ENTRIES;
	else
		fromNdx = MIN_ENTRIES + 1;
	toNdx = 0;
	rightPtr->numEntries = node->numEntries - fromNdx;
	while (fromNdx < node->numEntries)
		rightPtr->entries[toNdx++] = node->entries[fromNdx++];
	node->numEntries = node->numEntries - rightPtr->numEntries;

	// insert new entry
	if (entryNdx < MIN_ENTRIES) {
		if (compResult < 0)
			_insertEntry (node, entryNdx, *upEntry);
		else
			_insertEntry (node, entryNdx + 1, *upEntry);
	} else {
		_insertEntry(rightPtr, entryNdx - MIN_ENTRIES, *upEntry);
		(rightPtr->numEntries)++;
		(node->numEntries)--;
	}

	upEntry->dataPtr = node->entries[MIN_ENTRIES].dataPtr;
	upEntry->rightPtr = rightPtr;
	rightPtr->firstPtr = node->entries[MIN_ENTRIES].rightPtr;

	return;
}


void  _insertEntry (NODE* node, int entryNdx, ENTRY upEntry) {
	int shifter;

	// place in middle -- shift entries
	shifter = node->numEntries - 1;
	while (shifter >= entryNdx) {
		node->entries[shifter + 1] = node->entries[shifter];
		shifter--;
	}
	node->entries[entryNdx] = upEntry;
}

bool  _deleteEntry (NODE* node, int entryNdx) {
	int shifter;
	bool underflow;

	shifter = entryNdx;
	while (shifter < node->numEntries - 2) {
		node->entries[shifter] = node->entries[shifter + 1];
		shifter++;
	}
	(node->numEntries)--;
	if (node->numEntries >= MIN_ENTRIES)
		underflow = false;
	else
		underflow = true;
	return underflow;
}



bool  _deleteMid (NODE* root, int entryNdx, NODE* subtreePtr) {
	int dltNdx;
	int rightNdx;
	bool underflow;

	if (subtreePtr->firstPtr == NULL) {
		// leaf located. exchange data and delete leaf
		dltNdx = subtreePtr->numEntries - 1;
		root->entries[entryNdx].dataPtr = subtreePtr->entries[dltNdx].dataPtr;
		--subtreePtr->numEntries;
		underflow = subtreePtr->numEntries < MIN_ENTRIES;
	} // if leaf
	else {
		// not located. traverse right for predecessor
		rightNdx = subtreePtr->numEntries - 1;
		underflow = _deleteMid (root, entryNdx, subtreePtr->entries[rightNdx].rightPtr);
		if (underflow)
			underflow = _reflow (subtreePtr, rightNdx);
	} // else traverse right
	return underflow;
}


bool	 _reflow (NODE* root, int entryNdx) {
	NODE* leftTreePtr;
	NODE* rightTreePtr;
	bool underflow;

	if (entryNdx == 0)
		leftTreePtr = root->firstPtr;
	else
		leftTreePtr = root->entries[entryNdx - 1].rightPtr;
	rightTreePtr = root->entries[entryNdx].rightPtr;

	// try to borrow first
	if (rightTreePtr->numEntries > MIN_ENTRIES) {
		_borrowRight (root, entryNdx, leftTreePtr, rightTreePtr);
		underflow = false;
	} // if borrow right
	else {
		// can't borrow from right -- try left
		if (leftTreePtr->numEntries > MIN_ENTRIES) {
			_borrowLeft (root, entryNdx, leftTreePtr, rightTreePtr);
			underflow = false;
		} // if borrow left
		else {
			// can't borrow. must combine nodes
			_combine (root, entryNdx, leftTreePtr, rightTreePtr);
			underflow = (root->numEntries < MIN_ENTRIES);
		} // else combine
	} // else borrow right
	return underflow;
} // _reflow

void	 _borrowLeft (NODE* root, int entryNdx, NODE* leftTreePtr, NODE* rightTreePtr) {
	int fromNdx;
	int shifter;

	// move parent and subtree pointer to right tree. shift entries left
	fromNdx = leftTreePtr->numEntries - 1;

	shifter = rightTreePtr->numEntries;
	while (shifter >= 0) {
		rightTreePtr->entries[shifter] = rightTreePtr->entries[shifter - 1];
		shifter--;
	}
	rightTreePtr->entries[0].dataPtr = root->entries[entryNdx].dataPtr;
	rightTreePtr->entries[0].rightPtr = rightTreePtr->firstPtr;
	rightTreePtr->firstPtr = leftTreePtr->entries[fromNdx].rightPtr;
	++rightTreePtr->numEntries;

	// move left data to parent
	root->entries[entryNdx].dataPtr = leftTreePtr->entries[fromNdx].dataPtr;


	// set left tree last pointer to the second to last one
	--leftTreePtr->numEntries;

	return;
}

void  _borrowRight (NODE* root, int entryNdx, NODE* leftTreePtr, NODE* rightTreePtr) {
	int toNdx;
	int shifter;

	// move parent and subtree pointer to left tree
	toNdx = leftTreePtr->numEntries;
	leftTreePtr->entries[toNdx].dataPtr = root->entries[entryNdx].dataPtr;
	leftTreePtr->entries[toNdx].rightPtr = rightTreePtr->firstPtr;
	++leftTreePtr->numEntries;

	// move right data to parent
	root->entries[entryNdx].dataPtr = rightTreePtr->entries[0].dataPtr;

	// set right tree first pointer. shift entries left
	rightTreePtr->firstPtr = rightTreePtr->entries[0].rightPtr;
	shifter = 0;
	while (shifter < rightTreePtr->numEntries - 1) {
		rightTreePtr->entries[shifter] = rightTreePtr->entries[shifter + 1];
		++shifter;
	} // while
	--rightTreePtr->numEntries;
	return;
} // _borrowRight

void  _combine (NODE* root, int entryNdx, NODE* leftTreePtr, NODE* rightTreePtr) {
	int toNdx;
	int fromNdx;
	int shifter;

	// move parent and set its right pointer from right tree
	toNdx = leftTreePtr->numEntries;
	leftTreePtr->entries[toNdx].dataPtr = root->entries[entryNdx].dataPtr;
	leftTreePtr->entries[toNdx].rightPtr = rightTreePtr->firstPtr;
	++leftTreePtr->numEntries;
	--root->numEntries;

	// move data from right tree to left tree
	fromNdx = 0;
	toNdx++;
	while(fromNdx < rightTreePtr->numEntries)
		leftTreePtr->entries[toNdx++] = rightTreePtr->entries[fromNdx++];
	leftTreePtr->numEntries += rightTreePtr->numEntries;
	free(rightTreePtr);

	//now shift data in root to left
	shifter = entryNdx;
	while (shifter < root->numEntries) {
		root->entries[shifter] = root->entries[shifter + 1];
		shifter++;
	} // while
	return;
}













