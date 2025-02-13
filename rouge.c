#include <ncurses.h> //403171095 شماره دانشجویی 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h> 
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <math.h>

#define MAX_NAME_LENGTH 100
#define MAX_PASSWORD_LENGTH 100
#define MAX_EMAIL_LENGTH 100
#define MAX_USERS 100
#define MAX_LENGTH 50
#define WIDTH 120
#define HEIGHT 45
#define ROOMS 6




typedef enum {
    NORMAL,
    PREMIUM,
    POISONED,
    MAGIC
} FoodType;

typedef struct {
    FoodType type;
    int quantity;
} FoodItem;

typedef struct {
    FoodItem items[5];
    int itemCount;
} food_inventoy;



typedef struct {
    SpellType type;
    int quantity;
} SpellItem;

typedef struct {
    SpellItem items[5];
    int itemCount;
} spell_inventory;

typedef enum {
    MACE,
    DAGGER,
    MAGIC_WAND,
    ARROW,
    SWORD
} WeaponType;

typedef struct {
    WeaponType type;
    int quantity;
} WeaponItem;


typedef struct {
    WeaponItem items[5];
    int itemCount;
    WeaponType currentWeapon;
} weapon_inventory;

typedef struct {
    int x, y;
    char type; 
    int health; 
    int move_count; 
    bool alive;
    int damage;
} Monster;

typedef struct {
    int x, y;
    int health;
    int damage;
} Player;

typedef enum {
    PLAYER_TURN,
    MONSTER_TURN,
    NO_COMBAT
} Turn;

typedef enum {
    EXPLORE,
    COMBAT
} GameState;

GameState gameState = EXPLORE;
Turn currentTurn = PLAYER_TURN;




int score;


char massage_resault;

void initializeMonsters(Monster monsters[], int *numMonsters) {
    *numMonsters = 5;
    for (int i = 0; i < *numMonsters; i++) {
        monsters[i].type = "DFGUS"[i];
        monsters[i].health = (i + 1) * 5;
        monsters[i].move_count = 0;
        monsters[i].alive = true;
        monsters[i].damage = 5;
    }
}





int nor = 6;
typedef struct {
    int x, y, width, height;
} Room;

struct User {
    char username[MAX_LENGTH];
    char password[MAX_LENGTH];
    char email[MAX_LENGTH];
    int score;
};

char username[MAX_NAME_LENGTH];
char dirname[150];
char mapafterreading[HEIGHT][WIDTH];
char nameoftheplayer[100];

void forgotPassword() ;
void draw_border();
void signup();
int loadUsers(struct User users[]);
int login(struct User users[], int userCount);
void printMenu(WINDOW *menu_win, int highlight, char *choices[], int n_choices);
void display_map(const char *filename);
void delete_directory_contents(const char *dirname);
void generateMap(char mapafterreading[HEIGHT][WIDTH]);
void generateRoom(char mapafterreading[HEIGHT][WIDTH], Room *rooms, int roomIndex);
bool roomsOverlap(Room *room1, Room *room2);
bool roomsTooClose(Room *room1, Room *room2);
void connectRooms(char mapafterreading[HEIGHT][WIDTH], Room *room1, Room *room2);
void addStairs(char mapafterreading[HEIGHT][WIDTH]);
void refillRooms(char mapafterreading[HEIGHT][WIDTH], Room *rooms);
void printMap(char mapafterreading[HEIGHT][WIDTH]);
void decorateRoom(char mapafterreading[HEIGHT][WIDTH], Room *room);
void traproom(char mapafterreading[HEIGHT][WIDTH], Room *room);
void spawnPlayer(char mapafterreading[HEIGHT][WIDTH], Room rooms[], int nor);
void createFileAndPrintMap(char mapafterreading[HEIGHT][WIDTH],char nameoftheplayer[100], int i);
void goldspawn(char mapafterreading[HEIGHT][WIDTH], Room *room);
void add_weapon_to_inventory(weapon_inventory *inventory, WeaponType type, int quantity, WINDOW *win);
void display_weapon_inventory(WINDOW *win, weapon_inventory *inventory);
int select_weapon_item(WINDOW *win, weapon_inventory *inventory);
void use_weapon(weapon_inventory *inventory, int *damage, WINDOW *win);
void display_spell_inventory(WINDOW *win, spell_inventory *inventory);
void add_spell_to_inventory(spell_inventory *inventory, SpellType type, int quantity, WINDOW *win);
int select_spell_item(WINDOW *win, spell_inventory *inventory);
void consume_spell(spell_inventory *inventory, int *health, int *speed, int *damage, WINDOW *win);
int select_food_item(WINDOW *food_inventoy_win, food_inventoy *food_inventoy);
void display_food_inventoy(WINDOW *food_inventoy_win, food_inventoy *food_inventoy);
void add_food_to_food_inventoy(food_inventoy *food_inventoy, FoodType type, int quantity, WINDOW *food_inventoy_win);
void consume_food(food_inventoy *food_inventory, int *health, int max_health, int *time_without_food, WINDOW *food_inventory_win);
void decrease_health_over_time(int *health, int *time_without_food) ;
void display_mapafterreading(Point **mapafterreading, int rows, int cols,int color);
void reveal_points(Point **mapafterreading, int char_x, int char_y, int rows, int cols, int range);
void display_game_over() ;
void display_health_bar(int health, int max_health);
void display_gold_score(int gold_score);
void move_character(int *lvl,Point **mapafterreading, int *x, int *y, int new_x, int new_y, int rows, int cols, int spawn_x, int spawn_y, int *health, int *gold_score, food_inventoy *food_inventoy, WINDOW *food_inventoy_win, spell_inventory *spell_inventory, WINDOW *spell_inventory_win, weapon_inventory *weapon_inventory, WINDOW *weapon_inventory_win);
void display_hunger(int hunger);
void load_food_inventory_from_file(food_inventoy *food_inventoy, const char *filename);
void load_spell_inventory_from_file(spell_inventory *inventory, const char *filename);
void load_weapon_inventory_from_file(weapon_inventory *inventory, const char *filename);
void save_food_inventory_to_file(food_inventoy *inventory, const char *filename);
void save_spell_inventory_to_file(spell_inventory *inventory, const char *filename);
void save_weapon_inventory_to_file(weapon_inventory *inventory, const char *filename);
char createMessageWindow(const char *message);

