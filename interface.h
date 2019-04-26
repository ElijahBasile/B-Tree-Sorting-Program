/*
 * interface.h
 *
 *  Created on: Mar 31, 2019
 *      Author: Elijah Basile
 */

#ifndef INTERFACE_H_
#define INTERFACE_H_

#include "btree.h"

int compare (int*,int*);
void convertData(BTREE*);
void menu();
void process (int*);
void buildBTreeFromFile (BTREE*,FILE*);

#endif /* INTERFACE_H_ */
