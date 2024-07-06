#include <Arduino.h>
#include <FastLED.h>
#include <Encoder.h>

// Button
#define BUTTON_1 6
#define BUTTON_2 7
#define BUTTONTIME 50
// Buzzer
const int buzzerPin = 10; // 连接蜂鸣器的引脚
#define NOTE_DURATION 500 // 每个音符的基本持续时间（毫秒）
#define C4 262
#define D4 294
#define E4 330
#define F4 349
#define G4 392
#define A4 440
#define B4 494
#define C5 523
#define C2 65
#define E2 82
#define G2 98
#define C6 1047
#define E6 1319
#define G6 1568
#define MY_LOW 2
#define MY_HIGH 4
#define MY_MEDIUM 3
#define CORRECT 1
#define INCORRECT 0
int loseMelody[] = {A4, F4, D4, 0};
int loseNoteDurations[] = {50, 50, 50};
int winMelody[] = {C4, E4, G4, 0};
int winNoteDurations[] = {50, 50, 50};
int lowMelody[] = {C2, E2, G2, 0};
int lowNoteDurations[] = {300, 300, 300};
int mediumMelody[] = {C4, E4, G4, 0};
int mediumNoteDurations[] = {300, 300, 300};
int highMelody[] = {C6, E6, G6, 0};
int highNoteDurations[] = {300, 300, 300};
void musicPlay(int *Melody, int *noteDurations);
void soundEffects(int sound);
void initBuzzer();
// WS2812b
#define RED 0
#define GREEN 1
#define BLUE 2
#define LED_PIN 4
#define NUM_LEDS 5
CRGB leds[NUM_LEDS];
int cols[3];
void singleLight(int id, int col, int lum, int Time, int Flash = 0);
void initLight();

void setup()
{
	randomSeed(114514);
	Serial.begin(115200);
	Serial.println("Hello World!");
	pinMode(BUTTON_1, INPUT_PULLUP);
	pinMode(BUTTON_2, INPUT_PULLUP);
	pinMode(buzzerPin, OUTPUT);
	FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
	for (int i = 0; i < NUM_LEDS; i++)
		singleLight(i, 0, i + 1, 500);
}
// main
long buttonTime1, buttonTime2, pauseTime, buttonPush1, buttonPush2;
bool buttonLast1, buttonLast2;
int modeNow = 0, corAns, playAns;
bool modeFlag;
bool modeChange();
void buttonPush();
void Init();
// mode1
int corNum, incorNum, roundNum;
bool Round, End1;
void modePlay1();
void randLight();
CRGB randCol();
int Col(CRGB col);
int ansCol();
CRGB randCol(int Error);
void end1Light();

// mode2
void loop()
{
	if (pauseTime < 1000)
		pauseTime++;
	buttonPush();
	if (modeChange())
		return;
	if (modeNow == 1)
		modePlay1();

	delay(10);
}

void modePlay1()
{
	if (pauseTime <= 20)
		return;
	playAns = ansCol();
	if (playAns != -1)
	{
		Serial.print("playAns = ");
		Serial.println(playAns);
	}
	if (playAns == 2)
	{
		Serial.print("cor && incor: ");
		Serial.print(corNum);
		Serial.print(" && ");
		Serial.println(incorNum);
		End1 = 1;
		end1Light();
	}
	else if (playAns == 3)
	{
		End1 = 0;
		Init();
	}
	if (End1)
		return;

	if (Round == false)
	{
		randLight();
		Round = 1;
	}
	else
	{
		if (playAns == -1)
			return;
		if (playAns == corAns)
		{
			corNum++, soundEffects(CORRECT);
			Serial.println("correct");
		}
		else
		{

			incorNum++, soundEffects(INCORRECT);
			Serial.println("incorrect");
		}
		Round = false, pauseTime = 0ll;
	}
}

void end1Light()
{
	int per = corNum * 1.0 / (corNum + incorNum) * 100.0 - 1;
	int res = per % 20;
	if (per >= 80)
		soundEffects(MY_HIGH);
	else if (per >= 60)
		soundEffects(MY_MEDIUM);
	else
		soundEffects(MY_LOW);
	singleLight(per / 20, res / 5, 1, 100, 1);
}

int ansCol()
{
	if (buttonPush1 && buttonTime1 > 100)
		return 2;
	if (buttonPush2 && buttonTime2 > 100)
		return 3;
	if (buttonPush1 == -1 && buttonPush2 == 0)
		return 0;
	else if (buttonPush1 == 0 && buttonPush2 == -1)
		return 1;
	return -1;
}

