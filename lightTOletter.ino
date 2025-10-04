// Библиотека для работы с датчиком цвета
#include <Wire.h>
#include "Adafruit_TCS34725.h"

// Объект для работы с датчиком
Adafruit_TCS34725 colorSensor(
  TCS34725_INTEGRATIONTIME_154MS,
  TCS34725_GAIN_4X
);

// Подсветка датчика
constexpr uint8_t PIN_BACKLIGHT = 7;
int pin = 3;

// Буферы
String currentSymbolCode = "";     // Набор 0/1 для текущей буквы
String currentMorseLetter = "";    // · и – для текущей буквы
String currentWordMorse = "";      // Морзе для всего слова
String currentSentence = "";       // Расшифрованное предложение

// Таблица соответствия Морзе → Кириллица
struct MorseMap {
  const char* morse;
  const char* letter;
};

MorseMap morseTable[] = {
  {"·–", "А"}, {"–···", "Б"}, {"·––", "В"}, {"––·", "Г"}, {"–··", "Д"},
  {"·", "Е"}, {"···–", "Ж"}, {"––··", "З"}, {"··", "И"}, {"·–––", "Й"},
  {"–·–", "К"}, {"·–··", "Л"}, {"––", "М"}, {"–·", "Н"}, {"–––", "О"},
  {"·––·", "П"}, {"·–·", "Р"}, {"···", "С"}, {"–", "Т"}, {"··–", "У"},
  {"··–·", "Ф"}, {"····", "Х"}, {"–·–·", "Ц"}, {"–––·", "Ч"}, {"––––", "Ш"},
  {"––––·", "Щ"}, {"––––··", "Ъ"}, {"–·––", "Ы"}, {"–··–", "Ь"},
  {"··–··", "Э"}, {"··––", "Ю"}, {"·–·–", "Я"}
};

const int morseTableSize = sizeof(morseTable) / sizeof(MorseMap);

String morseToLetter(String morse) {
  for (int i = 0; i < morseTableSize; i++) {
    if (morse == morseTable[i].morse) {
      return String(morseTable[i].letter);
    }
  }
  return "?"; // если неизвестный код
}

void setup() {
  Serial.begin(9600);
  pinMode(PIN_BACKLIGHT, OUTPUT);
  digitalWrite(PIN_BACKLIGHT, HIGH);
  pinMode(pin, OUTPUT);

  if (colorSensor.begin()) {
    Serial.println("Датчик TCS34725 найден");
  } else {
    Serial.println("Датчик TCS34725 не найден...");
    while (1);
  }
}

void loop() {
  float r, g, b;
  colorSensor.getRGB(&r, &g, &b);

  float sum = r + g + b;
  float rn = (sum == 0) ? 0 : r / sum;
  float gn = (sum == 0) ? 0 : g / sum;
  float bn = (sum == 0) ? 0 : b / sum;

  String colorName;

  if ((r > 100 && g < 100 && b < 100) || 
      (r < 100 && g > 100 && b > 100) || 
      (r < 100 && b < 100 && g > 100)) {

    if (rn > gn && rn > bn) {
      colorName = "Красный";
    } else if (gn > rn && gn > bn) {
      colorName = "Зелёный";
    } else if (bn > rn && bn > gn) {
      colorName = "Синий";
    }

  } else {
    colorName = "Игнор";
  }

  // Анализ полученного цвета каждые 1000 мс
  if (colorName == "Красный") {
    currentSymbolCode += "0";
    currentMorseLetter += "·";
  } 
  else if (colorName == "Зелёный") {
    currentSymbolCode += "1";
    currentMorseLetter += "–";
  } 
  else if (colorName == "Игнор") {
    // Конец буквы
    if (currentMorseLetter.length() > 0) {
      String letter = morseToLetter(currentMorseLetter);
      currentSentence += letter;
      currentWordMorse += currentMorseLetter + " ";
      currentMorseLetter = "";
      currentSymbolCode = "";
    }
  } 
  else if (colorName == "Синий") {
    // Конец слова
    if (currentMorseLetter.length() > 0) {
      String letter = morseToLetter(currentMorseLetter);
      currentSentence += letter;
      currentWordMorse += currentMorseLetter + " ";
      currentMorseLetter = "";
      currentSymbolCode = "";
    }

    currentSentence += " ";     // пробел между словами
    currentWordMorse += "/ ";   // разделитель Морзе

    // Выводим накопленное
    Serial.print("Предложение: ");
    Serial.print(currentSentence);
    Serial.print(" | Код Морзе: ");
    Serial.println(currentWordMorse);

    // Готовим новые буферы
    currentSentence = "";
    currentWordMorse = "";
  }

  Serial.print("Цвет: ");
  Serial.println(colorName);
  delay(1000);
}