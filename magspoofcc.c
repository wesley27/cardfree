/*
 * MagSpoof - "wireless" magnetic stripe/credit card emulator
*
 * by Samy Kamkar
 * 
 * This version has been modified by W. Wesley Weidenhamer II for CardFree, a BitCamp Hackathon project.
 * It is geared slightly more towards producing standardized card data, such as ISO cards.
 *
 * http://samy.pl/magspoof/
 *
 * - Allows you to store all of your credit cards and magstripes in one device
 * - Works on traditional magstripe readers wirelessly (no NFC/RFID required)
 * - Can disable Chip-and-PIN (code not included)
 * - Correctly predicts Amex credit card numbers + expirations from previous card number (code not included)
 * - Supports all three magnetic stripe tracks, and even supports Track 1+2 simultaneously
 * - Easy to build using Arduino or ATtiny
 *
 */

#include <avr/sleep.h>
#include <avr/interrupt.h>

#define PIN_A 0
#define PIN_B 1
#define ENABLE_PIN 3 // also green LED
#define SWAP_PIN 4 // unused
#define BUTTON_PIN 2
#define CLOCK_US 650

#define PADDING 75 // number of 0's before and after tracks

#define TRACKS 3

// consts get stored in flash as we don't adjust them
const char* tracks[] = {
"%?\0", // Track 1
";419204063420026?3\0", // Track 2
";71?\0" // Track 3
};

char revTrack1[80];
char revTrack2[41];
char revTrack3[6];
char* revTracks[] = { revTrack1, revTrack2, revTrack3 };


const int sublen[] = {
  48, 48, 48 };
const int bitlen[] = {
  7, 5, 5 };

unsigned int curTrack = 0;
int dir;

