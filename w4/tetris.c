#include "tetris.h"

static struct sigaction act, oact;

int main(){
	int exit=0;
	createRankList();

	initscr();
	noecho();
	keypad(stdscr, TRUE);	

	srand((unsigned int)time(NULL));

	while(!exit){
		clear();
		switch(menu()){
		case MENU_RECOMMEND: recommended=1;recommendedPlay();recommended=0;break;
		case MENU_PLAY: play(); break;
		case MENU_RANK: rank(); break;
		case MENU_EXIT: exit=1; break;
		default: break;
		}
	}

	endwin();
	system("clear");
	writeRankFile();
	return 0;
}

void InitTetris(){
	int i,j;

	initField();

	initNextBlock();

	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;	
	gameOver=0;
	timed_out=0;

	DrawOutline();
	DrawField();

	if(recommended) {
		initRecommended();
		DrawRecommend(*nextBlock);
	}
	
	DrawShadow(blockY,blockX,*nextBlock,blockRotate);
	DrawBlock(blockY,blockX,*nextBlock,blockRotate,' ');

	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline(){	
	int i,j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2,WIDTH+10);
	printw("NEXT BLOCK");
	DrawBox(3,WIDTH+10,4,8);

	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(9,WIDTH+10);
	printw("SCORE");
	DrawBox(10,WIDTH+10,1,8);
}

int GetCommand(){
	int command;
	command = wgetch(stdscr);
	switch(command){
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ':	/* space key*/
		/*fall block*/
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	return command;
}

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
	switch(command){
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
			blockRotate=(blockRotate+1)%4;
		break;
	case KEY_DOWN:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
			blockX++;
		break;
	case KEY_LEFT:
		if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
			blockX--;
		break;
	default:
		break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(gField(j,i)) {
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}


void PrintScore(int score){
	move(11,WIDTH+11);
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
	int i, j;
	for( i = 0; i < 4; i++ ){
		move(4+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[1]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
	}
}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				attron(A_REVERSE);
				printw("%c",tile);
				attroff(A_REVERSE);
			}
		}

	move(HEIGHT,WIDTH+10);
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch(ACS_ULCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for(j=0;j<height;j++){
		move(y+j+1,x);
		addch(ACS_VLINE);
		move(y+j+1,x+width+1);
		addch(ACS_VLINE);
	}
	move(y+j+1,x);
	addch(ACS_LLCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_LRCORNER);
}

void play(){
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

/////////////////////////첫주차 실습에서 구현해야 할 함수/////////////////////////

int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
    // user code
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            if (*(*(*(*(block+currentBlock)+blockRotate)+i)+j) && ( getField(f,blockY+i,blockX+j) || (blockX+j>=WIDTH) || (blockX+j<0) || (blockY+i>=HEIGHT) || (blockY+i<0)) )
                return 0;
    return 1;
}

void delBlock(int y, int x, int currentBlock, int blockRotate) {
	for(int i=0; i<4; i++) for(int j=0;j<4;j++) if(*(*(*(*(block+currentBlock)+blockRotate)+i)+j) && !(y + i < 0)) {
		move(y+i+1, x+j+1);
		printw(".");
	}
	move(HEIGHT,WIDTH+10);
}

int yDistance(char f[HEIGHT][WIDTH], int y, int x, int blockID, int blockRotate) {
    while (CheckToMove(f, *nextBlock, blockRotate, ++y, x));
    return y-1;
}

void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
	// user code

	//1. 이전 블록 정보를 찾는다. ProcessCommand의 switch문을 참조할 것
	//2. 이전 블록 정보를 지운다. DrawBlock함수 참조할 것.
	//3. 새로운 블록 정보를 그린다. 

	switch (command)
	{
	case KEY_DOWN:
		delBlock(blockY-1,blockX,currentBlock,blockRotate);
		break;

	case KEY_UP:
		delBlock(blockY,blockX,currentBlock, (blockRotate+1)%4 );
		delBlock(yDistance(f,blockY,blockX,currentBlock, (blockRotate+1)%4 ), blockX, currentBlock, (blockRotate+1)%4 );
		break;

	case KEY_LEFT:
		delBlock(blockY,blockX+1,currentBlock,blockRotate);
		delBlock(yDistance(f,blockY,blockX+1,currentBlock,blockRotate), blockX+1, currentBlock, blockRotate);
		break;

	case KEY_RIGHT:
		delBlock(blockY,blockX-1,currentBlock,blockRotate);
		delBlock(yDistance(f,blockY,blockX-1,currentBlock,blockRotate), blockX-1, currentBlock, blockRotate);
		break;
	default:
		return;
	}

	if(recommended) DrawRecommend(currentBlock);
	DrawShadow(blockY,blockX,currentBlock,blockRotate);
	DrawBlock(blockY,blockX,currentBlock,blockRotate,' ');
}

