//
//  main.c
//  SMMarble
//
//  Created by Juyeop Kim on 2023/11/05.
//

#include <time.h>
#include <string.h>
#include "smm_object.h"
#include "smm_database.h"
#include "smm_common.h"

#define BOARDFILEPATH "marbleBoardConfig.txt"
#define FOODFILEPATH "marbleFoodConfig.txt"
#define FESTFILEPATH "marbleFestivalConfig.txt"


//board configuration parameters
static int smm_board_nr;
static int smm_food_nr;
static int smm_festival_nr;
static int smm_player_nr;

typedef struct {
	char name[MAX_CHARNAME];
	int pos;
	int credit;
	int energy;
	int flag_graduated;
} smm_player_t;

smm_player_t *smm_players;

void generatePlayers(int n, int initEnergy); //generate a new player
void printPlayerStatus(void); //print all player status at the beginning of each turn

//function prototypes
#if 0
float calcAverageGrade(int player); //calculate average grade of the player
smmGrade_e takeLecture(int player, char *lectureName, int credit); //take the lecture (insert a grade of the player)
void printGrades(int player); //print all the grade history of the player
#endif

void printGrades(int player) //print grade history of the player
{
	int size = smmdb_len(LISTNO_OFFSET_GRADE+player);
	int i;
	
	printf("Player %d grade history is : \n", player);
	
	if(size == 0)
	{
		printf("grade is 0.\n");
		return;
	}
	
	for(i=0;i<size;i++)
	{
		void *ptr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);
		
		printf("lecture : %s, credit : %d, grade : %d\n", smmObj_getObjectName(ptr), 
		                    smmObj_getObjectCredit(ptr), smmObj_getObjectGrade(ptr));
	}
	
}

void* findGrade(int player, char *lectureName) //find the grade from the player's grade history
{
	int size = smmdb_len(LISTNO_OFFSET_GRADE+player);
	int i;
	
	for (i=0;i<size;i++)
	{
		void *ptr = smmdb_getData(LISTNO_OFFSET_GRADE+player, i);
		
		if (strcmp(smmObj_getObjectName(ptr), lectureName) == 0)
		{
			return ptr;
		}
	}
	
	return NULL;
}

int isGraduated(void) //check if any player is graduated
{
	int i;
	for(i=0;i<smm_player_nr;i++) 
	{
		if (smm_players[i].flag_graduated == 1)
		return 1;
	}
	
	return 0;
}

void goForward(int player, int step) //make player go "step" steps on the board (check if player is graduated)
{
   int i;
   void *ptr;
   
   //player_pos[player] = player_pos[player] + 1)%board_nr;
   
   ptr = smmdb_getData(LISTNO_NODE, smm_players[player].pos);
   printf(" => moved to %i(%s)\n", smm_players[player].pos, smmObj_getObjectName(ptr), step);
   
   for(i=0;i<step;i++)
   {
   	 smm_players[player].pos = (smm_players[player].pos + 1)%smm_board_nr;
   	 printf(" => moved to %i(%s)\n", smm_players[player].pos, smmObj_getObjectName(ptr));
   }
}

void printPlayerStatus(void) //print all player status at the beginning of each turn
{
	int i;
	void *ptr;
	
	for (i=0;i<smm_player_nr;i++)
	{
		ptr = smmdb_getData(LISTNO_NODE, smm_players[i].pos);
        printf("%s - position:%i(%s), credit:%i, energy:%i\n",
                    smm_players[i].name, smm_players[i].pos, smmObj_getObjectName(ptr), smm_players[i].credit, smm_players[i].energy);
        
	}
}

void generatePlayers(int n, int initEnergy) //generate a new player
{
	int i;
	
	smm_players = (smm_player_t*)malloc(n*sizeof(smm_player_t));
	
	for (i=0;i<n;i++)
	{
		smm_players[i].pos = 0;
        smm_players[i].credit = 0;
        smm_players[i].energy = initEnergy;
        smm_players[i].flag_graduated = 0;
        
        printf("Input %i-th player name :", i);
        scanf("%s", &smm_players[i].name[0]);
        fflush(stdin);
        
	}
}