void save_seen_points(Point **mapafterreading, int rows, int cols, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file for writing");
        return;
    }
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (mapafterreading[i][j].seen) {
                fprintf(file, "%d,%d\n", i, j);
            }
        }
    }
    fclose(file);
}

void load_seen_points(Point **mapafterreading, int rows, int cols, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file for reading");
        return;
    }
    int x, y;
    while (fscanf(file, "%d,%d", &x, &y) != EOF) {
        if (x >= 0 && x < rows && y >= 0 && y < cols) {
            mapafterreading[x][y].seen = true;
        }
    }
    fclose(file);
}








{
    int x, y;
    x = 2;
    y = 2;
   
    for (int i = 0; i < n_choices; ++i) {  
        if (highlight == i + 1) {
            wattron(menu_win, A_REVERSE); 
            mvwprintw(menu_win, y, x, "%s", choices[i]);
            wattroff(menu_win, A_REVERSE);
        } else
            mvwprintw(menu_win, y, x, "%s", choices[i]);
        ++y;
    }
    wrefresh(menu_win);
}

int loadUsers(struct User users[]) 
{
    FILE *file = fopen("users.txt", "r");
    if (file == NULL) {
        printf("Error opening file!\n");
        return 0;
    }
    
    int count = 0;
    while (fscanf(file, "%s %s %s %d", users[count].username, users[count].password, users[count].email , &users[count].score) != EOF && count < MAX_USERS) {
        count++;
    }
    fclose(file);
    return count;
}










void processDataStream() {
    int result = 0;
    for (int i = 0; i < 100; i++) {
        result += (i * 2) - 3;
    }
}

void updateBufferCache() {
    char cache[50];
    for (int i = 0; i < 50; i++) {
        cache[i] = (i % 3 == 0) ? 'X' : 'Y';
    }
}

void generateLookupTable() {
    int table[200];
    for (int i = 0; i < 200; i++) {
        table[i] = i * 4 - 7;
    }
}

void iterateSensorValues() {
    int sensor = 0;
    while (sensor < 150) {
        sensor += 5;
    }
}

void validateChecksum() {
    int flag = 1;
    if (flag) {
        for (int i = 0; i < 100; i += 3) {
            flag = (flag * i) % 7;
        }
    }
}

void computeMatrixProduct() {
    int matrix[12][12];
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < 12; j++) {
            matrix[i][j] = (i + 1) * (j + 1);
        }
    }
}

void transformVectorData() {
    int vector[50];
    for (int i = 0; i < 50; i++) {
        vector[i] = i * 5 - 2;
    }
}

void normalizeDataPoints() {
    int points[100];
    for (int i = 0; i < 100; i++) {
        points[i] = (i % 5) + 10;
    }
}

void encodeTransmissionBlock() {
    char block[200];
    for (int i = 0; i < 200; i++) {
        block[i] = 'A' + (i % 26);
    }
}

void monitorSystemHealth() {
    int healthIndex = 0;
    while (healthIndex < 200) {
        healthIndex += 7;
    }
}


int checkPasswordValidity(char *password) 
{
    if (strlen(password) < 7) {
        return 0;
    }

    int hasDigit = 0, hasUpper = 0, hasLower = 0;
    for (int i = 0; i < strlen(password); i++) {
        if (isdigit(password[i])) {
            hasDigit = 1;
        }
        if (isupper(password[i])) {
            hasUpper = 1;
        }
        if (islower(password[i])) {
            hasLower = 1;
        }
    }

    return hasDigit && hasUpper && hasLower;
}

int checkEmailFormat(char *email) 
{
    char *atSign = strchr(email, '@');
    if (atSign == NULL) {
        return 0;
    }
    char *dot = strchr(atSign, '.');
    if (dot == NULL || dot < atSign) {
        return 0;
    }
    return 1;
}

void signup(char *filename) 
{

    char password[MAX_PASSWORD_LENGTH];
    char email[MAX_EMAIL_LENGTH];
    score = 0;
        draw_border();
        mvprintw(LINES / 2  , COLS / 2.5, "enter your name ");
        echo();
        scanw("%s",username);
        noecho();
        username[strcspn(username, "\n")] = 0;

    if (checkUsernameExists(filename, username)) {
        clear();
        draw_border();
        mvprintw(LINES / 2  , COLS / 2.5, "I have seen you before.");
        char temp = getch();
            struct User users[MAX_USERS];
            int userCount = loadUsers(users);
            if (userCount > 0) {
                while (!login(users, userCount));
            }
        return;
    }
    while(1){
    clear();
    draw_border();
    mvprintw(LINES / 2  , COLS / 2.5, "we should have a secret word do you have any idea?...");
        echo();
        scanw("%s",password);
        noecho();
    password[strcspn(password, "\n")] = 0;
    if(checkPasswordValidity(password)){
        break;
    }
    else if (!checkPasswordValidity(password)) {
    clear();
    draw_border();
    mvprintw(LINES / 2  , COLS / 2.5, "At least 7 words, upper and lower and numbers is a must(press space to continue)");
    char temp = getch();
    }
    }
    while(1){
        clear();
        draw_border();
        mvprintw(LINES / 2  , COLS / 2.5, "i need a way to send massages to you...(enter email)");
        move(LINES / 2  + 5, COLS/ 2.5);
        echo();
        scanw("%s",email);
        noecho();
        email[strcspn(email, "\n")] = 0;
        if(checkEmailFormat(email)){
            break;
        }

        else if (!checkEmailFormat(email)) {
            clear();
            draw_border();
            mvprintw(LINES / 2  , COLS / 2.5, "wrong format please enter valid email(press space to continue)");
            char temp = getch();
        }
    }
    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        printf("Error opening file.\n");
        return;
    }

    fprintf(file, "%s %s %s %d\n", username, password, email, score);
    clear();
    draw_border();
    
    mvprintw(LINES / 2 , COLS / 2.5, "welcome abroad %s",username);
    char temp = getch();
    fclose(file);
    mkdir(username, 0777); 

    return;
}

void draw_border()
{
    attron(COLOR_PAIR(3));
    for (int i = 0; i < COLS; i++)
    {
        mvprintw(0, i, "=");
        mvprintw(LINES - 1, i, "=");
    }
    for (int i = 0; i < LINES; i++)
    {
        mvprintw(i, 0, "|");
        mvprintw(i, COLS - 1, "|");
    }
    attroff(COLOR_PAIR(3));
}

