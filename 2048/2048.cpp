#include <SFML/Graphics.hpp>
#include <stdlib.h>
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <deque>

using namespace std;

int score = 0;
int scorebest = 0;
int movecount = 0;
int timeplay = 0;
int timeplaysec = 0;
int timebetween = 0;
int call = 0;
int empty = 0;
int status = 0;
// in milliseconds
int waiting = 400;
int tw = waiting;
int minutewin;
int secondwin;
int newcellx;
int newcelly;
int fadecoeff;
bool loaded = false;
bool changed = false;
bool faded = false;
bool menudisplay = true;
bool thereis2048 = false;
bool pmoves;
bool movedone;
// change if you want to test
bool tester = true;
// path to files ! CHANGE IT IF YOU USE THIS PROGRAM !
string path = "/Users/piotrsikora/Documents/GitHub/d270d988-gr31-repo/Project/2048/2048/";
// contents of scoreboard file
string read;
// pressed key
string pressed;
// for moving tiles
deque<int> popped;
// for readed file
vector<int> saved;
// used to store numbers
int grid[4][4] =
{
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
};
// used to store numbers in previous move
int gridbefore[4][4] =
{
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0},
    {0, 0, 0, 0}
};

class Score {
public:
    int score;
};

class Time {
public:
    int time;
};

// inheritance, multiple inheritance
class Player: public Score, public Time {
public:
    string player;
    void board() {
        cout << player << " Score: " << score << " Time: " << time << endl;
    }
    // polymorphism
    void info(int time) {
        cout << "Time: " << time << endl;
    }
    double info(int time, int score) {
        if (time == 0) {
            throw "Time must be over 0 seconds!";
        }
        double st = score / time;
        cout << "Points per second: " << st << endl;
        return st;
    }
};

// check how many empty tiles there are
void returnempty(){
    empty = 0;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (grid[i][j] == 0) {
                empty += 1;
            }
        }
    }
}

// check if move is possible
void possiblemoves(){
    pmoves = false;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (grid[i][j] == grid[i][j + 1] && j != 3) {
                pmoves = true;
            }
            if (grid[i][j] == grid[i + 1][j] && i != 3) {
                pmoves = true;
            }
        }
    }
}

// check for win or lose situation
void gamestatus(){
    // true = win false = lose
    returnempty();
    possiblemoves();
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (grid[i][j] == 2048) {
                thereis2048 = true;
            }
        }
    }
    if (thereis2048 == true){
        // 1 for win
        status = 1;
    }
    if (empty == 0 && pmoves == false) {
        // 2 for lose
        status = 2;
    }
}

// save grid on previous move
void savebefore(){
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            gridbefore[i][j] = grid[i][j];
        }
    }
}

// chech if change occured on the grid after a move
bool checkchange(int x, int y){
    if (grid[x][y] != gridbefore[x][y] && grid[x][y] != 0) {
        return true;
    }
    else {
        return false;
    }
}

// add new number to the grid
void newnumber(int start){
    // pick random cell/s
    changed = false;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (checkchange(i, j) == true){
                changed = true;
            }
        }
    }
    if (changed == true || movecount == 0) {
        int randomx = rand() % 4;
        int randomy = rand() % 4;
        if (start == 2) {
            grid[randomx][randomy] = 2;
        }
        while (true) {
            int add = 0;
            int prob;
            randomx = rand() % 4;
            randomy = rand() % 4;
            if (grid[randomx][randomy] == 0) {
                // generate either 2 or 4
                // probability of 4 is 1 to 8
                prob = rand() % 8;
                if (prob == 0) {
                    add = 1;
                }
                int twoorfour = 2 * (1 + add);
                // generate additional 2 at beggining
                if (start == 2) {
                    grid[randomx][randomy] = 2;
                }
                else {
                    grid[randomx][randomy] = twoorfour;
                    newcellx = randomx;
                    newcelly = randomy;
                    faded = false;
                    cout << endl << "New number on board: " << twoorfour << endl;
                }
                break;
            }
        }
    }
}

