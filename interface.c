/*
 * interface.c
 *
 *  Created on: Mar 31, 2019
 *      Author: Elijah Basile
 */

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "interface.h"
#include "btree.h"


void menu() {
	BTREE* tree = BTree_Create(compare);

	// overwrite the output file and start fresh

	FILE* output = fopen("outfile.txt","w");

	fprintf(output,"==== Sorted List ====\n");

	fclose(output);

	// close file for future input

	convertData(tree);

	BTree_Traverse(tree,process);

	free(BTree_Destroy(tree));
}


void convertData(BTREE* tree) {
	FILE* fptr;
	char filename[256];
	bool isValid = false;
	char choice;

	do {
		fflush(stdin);
		printf("Enter the TXT file you wish to extract the numbers from.\n");
		scanf("%s",filename);
		if (!(fptr = fopen(filename,"r"))) {
			printf("******* File Not Found *******\n");
			printf("e:		Enter a new file name \n");
			printf("q:		Quit this Menu.		  \n");
			while (!isValid) {
				isValid = true;
				fflush(stdin);
				choice = getchar();
				switch (choice) {
					case 'e':
						break;
					case 'q':
						return;
					default:
						printf("Enter valid input.\n");
						isValid = false;
				}
			}
		} else
			break;
	} while (true);



	buildBTreeFromFile(tree,fptr);

	fclose(fptr);

}

// this builds tree from the file
void buildBTreeFromFile(BTREE* tree,FILE* fptr) {
	char* line = (char*)malloc(sizeof(char)*256);

	while(!feof(fptr)) {
		int* value = (int*)malloc(sizeof(int));
		fgets(line,256,fptr);
		*value = atoi(line);
		BTree_Insert(tree,value);
	}
	free(line);
}


int compare (int* argu1, int* argu2) {
	if (argu2 == NULL)
		return 1;
	if (*argu1 > *argu2)
		return 1;
	if (*argu1 < *argu2)
		return -1;
	else
		return 0;
}

void process (int* dataPtr) {
	FILE* output = fopen("outfile.txt","a");

	fprintf(output, "%d\n", *dataPtr);

	fclose(output);

	return;
}