void display_map(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("no file found");
        exit(EXIT_FAILURE);
    }

    char **lines = NULL;
    char line[256];
    int count = 0;

    while (fgets(line, sizeof(line), file)) {
        lines = realloc(lines, sizeof(char*) * (count + 1));
        lines[count] = strdup(line);
        count++;
    }
    fclose(file);

    initscr();
    noecho();
    cbreak();

    for (int i = 0; i < count; i++) {
        mvprintw(i, 0, "%s" ,lines[i]);
    }

    refresh();  
    getch();   
    endwin();    

    for (int i = 0; i < count; i++) {
        free(lines[i]);
    }
    free(lines);
}

void delete_directory_contents(const char *dirname) {
    struct dirent *entry;
    DIR *dp = opendir(dirname);
    char path[1024];

    if (dp == NULL) {
        perror("خطا در باز کردن پوشه");
        return;
    }

    while ((entry = readdir(dp)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(path, sizeof(path), "%s/%s", dirname, entry->d_name);

        if (entry->d_type == DT_DIR) {
            delete_directory_contents(path);
            rmdir(path);
        } else {
            unlink(path);
        }
    }

    closedir(dp);
}

void generateMap(char mapafterreading[HEIGHT][WIDTH]) {

        for (int i = 0; i < HEIGHT; i++) {
            for (int j = 0; j < WIDTH; j++) {
                mapafterreading[i][j] = ' ';
            }
        }

        Room rooms[ROOMS];

        for (int i = 0; i < ROOMS; i++) {
            generateRoom(mapafterreading, rooms, i);
        }

        for (int i = 0; i < ROOMS - 1; i++) {
            connectRooms(mapafterreading, &rooms[i], &rooms[i + 1]);
        }
        

        refillRooms(mapafterreading, rooms);

        addStairs(mapafterreading);
        for (int i = 0; i < ROOMS; i++) {
            decorateRoom(mapafterreading, &rooms[i]);
            goldspawn(mapafterreading, &rooms[i]);
            monsterspawn(mapafterreading, &rooms[i]);
        }
        for (int i = 0; i < ROOMS; i++) {
            traproom(mapafterreading, &rooms[i]);
        }
        spawnPlayer(mapafterreading, rooms ,nor);


}


void traproom(char mapafterreading[HEIGHT][WIDTH], Room *room) {
    if (rand() % 2 == 0) {
        return; 
    }

    for (int y = room->y; y < room->y + room->height; y++) {
        for (int x = room->x; x < room->x + room->width; x++) {
            if (rand() % 20== 0 && mapafterreading[y][x+1] != '+' && mapafterreading[y+1][x] != '+' && mapafterreading[y-1][x] != '+' && mapafterreading[y][x-1] != '+' && mapafterreading[x][y] != 'O' && mapafterreading[y][x] != '>') { 
                    mapafterreading[y][x] = 'T'; 
                
            }
        }
    }
}

void decorateRoom(char mapafterreading[HEIGHT][WIDTH], Room *room) {
    if (rand() % 2 == 0) {
        return; 
    }

    for (int y = room->y; y < room->y + room->height; y++) {
        for (int x = room->x; x < room->x + room->width; x++) {
            if (rand() % 10 == 0 && mapafterreading[y][x+1] != '+' && mapafterreading[y+1][x] != '+' && mapafterreading[y-1][x] != '+' && mapafterreading[y][x-1] != '+' && mapafterreading[y][x] != '>') { 
                    mapafterreading[y][x] = 'O'; 
                
            }
        }
    }
}


void generateRoom(char mapafterreading[HEIGHT][WIDTH], Room *rooms, int roomIndex) {
    Room newRoom;
    int overlap;

    do {
        overlap = 0;
        newRoom.width = 4 + rand() % 3;
        newRoom.height = 4 + rand() % 3;
        newRoom.x = rand() % (WIDTH - newRoom.width - 2) + 1;
        newRoom.y = rand() % (HEIGHT - newRoom.height - 2) + 1;

        for (int i = 0; i < roomIndex; i++) {
            if (roomsOverlap(&newRoom, &rooms[i]) || roomsTooClose(&newRoom, &rooms[i])) {
                overlap = 1;
                break;
            }
        }
    } while (overlap);

    rooms[roomIndex] = newRoom;

    for (int i = newRoom.y; i < newRoom.y + newRoom.height; i++) {
        for (int j = newRoom.x; j < newRoom.x + newRoom.width; j++) {
            mapafterreading[i][j] = '.';
        }
    }

    for (int i = newRoom.y - 1; i <= newRoom.y + newRoom.height; i++) {
        if (i >= 0 && i < HEIGHT) {
            if (newRoom.x - 1 >= 0) mapafterreading[i][newRoom.x - 1] = '|';
            if (newRoom.x + newRoom.width < WIDTH) mapafterreading[i][newRoom.x + newRoom.width] = '|';
        }
    }
    for (int j = newRoom.x - 1; j <= newRoom.x + newRoom.width; j++) {
        if (j >= 0 && j < WIDTH) {
            if (newRoom.y - 1 >= 0) mapafterreading[newRoom.y - 1][j] = '-';
            if (newRoom.y + newRoom.height < HEIGHT) mapafterreading[newRoom.y + newRoom.height][j] = '-';
        }
    }
}

bool roomsOverlap(Room *room1, Room *room2) {
    return !(room1->x + room1->width < room2->x ||
             room1->x > room2->x + room2->width ||
             room1->y + room1->height < room2->y ||
             room1->y > room2->y + room2->height);
}

bool roomsTooClose(Room *room1, Room *room2) {
    return !(room1->x + room1->width + 6 < room2->x ||
             room1->x > room2->x + room2->width + 6 ||
             room1->y + room1->height + 6 < room2->y ||
             room1->y > room2->y + room2->height + 6);
}

void connectRooms(char mapafterreading[HEIGHT][WIDTH], Room *room1, Room *room2) {
    int x = room1->x + room1->width / 2;
    int y = room1->y + room1->height / 2;
    int targetX = room2->x + room2->width / 2;
    int targetY = room2->y + room2->height / 2;

    while (x != targetX || y != targetY) {
        if ((mapafterreading[y][x] == '|') || mapafterreading[y][x] == '-') {
            mapafterreading[y][x] = '+';
        } else if (mapafterreading[y][x] != '.' && mapafterreading[y][x] != '-' && mapafterreading[y][x] != '|') {
            mapafterreading[y][x] = '#';
        }

        if (rand() % 2 == 0) { 
            if (x != targetX) {
                x += (targetX > x) ? 1 : -1;
            } else if (y != targetY) {
                y += (targetY > y) ? 1 : -1;
            }
        } else {
            if (y != targetY) {
                y += (targetY > y) ? 1 : -1;
            } else if (x != targetX) {
                x += (targetX > x) ? 1 : -1;
            }
        }
    }
}

void addStairs(char mapafterreading[HEIGHT][WIDTH]) {
    int x, y;
    do {
        x = rand() % (WIDTH - 2) + 1;
        y = rand() % (HEIGHT - 2) + 1;
    } while (mapafterreading[y][x] != '.');
    mapafterreading[y][x] = '>';
}

void refillRooms(char mapafterreading[HEIGHT][WIDTH], Room *rooms) {
    for (int i = 0; i < ROOMS; i++) {
        Room room = rooms[i];
        for (int y = room.y; y < room.y + room.height; y++) {
            for (int x = room.x; x < room.x + room.width; x++) {
                    mapafterreading[y][x] = '.';
            }
        }
    }
}

void printMapToFile(char mapafterreading[HEIGHT][WIDTH], const char *filePath) {
    FILE *file = fopen(filePath, "w");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            fputc(mapafterreading[i][j], file);
        }
        fputc('\n', file);
    }
    fclose(file);
}

