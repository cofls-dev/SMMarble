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
	int flag_experiment;
	int experinmet_criteria;
} smm_player_t; //플레이어 구조체화  

smm_player_t *smm_players;

void generatePlayers(int n, int initEnergy); //generate a new player
void printPlayerStatus(void); //print all player status at the beginning of each turn

void printGrades(int player) //print grade history of the player
{
	int size = smmdb_len(LISTNO_OFFSET_GRADE+player); //플레이어 학점 데이터 개수   
	int i;
	
	printf("Player %i : %s's grade history is : \n", player, smm_players[player].name);
	
	if(size == 0) //수강 과목 없을 경우  
	{
		printf("grade is 0.\n");
		return;
	}
	
	for(i=0;i<size;i++)
	{
		void *ptr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i); //성적 데이터 가져옴  
		
		printf("lecture : %s, credit : %d, grade : %s\n", smmObj_getObjectName(ptr), 
		                    smmObj_getObjectCredit(ptr), smmObj_getGradeName(smmObj_getObjectGrade(ptr))); //성적 출력  
	}
	
}

void* findGrade(int player, char *lectureName) //find the grade from the player's grade history
{
	int size = smmdb_len(LISTNO_OFFSET_GRADE+player); //플레이어 성적 데이터 개수  
	int i;
	
	for (i=0;i<size;i++)
	{
		void *ptr = smmdb_getData(LISTNO_OFFSET_GRADE+player, i); //성적 데이터 가져옴  
		
		if (strcmp(smmObj_getObjectName(ptr), lectureName) == 0) //과목 이름이 찾는 이름과 같은 경우 (중복)  
		{
			return ptr; //해당 성적 데이터 반환  
		}
	}
	
	return NULL; //못 찾으면 반환  
}

int isGraduated(void) //check if any player is graduated
{
	int i;
	for(i=0;i<smm_player_nr;i++) 
	{
		if (smm_players[i].flag_graduated == 1) //1명의 플레이어가 졸업 학점 채운 경우  
		return 1; //1 반환  
	}
	
	return 0; //0 반환  
}

void goForward(int player, int step) //make player go "step" steps on the board (check if player is graduated)
{
   int i;
   void *ptr;
   
   //player_pos[player] = player_pos[player] + 1)%board_nr;
   
   ptr = smmdb_getData(LISTNO_NODE, smm_players[player].pos);
   printf(" %s => moved to %i(%s)\n", smm_players[player].name, smm_players[player].pos, smmObj_getObjectName(ptr), step); //현재 위치 정보 출력  
   
   for(i=0;i<step;i++)
   {
   	 smm_players[player].pos = (smm_players[player].pos + 1)%smm_board_nr; //마지막 노드 -> 첫번째 노드 연결 구조  
   	 ptr = smmdb_getData(LISTNO_NODE, smm_players[player].pos);
   	 printf(" %s => moved to %i(%s)\n", smm_players[player].name, smm_players[player].pos, smmObj_getObjectName(ptr)); //이동한 칸 정보 출력  
   	 
   	 if (smmObj_getObjectType(ptr) == SMMNODE_TYPE_HOME) //집 지나가는 경우  
       {
           int energy = smmObj_getObjectEnergy(ptr);
           smm_players[player].energy += energy; //보충 에너지를 현재 에너지에 더함  
           printf(" [%s] passed home! you get %d energy\n", smm_players[player].name, energy);
       }
   }
}

void printPlayerStatus(void) //print all player status at the beginning of each turn
{
	int i;
	void *ptr;
	
	for (i=0;i<smm_player_nr;i++)
	{
		ptr = smmdb_getData(LISTNO_NODE, smm_players[i].pos); //플레이어 현재 위치 보드  정보 가져옴  
        printf("%s - position:%i(%s), credit:%i, energy:%i\n",
                    smm_players[i].name, smm_players[i].pos, smmObj_getObjectName(ptr), 
					smm_players[i].credit, smm_players[i].energy); //정보 출력  
        
	}
}

void generatePlayers(int n, int initEnergy) //generate a new player
{
	int i;
	
	smm_players = (smm_player_t*)malloc(n*sizeof(smm_player_t)); //플레이어 구조체 배열 동적 할당  
	
	for (i=0;i<n;i++)
	{
		smm_players[i].pos = 0; //시작 위치  0  
        smm_players[i].credit = 0; //초기 학점 0 
        smm_players[i].energy = initEnergy; //초기 에너지 0  
        smm_players[i].flag_graduated = 0; //졸업 여부 0 
        smm_players[i].flag_experiment = 0; //실험중 여부 0  
        smm_players[i].experinmet_criteria = 0; //실험 성공 기준값 0 
        
        printf("Input %i-th player name :", i); //플레이어 이름 입력  
        scanf("%s", &smm_players[i].name[0]); //이름 저장  
        fflush(stdin);
        
	}
}



