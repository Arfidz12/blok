#include <raylib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

enum GameState {
    MENU,
    PLAYING,
    GAME_OVER
};

// Membuat CLass
class Alien {
    public:
        Alien(int type, Vector2 position);
        void Update(int direction);
        void Draw();
        int GetType();
        static void UnloadImages();
        Rectangle getRect();
        static Texture2D alienImages[3];
        int type;
        Vector2 position;
    private:
};

class Block {
    public:
        Block(Vector2 position);
        void Draw();
        Rectangle getRect();
    private:
        Vector2 position;
};

class Laser {
    public:
        Laser(Vector2 position, int speed);
        void Update();
        void Draw();
        Rectangle getRect();
        bool active;
    private:
        Vector2 position;
        int speed;
};

class MysteryShip {
    public:
        MysteryShip();
        ~MysteryShip();
        void Update();
        void Draw();
        void Spawn();
        Rectangle getRect();
        bool alive;
    private:
        Vector2 position;
        Texture2D image;
        int speed;
};

class Obstacle {
    public:
        Obstacle(Vector2 position);
        void Draw();
        Vector2 position;
        std::vector<Block> blocks;
        static std::vector<std::vector<int>> grid;
    private:
};

class Spaceship{
    public:
        Spaceship();
        ~Spaceship();
        void Draw();
        void MoveLeft();
        void MoveRight();
        void FireLaser();
        Rectangle getRect();
        void Reset();
        std::vector<Laser> lasers;

    private:
        Texture2D image;
        Vector2 position;
        double lastFireTime;
        Sound laserSound;
};

class Game {
    public:
        Game();
        ~Game();
        void Draw();
        void Update();
        void HandleInput();
        bool run;
        int lives;
        int score;
        int highscore;
        int GetScore() {return score;}
        int GetHighScore() {return highscore;}
        int currentLevel;
        std::vector<Alien>& GetAliens() { return aliens; }
        void Reset();
        void InitGame();
        GameState gameState;
    private:
        void DeleteInactiveLasers();
        void MoveAliens();
        void MoveDownAliens(int distance); 
        void AlienShootLaser();
        void CheckForCollisions();
        void GameOver();
        void checkForHighscore();
        void saveHighscoreToFile(int highscore);
        int loadHighscoreFromFile();
        int aliensDirection;
        std::vector<Obstacle> CreateObstacles();
        std::vector<Alien> CreateAliens();
        std::vector<Laser> alienLasers;
        std::vector<Obstacle> obstacles;
        std::vector<Alien> aliens;
        constexpr static float alienLaserShootInterval = 0.35;
        float timeLastAlienFired;
        float mysteryShipSpawnInterval;
        float timeLastSpawn;
        Spaceship spaceship;
        MysteryShip mysteryship;
        Sound explosionSound;

};

// Membuat ALien
Texture2D Alien::alienImages[3] = {};

Alien::Alien(int type, Vector2 position)
{
    this -> type = type;
    this -> position = position;

    if(alienImages[type -1].id == 0){

    switch (type) {
        case 1:
            alienImages[0] = LoadTexture("Graphics/alien_1.png");
            break;
        case 2:
            alienImages[1] = LoadTexture("Graphics/alien_2.png");
            break;
        case 3: 
            alienImages[2] = LoadTexture("Graphics/alien_3.png");
            break;
        default:
            alienImages[0] = LoadTexture("Graphics/alien_1.png");
            break;
    }
}
}

void Alien::Draw() {
    DrawTextureV(alienImages[type - 1], position, WHITE);
}

int Alien::GetType() {
    return type;
}

void Alien::UnloadImages()
{
    for(int i = 0; i < 4; i++) {
        UnloadTexture(alienImages[i]);
    }
}

Rectangle Alien::getRect()
{
    return {position.x, position.y,
    float(alienImages[type - 1].width),
    float(alienImages[type - 1].height)
    };
}

void Alien::Update(int direction) {
    position.x += direction;
}

// Membuat Blok
Block::Block(Vector2 position)
{
    this -> position = position;
}

