//
//  smm_object.h
//  SMMarble object
//
//  Created by Juyeop Kim on 2023/11/05.
//

#ifndef smm_object_h
#define smm_object_h

#define BOARDFILEPATH "marbleBoardConfig.txt"
#define FOODFILEPATH "marbleFoodConfig.txt"
#define FESTFILEPATH "marbleFestivalConfig.txt"


/* node type :
    lecture,
    restaurant,
    laboratory,
    home,
    experiment,
    foodChance,
    festival
*/
#define SMMNODE_MAX_GRADE       13
#define SMMNODE_MAX_FOODTYPE    14

#define SMMNODE_TYPE_LECTURE         0
#define SMMNODE_TYPE_RESTAURANT      1
#define SMMNODE_TYPE_LABORATORY      2
#define SMMNODE_TYPE_HOME            3
#define SMMNODE_TYPE_GOTOLAB         4
#define SMMNODE_TYPE_FOODCHANCE      5
#define SMMNODE_TYPE_FESTIVAL        6

#define SMMNODE_OBJTYPE_BOARD      0
#define SMMNODE_OBJTYPE_GRADE      1
#define SMMNODE_OBJTYPE_FOOD       2
#define SMMNODE_OBJTYPE_FEST       3

#define GRADE_A_PLUS           0
#define GRADE_A0               1
#define GRADE_A_MINUS          2
#define GRADE_B_PLUS           3
#define GRADE_B0               4
#define GRADE_B_MINUS          5
#define GRADE_C_PLUS           6
#define GRADE_C0               7
#define GRADE_C_MINUS          8
#define GRADE_D_PLUS           9
#define GRADE_D0               10
#define GRADE_D_MINUS          11
#define GRADE_F                12


/* grade :
    A+,
    A0,
    A-,
    B+,
    B0,
    B-,
    C+,
    C0,
    C-
*/



//object generation
void* smmObj_genObject(char* name, int objType, int type, int credit, int energy, char* message, int grade);

//member retrieving
char* smmObj_getObjectName (void *ptr);
int smmObj_getObjectObjType (void *ptr); 
int smmObj_getObjectType (void *ptr);
int smmObj_getObjectEnergy(void *ptr); 
char* smmObj_getTypeName (void *ptr);
char* smmObj_getObjectMessage (void *ptr); 
int smmObj_getObjectCredit (void *ptr); 
int smmObj_getObjectGrade (void *ptr); 


//element to string



#endif /* smm_object_h */