void randLight()
{
	for (int i = 0; i < NUM_LEDS; i++)
		leds[i] = randCol();
	FastLED.show();
	delay(200);
	int pos = random(NUM_LEDS), same = random(100) & 1;
	int col = Col(leds[pos]);
	corAns = same == 1;

	Serial.print("Answer: ");
	Serial.println(corAns ? "yes" : "no");

	for (int i = 0; i < NUM_LEDS; i++)
	{
		if (i == pos)
		{
			if (same)
				continue;
			leds[i] = randCol(col);
		}
		else
			leds[i] = CRGB(0, 0, 0);
	}
	FastLED.show();
}

int Col(CRGB col)
{
	if (col.red)
		return 0;
	else if (col.green)
		return 1;
	else
		return 2;
}

CRGB randCol()
{
	int num = random(3);
	if (num == 0)
		return CRGB(1, 0, 0);
	else if (num == 1)
		return CRGB(0, 1, 0);
	else
		return CRGB(0, 0, 1);
}

CRGB randCol(int Error)
{
	while (1)
	{
		int num = random(3);
		if (num == Error)
			continue;
		if (num == 0)
			return CRGB(1, 0, 0);
		else if (num == 1)
			return CRGB(0, 1, 0);
		else
			return CRGB(0, 0, 1);
	}
}

void buttonPush()
{
	bool buttonNow1 = digitalRead(BUTTON_1) == LOW;
	bool buttonNow2 = digitalRead(BUTTON_2) == LOW;

	if (buttonPush1)
		buttonTime1 = 0;
	if (buttonPush2)
		buttonTime2 = 0;
	buttonTime1 += buttonNow1;
	buttonTime2 += buttonNow2;

	if (buttonLast1 == 0 && buttonNow1)
		buttonPush1 = 1;
	else if (buttonLast1 && buttonNow1 == 0)
		buttonPush1 = -1;
	else
		buttonPush1 = 0;
	if (buttonLast2 == 0 && buttonNow2)
		buttonPush2 = 1;
	else if (buttonLast2 && buttonNow2 == 0)
		buttonPush2 = -1;
	else
		buttonPush2 = 0;

	buttonLast1 = buttonNow1;
	buttonLast2 = buttonNow2;
	if (buttonTime1)
	{
		Serial.print("Time1 = ");
		Serial.println(buttonTime1);
	}
	if (buttonTime2)
	{
		Serial.print("Time2 = ");
		Serial.println(buttonTime2);
	}
}

bool modeChange()
{
	if (modeFlag)
	{
		if (buttonTime1 == 0 && buttonTime2 == 0)
			modeFlag = 0;
		return true;
	}
	if (buttonTime1 >= 100 && buttonTime2 >= 100)
	{

		singleLight(modeNow, RED, 1, 500);
		modeNow = (modeNow + (modeFlag = true)) % 2;
		singleLight(modeNow, RED, 1, 500);
		Init();
	}
	return false;
}

void Init()
{
	if (modeNow == 1)
	{
		pauseTime = 0ll;
		Round = End1 = false, roundNum = 0;
		corNum = incorNum = 0;
	}
}

void singleLight(int id, int col, int lum, int Time, int Flash = 0)
{
	initLight();
	if (col == 3)
		cols[0] = 1, cols[1] = 1, cols[2] = 0;
	else
	{
		cols[col] = lum;
	}

	leds[id] = CRGB(cols[RED], cols[GREEN], cols[BLUE]);
	FastLED.show();
	delay(Time);
	leds[id] = CRGB(0, 0, 0);
	if (!Flash)
		FastLED.show();
}

void musicPlay(int *melody, int *noteDurations)
{
	int numNotes = 0, *temp = melody;
	while (*temp != 0)
	{
		++temp;
		++numNotes;
	}
	Serial.print("numNotes = ");
	Serial.println(numNotes);
	for (int thisNote = 0; thisNote < numNotes; thisNote++)
	{
		int noteDuration = noteDurations[thisNote];
		tone(buzzerPin, melody[thisNote], noteDuration);
		int pauseBetweenNotes = noteDuration * 1.3;
		delay(pauseBetweenNotes);
		noTone(buzzerPin);
	}
}
void soundEffects(int sound)
{
	if (sound == INCORRECT)
		musicPlay(loseMelody, loseNoteDurations);
	else if (sound == CORRECT)
		musicPlay(winMelody, winNoteDurations);
	else if (sound == MY_LOW)
		musicPlay(lowMelody, lowNoteDurations);
	else if (sound == MY_HIGH)
		musicPlay(highMelody, highNoteDurations);
	else if (sound == MY_MEDIUM)
		musicPlay(mediumMelody, mediumNoteDurations);
}
void initLight()
{
	cols[0] = cols[1] = cols[2] = 0;
	for (int i = 0; i < NUM_LEDS; i++)
		leds[i] = CRGB(0, 0, 0);
}

void initBuzzer()
{
	Serial.println("lose");
	soundEffects(0);
	Serial.println("win");
	soundEffects(1);
}