#include <ncurses.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define null_item {"",'',0,0,0,0,0,0,0,0,0,0}

#define short_sword {"short sword", '/', rand()%50, rand()%50, 0, 0, 0, 0, 0, 0, 1, 1, 0} 
#define long_sword {"long sword", '/', rand()%50, rand()%50, 0, 0, 0, 0, 0, 0, 1, 1, 0}

#define cap {"cap", '^', rand()%50, rand()%50, 0, 0, 0, 1, 0, 0, 1, 1, 1}

#define quilted_armor {"quilted armor", '}', 0, 0, 0, 0, 0, 1, 3, 0, 0 ,0, 2}
#define leather_armor {"leather armor", '}', 0, 0, 0, 0, 0, 3, 2, 0, 0 ,0, 2}
#define hard_leather_armor {"hard leather armor", '}', 0, 0, 0, 0, 0, 3, 2, 0, 0 ,0, 2}
#define ring_mail {"ring mail", '}', 0, 0, 0, 0, 0, 6, 0, 0, 0 ,0, 2}
#define scale_mail {"scale mail", '}', 0, 0, 0, 0, 0, 8, -3, 0, 0 ,0, 2}
#define plate_mail {"plate mail", '}', 0, 0, 0, 0, 0, 12, -6, 0, 0 ,0, 2}

typedef struct
{
  char *name;
	char character;
	char alive;
	int health, max_health;
	int x, y;
	int d1, d2; // (d1)d(d2) hit dice
} monster;


typedef struct
{
	char *name;
	char character;
	int x, y;
	int s, d, i, ac, ev, sh, d1, d2;	
	int type; //color?
} item;

struct s_player
{
	int health, max_health;
	int s, d, i;
	int ac, ev, sh;
	int x, y;
	item weapon, helm, armor, shield;
} player;

struct map
{
	char tile[50][50];
	int ex,ey;
	monster monsters[20];
	item items[5];

};


char messages[1000][80];
unsigned char dlevel = 1;


//gameplay
int roll(int d1, int d2);

//map
void clearmap(struct map* in);
void generatemap(struct map* in);
void createroom(struct map* in, int x, int y, int xs, int ys);
int scan(struct map* in, int x, int y, int xs, int ys);

//misc
void add_message(char *in);
char* get_item_string(item in);

