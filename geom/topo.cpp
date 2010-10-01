/// \file topo.cpp
/// \brief contains automatically generated refinement rules
/// \author LNM RWTH Aachen: Sven Gross, Joerg Peters, Volker Reichelt; SC RWTH Aachen:

/*
 * This file is part of DROPS.
 *
 * DROPS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * DROPS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with DROPS. If not, see <http://www.gnu.org/licenses/>.
 *
 *
 * Copyright 2009 LNM/SC RWTH Aachen, Germany
*/

/// \remarks generated from 'topo.cpp.in' by 'maketopo'

#include "geom/topo.h"

namespace DROPS
{

const Ubyte VertOfFaceAr[NumAllFacesC][3]= {
    {1,2,3}, {0,2,3}, {0,1,3}, {0,1,2},
    {1,6,8}, {6,2,9}, {6,8,9}, {8,9,3}, {1,2,9}, {1,9,3}, {1,2,8}, {2,8,3}, {1,6,3}, {6,2,3}, {2,8,9}, {6,8,3}, {6,9,3},
    {0,5,7}, {5,2,9}, {5,7,9}, {7,9,3}, {0,2,9}, {0,9,3}, {0,2,7}, {2,7,3}, {0,5,3}, {5,2,3}, {2,7,9}, {5,7,3}, {5,9,3},
    {0,4,7}, {4,1,8}, {4,7,8}, {7,8,3}, {0,1,8}, {0,8,3}, {0,1,7}, {1,7,3}, {0,4,3}, {4,1,3}, {1,7,8}, {4,7,3}, {4,8,3},
    {0,4,5}, {4,1,6}, {4,5,6}, {5,6,2}, {0,1,6}, {0,6,2}, {0,1,5}, {1,5,2}, {0,4,2}, {4,1,2}, {1,5,6}, {4,5,2}, {4,6,2},
    {0,1,9}, {0,2,8}, {0,4,9}, {0,5,8}, {0,6,3}, {0,6,7}, {0,6,8}, {0,6,9},
    {0,8,9}, {1,2,7}, {1,5,3}, {1,5,7}, {1,5,8}, {1,5,9}, {1,6,7}, {1,7,9},
    {2,7,8}, {4,1,9}, {4,2,3}, {4,2,7}, {4,2,8}, {4,2,9}, {4,5,3}, {4,5,7},
    {4,5,8}, {4,5,9}, {4,6,3}, {4,6,7}, {4,6,8}, {4,6,9}, {4,7,9}, {4,8,9},
    {4,9,3}, {5,2,8}, {5,6,3}, {5,6,7}, {5,6,8}, {5,6,9}, {5,7,8}, {5,8,3},
    {5,8,9}, {6,2,7}, {6,7,3}, {6,7,8}, {6,7,9}, {7,8,9} };

const Ubyte VertOfEdgeAr[NumAllEdgesC][2] = {
    {0,1}, {0,2}, {1,2}, {0,3}, {1,3}, {2,3},
    {0,4}, {4,1}, {0,5}, {5,2}, {1,6}, {6,2}, {0,7}, {7,3}, {1,8}, {8,3}, {2,9}, {9,3},
    {1,9}, {2,8}, {6,3}, {0,9}, {2,7}, {5,3}, {0,8}, {1,7}, {4,3}, {0,6}, {1,5}, {4,2},
    {6,8}, {6,9}, {8,9}, {5,7}, {5,9}, {7,9}, {4,7}, {4,8}, {7,8}, {4,5}, {4,6}, {5,6},
    {5,8}, {4,9}, {6,7} };

const byte EdgeByVertAr[NumAllVertsC][NumAllVertsC] = {
    {-1,  0,  1,  3,  6,  8, 27, 12, 24, 21},
    { 0, -1,  2,  4,  7, 28, 10, 25, 14, 18},
    { 1,  2, -1,  5, 29,  9, 11, 22, 19, 16},
    { 3,  4,  5, -1, 26, 23, 20, 13, 15, 17},
    { 6,  7, 29, 26, -1, 39, 40, 36, 37, 43},
    { 8, 28,  9, 23, 39, -1, 41, 33, 42, 34},
    {27, 10, 11, 20, 40, 41, -1, 44, 30, 31},
    {12, 25, 22, 13, 36, 33, 44, -1, 38, 35},
    {24, 14, 19, 15, 37, 42, 30, 38, -1, 32},
    {21, 18, 16, 17, 43, 34, 31, 35, 32, -1} };



const byte RefRuleAr[]= {
    6,    	0, 1, 2, 3, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    4,    	0, 1, 2, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    1,    	8, -1, -1, -1, -1, -1, -1, -1,

    9,    	1, 2, 3, 4, 5, 6, 7, 26, 29, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    7,    	0, 1, 38, 39, 51, 52, 74, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    2,    	21, 52, -1, -1, -1, -1, -1, -1,

    9,    	0, 2, 3, 4, 5, 8, 9, 23, 28, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    7,    	0, 2, 25, 26, 49, 50, 66, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    2,    	12, 37, -1, -1, -1, -1, -1, -1,

    12,    	2, 3, 4, 5, 6, 7, 8, 9, 23, 26, 29, 39, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    10,    	0, 25, 26, 38, 39, 43, 52, 54, 74, 78, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    3,    	24, 52, 62, -1, -1, -1, -1, -1,

    9,    	0, 1, 3, 4, 5, 10, 11, 20, 27, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    7,    	1, 2, 12, 13, 47, 48, 60, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    2,    	15, 27, -1, -1, -1, -1, -1, -1,

    12,    	1, 3, 4, 5, 6, 7, 10, 11, 20, 26, 29, 40, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    10,    	1, 12, 13, 38, 39, 44, 51, 55, 74, 82, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    3,    	21, 55, 71, -1, -1, -1, -1, -1,

    12,    	0, 3, 4, 5, 8, 9, 10, 11, 20, 23, 28, 41, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    10,    	2, 12, 13, 25, 26, 46, 49, 53, 66, 90, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    3,    	12, 40, 83, -1, -1, -1, -1, -1,

    15,    	3, 4, 5, 6, 7, 8, 9, 10, 11, 20, 23, 26, 39, 40, 41, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    13,    	12, 13, 25, 26, 38, 39, 43, 44, 45, 46, 78, 82, 90, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    4,    	24, 55, 65, 83, -1, -1, -1, -1,

    9,    	0, 1, 2, 4, 5, 12, 13, 22, 25, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    7,    	0, 3, 23, 24, 36, 37, 65, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    2,    	9, 34, -1, -1, -1, -1, -1, -1,

    12,    	1, 2, 4, 5, 6, 7, 12, 13, 22, 26, 29, 36, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    10,    	0, 23, 24, 30, 39, 41, 51, 52, 74, 75, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    3,    	22, 52, 57, -1, -1, -1, -1, -1,

    12,    	0, 2, 4, 5, 8, 9, 12, 13, 23, 25, 28, 33, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    10,    	0, 17, 26, 28, 36, 37, 49, 50, 66, 67, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    3,    	13, 37, 43, -1, -1, -1, -1, -1,

    15,    	2, 4, 5, 6, 7, 8, 9, 12, 13, 23, 26, 29, 33, 36, 39, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    13,    	0, 17, 26, 28, 30, 39, 41, 43, 52, 54, 74, 78, 79, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    4,    	0, 52, 62, 67, -1, -1, -1, -1,

    13,    	0, 1, 4, 5, 10, 11, 12, 13, 20, 22, 25, 27, 44, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    12,    	12, 13, 23, 24, 36, 37, 47, 48, 61, 70, 97, 98, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    4,    	16, 28, 47, 89, -1, -1, -1, -1,

    15,    	1, 4, 5, 6, 7, 10, 11, 12, 13, 20, 22, 26, 29, 36, 40, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    13,    	12, 13, 23, 24, 30, 39, 41, 44, 51, 55, 74, 75, 82, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    4,    	22, 55, 57, 71, -1, -1, -1, -1,

    15,    	0, 4, 5, 8, 9, 10, 11, 12, 13, 20, 23, 25, 28, 33, 41, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    13,    	12, 13, 17, 26, 28, 36, 37, 46, 49, 53, 66, 67, 90, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    4,    	13, 40, 43, 83, -1, -1, -1, -1,

    18,    	4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 20, 23, 26, 33, 36, 39, 40, 41, -1, -1, -1, -1, -1, -1, -1,
    16,    	12, 13, 17, 26, 28, 30, 39, 41, 43, 44, 45, 46, 78, 79, 82, 90, -1, -1, -1, -1, -1, -1, -1, -1,
    5,    	0, 55, 65, 67, 83, -1, -1, -1,

    9,    	0, 1, 2, 3, 5, 14, 15, 19, 24, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    7,    	1, 3, 10, 11, 34, 35, 57, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    2,    	10, 19, -1, -1, -1, -1, -1, -1,

    12,    	1, 2, 3, 5, 6, 7, 14, 15, 19, 26, 29, 37, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    10,    	1, 10, 11, 31, 38, 42, 51, 52, 74, 76, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    3,    	21, 53, 60, -1, -1, -1, -1, -1,

    13,    	0, 2, 3, 5, 8, 9, 14, 15, 19, 23, 24, 28, 42, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    12,    	10, 11, 25, 26, 34, 35, 49, 50, 59, 68, 89, 95, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    4,    	14, 26, 38, 81, -1, -1, -1, -1,

    15,    	2, 3, 5, 6, 7, 8, 9, 14, 15, 19, 23, 26, 29, 37, 39, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    13,    	10, 11, 25, 26, 31, 38, 42, 43, 52, 54, 74, 76, 78, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    4,    	24, 53, 60, 62, -1, -1, -1, -1,

    12,    	0, 1, 3, 5, 10, 11, 14, 15, 20, 24, 27, 30, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    10,    	1, 4, 13, 15, 34, 35, 47, 48, 60, 62, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    3,    	17, 27, 30, -1, -1, -1, -1, -1,

    15,    	1, 3, 5, 6, 7, 10, 11, 14, 15, 20, 26, 29, 30, 37, 40, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    13,    	1, 4, 13, 15, 31, 38, 42, 44, 51, 55, 74, 82, 84, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    4,    	1, 21, 71, 75, -1, -1, -1, -1,

    16,    	0, 3, 5, 8, 9, 10, 11, 14, 15, 20, 23, 24, 28, 30, 41, 42, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    15,    	4, 13, 15, 25, 26, 34, 35, 46, 49, 53, 59, 68, 90, 92, 95, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    5,    	14, 26, 42, 83, 85, -1, -1, -1,

    18,    	3, 5, 6, 7, 8, 9, 10, 11, 14, 15, 20, 23, 26, 30, 37, 39, 40, 41, -1, -1, -1, -1, -1, -1, -1,
    16,    	4, 13, 15, 25, 26, 31, 38, 42, 43, 44, 45, 46, 78, 82, 84, 90, -1, -1, -1, -1, -1, -1, -1, -1,
    5,    	1, 24, 65, 75, 83, -1, -1, -1,

    12,    	0, 1, 2, 5, 12, 13, 14, 15, 19, 22, 25, 38, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    10,    	3, 10, 11, 23, 24, 33, 36, 40, 65, 72, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    3,    	9, 35, 50, -1, -1, -1, -1, -1,

    15,    	1, 2, 5, 6, 7, 12, 13, 14, 15, 19, 22, 29, 36, 37, 38, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    13,    	10, 11, 23, 24, 30, 31, 32, 33, 51, 52, 72, 75, 76, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    4,    	22, 50, 53, 58, -1, -1, -1, -1,

    16,    	0, 2, 5, 8, 9, 12, 13, 14, 15, 19, 23, 25, 28, 33, 38, 42, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    15,    	10, 11, 17, 26, 28, 33, 36, 40, 49, 50, 67, 68, 89, 94, 95, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    5,    	13, 38, 44, 81, 87, -1, -1, -1,

    19,    	2, 5, 6, 7, 8, 9, 12, 13, 14, 15, 19, 23, 29, 33, 36, 37, 38, 39, 42, -1, -1, -1, -1, -1, -1,
    18,    	10, 11, 17, 26, 28, 30, 31, 32, 33, 43, 52, 54, 76, 79, 80, 89, 94, 95, -1, -1, -1, -1, -1, -1,
    6,    	0, 3, 53, 63, 81, 87, -1, -1,

    16,    	0, 1, 5, 10, 11, 12, 13, 14, 15, 20, 22, 25, 27, 30, 38, 44, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    15,    	4, 13, 15, 23, 24, 33, 36, 40, 47, 48, 61, 70, 97, 98, 99, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    5,    	16, 28, 48, 89, 91, -1, -1, -1,

    19,    	1, 5, 6, 7, 10, 11, 12, 13, 14, 15, 20, 22, 29, 30, 36, 37, 38, 40, 44, -1, -1, -1, -1, -1, -1,
    18,    	4, 13, 15, 23, 24, 30, 31, 32, 33, 44, 51, 55, 75, 83, 84, 97, 98, 99, -1, -1, -1, -1, -1, -1,
    6,    	1, 22, 72, 74, 89, 91, -1, -1,

    19,    	0, 5, 8, 9, 10, 11, 12, 13, 14, 15, 20, 23, 25, 28, 30, 33, 38, 41, 42, -1, -1, -1, -1, -1, -1,
    18,    	4, 13, 15, 17, 26, 28, 33, 36, 40, 46, 49, 53, 67, 68, 90, 92, 94, 95, -1, -1, -1, -1, -1, -1,
    6,    	13, 42, 44, 83, 85, 87, -1, -1,

    22,    	5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 20, 23, 30, 33, 36, 37, 38, 39, 40, 41, 42, -1, -1, -1,
    21,    	4, 13, 15, 17, 26, 28, 30, 31, 32, 33, 43, 44, 45, 46, 79, 80, 84, 90, 92, 94, 95, -1, -1, -1,
    7,    	0, 1, 2, 3, 83, 85, 87, -1,

    9,    	0, 1, 2, 3, 4, 16, 17, 18, 21, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    7,    	2, 3, 8, 9, 21, 22, 56, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    2,    	11, 18, -1, -1, -1, -1, -1, -1,

    13,    	1, 2, 3, 4, 6, 7, 16, 17, 18, 21, 26, 29, 43, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    12,    	8, 9, 21, 22, 38, 39, 51, 52, 58, 73, 77, 88, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    4,    	23, 25, 54, 56, -1, -1, -1, -1,

    12,    	0, 2, 3, 4, 8, 9, 16, 17, 18, 23, 28, 34, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    10,    	2, 8, 9, 18, 25, 29, 49, 50, 66, 69, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    3,    	12, 39, 46, -1, -1, -1, -1, -1,

    16,    	2, 3, 4, 6, 7, 8, 9, 16, 17, 18, 23, 26, 29, 34, 39, 43, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    15,    	8, 9, 18, 25, 29, 38, 39, 43, 52, 54, 73, 77, 78, 81, 88, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    5,    	24, 54, 56, 64, 70, -1, -1, -1,

    12,    	0, 1, 3, 4, 10, 11, 16, 17, 20, 21, 27, 31, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    10,    	2, 5, 12, 16, 21, 22, 47, 48, 60, 63, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    3,    	15, 29, 32, -1, -1, -1, -1, -1,

    16,    	1, 3, 4, 6, 7, 10, 11, 16, 17, 20, 21, 26, 29, 31, 40, 43, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    15,    	5, 12, 16, 21, 22, 38, 39, 44, 51, 55, 58, 77, 82, 85, 88, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    5,    	23, 25, 55, 73, 77, -1, -1, -1,

    15,    	0, 3, 4, 8, 9, 10, 11, 16, 17, 20, 23, 28, 31, 34, 41, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    13,    	2, 5, 12, 16, 18, 25, 29, 46, 49, 53, 66, 90, 93, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    4,    	4, 12, 40, 86, -1, -1, -1, -1,

    18,    	3, 4, 6, 7, 8, 9, 10, 11, 16, 17, 20, 23, 26, 31, 34, 39, 40, 41, -1, -1, -1, -1, -1, -1, -1,
    16,    	5, 12, 16, 18, 25, 29, 38, 39, 43, 44, 45, 46, 78, 82, 90, 93, -1, -1, -1, -1, -1, -1, -1, -1,
    5,    	4, 24, 55, 65, 86, -1, -1, -1,

    12,    	0, 1, 2, 4, 12, 13, 16, 17, 18, 22, 25, 35, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    10,    	3, 8, 9, 20, 23, 27, 36, 37, 65, 71, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    3,    	9, 36, 49, -1, -1, -1, -1, -1,

    16,    	1, 2, 4, 6, 7, 12, 13, 16, 17, 18, 22, 26, 29, 35, 36, 43, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    15,    	8, 9, 20, 23, 27, 30, 39, 41, 51, 52, 73, 75, 77, 86, 88, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    5,    	22, 54, 56, 59, 79, -1, -1, -1,

    15,    	0, 2, 4, 8, 9, 12, 13, 16, 17, 18, 25, 28, 33, 34, 35, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    13,    	8, 9, 17, 18, 19, 20, 36, 37, 49, 50, 67, 69, 71, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    4,    	13, 39, 45, 49, -1, -1, -1, -1,

    19,    	2, 4, 6, 7, 8, 9, 12, 13, 16, 17, 18, 26, 29, 33, 34, 35, 36, 39, 43, -1, -1, -1, -1, -1, -1,
    18,    	8, 9, 17, 18, 19, 20, 30, 39, 41, 43, 52, 54, 73, 77, 79, 81, 86, 88, -1, -1, -1, -1, -1, -1,
    6,    	0, 54, 56, 64, 68, 79, -1, -1,

    16,    	0, 1, 4, 10, 11, 12, 13, 16, 17, 20, 22, 25, 27, 31, 35, 44, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    15,    	5, 12, 16, 20, 23, 27, 36, 37, 47, 48, 61, 70, 97, 98, 100, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    5,    	16, 28, 47, 90, 93, -1, -1, -1,

    19,    	1, 4, 6, 7, 10, 11, 12, 13, 16, 17, 20, 22, 26, 29, 31, 35, 36, 40, 43, -1, -1, -1, -1, -1, -1,
    18,    	5, 12, 16, 20, 23, 27, 30, 39, 41, 44, 51, 55, 75, 77, 82, 85, 86, 88, -1, -1, -1, -1, -1, -1,
    6,    	22, 55, 59, 73, 77, 79, -1, -1,

    19,    	0, 4, 8, 9, 10, 11, 12, 13, 16, 17, 20, 25, 28, 31, 33, 34, 35, 41, 44, -1, -1, -1, -1, -1, -1,
    18,    	5, 12, 16, 17, 18, 19, 20, 36, 37, 46, 49, 53, 67, 70, 91, 93, 98, 100, -1, -1, -1, -1, -1, -1,
    6,    	4, 13, 41, 47, 84, 93, -1, -1,

    22,    	4, 6, 7, 8, 9, 10, 11, 12, 13, 16, 17, 20, 26, 31, 33, 34, 35, 36, 39, 40, 41, 43, -1, -1, -1,
    21,    	5, 12, 16, 17, 18, 19, 20, 30, 39, 41, 43, 44, 45, 46, 79, 81, 82, 85, 86, 88, 93, -1, -1, -1,
    7,    	0, 4, 55, 66, 68, 77, 79, -1,

    12,    	0, 1, 2, 3, 14, 15, 16, 17, 19, 21, 24, 32, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    10,    	3, 7, 10, 14, 21, 22, 34, 35, 57, 64, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    3,    	10, 20, 33, -1, -1, -1, -1, -1,

    16,    	1, 2, 3, 6, 7, 14, 15, 16, 17, 19, 21, 26, 29, 32, 37, 43, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    15,    	7, 10, 14, 21, 22, 31, 38, 42, 51, 52, 58, 76, 77, 87, 88, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    5,    	23, 25, 53, 61, 80, -1, -1, -1,

    16,    	0, 2, 3, 8, 9, 14, 15, 16, 17, 19, 23, 24, 28, 32, 34, 42, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    15,    	7, 10, 14, 18, 25, 29, 34, 35, 49, 50, 59, 68, 89, 95, 96, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    5,    	14, 26, 38, 82, 88, -1, -1, -1,

    19,    	2, 3, 6, 7, 8, 9, 14, 15, 16, 17, 19, 23, 26, 29, 32, 34, 37, 39, 42, -1, -1, -1, -1, -1, -1,
    18,    	7, 10, 14, 18, 25, 29, 31, 38, 42, 43, 52, 54, 76, 78, 80, 89, 95, 96, -1, -1, -1, -1, -1, -1,
    6,    	24, 53, 63, 69, 82, 88, -1, -1,

    15,    	0, 1, 3, 10, 11, 14, 15, 16, 17, 21, 24, 27, 30, 31, 32, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    13,    	4, 5, 6, 7, 21, 22, 34, 35, 47, 48, 62, 63, 64, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    4,    	17, 29, 31, 33, -1, -1, -1, -1,

    19,    	1, 3, 6, 7, 10, 11, 14, 15, 16, 17, 21, 26, 29, 30, 31, 32, 37, 40, 43, -1, -1, -1, -1, -1, -1,
    18,    	4, 5, 6, 7, 21, 22, 31, 38, 42, 44, 51, 55, 58, 77, 84, 85, 87, 88, -1, -1, -1, -1, -1, -1,
    6,    	1, 23, 25, 73, 76, 80, -1, -1,

    19,    	0, 3, 8, 9, 10, 11, 14, 15, 16, 17, 23, 24, 28, 30, 31, 32, 34, 41, 42, -1, -1, -1, -1, -1, -1,
    18,    	4, 5, 6, 7, 18, 25, 29, 34, 35, 46, 49, 53, 59, 68, 92, 93, 95, 96, -1, -1, -1, -1, -1, -1,
    6,    	4, 5, 14, 26, 42, 88, -1, -1,

    22,    	3, 6, 7, 8, 9, 10, 11, 14, 15, 16, 17, 23, 26, 30, 31, 32, 34, 37, 39, 40, 41, 42, -1, -1, -1,
    21,    	4, 5, 6, 7, 18, 25, 29, 31, 38, 42, 43, 44, 45, 46, 78, 80, 84, 92, 93, 95, 96, -1, -1, -1,
    7,    	1, 2, 4, 5, 24, 69, 88, -1,

    15,    	0, 1, 2, 12, 13, 14, 15, 16, 17, 19, 22, 25, 32, 35, 38, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    13,    	3, 7, 10, 14, 20, 23, 27, 33, 36, 40, 65, 72, 101, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    4,    	7, 9, 35, 51, -1, -1, -1, -1,

    18,    	1, 2, 6, 7, 12, 13, 14, 15, 16, 17, 19, 22, 29, 32, 35, 36, 37, 38, -1, -1, -1, -1, -1, -1, -1,
    16,    	7, 10, 14, 20, 23, 27, 30, 31, 32, 33, 51, 52, 72, 75, 76, 101, -1, -1, -1, -1, -1, -1, -1, -1,
    5,    	7, 22, 51, 53, 58, -1, -1, -1,

    19,    	0, 2, 8, 9, 12, 13, 14, 15, 16, 17, 19, 25, 28, 32, 33, 34, 35, 38, 42, -1, -1, -1, -1, -1, -1,
    18,    	7, 10, 14, 17, 18, 19, 20, 33, 36, 40, 49, 50, 67, 68, 89, 94, 96, 101, -1, -1, -1, -1, -1, -1,
    6,    	6, 7, 13, 38, 44, 82, -1, -1,

    22,    	2, 6, 7, 8, 9, 12, 13, 14, 15, 16, 17, 19, 29, 32, 33, 34, 35, 36, 37, 38, 39, 42, -1, -1, -1,
    21,    	7, 10, 14, 17, 18, 19, 20, 30, 31, 32, 33, 43, 52, 54, 76, 79, 80, 89, 94, 96, 101, -1, -1, -1,
    7,    	0, 3, 6, 7, 53, 63, 82, -1,

    19,    	0, 1, 10, 11, 12, 13, 14, 15, 16, 17, 22, 25, 27, 30, 31, 32, 35, 38, 44, -1, -1, -1, -1, -1, -1,
    18,    	4, 5, 6, 7, 20, 23, 27, 33, 36, 40, 47, 48, 61, 70, 97, 99, 100, 101, -1, -1, -1, -1, -1, -1,
    6,    	7, 16, 28, 48, 90, 92, -1, -1,

    22,    	1, 6, 7, 10, 11, 12, 13, 14, 15, 16, 17, 22, 29, 30, 31, 32, 35, 36, 37, 38, 40, 43, -1, -1, -1,
    21,    	4, 5, 6, 7, 20, 23, 27, 30, 31, 32, 33, 44, 51, 55, 75, 77, 84, 85, 86, 87, 101, -1, -1, -1,
    7,    	1, 7, 22, 59, 73, 76, 78, -1,

    22,    	0, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 25, 28, 30, 31, 32, 33, 34, 35, 38, 41, 42, -1, -1, -1,
    21,    	4, 5, 6, 7, 17, 18, 19, 20, 33, 36, 40, 46, 49, 53, 67, 68, 92, 93, 94, 96, 101, -1, -1, -1,
    7,    	4, 5, 6, 7, 13, 42, 44, -1,

    25,    	6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42,
    24,    	4, 5, 6, 7, 17, 18, 19, 20, 30, 31, 32, 33, 43, 44, 45, 46, 79, 80, 84, 92, 93, 94, 96, 101,
    8,    	0, 1, 2, 3, 4, 5, 6, 7
    };


const byte ChildDataAr[]= {
    0, 4, 5, 7,     6, 8, 39, 12, 36, 33,     79, 17, 30, 43,
    4, 1, 6, 8,     7, 40, 10, 37, 14, 30,     4, 84, 31, 44,
    4, 5, 6, 8,     39, 40, 41, 37, 42, 30,     92, 84, 80, 45,
    4, 5, 7, 8,     39, 36, 33, 37, 42, 38,     94, 32, 80, 79,
    5, 6, 2, 9,     41, 9, 11, 34, 31, 16,     5, 18, 93, 46,
    5, 6, 8, 9,     41, 42, 30, 34, 31, 32,     6, 96, 93, 92,
    5, 7, 8, 9,     33, 42, 38, 34, 35, 32,     101, 96, 19, 94,
    7, 8, 9, 3,     38, 35, 32, 13, 15, 17,     7, 20, 33, 101,
    0, 1, 2, 3,     0, 1, 2, 3, 4, 5,     0, 1, 2, 3,
    0, 1, 2, 7,     0, 1, 2, 12, 25, 22,     65, 23, 36, 3,
    0, 1, 2, 8,     0, 1, 2, 24, 14, 19,     10, 57, 34, 3,
    0, 1, 2, 9,     0, 1, 2, 21, 18, 16,     8, 21, 56, 3,
    0, 1, 5, 3,     0, 8, 28, 3, 4, 23,     66, 25, 2, 49,
    0, 1, 5, 7,     0, 8, 28, 12, 25, 33,     67, 17, 36, 49,
    0, 1, 5, 8,     0, 8, 28, 24, 14, 42,     68, 59, 34, 49,
    0, 1, 6, 3,     0, 27, 10, 3, 4, 20,     12, 60, 2, 47,
    0, 1, 6, 7,     0, 27, 10, 12, 25, 44,     70, 61, 36, 47,
    0, 1, 6, 8,     0, 27, 10, 24, 14, 30,     4, 62, 34, 47,
    0, 1, 9, 3,     0, 21, 18, 3, 4, 17,     9, 22, 2, 56,
    0, 2, 8, 3,     1, 24, 19, 3, 5, 15,     11, 35, 1, 57,
    0, 2, 8, 9,     1, 24, 19, 21, 16, 32,     14, 64, 21, 57,
    0, 4, 2, 3,     6, 1, 29, 3, 26, 5,     74, 1, 38, 51,
    0, 4, 2, 7,     6, 1, 29, 12, 36, 22,     75, 23, 30, 51,
    0, 4, 2, 9,     6, 1, 29, 21, 43, 16,     77, 21, 58, 51,
    0, 4, 5, 3,     6, 8, 39, 3, 26, 23,     78, 25, 38, 43,
    0, 4, 9, 3,     6, 21, 43, 3, 26, 17,     88, 22, 38, 58,
    0, 5, 8, 3,     8, 24, 42, 3, 23, 15,     95, 35, 25, 59,
    0, 6, 2, 3,     27, 1, 11, 3, 20, 5,     13, 1, 60, 48,
    0, 6, 2, 7,     27, 1, 11, 12, 44, 22,     97, 23, 61, 48,
    0, 6, 2, 9,     27, 1, 11, 21, 31, 16,     5, 21, 63, 48,
    0, 6, 8, 3,     27, 24, 30, 3, 20, 15,     15, 35, 60, 62,
    0, 6, 8, 9,     27, 24, 30, 21, 31, 32,     6, 64, 63, 62,
    0, 6, 9, 3,     27, 21, 31, 3, 20, 17,     16, 22, 60, 63,
    0, 8, 9, 3,     24, 21, 32, 3, 15, 17,     7, 22, 35, 64,
    1, 2, 7, 3,     2, 25, 22, 4, 5, 13,     24, 37, 0, 65,
    1, 2, 7, 8,     2, 25, 22, 14, 19, 38,     72, 40, 10, 65,
    1, 2, 7, 9,     2, 25, 22, 18, 16, 35,     27, 71, 8, 65,
    1, 5, 2, 3,     28, 2, 9, 4, 23, 5,     26, 0, 66, 50,
    1, 5, 2, 8,     28, 2, 9, 14, 42, 19,     89, 10, 68, 50,
    1, 5, 2, 9,     28, 2, 9, 18, 34, 16,     18, 8, 69, 50,
    1, 5, 6, 3,     28, 10, 41, 4, 23, 20,     90, 12, 66, 53,
    1, 5, 6, 7,     28, 10, 41, 25, 33, 44,     91, 70, 67, 53,
    1, 5, 6, 8,     28, 10, 41, 14, 42, 30,     92, 4, 68, 53,
    1, 5, 7, 3,     28, 25, 33, 4, 23, 13,     28, 37, 66, 67,
    1, 5, 7, 8,     28, 25, 33, 14, 42, 38,     94, 40, 68, 67,
    1, 5, 7, 9,     28, 25, 33, 18, 34, 35,     19, 71, 69, 67,
    1, 5, 9, 3,     28, 18, 34, 4, 23, 17,     29, 9, 66, 69,
    1, 6, 7, 3,     10, 25, 44, 4, 20, 13,     98, 37, 12, 70,
    1, 6, 7, 8,     10, 25, 44, 14, 30, 38,     99, 40, 4, 70,
    1, 7, 9, 3,     25, 18, 35, 4, 13, 17,     20, 9, 37, 71,
    2, 7, 8, 3,     22, 19, 38, 5, 13, 15,     33, 11, 24, 72,
    2, 7, 8, 9,     22, 19, 38, 16, 35, 32,     101, 14, 27, 72,
    4, 1, 2, 3,     7, 29, 2, 26, 4, 5,     0, 74, 39, 52,
    4, 1, 2, 8,     7, 29, 2, 37, 14, 19,     10, 76, 31, 52,
    4, 1, 2, 9,     7, 29, 2, 43, 18, 16,     8, 77, 73, 52,
    4, 1, 6, 3,     7, 40, 10, 26, 4, 20,     12, 82, 39, 44,
    4, 1, 9, 3,     7, 43, 18, 26, 4, 17,     9, 88, 39, 73,
    4, 2, 7, 3,     29, 36, 22, 26, 5, 13,     24, 41, 74, 75,
    4, 2, 7, 8,     29, 36, 22, 37, 19, 38,     72, 32, 76, 75,
    4, 2, 7, 9,     29, 36, 22, 43, 16, 35,     27, 86, 77, 75,
    4, 2, 8, 3,     29, 37, 19, 26, 5, 15,     11, 42, 74, 76,
    4, 2, 8, 9,     29, 37, 19, 43, 16, 32,     14, 87, 77, 76,
    4, 5, 2, 3,     39, 29, 9, 26, 23, 5,     26, 74, 78, 54,
    4, 5, 2, 8,     39, 29, 9, 37, 42, 19,     89, 76, 80, 54,
    4, 5, 2, 9,     39, 29, 9, 43, 34, 16,     18, 77, 81, 54,
    4, 5, 6, 3,     39, 40, 41, 26, 23, 20,     90, 82, 78, 45,
    4, 5, 6, 9,     39, 40, 41, 43, 34, 31,     93, 85, 81, 45,
    4, 5, 7, 3,     39, 36, 33, 26, 23, 13,     28, 41, 78, 79,
    4, 5, 7, 9,     39, 36, 33, 43, 34, 35,     19, 86, 81, 79,
    4, 5, 8, 3,     39, 37, 42, 26, 23, 15,     95, 42, 78, 80,
    4, 5, 9, 3,     39, 43, 34, 26, 23, 17,     29, 88, 78, 81,
    4, 6, 2, 3,     40, 29, 11, 26, 20, 5,     13, 74, 82, 55,
    4, 6, 2, 7,     40, 29, 11, 36, 44, 22,     97, 75, 83, 55,
    4, 6, 2, 9,     40, 29, 11, 43, 31, 16,     5, 77, 85, 55,
    4, 6, 7, 8,     40, 36, 44, 37, 30, 38,     99, 32, 84, 83,
    4, 6, 8, 3,     40, 37, 30, 26, 20, 15,     15, 42, 82, 84,
    4, 6, 8, 9,     40, 37, 30, 43, 31, 32,     6, 87, 85, 84,
    4, 6, 9, 3,     40, 43, 31, 26, 20, 17,     16, 88, 82, 85,
    4, 7, 8, 9,     36, 37, 38, 43, 35, 32,     101, 87, 86, 32,
    4, 7, 9, 3,     36, 43, 35, 26, 13, 17,     20, 88, 41, 86,
    4, 8, 9, 3,     37, 43, 32, 26, 15, 17,     7, 88, 42, 87,
    5, 2, 8, 3,     9, 42, 19, 23, 5, 15,     11, 95, 26, 89,
    5, 2, 8, 9,     9, 42, 19, 34, 16, 32,     14, 96, 18, 89,
    5, 6, 2, 3,     41, 9, 11, 23, 20, 5,     13, 26, 90, 46,
    5, 6, 7, 9,     41, 33, 44, 34, 31, 35,     100, 19, 93, 91,
    5, 6, 8, 3,     41, 42, 30, 23, 20, 15,     15, 95, 90, 92,
    5, 6, 9, 3,     41, 34, 31, 23, 20, 17,     16, 29, 90, 93,
    5, 7, 8, 3,     33, 42, 38, 23, 13, 15,     33, 95, 28, 94,
    5, 8, 9, 3,     42, 34, 32, 23, 15, 17,     7, 29, 95, 96,
    6, 2, 7, 3,     11, 44, 22, 20, 5, 13,     24, 98, 13, 97,
    6, 2, 7, 9,     11, 44, 22, 31, 16, 35,     27, 100, 5, 97,
    6, 7, 8, 3,     44, 30, 38, 20, 13, 15,     33, 15, 98, 99,
    6, 7, 8, 9,     44, 30, 38, 31, 35, 32,     101, 6, 100, 99,
    6, 7, 9, 3,     44, 31, 35, 20, 13, 17,     20, 16, 98, 100
    };


} // end of namespace DROPS