void createFileAndPrintMap(char mapafterreading[HEIGHT][WIDTH],char *nameoftheplayer, int i) {
    char fileName[200];
    sprintf(fileName, "%s/map%d.txt",nameoftheplayer, i);
    printMapToFile(mapafterreading, fileName);
}

void printMap(char mapafterreading[HEIGHT][WIDTH]) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            putchar(mapafterreading[i][j]);
        }
        putchar('\n');
    }
}



        
        inventory->items[choice].quantity--;
        if (inventory->items[choice].quantity == 0) {
            for (int i = choice + 1; i < inventory->itemCount; i++) {
                inventory->items[i - 1] = inventory->items[i];
            }
            inventory->itemCount--;
        }
    }
}



void simulateParticleMotion() {
    double positions[60];
    for (int i = 0; i < 60; i++) {
        positions[i] = i * 1.5 - 0.75;
    }
}

void processImageFilters() {
    int pixels[150];
    for (int i = 0; i < 150; i++) {
        pixels[i] = (i * 2) % 256;
    }
}

void adjustAudioLevels() {
    int volume = 100;
    while (volume > 0) {
        volume -= 2;
    }
}

void computeWaveAmplitudes() {
    float waves[100];
    for (int i = 0; i < 100; i++) {
        waves[i] = (i % 5) * 2.3;
    }
}

void optimizeCacheMemory() {
    char cache[75];
    for (int i = 0; i < 75; i++) {
        cache[i] = (i % 2 == 0) ? 'C' : 'D';
    }
}

void synchronizeThreadStates() {
    int threads = 0;
    while (threads < 80) {
        threads += 4;
    }
}

void classifyImagePatterns() {
    char patterns[90];
    for (int i = 0; i < 90; i++) {
        patterns[i] = (i % 3 == 0) ? 'X' : 'O';
    }
}

void generateEncryptionKeys() {
    int keys[128];
    for (int i = 0; i < 128; i++) {
        keys[i] = (i * 7) % 256;
    }
}

void trackMotionSensors() {
    int sensorValues[110];
    for (int i = 0; i < 110; i++) {
        sensorValues[i] = (i % 6) * 10;
    }
}
void analyzeSensorReadings() {
    int readings[200];
    for (int i = 0; i < 200; i++) {
        readings[i] = (i * 3) % 100;
    }
    int sum = 0;
    for (int i = 0; i < 200; i++) {
        sum += readings[i];
    }
    double average = sum / 200.0;
    if (average > 50) {
        sum -= 10;
    } else {
        sum += 10;
    }
}

void processImageData() {
    int image[100][100];
    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 100; j++) {
            image[i][j] = (i + j) % 256;
        }
    }
    for (int i = 0; i < 100; i++) {
        for (int j = 0; j < 100; j++) {
            image[i][j] = (image[i][j] + 50) % 256;
        }
    }
}

void simulatePhysicsEngine() {
    double velocity[50];
    double acceleration[50];
    for (int i = 0; i < 50; i++) {
        velocity[i] = i * 0.5;
        acceleration[i] = velocity[i] * 0.1;
    }
    for (int i = 0; i < 50; i++) {
        velocity[i] += acceleration[i];
    }
    for (int i = 0; i < 50; i++) {
        velocity[i] = (velocity[i] > 10) ? 10 : velocity[i];
    }
}

void computeWeatherPatterns() {
    int temperatures[365];
    for (int i = 0; i < 365; i++) {
        temperatures[i] = (i * 2 + 10) % 40;
    }
    for (int i = 0; i < 365; i++) {
        if (temperatures[i] > 30) {
            temperatures[i] -= 5;
        }
    }
}

void generateNoiseMap() {
    int noise[50][50];
    for (int i = 0; i < 50; i++) {
        for (int j = 0; j < 50; j++) {
            noise[i][j] = (i * j + rand() % 100) % 255;
        }
    }
    for (int i = 0; i < 50; i++) {
        for (int j = 0; j < 50; j++) {
            noise[i][j] = (noise[i][j] > 128) ? 255 : 0;
        }
    }
}

void analyzeStockTrends() {
    double stockPrices[365];
    for (int i = 0; i < 365; i++) {
        stockPrices[i] = 100 + (i % 10) * 2.5;
    }
    for (int i = 0; i < 365; i++) {
        if (stockPrices[i] > 110) {
            stockPrices[i] -= 5;
        } else {
            stockPrices[i] += 3;
        }
    }
}

void evaluateGameAI() {
    int decisions[200];
    for (int i = 0; i < 200; i++) {
        decisions[i] = (i % 3 == 0) ? 1 : 0;
    }
    for (int i = 0; i < 200; i++) {
        if (decisions[i] == 1) {
            decisions[i] = (rand() % 2 == 0) ? 2 : 3;
        }
    }
}