int rolldie(int player) //rolldie
{
    char c;
    printf(" Press any key to roll a die (press g to see grade): ");
    c = getchar();
    fflush(stdin);
    
    if (c == 'g')
        printGrades(player); //'g' 누르면 성적 출력  
    
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
	
	printf("--> player %i : %s pos : %i, type : %s, credit : %i, energy : %i\n", 
	            player, smm_players[player].name, smm_players[player].pos, smmObj_getTypeName(ptr), credit, energy); //플레이어 정보 출력  
	
    switch(type)
    {
    	case SMMNODE_TYPE_LECTURE : //강의  
    	    printf("Do you want to take this lecture? y/n : "); //수강 여부 선택  
    	    scanf("%c", &answer);
    		
    	    if(answer == 'y') //수강 여부 확인   
    	    {
		      if(findGrade(player, smmObj_getObjectName(ptr)) == NULL)
    	      {
    	        smm_players[player].credit += credit; //과목 학점을 현재 학점에 더함  
    		    smm_players[player].energy -= energy; //과목 소모 에너지를 현재 에너지에서 뺌  
    		
    		    grade = rand()%SMMNODE_MAX_GRADE; //성적 중 하나 랜덤 배정  
    		
    		    gradePtr = smmObj_genObject(smmObj_getObjectName(ptr), SMMNODE_OBJTYPE_GRADE,
			                 type, credit, energy, "", grade); 
			    smmdb_addTail(LISTNO_OFFSET_GRADE+player,gradePtr);
			
			    printf("player %i : %s's lecture grade is : %d\n", player, smm_players[player].name, grade); //과목 성적 출력  
              }
            }
            else
		    { 
		      printf("lecture dropped.\n"); //수강 안 함  출력  
		    } 
    	    break;
    		
    		
        case SMMNODE_TYPE_RESTAURANT : //식당  
    		smm_players[player].energy += energy; //보충에너지가 현재 에너지에 더해짐  
    		printf("player %s gained energy +%d.\n", smm_players[player].name, energy);
        	break;
        	
        	
        case SMMNODE_TYPE_LABORATORY : //실험실  
            printf("player %s is in the Laboratory.\n", smm_players[player].name); //실험실 도착 
        	break;
        	
        	
        case SMMNODE_TYPE_HOME : //집  
            printf("player %s arrived at home.\n", smm_players[player].name);
    		smm_players[player].energy += energy; //집에서 에너지 보충  
    		if (smm_players[player].credit >= GRADUATE_CREDIT) //졸업 학점 넘었는지 확인  
			{
				smm_players[player].flag_graduated = 1; //졸업 플래그 설정  
			}
        	break;
        
        
        case SMMNODE_TYPE_GOTOLAB : //실험
            printf("Let's start Experiment! Move to Laboratory.\n");
		    for (i=0;i<smm_board_nr;i++)
		    {
		    	void *node = smmdb_getData(LISTNO_NODE, i); //실험실 위치 찾음  
		    	if(smmObj_getObjectType(node) == SMMNODE_TYPE_LABORATORY)
		    	{
		    		smm_players[player].flag_experiment = 1; //실험중 상태로 전환
					smm_players[player].pos = i; //플레이어를 실험실로 이동    
		    		smm_players[player].experinmet_criteria = rand()%MAX_DIE + 1; //실험 성공 기준값 랜덤 지정  
				}
			}		  
        	break;
        
        	
        case SMMNODE_TYPE_FOODCHANCE : //보충 찬스  
            printf("Food Chance!\n");
        	if(smmdb_len(LISTNO_FOODCARD) > 0)
        	{
        	food = rand()%smmdb_len(LISTNO_FOODCARD); //음식 카드 한장을 랜덤으로 뽑음  
        	
        	foodPtr = smmdb_getData(LISTNO_FOODCARD, food); 
        	int gain = smmObj_getObjectEnergy(foodPtr);
        	smm_players[player].energy += gain; //명시된 보충 에너지를 현재 에너지에 더함
			printf("player %s gained energy +%d.\n", smm_players[player].name, gain);
            }
        	break;
        	
        	
        case SMMNODE_TYPE_FESTIVAL : //축제  
        	if(smmdb_len(LISTNO_FESTCARD) > 0)
        	{
        	festival = rand()%smmdb_len(LISTNO_FESTCARD); //축제 카드 한장을 랜덤으로 뽑음  
        	
        	festPtr = smmdb_getData(LISTNO_FESTCARD, festival);
        	printf ("festival mission : %s\n", smmObj_getObjectMessage(festPtr)); //카드에 적힌 미션 수행  
            }
        	break;
        	
        default:
            break;
    }
}