void setup()
{
  pinMode(PIN_A, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  pinMode(ENABLE_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // blink to show we started up
  blink(ENABLE_PIN, 200, 3);
  storeRevTrack(1);
  storeRevTrack(2);
  storeRevTrack(3);
}

void blink(int pin, int msdelay, int times)
{
  for (int i = 0; i < times; i++)
  {
    digitalWrite(pin, HIGH);
    delay(msdelay);
    digitalWrite(pin, LOW);
    delay(msdelay);
  }
}

// send a single bit out
void playBit(int sendBit)
{
  dir ^= 1;
  digitalWrite(PIN_A, dir);
  digitalWrite(PIN_B, !dir);
  delayMicroseconds(CLOCK_US);

  if (sendBit)
  {
    dir ^= 1;
    digitalWrite(PIN_A, dir);
    digitalWrite(PIN_B, !dir);
  }
  delayMicroseconds(CLOCK_US);

}

//plays tracks in different ways for maximum compatibility among magstripe readers
void playTracks()
{
  curTrack++;
  reverseTrack(1);
  playTrack(1);
  for(int i = 0; i < 2; i++) {
    //reverseTrack(1);
    //playTrack(1);
    //reverseTrack(2);
    playTrack(2);
  }
  //playTrack(2);
  //playTrack(3);
  /*if((curTrack % 4)==1)
  {
    //forward then reverse swipe
    playTrack(1);
    reverseTrack(2);
  } else if ((curTrack % 4)==2){
    //forward consecutive swipes
    playTrack(1);
    playTrack(2);
  } else if ((curTrack % 4)==3) {
    //reverse then forward swipe
    reverseTrack(1);
    playTrack(2);
  } else {
    //reverse consecutive swipes
    reverseTrack(1);
    reverseTrack(2);
  }*/
}

// plays out a full track, calculating CRCs and LRC
void playTrack(int track)
{
  int tmp, crc, lrc = 0;
  track--; // index 0
  dir = 0;

  // enable H-bridge and LED
  digitalWrite(ENABLE_PIN, HIGH);

  // First put out a bunch of leading zeros.
  for (int i = 0; i < PADDING; i++)
    playBit(0);

  for (int i = 0; tracks[track][i] != '\0'; i++)
  {
    crc = 1;
    tmp = tracks[track][i] - sublen[track];

    for (int j = 0; j < bitlen[track]-1; j++)
    {
      crc ^= tmp & 1;
      lrc ^= (tmp & 1) << j;
      playBit(tmp & 1);
      tmp >>= 1;
    }
    playBit(crc);
  }

  // finish calculating and send last "byte" (LRC)
  tmp = lrc;
  crc = 1;
  for (int j = 0; j < bitlen[track]-1; j++)
  {
    crc ^= tmp & 1;
    playBit(tmp & 1);
    tmp >>= 1;
  }
  playBit(crc);

  // finish with 0's
  for (int i = 0; i < PADDING; i++)
    playBit(0);

  digitalWrite(PIN_A, LOW);
  digitalWrite(PIN_B, LOW);
  digitalWrite(ENABLE_PIN, LOW);
}

// when reversing
void reverseTrack(int track)
{
  int i = 0;
  track--; // index 0
  dir = 0;

  digitalWrite(ENABLE_PIN, HIGH);

  // First put out a bunch of leading zeros.
  for (int i = 0; i < PADDING; i++)
    playBit(0);
  
  while (revTracks[track][i++] != '\0');
  i--;
  while (i--)
    for (int j = bitlen[track]-1; j >= 0; j--)
      playBit((revTracks[track][i] >> j) & 1);
    
  // finish with 0's
  for (int i = 0; i < PADDING; i++)
    playBit(0);

  digitalWrite(PIN_A, LOW);
  digitalWrite(PIN_B, LOW);
  digitalWrite(ENABLE_PIN, LOW);
}

// stores track for reverse usage later
void storeRevTrack(int track)
{
  int i, tmp, crc, lrc = 0;
  track--; // index 0
  dir = 0;

  for (i = 0; tracks[track][i] != '\0'; i++)
  {
    crc = 1;
    tmp = tracks[track][i] - sublen[track];

    for (int j = 0; j < bitlen[track]-1; j++)
    {
      crc ^= tmp & 1;
      lrc ^= (tmp & 1) << j;
      tmp & 1 ?
        (revTracks[track][i] |= 1 << j) :
        (revTracks[track][i] &= ~(1 << j));
      tmp >>= 1;
    }
    crc ?
      (revTracks[track][i] |= 1 << 4) :
      (revTracks[track][i] &= ~(1 << 4));
  }

  // finish calculating and send last "byte" (LRC)
  tmp = lrc;
  crc = 1;
  for (int j = 0; j < bitlen[track]-1; j++)
  {
    crc ^= tmp & 1;
    tmp & 1 ?
      (revTracks[track][i] |= 1 << j) :
      (revTracks[track][i] &= ~(1 << j));
    tmp >>= 1;
  }
  crc ?
    (revTracks[track][i] |= 1 << 4) :
    (revTracks[track][i] &= ~(1 << 4));

  i++;
  revTracks[track][i] = '\0';
}

void sleep()
{
  GIMSK |= _BV(PCIE);                     // Enable Pin Change Interrupts
  PCMSK |= _BV(PCINT2);                   // Use PB3 as interrupt pin
  ADCSRA &= ~_BV(ADEN);                   // ADC off
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);    // replaces above statement

  MCUCR &= ~_BV(ISC01);
  MCUCR &= ~_BV(ISC00);       // Interrupt on rising edge
  sleep_enable();                         // Sets the Sleep Enable bit in the MCUCR Register (SE BIT)
  sei();                                  // Enable interrupts
  sleep_cpu();                            // sleep

  cli();                                  // Disable interrupts
  PCMSK &= ~_BV(PCINT2);                  // Turn off PB3 as interrupt pin
  sleep_disable();                        // Clear SE bit
  ADCSRA |= _BV(ADEN);                    // ADC on

  sei();                                  // Enable interrupts
}

ISR(PCINT0_vect) {
}

void loop()
{
  sleep();

  noInterrupts();
  while (digitalRead(BUTTON_PIN) == LOW);

  delay(100);
  while (digitalRead(BUTTON_PIN) == LOW);
  playTracks();
  delay(400);

  interrupts();
}