// used to print info to console
void printer(){
    // print grid
    for (int g = 0; g < 4; g++) {
        for (int h = 0; h < 4; h++) {
            if (grid[g][h] == 0) {
                cout  << ". ";
            }
            else {
                cout << grid[g][h] << " ";
            }
        }
        cout << endl;
    }
    cout << "Pressed key: " + pressed << endl;
    cout << "Time at press: " << timeplay << endl;
    cout << "Moves: " << movecount << " " << movedone << endl;
    // boolalpha converts printed bool to true/false output
    cout << "Empty: " << empty << " Possible moves: " << boolalpha << pmoves << endl;
    if (thereis2048 == true){
        cout << "You won\n";
    }
    if (empty == 0 && pmoves == false) {
        cout << "You LOST\n";
    }
}

// move functions for moving of tiles
void moveRight(){
    int last, secondlast, thirdlast, fourthlast;
    // save one row, calculate, clear, save next
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            if (grid[r][c] != 0) {
                popped.push_back(grid[r][c]);
            }
        }
        while (popped.size() > 1) {
            last = popped.back();
            popped.pop_back();
            secondlast = popped.back();
            popped.pop_back();
            if (last == secondlast) {
                last += secondlast;
                popped.push_back(last);
                score += last;
            }
            else {
                popped.push_back(secondlast);
                popped.push_back(last);
                break;
            }
        }
        if (popped.size() > 2) {
            last = popped.back();
            popped.pop_back();
            secondlast = popped.back();
            popped.pop_back();
            thirdlast = popped.back();
            popped.pop_back();
            if (secondlast == thirdlast) {
                secondlast += thirdlast;
                popped.push_back(secondlast);
                popped.push_back(last);
                score += secondlast;
            }
            else {
                popped.push_back(thirdlast);
                popped.push_back(secondlast);
                popped.push_back(last);
            }
        }
        if (popped.size() > 3) {
            last = popped.back();
            popped.pop_back();
            secondlast = popped.back();
            popped.pop_back();
            thirdlast = popped.back();
            popped.pop_back();
            fourthlast = popped.back();
            popped.pop_back();
            if (thirdlast == fourthlast) {
                thirdlast += fourthlast;
                popped.push_back(thirdlast);
                popped.push_back(secondlast);
                popped.push_back(last);
                score += thirdlast;
            }
            else {
                popped.push_back(fourthlast);
                popped.push_back(thirdlast);
                popped.push_back(secondlast);
                popped.push_back(last);
            }
        }
        // fill array
        if (popped.size() >= 1) {
            grid[r][0] = 0;
            grid[r][1] = 0;
            grid[r][2] = 0;
            grid[r][3] = popped.at(0);
            if (popped.size() >= 2) {
                grid[r][0] = 0;
                grid[r][1] = 0;
                grid[r][2] = popped.at(0);
                grid[r][3] = popped.at(1);
                if (popped.size() >= 3) {
                    grid[r][0] = 0;
                    grid[r][1] = popped.at(0);
                    grid[r][2] = popped.at(1);
                    grid[r][3] = popped.at(2);
                    if (popped.size() == 4) {
                        grid[r][0] = popped.at(0);
                        grid[r][1] = popped.at(1);
                        grid[r][2] = popped.at(2);
                        grid[r][3] = popped.at(3);
                    }
                }
            }
        }
        popped.clear();
    }
    movecount += 1;
}