void Block::Draw() {
    DrawRectangle(position.x, position.y, 3, 3, {0, 228, 48, 255});
}

Rectangle Block::getRect()
{
    Rectangle rect;
    rect.x = position.x;
    rect.y = position.y;
    rect.width = 3;
    rect.height = 3;
    return rect;
}

// Membuat Laser
Laser::Laser(Vector2 position, int speed)
{
    this -> position = position;
    this -> speed = speed;
    active = true;
}

void Laser::Draw() {
    if(active)
        DrawRectangle(position.x, position.y, 4, 15, {255, 255, 255, 255});
}

Rectangle Laser::getRect()
{
    Rectangle rect;
    rect.x = position.x;
    rect.y = position.y;
    rect.width = 4;
    rect.height = 15;
    return rect;
}

void Laser::Update() {
    position.y += speed;
    if(active) {
        if(position.y > GetScreenHeight() - 100 || position.y < 25) {
            active = false;
        }
    }
}

// Membuat MYsteryShip
MysteryShip::MysteryShip()
{
    image = LoadTexture("Graphics/mystery.png");
    alive = false;
}

MysteryShip::~MysteryShip() {
    UnloadTexture(image);
}

void MysteryShip::Spawn() {
    position.y = 90;
    int side = GetRandomValue(0, 1);

    if(side == 0) {
        position.x = 25;
        speed = 3;
    } else {
        position.x = GetScreenWidth() - image.width - 25;
        speed = -3;
    }
    alive = true;
}

Rectangle MysteryShip::getRect()
{
    if(alive){
        return {position.x, position.y, float(image.width), float(image.height)};
    } else {
        return {position.x, position.y, 0, 0};
    }
}

void MysteryShip::Update() {
    if(alive) {
        position.x += speed;
        if(position.x > GetScreenWidth() - image.width -25 || position.x < 25) {
            alive = false;
        }
    }
}

void MysteryShip::Draw() {
    if(alive) {
        DrawTextureV(image, position, WHITE);
    }
}

// Membuat Obstacle
std::vector<std::vector<int>> Obstacle:: grid = {
    {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1}
    };

Obstacle::Obstacle(Vector2 position)
{
    this -> position = position;

    for(unsigned int row = 0; row < grid.size(); ++row) {
        for(unsigned int column = 0; column < grid[0].size(); ++column){
            if(grid[row][column] == 1) {
                float pos_x = position.x + column *3;
                float pos_y = position.y + row * 3;
                Block block = Block({pos_x, pos_y});
                blocks.push_back(block);
            }
        }
    }
}

void Obstacle::Draw() {
    for(auto& block: blocks) {
        block.Draw();
    }
}

// Membuat Spaceship (Player)
Spaceship::Spaceship()
{
    image = LoadTexture("Graphics/spaceship.png");
    position.x = (GetScreenWidth() - image.width)/2;
    position.y = GetScreenHeight() - image.height - 100;
    lastFireTime = 0.0;
    laserSound = LoadSound("Sounds/laser.ogg");
}

Spaceship::~Spaceship() {
    UnloadTexture(image);
    UnloadSound(laserSound);
}

void Spaceship::Draw() {
    DrawTextureV(image, position, WHITE);
}

void Spaceship::MoveLeft() {
    position.x -= 7;
    if(position.x < 25) {
        position.x = 25;
    }
}

void Spaceship::MoveRight() {
    position.x += 7;
    if(position.x > GetScreenWidth() - image.width - 25) {
        position.x = GetScreenWidth() - image.width - 25;
    }
}

void Spaceship::FireLaser()
{
    if(GetTime() - lastFireTime >= 0.35) {
        lasers.push_back(Laser({position.x + image.width/2 - 2,position.y},-6));
        lastFireTime = GetTime();
        PlaySound(laserSound);
    }
}

Rectangle Spaceship::getRect()
{
    return {position.x, position.y, float(image.width), float(image.height)};
}

void Spaceship::Reset()
{
    position.x = (GetScreenWidth() - image.width)/ 2.0f;
    position.y = GetScreenHeight() - image.height - 100;
    lasers.clear();
}

