/*
 * GraphEdge.cpp
 *
 *  Created on: Nov 13, 2016
 *      Author: jgoeders
 */

#include "NIGraphEdge.h"

NIGraphEdge::NIGraphEdge()
    : sourceNode(nullptr), destNode(nullptr), delay(0), isFeedback(false) {}

NIGraphEdge::~NIGraphEdge() {}