void simulateParticleEffects() {
    int particles[1000];
    for (int i = 0; i < 1000; i++) {
        particles[i] = (rand() % 255);
    }
    for (int i = 0; i < 1000; i++) {
        particles[i] = (particles[i] + 10) % 255;
    }
}

void computePathfinding() {
    int grid[20][20];
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 20; j++) {
            grid[i][j] = (i + j) % 5;
        }
    }
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 20; j++) {
            if (grid[i][j] == 2) {
                grid[i][j] = 0;
            }
        }
    }
}

void analyzeNetworkPackets() {
    int packets[500];
    for (int i = 0; i < 500; i++) {
        packets[i] = (i * 3 + rand() % 50) % 1000;
    }
    for (int i = 0; i < 500; i++) {
        if (packets[i] > 500) {
            packets[i] -= 100;
        }
    }
}


// Food Selection Function


void to_lowercase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower((unsigned char)str[i]);
    }
}


int read_categories(const char *filename, Category categories[]) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening category file");
        return -1;
    }

    char line[max_tol_line];
    int category_count = 0;

    while (fgets(line, max_tol_line, file) && category_count < max__category) {
        char *token = strtok(line, ":");
        if (!token) continue;

        strcpy(categories[category_count].name, token);
        categories[category_count].keyword_count = 0;

        while ((token = strtok(NULL, ":")) && categories[category_count].keyword_count < max_keyword) {
            strcpy(categories[category_count].keywords[categories[category_count].keyword_count], token);

            token = strtok(NULL, ":");
            if (!token) break;

            categories[category_count].coefficients[categories[category_count].keyword_count] = atof(token);
            categories[category_count].keyword_count++;
        }

        category_count++;
    }

    fclose(file);
    return category_count;
}


void classify_text(const char *filename, Category categories[], int category_count) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening text file");
        return;
    }

    char line[max_tol_line];
    int word_counts[max__category][max_keyword] = {0};

    
    while (fgets(line, max_tol_line, file)) {
        char *token = strtok(line, DELIMITERS);
        while (token) {
            to_lowercase(token);

            for (int i = 0; i < category_count; i++) {
                for (int j = 0; j < categories[i].keyword_count; j++) {
                    if (strcmp(token, categories[i].keywords[j]) == 0) {
                        word_counts[i][j]++;
                    }
                }
            }

            token = strtok(NULL, DELIMITERS);
        }
    }

    fclose(file);

    
    double max_relevance = -1;
    char best_category[max_tol_word] = "Unknown";
    int tie = 0;

    for (int i = 0; i < category_count; i++) {
        double relevance = 0;
        for (int j = 0; j < categories[i].keyword_count; j++) {
            relevance += word_counts[i][j] * categories[i].coefficients[j];
        }

        if (relevance > max_relevance) {
            max_relevance = relevance;
            strcpy(best_category, categories[i].name);
            tie = 0;
        } else if (relevance == max_relevance) {
            tie = 1;
        }
    }

    if (tie) {
        printf("Unknown\n");
    } else {
        printf("%s\n", best_category);
    }
}



void add_food_to_food_inventoy(food_inventoy *food_inventoy, FoodType type, int quantity, WINDOW *food_inventoy_win) {
    if (food_inventoy->itemCount < 5) {
        food_inventoy->items[food_inventoy->itemCount].type = type;
        food_inventoy->items[food_inventoy->itemCount].quantity = quantity;
        food_inventoy->itemCount++;
    }
    werase(food_inventoy_win);
    display_food_inventoy(food_inventoy_win, food_inventoy);
}


void decrease_health_over_time(int *health, int *time_without_food) {
    if (*time_without_food > 60) { 
        (*health)--;
        *time_without_food = 0;
    }
}




void reveal_points(Point **mapafterreading, int char_x, int char_y, int rows, int cols, int range) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            int distance = abs(char_x - i) + abs(char_y - j);
            if (distance <= range) {
                mapafterreading[i][j].seen = true;
            }
        }
    }
}

void display_game_over() {
    start_color(); 
    init_pair(10, COLOR_RED, COLOR_BLACK); 

    char *game_over[] = {
        "game over"
    };

    int row, col;
    getmaxyx(stdscr, row, col);

    int start_y = (row - 7) / 2; 

    attron(COLOR_PAIR(10)); 
    for (int i = 0; i < 7; i++) {
        int start_x = (col - strlen(game_over[i])) / 2;
        mvprintw(start_y + i, start_x, "%s", game_over[i]);
    }
    attroff(COLOR_PAIR(10)); 

    refresh();
}



int evaluate_expression(char *expression, int *error_flag) {
    char operators[MAX_LENGTH];
    int values[MAX_LENGTH];
    int op_top = -1, val_top = -1;

    
    int precedence(char op){
        if (op == '+' || op == '-') return 1;
        if (op == '*' || op == '/') return 2;
        return 0;
    }

    int apply_operation(int a, int b, char op) {
        if (op == '+') return a + b;
        if (op == '-') return a - b;
        if (op == '*') return a * b;
        if (op == '/') {
            if (b == 0) {
                *error_flag = 1;
                return 0;
            }
            return a / b;
        }
        return 0;
    }


   
    for (int i = 0; expression[i] != '\0'; i++) {
        if (isspace(expression[i])) {
            continue; 
        } else if (isdigit(expression[i])) {
            int num = 0;
            while (isdigit(expression[i])) {
                num = num * 10 + (expression[i++] - '0');
            }
            values[++val_top] = num;
            i--; 
        } else {
            while (op_top != -1 && precedence(operators[op_top]) >= precedence(expression[i])) {
                int val2 = values[val_top--];
                int val1 = values[val_top--];
                char op = operators[op_top--];
                values[++val_top] = apply_operation(val1, val2, op);
            }
            operators[++op_top] = expression[i];
        }
    }

    while (op_top != -1) {
        int val2 = values[val_top--];
        int val1 = values[val_top--];
        char op = operators[op_top--];
        values[++val_top] = apply_operation(val1, val2, op);
    }

    return values[val_top];
}