//Membuat gamenya
Game::Game():score(0), highscore(0), currentLevel(1)
{
    explosionSound = LoadSound("Sounds/explosion.ogg");
    InitGame();
}

Game::~Game() {
    Alien::UnloadImages();
    UnloadSound(explosionSound);
}

void Game::Update() {
    if(run) {

        double currentTime = GetTime();
        if(currentTime - timeLastSpawn > mysteryShipSpawnInterval) {
            mysteryship.Spawn();
            timeLastSpawn = GetTime();
            mysteryShipSpawnInterval = GetRandomValue(10, 20);
        }

        for(auto& laser: spaceship.lasers) {
            laser.Update();
        }

        MoveAliens();

        AlienShootLaser();

        for(auto& laser: alienLasers) {
            laser.Update();
        }

        DeleteInactiveLasers();
        
        mysteryship.Update();

        CheckForCollisions();
    } else {
        if(IsKeyDown(KEY_ENTER)){
            Reset();
            InitGame();
        }
    }
}

void DrawMenu(Font font, Texture2D alienImage1,Texture2D alienImage2, Texture2D alienImage3) {
    const char* title = "SPACE INVADERS";
    const char* start = "PRESS ENTER TO START";
    const char* instructions = "USE ARROW KEYS TO MOVE, SPACE TO SHOOT";

    DrawTextEx(font, title, {(GetScreenWidth() - MeasureTextEx(font, title, 64, 2).x) / 2, 200}, 64, 2, GREEN);
    DrawTextEx(font, start, {(GetScreenWidth() - MeasureTextEx(font, start, 34, 2).x) / 2, 400}, 34, 2, GREEN);
    DrawTextEx(font, instructions, {(GetScreenWidth() - MeasureTextEx(font, instructions, 24, 2).x) / 2, 500}, 24, 2, GREEN);

    DrawTexture(alienImage1, (GetScreenWidth() - alienImage1.width) / 1.5, 315, WHITE);
    DrawTexture(alienImage2, (GetScreenWidth() - alienImage2.width) / 2, 315, WHITE);
    DrawTexture(alienImage3, (GetScreenWidth() - alienImage3.width) / 3, 315, WHITE);
}

void Game::Draw() {
    spaceship.Draw();

    for(auto& laser: spaceship.lasers) {
        laser.Draw();
    }

    for(auto& obstacle: obstacles) {
        obstacle.Draw();
    }

    for(auto& alien: aliens) {
        alien.Draw();
    }

    for(auto& laser: alienLasers) {
        laser.Draw();
    }

    mysteryship.Draw();
}

void Game::HandleInput() {
    if(run){
        if(IsKeyDown(KEY_LEFT)) {
            spaceship.MoveLeft();
        } else if (IsKeyDown(KEY_RIGHT)){
            spaceship.MoveRight();
        } else if (IsKeyDown(KEY_SPACE)) {
            spaceship.FireLaser();
        }
    }
}

void Game::DeleteInactiveLasers()
{
    for(auto it = spaceship.lasers.begin(); it != spaceship.lasers.end();){
        if(!it -> active) {
            it = spaceship.lasers.erase(it);
        } else {
            ++ it;
        }
    }

    for(auto it = alienLasers.begin(); it != alienLasers.end();){
        if(!it -> active) {
            it = alienLasers.erase(it);
        } else {
            ++ it;
        }
    }
}

std::vector<Obstacle> Game::CreateObstacles()
{
    int obstacleWidth = Obstacle::grid[0].size() * 3;
    float gap = (GetScreenWidth() - (4 * obstacleWidth))/5;

    for(int i = 0; i < 4; i++) {
        float offsetX = (i + 1) * gap + i * obstacleWidth;
        obstacles.push_back(Obstacle({offsetX, float(GetScreenHeight() - 200)}));
    }
    return obstacles;
}