int rolldie(int player) //rolldie
{
    char c;
    printf(" Press any key to roll a die (press g to see grade): ");
    c = getchar();
    fflush(stdin);
    
#if 0
    if (c == 'g')
        printGrades(player);
#endif
    
    return (rand()%MAX_DIE + 1);
}


//action code when a player stays at a node
void actionNode(int player)
{
	void *ptr;
	
	ptr = smmdb_getData(LISTNO_NODE, smm_players[player].pos);
	
	int type = smmObj_getObjectType(ptr);
	int credit = smmObj_getObjectCredit(ptr);
	int energy = smmObj_getObjectEnergy(ptr);
	int grade;
	int i;
	int food;
	int festival;
	char answer;
	void *gradePtr;
	void *foodPtr;
	void *festPtr;
	
	printf("--> player%i pos : %i, type : %s, credit : %i, energy : %i\n", 
	            player, smm_players[player].pos, smmObj_getTypeName(ptr), credit, energy);
	
    switch(type)
    {
    	case SMMNODE_TYPE_LECTURE : //강의  
    	printf("Do you want to take this lecture? : "); //수강 여부 선택  
    	scanf("%c", &answer);
    		
    	if(answer == 'y') //수강 여부 확인   
    	{
		  if(findGrade(player, smmObj_getObjectName(ptr)) == NULL)
    	  {
    		smm_players[player].credit += credit; //과목 학점을 현재 학점에 더함  
    		smm_players[player].energy -= energy; //과목 소모 에너지를 현재 에너지에서 뺌  
    		
    		grade = rand()%SMMNODE_MAX_GRADE; //성적 중 하나 랜덤 
    		
    		gradePtr = smmObj_genObject(smmObj_getObjectName(ptr), SMMNODE_OBJTYPE_GRADE,
			                 type, credit, energy, 0, grade); 
			smmdb_addTail(LISTNO_OFFSET_GRADE+player,gradePtr);
			
			printf("lecture grade is : %d\n", grade); //과목 성적 출력  
          }
        }
         else
		  { 
		    printf("lecture dropped\n"); //수강 안 함  출력  
		  } 
    	break;
    		
        case SMMNODE_TYPE_RESTAURANT : //식당  
    		smm_players[player].energy += energy; //보충에너지가 현재 에너지에 더해짐  
        	break;
        	
        case SMMNODE_TYPE_LABORATORY : //실험실  
            printf("you are in the laboratory.\n");
        	break;
        	
        case SMMNODE_TYPE_HOME : //집  
    		smm_players[player].energy += energy; //집에서 에너지 보충  
    		if (smm_players[player].credit >= GRADUATE_CREDIT) //졸업 학점 넘었는지 확인  
			{
				smm_players[player].flag_graduated = 1; //졸업 플래그 설정  
			}
        	break;
        #if 0	
        case SMMNODE_TYPE_GOTOLAB : //실험
		    for (i=0;i<smm_board_nr;i++)
		    {
		    	void *node = smmdb_getData(LISTNO_NODE, i);
		    	if(smmObj_getObjectType(node) == SMMNODE_TYPE_LABORATORY)
		    	{
				}
			}		  
        	break;
        #endif
        	
        case SMMNODE_TYPE_FOODCHANCE : //보충 찬스  
        	if(smmdb_len(LISTNO_FOODCARD) > 0)
        	{
        	food = rand()%SMMNODE_MAX_FOODTYPE; //음식 카드 한장을 랜덤으로 뽑음  
        	
        	foodPtr = smmdb_getData(LISTNO_FOODCARD, food); 
        	int gain = smmObj_getObjectEnergy(foodPtr);
        	smm_players[player].energy += gain; //명시된 보충 에너지를 현재 에너지에 더함  
            }
        	break;
        	
        case SMMNODE_TYPE_FESTIVAL : //축제  
        	if(smmdb_len(LISTNO_FESTCARD) > 0)
        	{
        	festival = rand()%SMMNODE_MAX_FOODTYPE; //축제 카드 한장을 랜덤으로 뽑음  
        	
        	festPtr = smmdb_getData(LISTNO_FESTCARD, festival);
        	printf ("festival mission : %s\n", smmObj_getObjectMessage(festPtr)); //카드에 적힌 미션 수행  
            }
        	break;
        	
        //case lecture:
        default:
            break;
    }
}



