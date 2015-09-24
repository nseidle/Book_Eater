 /*
 Make a hilarious noise each time you return a book to the library.
 By: Nathan Seidle
 SparkFun Electronics
 Date: May 6th, 2015
 License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).
 
 The Book Eater is a noise maker for the Boulder Public Library return system. When you return a book you 
 place it in the 'mouth' of a return booth. This system makes a cookie monster noise (a few random tracks)
 each time you insert a book. It relies upon a Sharp IR sensor for detecting distance change.
 
 Buzzing happens when powered from USB. Power from a 9V AC/DC supply cuts down on the buzzing dramatically.
 
 13 - SCK/Status
 12 - MISO
 11 - MOSI
 10 - 
 
 9 - SD-CS
 8 - MP3-RST
 7 - MP3-DCS
 6 - MP3-CS
 5 - 
 4 - 
 3 - 
 2 - MP3-DREQ
 1/0 - 
 
 A0 - Presence Sensor
 A1 - Amp enable
 A2 - 
 A3 - 
 A4 - 
 A5 - 
 */

//Include various libraries
#include <SPI.h>           // SPI library
#include <SdFat.h>         // SDFat Library
#include <SdFatUtil.h>     // SDFat Util Library
#include <SFEMP3Shield.h>  // Mp3 Shield Library

SdFat sd; // Create object to handle SD functions

SFEMP3Shield MP3player; // Create Mp3 library object

#define MIN_PRESENCE_DELTA 18 //The distance sensor must cross this threshold to active the noise

//It gets really jumpy when the sensor is pointed at the air
//Presence_Minimum is the value we have to be above in order for us to pay attention
//Less than the min, we throw out the reading
#define PRESENCE_MINIMUM  54 


//Hardware connections
const byte presenceSensor = A0;
const byte ampEnable = A1; //It's open...

//Global variables
int previousTrack1 = 4; //Used to prevent the playing of sounds twice in a row
int previousTrack2 = 5;

#define SMOOTHING_SIZE 32 //Average the zero readings across some number of readings. 32 is reasonable.
unsigned int zeroReadings[SMOOTHING_SIZE]; //This tracks the zero reading upon power up
byte currentReadingSpot = 0;

boolean playerStopped = false; //These are booleans used to control the main loop

void setup()
{
  Serial.begin(57600);
  Serial.println("Bank Eater"); 

  pinMode(A0, INPUT); //Just for a second so we can see the random generator
  randomSeed(analogRead(0)); //For picking random audio tracks

  pinMode(presenceSensor, INPUT);

  pinMode(ampEnable, OUTPUT);
  digitalWrite(ampEnable, LOW); //Turn off amp for now
  
  //Get a baseline reading
  for(currentReadingSpot = 0 ; currentReadingSpot < SMOOTHING_SIZE ; currentReadingSpot++)
    zeroReadings[currentReadingSpot] = averageAnalogRead(presenceSensor);

  initSD(); //Initialize the SD card
  initMP3Player(); // Initialize the MP3 Shield
}

void loop()
{
  if(checkBook() == true)
  {
    digitalWrite(ampEnable, HIGH); //Turn on the amplifier!

    playRandomTrack(); //Play sound
    
    //Wait for track to finish playing
    while(MP3player.isPlaying())
      delay(100);

    digitalWrite(ampEnable, LOW); //Disable the ampifier
  }
  
  //Check for test mode
  if(Serial.available())
  {
    if(Serial.read() == 't') testPresenceSensor(); 
  }
  
  delay(50);
}

//Checks the distance sensor to see if a book is present
//Returns true if it sees ones
boolean checkBook()
{
  int bookReading = averageAnalogRead(presenceSensor);
  //Serial.print("Reading: ");
  //Serial.println(bookReading);
  
  if(bookReading < PRESENCE_MINIMUM)
  {
    Serial.print("Too low. Reading: ");
    Serial.println(bookReading); 
    return(false); //Ignore very small values (sensor pointed at air)
  }

  //Add this reading to the smoothing array    
  zeroReadings[currentReadingSpot++] = bookReading; //Add this reading to the average
  currentReadingSpot %= SMOOTHING_SIZE;

  if( abs(bookReading - takeSmoothAverage()) > MIN_PRESENCE_DELTA )
  {
    if(MP3player.isPlaying()) MP3player.stopTrack(); //Stop any previous track

    Serial.print("Book! Delta:");
    Serial.println(abs(bookReading - takeSmoothAverage()));
    return true;
  }
  else
  {
    Serial.print("No book. Delta: ");
    Serial.println(abs(bookReading - takeSmoothAverage()));
    return false;
  }
}

//Take the average of the smoothing array
int takeSmoothAverage(void)
{
  long average = 0;
  
  for(int x = 0 ; x < SMOOTHING_SIZE ; x++)
    average += zeroReadings[x];
  average /= SMOOTHING_SIZE;
  
  return(average);
}

//Plays a random track
//Then turns off audio to reduce hissing
void playRandomTrack()
{
  //Used for sound effects
  char track_name[13];
  byte trackNumber = previousTrack1;

  //Track 1 (mmmm cookie monster) needs to play more often
  while(trackNumber == previousTrack1 || trackNumber == previousTrack2) //Don't play the same track as the last donation
  {
    //Pick a random number but more than the available tracks
    //This is so we can give extra weight to track 1
    int myNumber = random(1, 8 + 5); //(inclusive min, exclusive max)
    
    if(myNumber > 7) trackNumber = 1; //Give a big weight to cookie monster track
    else trackNumber = myNumber;
  }

  sprintf(track_name, "%d.mp3", trackNumber); //Splice the track number into file name

  Serial.print("Playing: ");
  Serial.println(track_name);

  if(MP3player.isPlaying()) MP3player.stopTrack(); //Stop any previous track

  //Not sure how long these functions take
  MP3player.begin();
  MP3player.playMP3(track_name);

  //Update the previous variables
  previousTrack1 = trackNumber;
  
  playerStopped = false; //Boolean for main loop to turn off MP3 IC
}

// initSD() initializes the SD card and checks for an error.
void initSD()
{
  //Initialize the SdCard.
//  if(!sd.begin(SD_SEL, SPI_HALF_SPEED)) 
  if(!sd.begin(9, SPI_HALF_SPEED)) 
    sd.initErrorHalt();
}

// initMP3Player() sets up all of the initialization for the
// MP3 Player Shield. It runs the begin() function, checks
// for errors, applies a patch if found, and sets the volume/
// stero mode.
void initMP3Player()
{
  MP3player.begin(); // init the mp3 player shield

  //Volume level 5/6/2015 - Original setting is 10, 10
  MP3player.setVolume(10, 10); // MP3 Player volume 0=max, 255=lowest (off)

  MP3player.setMonoMode(1); // Mono setting: 0=off, 1 = on, 3=max
}

//Takes an average of readings on a given pin
//Returns the average
int averageAnalogRead(byte pinToRead)
{
  byte numberOfReadings = 8;
  unsigned int runningValue = 0; 

  for(int x = 0 ; x < numberOfReadings ; x++)
    runningValue += analogRead(pinToRead);
  runningValue /= numberOfReadings;

  return(runningValue);  
}

//Infinite loop showing the output of the Sharp sensor
void testPresenceSensor()
{
  while(1)
  {
    Serial.print("Sensor: ");
    Serial.println(averageAnalogRead(presenceSensor));
    
    //Check for exit
    if(Serial.available())
    {
      Serial.read();
      return;
    }

    delay(100);
  }
}
