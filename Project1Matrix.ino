#include <FrequencyTimer2.h>

//Define score numbers
#define N0 {{1,1,1},{1,0,1},{1,0,1},{1,0,1},{1,1,1}}
#define N1 {{0,1,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1}}
#define N2 {{1,1,1},{0,0,1},{1,1,1},{1,0,0},{1,1,1}}
#define N3 {{1,1,1},{0,0,1},{0,1,1},{0,0,1},{1,1,1}}
#define N4 {{1,0,1},{1,0,1},{1,1,1},{0,0,1},{0,0,1}}
#define N5 {{1,1,1},{1,0,0},{1,1,1},{0,0,1},{1,1,1}}
#define N6 {{1,1,1},{1,0,0},{1,1,1},{1,0,1},{1,1,1}}
#define N7 {{1,1,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1}}
#define N8 {{1,1,1},{1,0,1},{1,1,1},{1,0,1},{1,1,1}}
#define N9 {{1,1,1},{1,0,1},{1,1,1},{0,0,1},{0,0,1}}

byte col = 0;
byte leds[8][8];
byte player[8][8];

// pin[xx] on led matrix connected to nn on Arduino (-1 is dummy to make array start at pos 1)
int pins[17]= {-1, 5, 4, 3, 2, 14, 15, 16, 17, 13, 12, 11, 10, 9, 8, 7, 6};
int potPin = 19;
int potVal = 0;
int xPos = 0;
int yPos = 7;
int i = 0;
int j = 0;
int platformLoc;
int platformLength = 1;
int difficulty;
int timeDelayed;
int score;
int randNum = 2;
int playerRow = 4;
int scoreNum[10][5][3] = {N0,N1,N2,N3,N4,N5,N6,N7,N8,N9};
int firstDigit;
int secondDigit;
boolean play;

// col[xx] of leds = pin yy on led matrix
int cols[8] = {pins[13], pins[3], pins[4], pins[10], pins[6], pins[11], pins[15], pins[16]};

// row[xx] of leds = pin yy on led matrix
int rows[8] = {pins[9], pins[14], pins[8], pins[12], pins[1], pins[7], pins[2], pins[5]};

int pattern = 0;

void setup() {
  play = true;
  Serial.begin(9600);
  pinMode(potPin, INPUT);
  platformLoc = 4;
  timeDelayed = 200;

  // sets the pins as output
  for (int i = 1; i <= 16; i++) {
    pinMode(pins[i], OUTPUT);
  }

  // set up cols and rows
  for (int i = 1; i <= 8; i++) {
    digitalWrite(cols[i - 1], LOW);
  }
  for (int i = 1; i <= 8; i++) {
    digitalWrite(rows[i - 1], HIGH);
  }
    // Turn off toggling of pin 11
  FrequencyTimer2::disable();
  // Set refresh rate (interrupt timeout period)
  FrequencyTimer2::setPeriod(2000);
  // Set interrupt routine to be called
  FrequencyTimer2::setOnOverflow(display);
}

void clearBackground(){
  //sets background leds to 0 to clear platforms
  for(int i = 0; i<= 7; i ++)
  {
    for (int k = 0; k <= 7; k++)
    {
    leds[i][k] = 0;
    }
  }
}
void clearPlayer(){
  //sets player array to 0 to clear the player led
  for(int i = 0; i<= 7; i ++)
  {
  for (int k = 0; k <= 7; k++)
    {
    player[i][k] = 0;
    }
  }
}

void gameOver(){
  //clear the LEDS
  clearBackground();
  clearPlayer();
  playerRow = 8;
  //Animate a game over screen
  for (int i = 0; i < 8; i++){
      leds[i][0]= 1;
      leds[i][1]= 1;
      leds[i][2]= 1;
      leds[i][3]= 1;
      leds[i][4]= 1;
      leds[i][5]= 1;
      leds[i][6]= 1;
      leds[i][7]= 1;
      delay(200);
      }
      clearBackground();
     //choose matrix to display based on score
      for (int i = 0; i<5;i++){
        for (int j = 0; j<3;j++){
          leds[i+1][j] = scoreNum[firstDigit][i][j];
          leds[i+1][j+4] = scoreNum[secondDigit][i][j];
        }
      }

      
      delay(10000);
      
}

// Interrupt routine
void display() {
  clearPlayer();
  
  checkPot();
  
  digitalWrite(cols[col], LOW);  // Turn whole previous column off
  col++;
  if (col == 8) {
    col = 0;
  }
  //check if led in matrx is == to 1 and turn on corresponding LED
  for (int row = 0; row < 8; row++) {
    if (leds[row][col] == 1 || player[row][col] == 1) {
      digitalWrite(rows[row], LOW);  // Turn on this led
    }
    else {
      digitalWrite(rows[row], HIGH); // Turn off this led
    }
  }
  digitalWrite(cols[col], HIGH); // Turn whole column on at once (for equal lighting times)
}

void checkPot(){
  //check potentiometer value
  potVal = analogRead(potPin);
  //map the value to a value 0-7 in the led matrix
  xPos=map(potVal,0,1023,0,7);
  //turn on led
  player[playerRow][xPos] = 1;
}

void loop() {
  //loops while player has not gotten squished at the top of the matrix
  while (play == true){
    
    if (playerRow == 0){
    play = false;
  }
  //check for a playform below the player and move the player up. Else move the player down if they are below the 5th row
  if (player[playerRow][xPos] == leds[playerRow+1][xPos]){
        playerRow--;
      }else if (playerRow < 4){
        playerRow++;
   }   
      
    clearBackground();
    //create a platform of random size between 3 and 5 leds in length
  for (int i = 1; i < randNum; i++){
      leds[yPos][platformLoc]= 1;
      leds[yPos][platformLoc + i]= 1;
      leds[yPos][platformLoc - i]= 1;
      }
     //move the playform up
      yPos--;
      //destroy the platforms that go off screen and set a new position at the bottom of the screen
      if (yPos < 0){
        yPos = 7;
        platformLoc = random(0, 7);
        difficulty++;
        //increment score
        score++;
        
        randNum = random(2, 4);
      }
      //speed at which platforms move up
      delay(timeDelayed);
      //for every 2 rows past, increase speed by reducing delay by 20
      if (difficulty == 2){
        //do not let time delayed become less than 80
        if (timeDelayed > 80){
          timeDelayed = timeDelayed - 20;
        }
        difficulty = 0;
      }
  }
//assign first and second digit of score by dividing by 10 and finding its remainder
firstDigit = (int)score / 10;
secondDigit = score % 10;

  //display game over screen when while loop exits
  gameOver();
}