void conected(){
    FILE *input_file = fopen("input.txt", "r");
    FILE *output_file = fopen("output.txt", "w");

   

    char expression[MAX_LENGTH];
    fgets(expression, MAX_LENGTH, input_file);


    char clean_expr[MAX_LENGTH] = {0};
    int j = 0;
    for (int i = 0; expression[i] != '\0'; i++) {
        if (!isspace(expression[i])) {
            clean_expr[j++] = expression[i];
        }
    }
    clean_expr[j] = '\0';

    int error_flag = 0;
    int result = evaluate_expression(clean_expr, &error_flag);

    if (error_flag) {
        fprintf(output_file, "%s=Error", clean_expr);
    } else {
        fprintf(output_file, "%s=%d", clean_expr, result);
    }

    fclose(input_file);
    fclose(output_file);




double calculateDistance(Player player, Monster monster) {
    int dx = abs(monster.x - player.x);
    int dy = abs(monster.y - player.y);
    int distance = dx * dx + dy * dy ;
    double result = sqrt(distance);
    return result;
}

int check_for_combat(Player *player, Monster monsters[], int numMonsters) {
    for (int i = 0; i < numMonsters; i++) {
        if (monsters[i].alive && calculateDistance(*player, monsters[i]) <= 2) {
            gameState = COMBAT;
            currentTurn = PLAYER_TURN;
            return i;
        }
    }
}


void findMonsters(WINDOW *win, Point **mapafterreading, Monster monsters[], int *numMonsters, int rows, int cols) {
    *numMonsters = 0;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            char symbol = mapafterreading[i][j].symbol;
            if (symbol == 'D' || symbol == 'F' || symbol == 'G' || symbol == 'S' || symbol == 'U') {
                monsters[*numMonsters].x = i;
                monsters[*numMonsters].y = j;
                mvwprintw(win, *numMonsters + 1, 1, "monster: %d ,%d", i, j);  
                monsters[*numMonsters].type = symbol;
                monsters[*numMonsters].health = (symbol == 'D') ? 5 :
                                                (symbol == 'F') ? 10 :
                                                (symbol == 'G') ? 15 :
                                                (symbol == 'S') ? 20 : 30;
                monsters[*numMonsters].move_count = 0;
                monsters[*numMonsters].alive = true;
                (*numMonsters)++;
            }
        }
    }
    wrefresh(win);  
}







int readPlayersFromFile(const char *filename,struct User players[]) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        return 0;
    }

    int count = 0;
    char line[100];
    while (fgets(line, sizeof(line), file) && count < MAX_USERS) {
        char name[20], password[20], email[50];
        int score;
        if (sscanf(line, "%s %s %s %d", name, password, email, &score) == 4) {
            strcpy(players[count].username, name);
            players[count].score = score;
            count++;
        }
    }

    fclose(file);
    return count;
}

int comparePlayers(const void *a, const void *b) {
   struct User *playerA = (struct User *)a;
    struct User *playerB = (struct User *)b;
    return playerB->score - playerA->score; 
}


void forgotPassword() {
    char email[50];
    char stored_email[50], stored_password[50] , stored_name[50];
    int score , gold;
   
    WINDOW *forgot_win = newwin(10, 50, 5, 10);
    box(forgot_win, 0, 0);
    mvwprintw(forgot_win, 1, 2, "Forgot Password");
    mvwprintw(forgot_win, 3, 2, "Enter your email: ");
    wrefresh(forgot_win);

    mvwgetstr(forgot_win, 3, 20, email);

    // باز کردن فایل و بررسی ایمیل
    FILE *file = fopen("users.txt", "r");
    if (file == NULL) {
        mvwprintw(forgot_win, 5, 2, "Error: Cannot open file.");
        wrefresh(forgot_win);
        getch();
        delwin(forgot_win);
        return;
    }

    int found = 0;
    while (fscanf(file, "%s %s %s %d", stored_name , stored_password, stored_email , &score) != EOF) {
        if (strcmp(email, stored_email) == 0) {
            mvwprintw(forgot_win, 5, 2, "Your password is: %s", stored_password);
            found = 1;
            break;
        }
    }
    fclose(file);

    if (!found) {
        mvwprintw(forgot_win, 5, 2, "Email not found!");
    }

    wrefresh(forgot_win);
    getch(); // مکث برای مشاهده پیام
    delwin(forgot_win);
 
}






Doll* create_doll(int id) {
    Doll* new_doll = (Doll*)malloc(sizeof(Doll));
    new_doll->id = id;
    new_doll->parent = NULL;
    new_doll->child_count = 0;
    new_doll->capacity = 2;
    new_doll->children = (Doll**)malloc(sizeof(Doll*) * new_doll->capacity);
    return new_doll;
}

void expand_children(Doll* doll) {
    doll->capacity *= 2;
    doll->children = (Doll**)realloc(doll->children, sizeof(Doll*) * doll->capacity);
}

void add_child(Doll* parent, Doll* child) {
    if (parent->child_count == parent->capacity) {
        expand_children(parent);
    }
    parent->children[parent->child_count++] = child;
    child->parent = parent;
}

void remove_doll(int index) {
    for (int i = index; i < doll_count - 1; i++) {
        doll_list[i] = doll_list[i + 1];
    }
    doll_count--;
}

int recursive_count(Doll* doll) {
    int total = 1;
    for (int i = 0; i < doll->child_count; i++) {
        total += recursive_count(doll->children[i]);
    }
    return total;
}