void moveDown(){
    int last, secondlast, thirdlast, fourthlast;
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            if (grid[c][r] != 0) {
                popped.push_back(grid[c][r]);
            }
        }
        while (popped.size() > 1) {
            last = popped.back();
            popped.pop_back();
            secondlast = popped.back();
            popped.pop_back();
            if (last == secondlast) {
                last += secondlast;
                popped.push_back(last);
                score += last;
            }
            else {
                popped.push_back(secondlast);
                popped.push_back(last);
                break;
            }
        }
        if (popped.size() > 2) {
            last = popped.back();
            popped.pop_back();
            secondlast = popped.back();
            popped.pop_back();
            thirdlast = popped.back();
            popped.pop_back();
            if (secondlast == thirdlast) {
                secondlast += thirdlast;
                popped.push_back(secondlast);
                popped.push_back(last);
                score += secondlast;
            }
            else {
                popped.push_back(thirdlast);
                popped.push_back(secondlast);
                popped.push_back(last);
            }
        }
        if (popped.size() > 3) {
            last = popped.back();
            popped.pop_back();
            secondlast = popped.back();
            popped.pop_back();
            thirdlast = popped.back();
            popped.pop_back();
            fourthlast = popped.back();
            popped.pop_back();
            if (thirdlast == fourthlast) {
                thirdlast += fourthlast;
                popped.push_back(thirdlast);
                popped.push_back(secondlast);
                popped.push_back(last);
                score += thirdlast;
            }
            else {
                popped.push_back(fourthlast);
                popped.push_back(thirdlast);
                popped.push_back(secondlast);
                popped.push_back(last);
            }
        }
        if (popped.size() >= 1) {
            grid[0][r] = 0;
            grid[1][r] = 0;
            grid[2][r] = 0;
            grid[3][r] = popped.at(0);
            if (popped.size() >= 2) {
                grid[0][r] = 0;
                grid[1][r] = 0;
                grid[2][r] = popped.at(0);
                grid[3][r] = popped.at(1);
                if (popped.size() >= 3) {
                    grid[0][r] = 0;
                    grid[1][r] = popped.at(0);
                    grid[2][r] = popped.at(1);
                    grid[3][r] = popped.at(2);
                    if (popped.size() == 4) {
                        grid[0][r] = popped.at(0);
                        grid[1][r] = popped.at(1);
                        grid[2][r] = popped.at(2);
                        grid[3][r] = popped.at(3);
                    }
                }
            }
        }
        popped.clear();
    }
    movecount += 1;
}

void moveLeft(){
    int first, second, third, fourth;
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            if (grid[r][c] != 0) {
                popped.push_back(grid[r][c]);
            }
        }
        while (popped.size() > 1) {
            first = popped.front();
            popped.pop_front();
            second = popped.front();
            popped.pop_front();
            if (first == second) {
                first += second;
                popped.push_front(first);
                score += first;
            }
            else {
                popped.push_front(second);
                popped.push_front(first);
                break;
            }
        }
        if (popped.size() > 2) {
            first = popped.front();
            popped.pop_front();
            second = popped.front();
            popped.pop_front();
            third = popped.front();
            popped.pop_front();
            if (second == third) {
                second += third;
                popped.push_front(second);
                popped.push_front(first);
                score += second;
            }
            else {
                popped.push_front(third);
                popped.push_front(second);
                popped.push_front(first);
            }
        }
        if (popped.size() > 3) {
            first = popped.front();
            popped.pop_front();
            second = popped.front();
            popped.pop_front();
            third = popped.front();
            popped.pop_front();
            fourth = popped.front();
            popped.pop_front();
            if (third == fourth) {
                third += fourth;
                popped.push_front(third);
                popped.push_front(second);
                popped.push_front(first);
                score += third;
            }
            else {
                popped.push_front(fourth);
                popped.push_front(third);
                popped.push_front(second);
                popped.push_front(first);
            }
        }
        if (popped.size() >= 1) {
            grid[r][0] = popped.at(0);
            grid[r][1] = 0;
            grid[r][2] = 0;
            grid[r][3] = 0;
            if (popped.size() >= 2) {
                grid[r][0] = popped.at(0);
                grid[r][1] = popped.at(1);
                grid[r][2] = 0;
                grid[r][3] = 0;
                if (popped.size() >= 3) {
                    grid[r][0] = popped.at(0);
                    grid[r][1] = popped.at(1);
                    grid[r][2] = popped.at(2);
                    grid[r][3] = 0;
                    if (popped.size() == 4) {
                        grid[r][0] = popped.at(0);
                        grid[r][1] = popped.at(1);
                        grid[r][2] = popped.at(2);
                        grid[r][3] = popped.at(3);
                    }
                }
            }
        }
        popped.clear();
    }
    movecount += 1;
}

