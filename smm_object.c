//
//  smm_node.c
//  SMMarble
//
//  Created by Juyeop Kim on 2023/11/05.
//

#include "smm_common.h"
#include "smm_object.h"
#include <string.h>
#include <stdlib.h>

#define MAX_NODENR      100
#define MAX_NODETYPE    7
#define MAX_GRADE       9

#define MAX_FOODNR      100
#define MAX_FOODENERGY  100

#define MAX_FESTIVALNR  100



static char smmObj_nodeName[MAX_NODETYPE][MAX_CHARNAME] = {
       "lecture",
       "restaurant",
       "laboratory",
       "home",
       "gotolab",
       "foodchance",
       "festival"
};


static int smmObj_nodeNr = 0;
static int smmObj_foodNr = 0;
static int smmObj_festivalNr = 0;

//structure type definition 

typedef struct {
	char name[MAX_CHARNAME];
	int type;
	int credit;
	int energy;
} smmObj_board_t;

typedef struct {
	char name[MAX_CHARNAME];
	int energy;
} smmObj_food_t;

typedef struct {
	char message[MAX_CHARNAME];
} smmObj_festival_t;

//structure instance array definition 
static smmObj_board_t smmObj_board[MAX_NODENR];
static smmObj_food_t smmObj_food[MAX_FOODNR];
static smmObj_festival_t smmObj_festival[MAX_FESTIVALNR];

//object generation
int smmObj_genNode(char* name, int type, int credit, int energy)
{
    strcpy(smmObj_board[smmObj_nodeNr].name, name);
	smmObj_board[smmObj_nodeNr].type = type;
    smmObj_board[smmObj_nodeNr].credit = credit;
    smmObj_board[smmObj_nodeNr].energy = energy;
    
    smmObj_nodeNr++;
    
    return (smmObj_nodeNr);
}

int smmObj_genFood(char* name, int energy)
{
    strcpy(smmObj_food[smmObj_foodNr].name, name);
    smmObj_food[smmObj_foodNr].energy = energy;
    
    smmObj_foodNr++;
    
    return (smmObj_foodNr);
}

int smmObj_genFestival(char* message)
{
    strcpy(smmObj_festival[smmObj_festivalNr].message, message);

    smmObj_festivalNr++;
    
    return (smmObj_festivalNr);
}


//member retrieving

char* smmObj_getNodeName (int node_nr) //node name
{
	return (smmObj_board[node_nr].name);
}

int smmObj_getNodeType (int node_nr) //node type
{
	return (smmObj_board[node_nr].type);
}

int smmObj_getNodeEnergy(int node_nr) //node energy
{
	return (smmObj_board[node_nr].energy);
}

char* smmObj_getTypeName (int node_type)
{
	return (smmObj_nodeName[node_type]);
}

char* smmObj_getFoodName (int food_nr) //food name
{
	return (smmObj_food[food_nr].name);
}

int smmObj_getFoodEnergy(int food_nr) //food energy
{
	return (smmObj_food[food_nr].energy);
}

char* smmObj_getFestivalMessage (int festival_nr) //festival message
{
	return (smmObj_festival[festival_nr].message);
}

int smmObj_getNodeCredit (int node_nr) //player credit
{
	return (smmObj_board[node_nr].credit);
}



#if 0
//element to string

char* smmObj_getGradeName(smmGrade_e grade)
{
    return smmGradeName[grade];
}
#endif