void BlockDown(int sig){
    // user code
    
    int i=0;

    if(CheckToMove(field,*nextBlock,blockRotate,++blockY,blockX)) {
		DrawField();
		DrawChange(field,KEY_DOWN,*nextBlock,blockRotate,blockY,blockX);
        timed_out = 0;
		return;
	}
	blockY--;

    score += AddBlockToField(field,*nextBlock,blockRotate,blockY,blockX);
    if(!(gameOver=(blockY==-1))) {
        score += DeleteLine(field);
        for(;i<BLOCK_NUM-1;i++){
            *(nextBlock+i) = *(nextBlock+i+1);
        }
        *(nextBlock+BLOCK_NUM-1) = rand()%NUM_OF_SHAPE;
        DrawNextBlock(nextBlock);
        PrintScore(score);

        blockY=-1;
        blockX=(WIDTH-4)/2;
        blockRotate=0;
    }
	initRecommended();
    DrawField();
    timed_out = 0;
}

int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
    // user code

    //Block이 추가된 영역의 필드값을 바꾼다.

    int a,i,j;
	a=i=j=0;
    for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) if (*(*(*(*(block+currentBlock)+blockRotate)+i)+j) && blockY+i>=0 && blockY+i<HEIGHT && blockX+j>=0 && blockX+j<WIDTH) {
		_setField(f,blockY+i,blockX+j,1);
        a+=(HEIGHT - 1 == i + blockY);
    }
    
    return 10*a;
}

int DeleteLine(char f[HEIGHT][WIDTH]) {
    // user code

    //1. 필드를 탐색하여, 꽉 찬 구간이 있는지 탐색한다.
    int line,i,j,x,y;
	line=i=j=0;
	bool cont = 0;

    for (;i<HEIGHT;i++) {
		cont=0;
		// for (j=0;j<WIDTH;j++) if(!(*(*(f+i)+j))) {cont=1;break;}
		for (j=0;j<WIDTH;j++) if(!getField(f,i,j)) {cont=1;break;}
        
        if(cont) continue;
        line++;

		for (y=i;y>=1;y--) for (x=0;x<WIDTH;x++) _setField(f,y,x,getField(f,y-1,x));
        _zeroField(f,x);
        i--;
    }
    //2. 꽉 찬 구간이 있으면 해당 구간을 지운다. 즉, 해당 구간으로 필드값을 한칸씩 내린다.
	line*=10;
    return line*line;
}


///////////////////////////////////////////////////////////////////////////

void DrawShadow(int y, int x, int blockID,int blockRotate) {
	// user code
	DrawBlock(yDistance(field,y,x,blockID,blockRotate), x, blockID, blockRotate, '/' );
}

void createRankList(){
	// user code

	userlist.node = NULL;
	userlist.totalUser = 0;

	char name[NAMELEN];
	int score,i=0;
	
	Usernode *n = userlist.node;
	if(n) while(n->next){n=n->next;}
	Usernode *newUser = NULL;
	FILE *f = fopen("rank.txt","r");
	if(!f) {
        printf("Error opening rank.txt");
        return;
    }
	while(!feof(f)) {
		fscanf(f, "%s %d", name, &score);
		
		userlist.totalUser++;
		newUser = (Usernode*)malloc(sizeof(Usernode));
		strcpy(newUser->name, name);
		newUser->score = score;
		newUser->next = NULL;
		//printw("Nombre: %s\n",newUser->name);
		if(n==NULL) {userlist.node = newUser;n=newUser;continue;}
		n->next = newUser;
		n = newUser;
	}
	fclose(f);
	return ;
}