int main()
{	
	struct map map1;
	clearmap(&map1);
	player.x = 10;
	player.y = 10;
	item a = cap;
	player.helm = a;
	item b = ring_mail;
	player.armor = b;
	item c = long_sword;
	player.weapon = c;

	int playing = 1;	
	initscr();			/* Start curses mode 		  */
	
	add_message("welcome to qrogue?");
	while(playing)
	{
	clear();
	int i, j;
	for(i = 0; i < 35; i++) //render map
	{
		for(j = 0; j < 18; j++)
		{
			if(player.x + i - 17 >= 0 && player.x + i - 17 < 50 && player.y + j - 9 >= 0 && player.y + j - 9 < 50) 
				mvaddch(j, i, map1.tile[player.x + i - 17][player.y + j - 9]);
			else
				mvaddch(j, i, ' ');
		}
	}
	
	mvaddch(map1.ey - player.y + 9, map1.ex - player.x + 17, '>');
	
	for(i = 0; i < 20; i++) // render monsters
	{
		if(map1.monsters[i].x - player.x + 17 > 0 && map1.monsters[i].x - player.x + 17 < 35 && map1.monsters[i].y - player.y + 9 > 0 && map1.monsters[i].y - player.y + 9 < 18) 
		mvaddch(map1.monsters[i].y - player.y + 9, map1.monsters[i].x - player.x + 17, map1.monsters[i].character);
	
	}
	
	for(i = 0; i < 5; i++) // render items
	{
		if(map1.items[i].x - player.x + 17 > 0 && map1.items[i].x - player.x + 17 < 35 && map1.items[i].y - player.y + 9 > 0 && map1.items[i].y - player.y + 9 < 18) 
		mvaddch(map1.items[i].y - player.y + 9, map1.items[i].x - player.x + 17, map1.items[i].character);
	
	}
	
	mvaddch(9, 17, '@');
	mvprintw(0, 37, "name the class");
	mvprintw(1, 37, "health");
	mvprintw(2, 37, "mana"); 
	mvprintw(3, 37, "str"); mvprintw(3, 47, "ac %d", player.ac); 
	mvprintw(4, 37, "dex"); mvprintw(4, 47, "ev %d", player.ev);
	mvprintw(5, 37, "int");	mvprintw(5, 47, "sh %d", player.sh);
	mvprintw(6, 37, "weapon : %dd%d %s", player.weapon.d1, player.weapon.d2, player.weapon.name);
	mvprintw(7, 37, "helm   : %d %s", player.helm.ac, player.helm.name);
	mvprintw(8, 37, "armor  : %d %s", player.armor.ac, player.armor.name);
	mvprintw(9, 37, "shield : %d %s", player.shield.ac, player.shield.name);
	mvprintw(11, 37, "dungeon level : %d", dlevel);

	for(i = 0; i < 6; i++) // print log
	{
		if(messages[i] != NULL)
			mvprintw(23 - i, 0, "%s", messages[i]);	
	}	
	mvaddch(23, 79, ' '); // move cursor
	refresh();
	int ch = getch();
	int movex = 0, movey = 0;
	if(ch == KEY_LEFT || ch == 'h')
	{
		movex = -1;
	}
	if(ch == KEY_RIGHT || ch == 'l')
	{
		movex = 1;
	}
	if(ch == KEY_UP || ch == 'k')
	{
		movey = -1;
	}
	if(ch == KEY_DOWN || ch == 'j')
	{
		movey = 1;
	}
	if(ch == 'y')
	{
		movex = -1;
		movey = -1;
	}	
	if(ch == 'u')
	{
		movex = 1;
		movey = -1;
	}
	if(ch == 'b')
	{
		movex = -1;
		movey = 1;
	}
	if(ch == 'n')
	{
		movex = 1;
		movey = 1;
	}	
	if(ch == 'g')
	{
		for(i = 0; i < 5; i++)
		{
			if(player.x == map1.items[i].x && player.y == map1.items[i].y)
			{
				item a = map1.items[i];
				char msg[80];
				sprintf(msg, "you drop your %s, and pick up and equip the %s", player.weapon.name, a.name);
				add_message(msg);
				item b = player.weapon;
				b.x = map1.items[i].x;
				b.y = map1.items[i].y;
				player.weapon = a;
				map1.items[i] = b;	
			}
		}
	}
	if(ch == '>')
	{
		if(player.x == map1.ex && player.y == map1.ey)
		{
			clearmap(&map1);
			dlevel++;
			char msg[80];
			sprintf(msg, "you descend into dungeon level %i!", dlevel);
			add_message(msg);
		}	
	}	
	
	char monsterhere = 0;
	for(i = 0; i < 20; i++)
	{
		if(player.x + movex == map1.monsters[i].x && player.y + movey == map1.monsters[i].y)
		{
			monsterhere = 1;
			char msg[80];
			strcpy(msg, "you attack the ");
			strcat(msg, map1.monsters[i].name);
			add_message(msg);
		}
	}
	
	if(monsterhere == 0)
	{
	
	for( i = 0; i < 5; i++)
	{
		if(player.x + movex == map1.items[i].x && player.y + movey == map1.items[i].y)
		{
			char msg[80];
			strcpy(msg, "here is a ");
			strcat(msg, map1.items[i].name);
			add_message(msg);
		}
	}
	if(strchr(".()[]-", map1.tile[player.x + movex][player.y + movey]) != NULL) // movable tiles
	{
		player.x += movex;
		player.y += movey;
	}
	else if(strchr("#~", map1.tile[player.x + movex][player.y +movey]) != NULL) //unmovable tiles
	{
		add_message("you can't walk there");
	}	
	
	
	
	}
	
		//monster turns
		for(i = 0; i < 20; i++)
		{
		
	
		}	
		//update player
		player.ac = player.helm.ac + player.armor.ac + player.shield.ac + player.weapon.ac;
		player.ev = player.helm.ev + player.armor.ev + player.shield.ev + player.weapon.ev;
		player.sh = player.helm.sh + player.armor.sh + player.shield.sh + player.weapon.sh;
	}
	endwin();
	return 0;
}

int roll(int d1, int d2)
{
	int i, roll = 0;
	for(i = 0; i < d1; i++)
	{
		roll += rand() % d2;
	}
	return roll;
}

void clearmap(struct map* in)
{
	int i,j;
	for(i = 0; i < 50; i++)
		for(j = 0; j < 50; j++)
		{
			in->tile[i][j] = '.';
			if(i % 49 == 0 || j % 49 == 0)
				in->tile[i][j] = '#';
		}
		
	for(i = 0; i < 20; i++)
	{
		monster a = {"orc", 'o', 1, 10, 10, rand() % 50, rand() % 50, 1, 5};
		in->monsters[i] = a;
	}
	for(i = 0; i < 5; i++)
	{
		item a = short_sword;
		in->items[i] = a;
	}
	in->ex = rand()%49;
	in->ey = rand()%49;
}

void generatemap(struct map* in)
{
	int i, j;
	for(i = 0; i < 50; i++)
		for(j = 0; j < 50; j++)
			in->tile[i][j] = '#';
	int x = rand() % 50, y = rand() % 50;
	int xs = rand() % 10 - 5, ys = rand() % 10 - 5;
	for(i = 0; i < 10; i++)
	{
	if(scan(in, x, y, xs, ys) == 0)
		createroom(in, x, y, xs, ys);
	}
}

void createroom(struct map* in, int x, int y, int xs, int ys)
{
	int i, j;
	for(i = x; i < xs; i++)
		for(j = y; j < ys; j++)
			in->tile[i][j] = '.';
}		

int scan(struct map* in, int x, int y, int xs, int ys)
{
	int i, j;
	for(i = x; i < xs; i++)
		for(j = y; j < ys; j++)
			if(i < 0 || i > 49 || j < 0 || j > 49);
				return 1;
	return 0;

}

void add_message(char *in)
{
	int i;
	for(i = 998; i>= 0; i--)
	{
		strcpy(messages[i + 1], messages[i]);
	}
	strcpy(messages[0],in);
}

char* get_item_string(item in)
{
	char str[80];
	if(in.type == 0)
	{
		sprintf(str,
	}
	else
	{
	
	}
}