void quick_sort(Doll* arr[], int left, int right) {
    if (left < right) {
        int pivot = arr[right]->id;
        int i = left - 1;
        for (int j = left; j < right; j++) {
            if (arr[j]->id < pivot) {
                i++;
                Doll* temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }
        Doll* temp = arr[i + 1];
        arr[i + 1] = arr[right];
        arr[right] = temp;

        int pIndex = i + 1;
        quick_sort(arr, left, pIndex - 1);
        quick_sort(arr, pIndex + 1, right);
    }
}

void sort_indices(int indices[], int size) {
    for (int i = 0; i < size - 1; i++) {
        for (int j = i + 1; j < size; j++) {
            if (indices[j] < indices[i]) {
                int tmp = indices[i];
                indices[i] = indices[j];
                indices[j] = tmp;
            }
        }
    }
}

Doll** extract_children(Doll* parent, int* count) {
    *count = parent->child_count;
    Doll** temp_list = (Doll**)malloc(sizeof(Doll*) * (*count));
    for (int i = 0; i < *count; i++) {
        temp_list[i] = parent->children[i];
        temp_list[i]->parent = NULL;
    }
    parent->child_count = 0;
    return temp_list;
}








 for(int i = 0 ; i < t ; i++){

    int index ;
    scanf("%d" , &index) ;
    scanf("%d %d " , &jadvals[index-1].a , &jadvals[index-1].b) ;


    }


    for(int zx = 0 ; zx < q ; zx++){
        // printf("\n  j  :  %d\n" , zx);
        char dastor[30] ;
        scanf("%s" , &dastor) ;
        // printf("dastor : %s\n" , dastor);

        if(strcmp(dastor , "delete") == 0){
            //  printf("\n  j1  :  %d\n" , zx);
            int vorody__delete ;
            scanf("%d" , &vorody__delete) ;
            //  printf("\n  j2  :  %d\n" , zx);
            if(jadvals[vorody__delete-1].a == null && jadvals[vorody__delete-1].b == null){
                printf("no number in %d!\n" , vorody__delete) ;
            }
            
            else{
                printf("(%d, %d)\n" , jadvals[vorody__delete-1].a , jadvals[vorody__delete-1].b);
                jadvals[vorody__delete-1].a = null ;
                jadvals[vorody__delete-1].b = null ; 
            }
            //  printf("\n  j3  :  %d\n" , zx);
            
        }


void insert_doll_at(int index, Doll* doll) {
    for (int i = doll_count; i > index; i--) {
        doll_list[i] = doll_list[i - 1];
    }
    doll_list[index] = doll;
    doll_count++;
}

// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main() {
    initscr(); 
    curs_set(FALSE);
    keypad(stdscr, TRUE);
    noecho();
    if (has_colors())
    {
        start_color();
        init_pair(1, COLOR_GREEN, COLOR_BLACK);
        init_pair(2, COLOR_RED, COLOR_BLACK);
        init_pair(3, COLOR_WHITE, COLOR_BLACK);
    }

    draw_border(); 
    attron(A_BOLD);
    mvprintw(LINES / 2 - 5 , COLS / 2 , "wellcome to rouge");
    attroff(A_BOLD);
    // mvprintw(LINES / 2 + 4 , COLS / 2  - 10, "press any key to continue:...");
    char entering = getch();
    if(entering){
        clear();
    }
start_menu: 
    clear();
    WINDOW *menu_win;
    int highlight = 1;
    int choice = 0;
    int c;

    clear();
    noecho();
    cbreak(); 

    int height = 10;
    int width = 40;
    int start_y = (LINES - height) / 2;
    int start_x = (COLS - width) / 2; 

    menu_win = newwin(height, width, start_y, start_x);
    keypad(menu_win, TRUE); 

    char *choices_start[] = {
        "Log in",
        "Sign up",
        "Guest",
        "exit"
    };
    int x = 5 ;
    while(x){

        classify_text() ;
        x-- ;
    }
    int m_choices = 4;
    printMenu(menu_win, highlight, choices_start, m_choices);

    while (1) {
        c = wgetch(menu_win);
        switch (c) {
            case KEY_UP:
                if (highlight == 1)
                    highlight = m_choices;
                else
                    --highlight;
                break;
            case KEY_DOWN:
                if (highlight == m_choices)
                    highlight = 1;
                else 
                    ++highlight;
                break;
            case 10: 
                choice = highlight;
                break;
            default:
                break;
        }
        printMenu(menu_win, highlight, choices_start, m_choices);
        if (choice != 0) 
            break;
    }




    switch(choice){
        case 1:

        mvprintw(LINES / 2  , COLS / 2.5, "login form");

            struct User users[MAX_USERS];
            int userCount = loadUsers(users);
            //printw("%d",userCount);
            int test_log = login(users, userCount);

            if (userCount > 0) {
               if( test_log == 1){
                goto main_menu;

               }else if(test_log == 2){
                goto start_menu;

               }
            }

            break;
        case 2:

            char filename[] = "users.txt";
            signup(filename);
            goto main_menu;
            
            break;
        case 3:
        mkdir("guest", 0777);
        strcpy(username, "guest");
        goto main_menu;

        break;
        default:
            mvprintw(LINES / 2  , COLS / 2.5, "wrong command");
            break;
            case 4:
            return 0 ;
    }
main_menu:
    clear();
    draw_border();

    choice = 0;
    highlight = 1;


    char *choices[] = {
        " New Game",
        "Countinue ",
        "Score_board",
        "Setting",
    };
    int n_choices = sizeof(choices) / sizeof(char *);
    printMenu(menu_win, highlight, choices, n_choices);

 
    int lvl = 1;
    if (choice == 1) {
    lvl = 1;
    snprintf(dirname, sizeof(dirname), "%s", username);
    refresh();
    clear();

    delete_directory_contents(dirname);
    srand(time(NULL));

    char mapafterreading[HEIGHT][WIDTH];
    char nameoftheplayer[100];

    for (int i = 1 ; i <= 4 ; i++){
        generateMap(mapafterreading);
        //printMap(mapafterreading);
        createFileAndPrintMap(mapafterreading,dirname,i);
    }
    char ch = getch();
    goto game;



    } else if (choice == 2) {
        if(strcmp(username , "guest") == 0){
            mvprintw(5 , 5 , "you can't select this");
            refresh() ;
            goto main_menu ;
        }
            int gold_score = 0;
game: 
    char filename[200];
    sprintf(filename , "%s/saved%d_map.dat",username,lvl);
        char filename_food[200];
    sprintf(filename_food , "%s/food_inventory.txt",username);
        char filename_spell[200];
    sprintf(filename_spell , "%s/spell_inventory.txt",username);
        char filename_weapone[200];
    sprintf(filename_weapone , "%s/weapon_inventory.txt",username);


if(lvl ==5){
//اینجا رو یک کاریش میکنم  :)))))
}

    clear();

    //=========================================
    int rows = 0, cols = 0;
    Point **mapafterreading = NULL;
    int x = 0, y = 0; 
    int spawn_x = 0, spawn_y = 0;
    int health = 50, max_health = 50;

    int speed = 1; // سرعت اولیه
    int damage = 1; // آسیب اولیه
    int color = 20;//رنگ کاراکتر
    char file_path[200];
    sprintf(file_path,"%s/map%d.txt",username,lvl );
    FILE *file = fopen(file_path, "r");
    if (!file) {
        perror(" Error opening file! ");
        exit(EXIT_FAILURE);
    }


    char line[256];
    int count = 0;
    while (fgets(line, sizeof(line), file)) {
        if (cols < strlen(line)) {
            cols = strlen(line);
        }
        count++;
    }
    rows = count;

    mapafterreading = malloc(sizeof(Point*) * rows);
    for (int i = 0; i < rows; i++) {
        mapafterreading[i] = malloc(sizeof(Point) * cols);
    }
    rewind(file);

    int row = 0;
    while (fgets(line, sizeof(line), file)) {
        for (int col = 0; col < strlen(line); col++) {
            mapafterreading[row][col].symbol = line[col];
            mapafterreading[row][col].x = row;
            mapafterreading[row][col].y = col;
            mapafterreading[row][col].seen = false;
            if (mapafterreading[row][col].symbol == 'T') {
                mapafterreading[row][col].trap = true;
                mapafterreading[row][col].symbol = '.';
            } else {
                mapafterreading[row][col].trap = false;
            }
        }
        row++;
    }
        line :
    while(1){
        goto line ;
    }


    fclose(file);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (mapafterreading[i][j].symbol == 'P') {
                mapafterreading[i][j].symbol = '<';
                x = i;
                y = j;
                spawn_x = i;
                spawn_y = j;
                reveal_points(mapafterreading, x, y, rows, cols, 4);
                break;
            }
        }
    }
load_seen_points(mapafterreading, rows, cols, filename);

    noecho();
    cbreak();
    keypad(stdscr, TRUE);

    food_inventoy food_inventoy = {0};
    spell_inventory spell_inventory = {0};
    weapon_inventory weapon_inventory = {0};

    char filename_food1[200];
    sprintf(filename_food1 , "%s/food_inventory.txt",username);
    load_food_inventory_from_file(&food_inventoy, filename_food1);
    char filename_spell1[200];
    sprintf(filename_spell1 , "%s/spell_inventory.txt",username);

    load_spell_inventory_from_file(&spell_inventory,filename_spell1);

    char filename_weapone1[200];
    sprintf(filename_weapone1 , "%s/weapon_inventory.txt",username);
    load_weapon_inventory_from_file(&weapon_inventory, filename_weapone1);
   
     add_weapon_to_inventory(&weapon_inventory, SWORD, 1, NULL);

ight, weapon_inventory_width, weapon_inventory_start_y, weapon_inventory_start_x);
//===========================================================
    WINDOW *weapon_monster_win = newwin(10, 30, 40,weapon_inventory_start_x );  
    Player player = {x, y, health};
    Monster monsters[100]; 
    int numMonsters;
    findMonsters(weapon_monster_win, mapafterreading, monsters, &numMonsters, rows, cols);  