void moveUp(){
    int first, second, third, fourth;
    for (int r = 0; r < 4; r++) {
        for (int c = 0; c < 4; c++) {
            if (grid[c][r] != 0) {
                popped.push_back(grid[c][r]);
            }
        }
        while (popped.size() > 1) {
            first = popped.front();
            popped.pop_front();
            second = popped.front();
            popped.pop_front();
            if (first == second) {
                first += second;
                popped.push_front(first);
                score += first;
            }
            else {
                popped.push_front(second);
                popped.push_front(first);
                break;
            }
        }
        if (popped.size() > 2) {
            first = popped.front();
            popped.pop_front();
            second = popped.front();
            popped.pop_front();
            third = popped.front();
            popped.pop_front();
            if (second == third) {
                second += third;
                popped.push_front(second);
                popped.push_front(first);
                score += second;
            }
            else {
                popped.push_front(third);
                popped.push_front(second);
                popped.push_front(first);
            }
        }
        if (popped.size() > 3) {
            first = popped.front();
            popped.pop_front();
            second = popped.front();
            popped.pop_front();
            third = popped.front();
            popped.pop_front();
            fourth = popped.front();
            popped.pop_front();
            if (third == fourth) {
                third += fourth;
                popped.push_front(third);
                popped.push_front(second);
                popped.push_front(first);
                score += third;
            }
            else {
                popped.push_front(fourth);
                popped.push_front(third);
                popped.push_front(second);
                popped.push_front(first);
            }
        }
        if (popped.size() >= 1) {
            grid[0][r] = popped.at(0);
            grid[1][r] = 0;
            grid[2][r] = 0;
            grid[3][r] = 0;
            if (popped.size() >= 2) {
                grid[0][r] = popped.at(0);
                grid[1][r] = popped.at(1);
                grid[2][r] = 0;
                grid[3][r] = 0;
                if (popped.size() >= 3) {
                    grid[0][r] = popped.at(0);
                    grid[1][r] = popped.at(1);
                    grid[2][r] = popped.at(2);
                    grid[3][r] = 0;
                    if (popped.size() == 4) {
                        grid[0][r] = popped.at(0);
                        grid[1][r] = popped.at(1);
                        grid[2][r] = popped.at(2);
                        grid[3][r] = popped.at(3);
                    }
                }
            }
        }
        popped.clear();
    }
    movecount += 1;
}