int main(int argc, const char * argv[]) {
    
    FILE* fp;
    char name[MAX_CHARNAME];
    int type;
    int credit;
    int energy;
    char message[MAX_CHARNAME];
    int turn;
    
    smm_board_nr = 0;
    smm_food_nr = 0;
    smm_festival_nr = 0;
    
    srand(time(NULL));
    
    
    //1. import parameters ---------------------------------------------------------------------------------
    //1-1. boardConfig 
    if ((fp = fopen(BOARDFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", BOARDFILEPATH);
        getchar();
        return -1;
    }
    
    printf("Reading board component......\n");
    while ( fscanf(fp, "%s %i %i %i", name, &type, &credit, &energy) == 4 )  //read a node parameter set
    {
        //store the parameter set
        void* ptr;
        ptr = smmObj_genObject(name, SMMNODE_OBJTYPE_BOARD, type, credit, energy, 0, 0);
        smm_board_nr = smmdb_addTail(LISTNO_NODE, ptr);
    }
    fclose(fp);
    printf("Total number of board nodes : %i\n", smm_board_nr);
    
    

    //2. food card config 
    if ((fp = fopen(FOODFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FOODFILEPATH);
        return -1;
    }
    
    printf("\n\nReading food card component......\n");
    while ( fscanf(fp, "%s %d", name, &energy) == 2) //read a food parameter set
    {
        //store the parameter set
        void* ptr;
        ptr = smmObj_genObject(name, SMMNODE_OBJTYPE_FOOD, 0, 0, energy, 0, 0);
        smm_food_nr = smmdb_addTail(LISTNO_FOODCARD, ptr);
    }
    fclose(fp);
    printf("Total number of food cards : %i\n", smm_food_nr);
    
    
    
    //3. festival card config 
    if ((fp = fopen(FESTFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FESTFILEPATH);
        return -1;
    }
    
    printf("\n\nReading festival card component......\n");
    while (fscanf(fp, "%s", message) == 1) //read a festival card string
    {
        //store the parameter set
        void* ptr;
        ptr = smmObj_genObject(0, SMMNODE_OBJTYPE_FEST, 0, 0, 0, message, 0);
        smm_festival_nr = smmdb_addTail(LISTNO_FESTCARD, ptr);
    }
    fclose(fp);
    printf("Total number of festival cards : %i\n", smm_festival_nr);
    
    

    //2. Player configuration ---------------------------------------------------------------------------------
    
    do
    {
        //input player number to player_nr
        printf("Input player number :");
        scanf("%i", &smm_player_nr);
        fflush(stdin);
        
        if (smm_player_nr <= 0 || smm_player_nr > MAX_PLAYER)
          printf("Invalid player number!\n");
    }
    while (smm_player_nr <= 0 || smm_player_nr > MAX_PLAYER);
    
    
    
    generatePlayers(smm_player_nr, smmObj_getObjectEnergy(smmdb_getData (SMMNODE_OBJTYPE_BOARD,0)));
    


    
    turn = 0;
    //3. SM Marble game starts ---------------------------------------------------------------------------------
    while (isGraduated() == 0) //is anybody graduated?
    {
        int die_result;
        
        //4-1. initial printing
        printPlayerStatus();
        
        //4-2. die rolling (if not in experiment)
        die_result = rolldie(turn);
        
        //4-3. go forward
        goForward(turn, die_result);
        //pos = pos + 2;
        

		//4-4. take action at the destination node of the board
        actionNode(turn);
        
        if(smm_players[turn].flag_graduated == 1) //졸업 조건 확인  
            {
        		printf("player %d (%s) graduated!\n",turn, smm_players[turn].name); //졸업생 정보 출력  
        		printGrades(turn); //성적 출력  
        		
        		break;
			}
		
        //4-5. next turn
        turn = (turn + 1)%smm_player_nr; //다음플레이어턴  
      
    
    }

    free(smm_players);
    
	system("PAUSE");
    return 0;
}