std::vector<Alien> Game::CreateAliens()
{
    std::vector<Alien> aliens;
    for(int row = 0; row < 5; row++) {
        for(int column = 0; column < 11; column++) {

            int alienType;
            if(row == 0) {
                alienType = 3;
            } else if (row == 1 || row == 2) {
                alienType = 2;
            } else {
                alienType = 1;
            }

            float x = 75 + column * 55;
            float y = 110 + row * 55;
            aliens.push_back(Alien(alienType, {x, y}));
        }
    }
    return aliens;
}

void Game::MoveAliens() {
    for(auto& alien: aliens) {
        if(alien.position.x + alien.alienImages[alien.type - 1].width > GetScreenWidth() - 25) {
            aliensDirection = -1;
            MoveDownAliens(4);
        }
        if(alien.position.x < 25) {
            aliensDirection = 1;
            MoveDownAliens(4);
        }

        alien.Update(aliensDirection);
    }
}

void Game::MoveDownAliens(int distance)
{
    for(auto& alien: aliens) {
        alien.position.y += distance;
    }
}

void Game::AlienShootLaser()
{
    double currentTime = GetTime();
    if(currentTime - timeLastAlienFired >= alienLaserShootInterval && !aliens.empty()) {
        int randomIndex = GetRandomValue(0, aliens.size() - 1);
        Alien& alien = aliens[randomIndex];
        alienLasers.push_back(Laser({alien.position.x + alien.alienImages[alien.type -1].width/2, 
                                    alien.position.y + alien.alienImages[alien.type - 1].height}, 6));
        timeLastAlienFired = GetTime();
    }
}

void Game::CheckForCollisions()
{
    //Spaceship Lasers

    for(auto& laser: spaceship.lasers) {
        auto it = aliens.begin();
        while(it != aliens.end()){
            if(CheckCollisionRecs(it -> getRect(), laser.getRect()))
            {
                PlaySound(explosionSound);
                if(it -> type == 1) {
                    score += 100;
                } else if (it -> type == 2) {
                    score += 200;
                } else if(it -> type == 3) {
                    score += 300;
                }
                checkForHighscore();

                it = aliens.erase(it);
                laser.active = false;
            } else {
                ++it;
            }
        }

        for(auto& obstacle: obstacles){
            auto it = obstacle.blocks.begin();
            while(it != obstacle.blocks.end()){
                if(CheckCollisionRecs(it -> getRect(), laser.getRect())){
                    it = obstacle.blocks.erase(it);
                    laser.active = false;
                } else {
                    ++it;
                }
            }
        }

        if(CheckCollisionRecs(mysteryship.getRect(), laser.getRect())){
            mysteryship.alive = false;
            laser.active = false;
            score += 500;
            checkForHighscore();
            PlaySound(explosionSound);
        }
    }

    //Alien Lasers

    for(auto& laser: alienLasers) {
        if(CheckCollisionRecs(laser.getRect(), spaceship.getRect())){
            laser.active = false;
            lives --;
            if(lives == 0) {
                GameOver();
            }
        }

          for(auto& obstacle: obstacles){
            auto it = obstacle.blocks.begin();
            while(it != obstacle.blocks.end()){
                if(CheckCollisionRecs(it -> getRect(), laser.getRect())){
                    it = obstacle.blocks.erase(it);
                    laser.active = false;
                } else {
                    ++it;
                }
            }
        }
    }

    //Alien Collision with Obstacle
    
    for(auto& alien: aliens) {
        for(auto& obstacle: obstacles) {
            auto it = obstacle.blocks.begin();
            while(it != obstacle.blocks.end()) {
                if(CheckCollisionRecs(it -> getRect(), alien.getRect())) {
                    it = obstacle.blocks.erase(it);
                } else {
                    it ++;
                }
            }
        }

        if(CheckCollisionRecs(alien.getRect(), spaceship.getRect())) {
            GameOver();
        }
    }
}

void Game::GameOver()
{
    run = false;
    currentLevel = 1;
    score = 0;
}

void Game::Reset() {
    spaceship.Reset();
    aliens.clear();
    alienLasers.clear();
    obstacles.clear();
}