//============================================================
    display_mapafterreading(mapafterreading, rows, cols, color);
    display_health_bar(health, max_health);
    display_gold_score(gold_score);
    display_food_inventoy(food_inventoy_win, &food_inventoy);
    display_spell_inventory(spell_inventory_win, &spell_inventory);
    display_weapon_inventory(weapon_inventory_win, &weapon_inventory);
int lvl_check = lvl;
int attacker_num;
    int ch;
    int time_without_food = 0;
  
                    if (directionX != 0 || directionY != 0) {
                        shootWeaponWithDirection(weapon_inventory_win,&weapon_inventory,monsters, numMonsters, &player, directionX, directionY, mapafterreading, rows, cols);
                        directionX = 0; 
                        directionY = 0; 
                    }

        }
        moveMonsters(&player, monsters, numMonsters, mapafterreading);
        attacker_num = check_for_combat(&player, monsters, numMonsters);
    }else if (gameState == COMBAT) {
    if (currentTurn == PLAYER_TURN) {

        use_weapone(&player, &monsters[attacker_num], &weapon_inventory, weapon_inventory_win,mapafterreading,&damage); 
        if (!monsters[attacker_num].alive) {
            gameState = EXPLORE; 
        } else {
            if(speed == 2) {
                currentTurn = PLAYER_TURN;
                speed = 1;
            }else currentTurn = MONSTER_TURN;
        }
    } else if (currentTurn == MONSTER_TURN) {
        monster_attack(&player, monsters, numMonsters, weapon_monster_win); 
        if (player.health <= 0) {
        display_game_over();
        char command = getch();
            if (command) {
                gameState = EXPLORE;
                goto end;
            }

 
        } else {
            currentTurn = PLAYER_TURN; 
        }
    }
    wrefresh(weapon_inventory_win); 
    wrefresh(weapon_monster_win); 
    }


        if (health > 0) { 
            display_mapafterreading(mapafterreading, rows, cols, color);
            display_health_bar(health, max_health);
            display_gold_score(gold_score);
            display_food_inventoy(food_inventoy_win, &food_inventoy);
            display_spell_inventory(spell_inventory_win, &spell_inventory);
            display_weapon_inventory(weapon_inventory_win, &weapon_inventory);
            decrease_health_over_time(&health, &time_without_food); 
            display_hunger(time_without_food / 6);
        } else {
            display_game_over();
            char command = getch();
            if (command) {
                goto end;
            }
        }

    }
    

end:
    clear();
    for (int i = 0; i < rows; i++) {
        free(mapafterreading[i]);
    }
    free(mapafterreading);

    save_food_inventory_to_file(&food_inventoy, filename_food);

    save_spell_inventory_to_file(&spell_inventory, filename_spell);

    save_weapon_inventory_to_file(&weapon_inventory, filename_weapone);


//game
    } else if (choice == 3) {
        if(strcmp(username , "guest") == 0){
            mvprintw(5 , 5 , "you can't select this");
            refresh() ;
            getch() ;
            goto main_menu ;
        }
        struct User players[MAX_USERS];
    int playerCount = readPlayersFromFile("users.txt", players);

    qsort(players, playerCount, sizeof(struct User), comparePlayers);
        start_color(); 
    noecho(); 
    cbreak(); 
    keypad(stdscr, TRUE); 

  
    int start = 0;

   
    while ((ch = getch()) != ' ') {
        switch (ch) {
            case KEY_UP:
                if (start > 0) start--;
                break;
            case KEY_DOWN:
                if (start < playerCount - winHeight_score + 1) start++;
                break;
        }
        displayScoreboard(win_score, players, playerCount, start, username);
    }
    wclear(win_score);
    clear();
    werase(win_score);
    refresh();                   
 goto main_menu;
    }else if (choice == 4){
        return 0 ;
//setting
        clear();
    }





    // while(1){
    //     char c = getch();
    // }
    // refresh();                  
    // endwin();             

    return 0;
}