void rank(){
	// user code
	char com;
	char name[NAMELEN];
	do{
		clear();
		printw("1.\tlist ranks from X to Y\n");
        printw("2.\tlist ranks by a specific name\n");
        printw("3.\tdelete a specific rank\n");
		com = wgetch(stdscr);
	}while(com<'1'||com>'3');

	echo();
	int x,idx=1,y=-1;
	Usernode *n;
	switch(com) {
		case '1':
			printw("X: ");
			x=-1;
			scanw("%d", &x);
			printw("Y: ");
			scanw("%d", &y);
			if (x<0) x = 1;
			if (y<0) y = userlist.totalUser;
			noecho();

			printw("\tname\t|\tscore\n");
			printw("--------------------------------------\n");
			if(x>y){
				printw("search failure: no rank in the list between x:%i and y:%i\n",x,y);
				break;
			}

			n = userlist.node;
			while(n!=NULL){
				if (x <= idx && idx <= y) printw(" %-19s| %-12d\n", n->name, n->score);
				idx++;
				n = n->next;
			}
			break;
		case '2':
			echo();
			printw("input the name: ");
            scanw("%s", name);
            noecho();
			findByName(name);
			break;
		case '3':
			echo();
            printw("Input the rank: ");
            scanw("%d", &x);
            noecho();
            deleteFromUserlist(x);
			break;
	}
	noecho();
	getch();
}

void findByName(char *name) {
	bool r = false;
	printw("        name        |    score\n");
    printw("---------------------------------------\n");
	for(Usernode *n = userlist.node;n;n=n->next) {
		if(strcmp(n->name,name)) continue;
		r = true;
		printw(" %-19s| %-12d\n", n->name, n->score);
	}
	if(!r) printw("search failure: no rank in the list\n");
}

void deleteFromUserlist(int del) {
	Usernode *n = userlist.node;
	if(del<1 || del >userlist.totalUser) {printw("search failure: no rank in the list\n");return;}

	if((del--)==1) {
		userlist.node = n->next;
		free(n);
	}
	else {
		while(--del) n=n->next;
		Usernode *tmp = n->next;
		n->next = tmp->next;
		free(tmp);
	}

	printw("result: the rank deleted\n");
}

void writeRankFile(){
	// user code
	FILE *f = fopen("rank.txt", "w");
	Usernode *n = userlist.node;
	while(n) {
		fprintf(f, "%s %d\n", n->name, n->score);
		printw("%s %d\n", n->name, n->score);
		n = n->next;
	}
	fclose(f);
}

void newRank(int score){
	// user code
	clear();
	echo();
	char name[NAMELEN];
	printw("your name: ");
    scanw("%s", name);
    noecho();

	Usernode *newUser = (Usernode*)malloc(sizeof(Usernode));
	newUser->name[0] = '\0'; //End of string.
	strcpy(newUser->name, name);
	newUser->score = score;
	
	Usernode *n = userlist.node;
	if(!n) {userlist.node = newUser;return;}

	while(n->next && n->next->score>score) n = n->next;
	newUser->next = n->next;
	n->next = newUser;
}

void DrawRecommend(int blockID){
	DrawBlock(recommendY,recommendX,blockID,recommendR,'R');
}

int recommend(RecNode *root){
	int i,j,x,y,r,score,max=-1;
	RecNode node;
	RecNode* n = &node;

	space+=sizeof(*root);
	for(r=0;r<4;r++) for(x=-2;x<13;x++) {
		if(!CheckToMove(root->f,nextBlock[root->lv],r,0,x)) continue;

		for(i=0;i<HEIGHT;i++) for(j=0;j<WIDTH;j++) n->f[i][j] = root->f[i][j];

		n->lv = root->lv + 1;

		y=0;
		y=yDistanceRec(n->f,y,x,nextBlock[root->lv],r);

		score = AddBlockToField(n->f,nextBlock[root->lv],r,y,x);
		score += DeleteLine(n->f);

		if(n->lv < BLOCK_NUM) score += recommend(n);
		if(max<=score) {
			if(!(root->lv) && (recommendY<y || max<score)) {
				recommendR=r;
				recommendX=x;
				recommendY=y;
			}
			max = score;
		}
	}
	return max;
}