void Game::InitGame()
{
    obstacles = CreateObstacles();
    aliens = CreateAliens();
    aliensDirection = 1;
    timeLastAlienFired = 0.0;
    timeLastSpawn = 0.0;
    lives = 3;
    highscore = loadHighscoreFromFile();
    run = true;
    mysteryShipSpawnInterval = GetRandomValue(10, 20);
}

void Game::checkForHighscore()
{
    if(score > highscore) {
        highscore = score;
        saveHighscoreToFile(highscore);
    }
}

void Game::saveHighscoreToFile(int highscore)
{
    std::ofstream highscoreFile("highscore.txt");
    if(highscoreFile.is_open()) {
        highscoreFile << highscore;
        highscoreFile.close();
    } else {
        std::cerr << "Failed to save highscore to file" << std::endl;
    }
}

int Game::loadHighscoreFromFile() {
    int loadedHighscore = 0;
    std::ifstream highscoreFile("highscore.txt");
    if(highscoreFile.is_open()) {
        highscoreFile >> loadedHighscore;
        highscoreFile.close();
    } else {
        std::cerr << "Failed to load highscore from file." << std::endl;
    }
    return loadedHighscore;
}

std::string FormatWithLeadingZeros(int number, int width) {
    std::string numberText = std::to_string(number);
    int leadingZeros = width - numberText.length();
    return numberText = std::string(leadingZeros, '0') + numberText;
}

int main()  
{
    int offset = 50;
    int windowWidth = 750;
    int windowHeight = 700;

    InitWindow(windowWidth + offset, windowHeight + 2 * offset, "Space Invaders");
    InitAudioDevice();

    Font font = LoadFontEx("Font/monogram.ttf", 64, 0, 0);
    Texture2D spaceshipImage = LoadTexture("Graphics/spaceship.png");

    SetTargetFPS(60);

    Game game;
    Texture2D menuAlienImage1 = LoadTexture("Graphics/alien_1.png");
    Texture2D menuAlienImage2 = LoadTexture("Graphics/alien_2.png");
    Texture2D menuAlienImage3 = LoadTexture("Graphics/alien_3.png");
    game.gameState = MENU;
    int& currentLevel = game.currentLevel;

    while(WindowShouldClose() == false) {
        if (game.gameState == MENU) {
            if (IsKeyPressed(KEY_ENTER)) {
                game.gameState = PLAYING;
                game.InitGame();
            }
            BeginDrawing();
            ClearBackground(BLACK);
            DrawMenu(font,menuAlienImage1,menuAlienImage2,menuAlienImage3);
            EndDrawing();
        } else if (game.gameState == PLAYING) {
            game.HandleInput();
            game.Update();
            BeginDrawing();
            ClearBackground(BLACK);
            DrawRectangleRoundedLines({10, 10, 780, 780}, 0.18f, 20, 2, GREEN);
            DrawLineEx({25, 730}, {775, 730}, 3, GREEN);

            if (game.run) {
                DrawTextEx(font, ("LEVEL " + std::to_string(currentLevel)).c_str(), {570, 740}, 34, 2, GREEN);
            } else {
                DrawTextEx(font, "GAME OVER", {570, 740}, 34, 2, GREEN);
            }
            float x = 50.0;
            for (int i = 0; i < game.lives; i++) {
                DrawTextureV(spaceshipImage, {x, 745}, WHITE);
                x += 50;
            }

            DrawTextEx(font, "SCORE", {50, 15}, 34, 2, GREEN);
            std::string scoreText = FormatWithLeadingZeros(game.score, 6);
            DrawTextEx(font, scoreText.c_str(), {50, 40}, 34, 2, WHITE);

            DrawTextEx(font, "HIGH-SCORE", {570, 15}, 34, 2, GREEN);
            std::string highscoreText = FormatWithLeadingZeros(game.highscore, 6);
            DrawTextEx(font, highscoreText.c_str(), {640, 40}, 34, 2, WHITE);

            game.Draw();
            EndDrawing();

            if (game.GetAliens().empty()) {
                currentLevel++;
                game.Reset();
                game.InitGame();
            }
        }
    }

    UnloadTexture(menuAlienImage1);
    CloseWindow();
    CloseAudioDevice();
}