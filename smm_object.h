//
//  smm_object.h
//  SMMarble object
//
//  Created by Juyeop Kim on 2023/11/05.
//

#ifndef smm_object_h
#define smm_object_h



/* node type :
    lecture,
    restaurant,
    laboratory,
    home,
    experiment,
    foodChance,
    festival
*/


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
int smmObj_genNode(char* name, int type, int credit, int energy); 
int smmObj_genFood(char* name, int energy);
int smmObj_genFestival(char* message); 

//member retrieving
char smmObj_getName(void* Obj);

//element to string



#endif /* smm_object_h */