int recommend_v2(RecNode *root){
	int i,j,x,y,r,score,max=-1;
	RecNode node;
	RecNode* n = &node;

	space+=sizeof(*root);
	for(r=0;r<rotations[nextBlock[root->lv]];r++) for(x=-2;x<13;x++) {
		if(!CheckToMove(root->f,nextBlock[root->lv],r,0,x)) continue;

		for(i=0;i<HEIGHT;i++) for(j=0;j<WIDTH;j++) n->f[i][j] = root->f[i][j];

		n->lv = root->lv + 1;

		y=0;
		y=yDistanceRec(n->f,y,x,nextBlock[root->lv],r);

		score = AddBlockToField(n->f,nextBlock[root->lv],r,y,x);
		score += DeleteLine(n->f);
		score += SQRTYRET_WEIGHT*y*y;

		if(n->lv < BLOCK_NUM) score += recommend_v2(n);
		if(max<=score) {
			if(!(root->lv) && (recommendY<y || max<score)) {
				recommendR=r;
				recommendX=x;
				recommendY=y;
			}
			max = score;
		}
	}
	return max;
}

int yDistanceRec(char f[HEIGHT][WIDTH], int y, int x, int blockID, int blockRotate) {
	int i, j;
    for (; y < HEIGHT; y++) for (i = 0; i < 4; i++) for (j = 0; j < 4; j++) if( block[blockID][blockRotate][i][j] && ( (y+i>= 0) && getField(f,y+i,x+j) || (y + i >= HEIGHT) ) ) return y - 1;
    return y - 1;
}

void initRecommended(){
	RecNode node;
	RecNode* n = &node;
	int i,j;
	i=j=0;
	
	while(i<HEIGHT) {for(j=0;j<WIDTH;j++) n->f[i][j] = field[i][j];  i++;}
	n->lv = 0;
	recommendX = 0;
	recommendY = 0;
	recommendR = 0;
	recommend_v2(n);
}

void automaticBlockDown(int sig) {
	int i=0;
	if(CheckToMove(field,*nextBlock,blockRotate,blockY,blockX)) {
		score+=AddBlockToField(field,*nextBlock,recommendR,recommendY,recommendX) + DeleteLine(field);
		blockY=-1;
        blockX=(WIDTH-1)/2;
        blockRotate=0;
		for(;i<BLOCK_NUM-1;i++) *(nextBlock+i) = *(nextBlock+i+1);
		*(nextBlock+BLOCK_NUM-1) = rand()%NUM_OF_SHAPE;
		
        initRecommended();
        PrintScore(score);
        DrawField();

		//DrawShadow(blockY,blockX,*nextBlock,blockRotate);
		DrawRecommend(*nextBlock);
		DrawBlock(blockY,blockX,*nextBlock,blockRotate,' ');
		DrawNextBlock(nextBlock);
        timed_out = 0;
		return;
	} 
	gameOver = 1;
}

void recommendedPlay() {
    int command;
	stime=time(NULL);
    clear();
    act.sa_handler = automaticBlockDown;
    sigaction(SIGALRM,&act,&oact);
    InitTetris();
    do {
        if (timed_out==0) {
            alarm(1);
            timed_out=1;
        }

        command = GetCommand();
        if (ProcessCommand(command) == QUIT) {
            alarm(0);
            DrawBox(HEIGHT/2-1, WIDTH/2-5,1,10);
            move(HEIGHT/2, WIDTH/2-4);
            printw("Good-bye!!");
            gameOver = 0;
            refresh();
            getch();

            return;
        }

        ftime = time(NULL);
        double lenght = (double)difftime(ftime,stime);

        move(4,WIDTH+30);
        printw("time\t\t:%10.2f s",lenght);
        move(5,WIDTH+30);
        printw("score per time\t:%10.2f score/s",score/lenght);
        move(7,WIDTH+30);
        printw("space\t\t:%10.2f KB",(double)space/1024);
        move(8,WIDTH+30);
        printw("space per time\t:%10.2f KB/s",(double)space/(lenght*1024));
        move(10,WIDTH+30);
        printw("score per space\t:%10.2f score/KB",score/((double)space/1024));
    }while(!gameOver);

    alarm(0);
    getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	//newRank(score);
}
