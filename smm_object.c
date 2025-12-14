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

#define MAX_FOODTYPE    14
#define MAX_FOODNR      100
#define MAX_FOODENERGY  100

#define MAX_FESTIVALNR  100

#define OBJTYPE_BOARD      0
#define OBJTYPE_GRADE      1
#define OBJTYPE_FOOD       2
#define OBJTYPE_FESTIVAL   3

#define FOOD_CHICKEN       0
#define FOOD_BEER          1
#define FOOD_TANGHULU      2
#define FOOD_CUPRAMEN      3
#define FOOD_GAMJATANG     4
#define FOOD_CUPRICE       5
#define FOOD_BLACKNOODLES  6
#define FOOD_SCHOOLFOOD    7
#define FOOD_PASTA         8
#define FOOD_PIZZA         9
#define FOOD_HAMBURGER     10
#define FOOD_SANDWICH      11
#define FOOD_YOGURT        12
#define FOOD_BIBIMBAP      13


static char smmObj_nodeName[MAX_NODETYPE][MAX_CHARNAME] = {
       "lecture",
       "restaurant",
       "laboratory",
       "home",
       "gotolab",
       "foodchance",
       "festival"
};

static char smmObj_foodName[SMMNODE_MAX_FOODTYPE][MAX_CHARNAME] = {
       "chicken",
       "beer",
       "tanghulu",
       "cupramen",
       "gamjatang",
       "cuprice",
       "blacknoodle",
       "schoolfood",
       "pasta",
       "pizza",
       "hamburger",
       "sandwich",
       "yogurt",
       "bibimbap"
};

static char smmObj_gradeName[SMMNODE_MAX_GRADE][MAX_CHARNAME] = {
       "A+",
       "A0",
       "A-",
       "B+",
       "B0",
       "B-",
       "C+",
       "C0",
       "C-",
       "D+",
       "D0",
       "D-",
       "F"
};


//structure type definition 
typedef struct {
	char name[MAX_CHARNAME];
	int objType;
	int type;
	int credit;
	int energy;
	char message[MAX_CHARNAME];
	int grade;
} smmObj_object_t; //게임 보드 구조체화  


//object generation
void* smmObj_genObject(char* name, int objType, int type, int credit, int energy, char* message, int grade)
{
	smmObj_object_t* ptr;
	
	ptr = (smmObj_object_t*)malloc(sizeof(smmObj_object_t));
	
    strcpy(ptr->name, name);
    ptr->objType = objType;
	ptr->type = type;
    ptr->credit = credit;
    ptr->energy = energy;
    strcpy(ptr->message, message);
    ptr->grade = grade;
    
    return ((void*)ptr);
}


//member retrieving

char* smmObj_getObjectName (void *ptr) //이름  
{
	smmObj_object_t* objPtr = (smmObj_object_t*)ptr;
	
	return (objPtr->name);
}

int smmObj_getObjectObjType (void *ptr) //분류  
{
	smmObj_object_t* objPtr = (smmObj_object_t*)ptr;
	
	return (objPtr->objType);
}

int smmObj_getObjectType (void *ptr) //타입  
{
	smmObj_object_t* objPtr = (smmObj_object_t*)ptr;
	
	return (objPtr->type);
}

int smmObj_getObjectEnergy(void *ptr) //에너지  
{
	smmObj_object_t* objPtr = (smmObj_object_t*)ptr;
	
	return (objPtr->energy);
}

char* smmObj_getTypeName (void *ptr) //타입 이름  
{
	smmObj_object_t* objPtr = (smmObj_object_t*)ptr;
	 
	return smmObj_nodeName[objPtr->type];
}

char* smmObj_getObjectMessage (void *ptr) //내용  
{
	smmObj_object_t* objPtr = (smmObj_object_t*)ptr;
	
	return (objPtr->message);
}

int smmObj_getObjectCredit (void *ptr) //학점  
{
	smmObj_object_t* objPtr = (smmObj_object_t*)ptr;
	
	return (objPtr->credit);
}

int smmObj_getObjectGrade (void *ptr) //성적  
{
	smmObj_object_t* objPtr = (smmObj_object_t*)ptr;
	
	return (objPtr->grade);
}


//element to string
char* smmObj_getGradeName(int grade) //성적 이름  
{
    return smmObj_gradeName[grade];
}