int main(int argc, const char * argv[]) {

    //변수 선언  
    FILE* fp;
    char name[MAX_CHARNAME]; 
    int type;
    int credit;
    int energy;
    char message[MAX_CHARNAME];
    int turn;
    
    smm_board_nr = 0; //보드 노트 개수  
    smm_food_nr = 0; //음식 카드 개수  
    smm_festival_nr = 0; //축제 카드 개수  
    
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
        ptr = smmObj_genObject(name, SMMNODE_OBJTYPE_BOARD, type, credit, energy, "", 0);
        smmdb_addTail(LISTNO_NODE, ptr);
    }
    fclose(fp);
    smm_board_nr = smmdb_len(LISTNO_NODE);
    printf("Total number of board nodes : %i\n", smm_board_nr); //보드 노드 개수 출력  
    
    

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
        ptr = smmObj_genObject(name, SMMNODE_OBJTYPE_FOOD, 0, 0, energy, "", 0);
        smmdb_addTail(LISTNO_FOODCARD, ptr);
    }
    fclose(fp);
    smm_food_nr = smmdb_len(LISTNO_FOODCARD);
    printf("Total number of food cards : %i\n", smm_food_nr); //음식 카드 개수 출력  
    
    
    
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
        ptr = smmObj_genObject("", SMMNODE_OBJTYPE_FEST, 0, 0, 0, message, 0);
        smmdb_addTail(LISTNO_FESTCARD, ptr);
    }
    fclose(fp);
    smm_festival_nr = smmdb_len(LISTNO_FESTCARD);
    printf("Total number of festival cards : %i\n", smm_festival_nr); //축제 카드 개수 출력  
    
    

    //2. Player configuration ---------------------------------------------------------------------------------
    
    do
    {
        //input player number to player_nr
        printf("Input player number :");
        scanf("%i", &smm_player_nr);
        fflush(stdin);
        
        if (smm_player_nr <= 0 || smm_player_nr > MAX_PLAYER) //플레이어 수가 0 이하거나 최대 인원 초과  
          printf("Invalid player number!\n");
    }
    while (smm_player_nr <= 0 || smm_player_nr > MAX_PLAYER); 
    
    
    
    generatePlayers(smm_player_nr, smmObj_getObjectEnergy(smmdb_getData (SMMNODE_OBJTYPE_BOARD,0))); //플레이어 생성  
    


    
    turn = 0; //게임 턴 초기화 
	 
    //3. SM Marble game starts ---------------------------------------------------------------------------------
    while (isGraduated() == 0) //is anybody graduated?
    {
        int die_result;
        
        //4-1. initial printing
        printPlayerStatus();
        
        //4-2. die rolling (if not in experiment)
        die_result = rolldie(turn);
        
        if(smm_players[turn].flag_experiment == 1) //플레이어가 실험중인 경우  
		{
			void *node_ptr = smmdb_getData(LISTNO_NODE, smm_players[turn].pos); //실험실 정보 가져옴  
            int energy = smmObj_getObjectEnergy(node_ptr);
            
            smm_players[turn].energy -= energy; //실험 소모 애너지를 현재 에너지에서 뺌  
		  
		 
		    if(die_result >= smm_players[turn].experinmet_criteria) //실험 성공 기준값 이상이 나올 경우   
		    {
			   printf("Experiment is end! Escape! \n"); //실험 종료 출력  
			   smm_players[turn].flag_experiment = 0; //실험 탈출
			   smm_players[turn].experinmet_criteria = 0; //실험 성공 기준값 초기화   
			
			   goForward(turn, die_result);
			   actionNode(turn);
	        }
	        else
		    {
			   printf("Experiment failed.\n"); //실험 탈출 실패 -> 실험중 상태로 머무름  
		    } 
        }
         
        else
		{
        //4-3. go forward
        goForward(turn, die_result);
        //pos = pos + 2;
        

		//4-4. take action at the destination node of the board
        actionNode(turn);
        }
        
        if(smm_players[turn].flag_graduated == 1) //졸업 조건 확인  
            {
        		printf("player %d (%s) graduated!\n",turn, smm_players[turn].name); //졸업생 정보 출력  
        		printGrades(turn); //성적 출력  
        		
        		break;
			}
		
        //4-5. next turn
        turn = (turn + 1)%smm_player_nr; //다음플레이어턴  
      
    
    }

    free(smm_players); //메모리 해제  
    
	system("PAUSE");
    return 0;
}