int main(){
    Player p1;
    
    // excpetions
    try {
        double pps = p1.info(0, 100);
        cout << pps << endl;
    }
    catch (const char* msg) {
        cerr << msg << endl;
    }
    
    // file streams
    // read saved best score from scoreboard file
    ifstream readsave(path + "save.txt");
    if (readsave.is_open())
    {
        while (readsave >> read)
        {
            // convert string to int
            saved.push_back(stoi(read));
            scorebest = saved.at(0);
        }
    }
    else {
        cout << "Unable to open save file";
    }
    readsave.close();
    
    sf::Font font;
    if (!font.loadFromFile(path + "ClearSans-Bold.ttf")) {
        printf("Font load failed");
        return 1;
    }
    
    // window size and title
    sf::RenderWindow Window(sf::VideoMode(800, 1000), "2048");
    // framerate limit to 30 to prevent processor overwork
    Window.setFramerateLimit(30);
    // measure time
    sf::Clock clockElapsed;
    sf::Clock clockWait;
    sf::Clock clockFade;
    
    // defining shapes
    sf::RectangleShape shapeTile(sf::Vector2f(160, 160));
    sf::RectangleShape shapeGrid(sf::Vector2f(800, 800));
    sf::RectangleShape shapeMenu(sf::Vector2f(800, 1000));
    sf::RectangleShape shapeTextMenuStart(sf::Vector2f(200, 200));
    sf::RectangleShape shapeScore(sf::Vector2f(170, 60));
    sf::RectangleShape shapeScoreBest(sf::Vector2f(170, 60));
    sf::RectangleShape shapeRestart(sf::Vector2f(270, 90));
    sf::CircleShape dotOfPatience(30);
    
    // defining colors
    sf::Color colorText(118, 110, 102, 255);
    sf::Color colorBackground(255, 255, 255, 255);
    sf::Color colorGrid(185, 173, 162, 235);
    sf::Color colorTile(203, 193, 181, 255);
    sf::Color colorMenu(185, 173, 162, 255);
    sf::Color colorScore(165, 173, 162, 255);
    sf::Color colorRestart(140, 122, 104, 255);
    sf::Color colorWin(102, 204, 0, 210);
    sf::Color colorLose(255, 51, 52, 210);
    sf::Color colorCoolWhite(255, 255, 255, 210);
    sf::Color colorSemiTransparent(255, 255, 255, 50);
    // color of individual tiles
    sf::Color colorTile2(237, 228, 219, 255);
    sf::Color colorTile4(234, 224, 204, 255);
    sf::Color colorTile8(233, 179, 129, 255);
    sf::Color colorTile16(224, 146, 94, 255);
    sf::Color colorTile32(231, 130, 103, 255);
    sf::Color colorTile64(217, 97, 68, 255);
    sf::Color colorTile128(240, 216, 125, 255);
    sf::Color colorTile256(237, 209, 99, 255);
    sf::Color colorTile512(224, 193, 76, 255);
    sf::Color colorTile1024(225, 188, 78, 255);
    sf::Color colorTile2048(231, 197, 66, 255);
    
    // defining text
    sf::Text textTitle("2048", font, 100);
    sf::Text textScore(to_string(score), font, 50);
    sf::Text textScoreBest(to_string(scorebest), font, 50);
    sf::Text textInstruction("Play 2048 Game \nPress 'L' to load previous game", font, 30);
    sf::Text textRestart("New Game", font, 50);
    sf::Text textTile("", font, 56);
    sf::Text textMenuStart("Start", font, 50);
    sf::Text textPlayTime("", font, 30);
    sf::Text textWinTime("", font, 40);
    // for mouse testing
    //sf::Text textMousePositionx("", font, 30);
    //sf::Text textMousePositiony("", font, 30);
    //sf::Text textWaitTime("", font, 30);
    
    // set positions of texts and shapes
    textTitle.setPosition(32, 0);
    textScore.setPosition(332, 16);
    textScoreBest.setPosition(532, 16);
    textInstruction.setPosition(32, 110);
    textRestart.setPosition(532, 110);
    textMenuStart.setPosition(340, 565);
    textWinTime.setPosition(335, 650);
    shapeGrid.setPosition(0, 200);
    shapeScore.setPosition(320, 20);
    shapeScoreBest.setPosition(520, 20);
    shapeRestart.setPosition(520, 100);
    dotOfPatience.setPosition(720, 20);
    
    // assign colors for texts and shapes
    textTitle.setFillColor(colorText);
    textScore.setFillColor(sf::Color::White);
    textScoreBest.setFillColor(sf::Color::White);
    textInstruction.setFillColor(colorText);
    textRestart.setFillColor(sf::Color::White);
    textTile.setFillColor(colorText);
    shapeGrid.setFillColor(colorGrid);
    shapeScore.setFillColor(colorScore);
    shapeScoreBest.setFillColor(colorScore);
    shapeRestart.setFillColor(colorRestart);
    shapeTextMenuStart.setFillColor(colorCoolWhite);
    
    while(Window.isOpen())
    {
        // save previous state
        savebefore();
        // closing window event
        sf::Event event;
        while(Window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed)
                Window.close();
        }
        
        // time between moves
        sf::Time timeWait = clockWait.getElapsedTime();
        tw = timeWait.asMilliseconds();
        
        // clear frame
        Window.clear(colorBackground);
        
        // start drawing new frame
        
        // draw texts and shapes
        Window.draw(shapeScore);
        Window.draw(shapeScoreBest);
        Window.draw(shapeRestart);
        Window.draw(textTitle);
        Window.draw(textScore);
        Window.draw(textScoreBest);
        Window.draw(textInstruction);
        Window.draw(textRestart);
        Window.draw(shapeGrid);
        
        // refresh score
        textScore.setString(to_string(score));
        textScoreBest.setString(to_string(scorebest));
        
        sf::Time timeElapsed = clockElapsed.getElapsedTime();
        // round down elapsed time
        timeplay = floor(timeElapsed.asSeconds());
        if (timeplay / 60 > 0) {
            timeplaysec = timeplay - ((timeplay / 60) * 60);
        }
        else{
            timeplaysec = timeplay;
        }
        textPlayTime.setString("Time: " + to_string(timeplay / 60) + "m" + to_string(timeplaysec) + "s");
        textPlayTime.setFillColor(colorText);
        textPlayTime.setPosition(320, 100);
        Window.draw(textPlayTime);
        
        // dot of patience
        if (tw > waiting) {
            dotOfPatience.setFillColor(colorWin);
        }
        else {
            dotOfPatience.setFillColor(colorLose);
        }
        Window.draw(dotOfPatience);

        // draw tiles
        int x = 0, y = 0;
        for(int ypos = 232; ypos < 1000; ypos += 192){
            for(int xpos = 32; xpos < 800; xpos += 192){
                shapeTile.setPosition(xpos, ypos);
                
                sf::Time timeFade = clockFade.getElapsedTime();
                if (fadecoeff > 255) {
                    clockFade.restart();
                }
                fadecoeff = timeFade.asMilliseconds() / 3;
                //cout << fadecoeff << endl;
                switch(grid[x][y]) {
                    case 2:
                        textTile.setString(to_string(grid[x][y]));
                        shapeTile.setFillColor(colorTile2);
                        if (x == newcellx && y == newcelly && faded == false) {
                            shapeTile.setFillColor(sf::Color(237, 228, 219, fadecoeff));
                            if (fadecoeff > 245) {
                                faded = true;
                            }
                        }
                        textTile.setPosition(xpos+64, ypos + 42);
                        break;
                    case 4:
                        textTile.setString(to_string(grid[x][y]));
                        shapeTile.setFillColor(colorTile4);
                        if (x == newcellx && y == newcelly && faded == false) {
                            shapeTile.setFillColor(sf::Color(234, 224, 204, fadecoeff));
                            if (fadecoeff > 245) {
                                faded = true;
                            }
                        }
                        textTile.setPosition(xpos+64, ypos + 42);
                        break;
                    case 8:
                        textTile.setString(to_string(grid[x][y]));
                        shapeTile.setFillColor(colorTile8);
                        textTile.setPosition(xpos + 64, ypos + 42);
                        break;
                    case 16:
                        textTile.setString(to_string(grid[x][y]));
                        shapeTile.setFillColor(colorTile16);
                        textTile.setPosition(xpos + 48, ypos + 42);
                        break;
                    case 32:
                        textTile.setString(to_string(grid[x][y]));
                        shapeTile.setFillColor(colorTile32);
                        textTile.setPosition(xpos + 48, ypos + 42);
                        break;
                    case 64:
                        textTile.setString(to_string(grid[x][y]));
                        shapeTile.setFillColor(colorTile64);
                        textTile.setPosition(xpos + 48, ypos + 42);
                        break;
                    case 128:
                        textTile.setString(to_string(grid[x][y]));
                        shapeTile.setFillColor(colorTile128);
                        textTile.setPosition(xpos + 32, ypos + 42);
                        break;
                    case 256:
                        textTile.setString(to_string(grid[x][y]));
                        shapeTile.setFillColor(colorTile256);
                        textTile.setPosition(xpos + 32, ypos + 42);
                        break;
                    case 512:
                        textTile.setString(to_string(grid[x][y]));
                        shapeTile.setFillColor(colorTile512);
                        textTile.setPosition(xpos + 32, ypos + 42);
                        break;
                    case 1024:
                        textTile.setString(to_string(grid[x][y]));
                        shapeTile.setFillColor(colorTile1024);
                        textTile.setPosition(xpos + 10, ypos + 42);
                        break;
                    case 2048:
                        textTile.setString(to_string(grid[x][y]));
                        shapeTile.setFillColor(colorTile2048);
                        textTile.setPosition(xpos + 10, ypos + 42);
                        break;
                    default:
                        shapeTile.setFillColor(colorTile);
                        break;
                }
                Window.draw(shapeTile);
                Window.draw(textTile);
                y++;
            }
            y = 0;
            x++;
        }
        
        // keyboard
        // testing win
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
            if (tester == true) {
                grid[0][0] = 2048;
            }
        }
        // testing lose
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::F) && tester == true){
            if (tester == true) {
                grid[0][0] = grid[0][2] = grid[1][1] = grid[1][3] = grid[2][0] = grid[2][2] = grid[3][1] = grid[3][3] = 2;
                grid[0][1] = grid[0][3] = grid[1][0] = grid[1][2] = grid[2][1] = grid[2][3] = grid[3][0] = 4;
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)){
            if (tw > waiting) {
                movedone = false;
                pressed = "right";
                clockWait.restart();
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)){
            if (tw > waiting) {
                movedone = false;
                pressed = "left";
                clockWait.restart();
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)){
            if (tw > waiting) {
                movedone = false;
                pressed = "up";
                clockWait.restart();
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)){
            if (tw > waiting) {
                movedone = false;
                pressed = "down";
                clockWait.restart();
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::L)){
            for (int i = 0; i < 4; i++) {
                for (int j = 0; j < 4; j++) {
                    grid[i][j] = saved.at(1 + (4 * i) + j);
                }
            }
            loaded = true;
        }
        if (event.type == sf::Event::KeyReleased && movedone == false) {
            movedone = true;
            if (pressed == "right") {
                moveRight();
            }
            if (pressed == "left") {
                moveLeft();
            }
            if (pressed == "up") {
                moveUp();
            }
            if (pressed == "down") {
                moveDown();
            }
            if (changed == true) {
                clockFade.restart();
            }
            // add new number
            if (loaded == false) {
                newnumber(1);
            }
            // check game status
            gamestatus();
            // print info in console
            printer();
            loaded = false;
        }
        
        // mouse
        sf::Vector2i mousePosition = sf::Mouse::getPosition(Window);
        int mousePositionx = mousePosition.x;
        int mousePositiony = mousePosition.y;
        
        // testing mouse
        //textMousePositionx.setString(to_string(mousePositionx));
        //textMousePositionx.setPosition(700, 900);
        //textMousePositiony.setString(to_string(mousePositiony));
        //textMousePositiony.setPosition(700, 940);
        //textMousePositionx.setFillColor(sf::Color::Black);
        //textMousePositiony.setFillColor(sf::Color::Black);
        //Window.draw(textMousePositionx);
        //Window.draw(textMousePositiony);
        
        // display menu
        if (menudisplay == true){
            status = 0;
            movecount = 0;
            thereis2048 = false;
            shapeMenu.setFillColor(colorSemiTransparent);
            textMenuStart.setFillColor(colorText);
            shapeMenu.setPosition(0, 0);
            shapeTextMenuStart.setPosition(300, 500);
            textWinTime.setFillColor(colorText);
            textWinTime.setString(to_string(minutewin) + "m:" + to_string(secondwin) + "s");
            Window.draw(shapeMenu);
            Window.draw(shapeTextMenuStart);
            Window.draw(textMenuStart);
            if (secondwin != 0) {
                Window.draw(textWinTime);
            }
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left)){
                if (mousePositionx > 300 && mousePositionx < 500 && mousePositiony > 500 && mousePositiony < 700 ){
                    shapeTextMenuStart.setFillColor(colorCoolWhite);
                    textMenuStart.setPosition(310, 565);
                    textMenuStart.setString("Let's go");
                    for (int i = 0; i < 4; i++) {
                        for (int j = 0; j < 4; j++) {
                            grid[i][j] = 0;
                        }
                    }
                    // two numbers for new game
                    newnumber(2);
                    menudisplay = false;
                    minutewin = 0;
                    secondwin = 0;
                    score = 0;
                    clockElapsed.restart();
                }
            }
        }
        
        // menu start button mouse
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)){
            if (mousePositionx > 530 && mousePositionx < 780 && mousePositiony > 115 && mousePositiony < 160 ) {
                menudisplay = true;
            }
        }
        
        // win situation
        if (status == 1) {
            textMenuStart.setString("You won");
            shapeTextMenuStart.setFillColor(colorWin);
            textMenuStart.setPosition(303, 565);
            minutewin = (timeplay / 60);
            secondwin = timeplaysec;
            menudisplay = true;
        }
        
        // lose situation
        if (status == 2) {
            textMenuStart.setString("You lost");
            shapeTextMenuStart.setFillColor(colorLose);
            textMenuStart.setPosition(307, 565);
            menudisplay = true;
        }
        
        // write save
        ofstream writesave(path + "save.txt", ios::trunc);
        writesave << scorebest << endl;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                writesave << grid[i][j] << endl;
            }
        }
        writesave.close();
        
        // update new best score to scoreboard file
        if (score > scorebest) {
            scorebest = score;
            ofstream writescoreboard(path + "save.txt", ios::trunc);
            writescoreboard << to_string(score);
            writescoreboard.close();
        }
        
        // display new frame
        Window.display();
    }
    return 0;
}
