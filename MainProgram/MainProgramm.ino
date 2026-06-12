#include <Wire.h>
#include <AS5600.h>



#define IN1 5                                   // direction of rotation
#define IN2 18                                  // speed of rotation
#define IN3 19                                  // speed of rotation
#define IN4 23                                  // direction of rotation
#define K_ERROR 1.9                             // coefficient of error

#define ECHO_TX 12
#define TRIG_RX 14
#define TIME_TO_DISTANCE 58.0                   // coefficient allows to convert to the distance from time_of_impulse
#define TIMEOUT 30000                           // maximal time of waiting the signal of sensor

#define SPEAKER 2                               // pin of the speaker



const byte COUNT_NOTES = 25;                    // мелодия
                                                //
// Частоты нот для "Happy Birthday to You"      //
int tones[COUNT_NOTES] = {                      //
  // Hap-py birth-day to you                    //
  264, 264, 297, 264, 352, 330,                 //
  // Hap-py birth-day to you                    //
  264, 264, 297, 264, 396, 352,                 //
  // Hap-py birth-day dear [Name]               //
  264, 264, 528, 440, 352, 330, 297,            //
  // Hap-py birth-day to you                    //
  466, 466, 440, 352, 396, 352                  //
                                                //
};                                              //
                                                //
// Длительности нот (миллисекунды)              //
int durations[COUNT_NOTES] = {                  //
  // Hap-py birth-day to you                    //
  350, 350, 700, 700, 700, 1400,                //
  // Hap-py birth-day to you                    //
  350, 350, 700, 700, 700, 1400,                //
  // Hap-py birth-day dear [Name]               //
  350, 350, 700, 700, 700, 700, 1400,           //
  // Hap-py birth-day to you                    //
  350, 350, 700, 700, 700, 1400                 //
                                                //
};                                              //
                                                //
// Паузы между нотами (миллисекунды)            //
int pauses[COUNT_NOTES] = {                     //
  50, 50, 50, 50, 50, 100,                      //
  50, 50, 50, 50, 50, 100,                      //
  50, 50, 50, 50, 50, 50, 100,                  //
  50, 50, 50, 50, 50, 100                       //
                                                //
};                                              //



// Создаем объекты энкодеров, привязывая их к разным аппаратным шинам I2C ESP32
AS5600 enc1(&Wire);  // Первый энкодер (пины 21, 22)
AS5600 enc2(&Wire1); // Второй энкодер (пины 25, 26)



// Переменные для хранения предыдущих значений (нужны для расчета разницы)
int prev1 = 0;
int prev2 = 0;
unsigned long prevTime = 0;

// Переменные для динамика
unsigned long lastNoteTime = 0;
int currentNote = 0;
bool melodyPlaying = false;



void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(TRIG_RX, OUTPUT);
  pinMode(ECHO_TX, INPUT);

  pinMode(SPEAKER, OUTPUT);
  
  // Инициализируем шины I2C на нужных пинах
  Wire.begin(21, 22);
  Wire1.begin(25, 26);
  
  // Запускаем энкодеры
  enc1.begin();
  enc2.begin();

  playMelody();                                                                       // Запускаем мелодию
  
}



int speed_of_motors = 130;
int distance_to_object = 0;
int critical_distance = 19;                                                           // если датчик видит расстояние меньше этого (в см), то начинает объезжать

float speed_of_left = speed_of_motors;
float speed_of_right = speed_of_motors;
float speed1 = 0;                                                                     // скорость первого мотора которая будет измеряться энкодером
float speed2 = 0;                                                                     // скорость второго мотора которая будет измеряться энкодером



void loop() {
  updateMelody();                                                                     // Обновляем мелодию (это необходимо, чтобы программа не останавливалась и тупо музыку играла, а чтобы играла параллельно)
  
//------------------------------------------------------------------------------//
//             Поиск препятствия датчиком расстояния и его объезд               //
//------------------------------------------------------------------------------//
  delay(60);     // необходимая штука просто для работы датчика расстояния
  digitalWrite(TRIG_RX, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_RX, LOW);
  distance_to_object = pulseIn(ECHO_TX, HIGH, TIMEOUT) / TIME_TO_DISTANCE;
  
  if (distance_to_object < critical_distance) {                                        // Обнаружили препятствие поворачиваем
    digitalWrite(IN1, HIGH);                                                           //
    analogWrite(IN2, 70);                                                              //
    digitalWrite(IN4, LOW);                                                            //
    analogWrite(IN3, 150);                                                             //

    speed_of_left = 100;                                                               // Блок измерений на случай если будет необхходимо ехать вперед, значения для вычисления скоростей (энкодеры)
    speed_of_right = 100;                                                              //
    prev1 = enc1.readAngle();                                                          //
    prev2 = enc2.readAngle();                                                          //
    prevTime = micros();                                                               //
    
  } else {                                                                             // Препятствия нет, движемся прямо, но просто прямо ехать не получится, надо считывать показания энкодеров и подстраивать скорости на каждый мотор, поэтому сначала вычисляем необходимые скорости затем движемся
  //------------------------------------------------------------------------------//
  //                  Измеряем скорость при помощи энкодеров                      //
  //------------------------------------------------------------------------------//
    // Считаем прошедшее время
    unsigned long now = micros();
    float dt = (now - prevTime) / 1000000.0; // Переводим микросекунды в секунды
    
    // Защита: если прошло слишком мало времени, пропускаем цикл, 
    // чтобы не делить на ноль и не получать шум
    if (dt < 0.01) return; 
  
    // Считываем текущие "сырые" значения угла (от 0 до 4095)
    int curr1 = enc1.readAngle();
    int curr2 = enc2.readAngle();
  
    // Находим разницу (насколько повернулся вал)
    int d1 = curr1 - prev1;
    int d2 = curr2 - prev2;
  
    // Исправление перехода через ноль (граница 360° -> 0°)
    // Если разница больше половины круга (2048), значит мы перешли границу
    if (d1 > 2048) d1 -= 4096;
    if (d1 < -2048) d1 += 4096;
    
    if (d2 > 2048) d2 -= 4096;
    if (d2 < -2048) d2 += 4096;
  
    // Расчет скорости в единицах измерения как в переменной speed_of_motorsspeed_of_motors (ну то есть как в обычных ардуиновских моторчиках)
    // Формула: (разница_в_шагах * градусы_на_шаг) / прошедшее_время
    speed1 = - (d1 * 360.0 / 4096.0) / (5.7 * dt);
    speed2 = (d2 * 360.0 / 4096.0) / (5.7 * dt);
   
    // Сохраняем текущие значения как "предыдущие" для следующего цикла
    prev1 = curr1;
    prev2 = curr2;
    prevTime = now;
    
    // Небольшая задержка, чтобы не перегружать процессор и порт
    delay(20); 
  
  //------------------------------------------------------------------------------//
  //                           Конец измерения скорости                           //
  //------------------------------------------------------------------------------//
  
  //------------------------------------------------------------------------------//
  //     Вычисляем скорость для каждого мотора и стараемся двигаться по прямой    //
  //------------------------------------------------------------------------------//

    digitalWrite(IN1, LOW);                                                             // Движемся прямо с вычисленными скоростями
    analogWrite(IN2, speed_of_right);                                                   //
    digitalWrite(IN4, LOW);                                                             //
    analogWrite(IN3, speed_of_left);                                                    //
  
    speed_of_left = speed_of_motors + K_ERROR * (speed_of_motors - speed1);             // Вычисляем скорость для каждого мотора отдельно, чтобы он всегда ехал по прямой, для этого к выбранной скорости speed_of_motors прибавляем отклонение (сколько хотим - сколько измерили) умноженное на коэффициент ошибки, его можно подобрать вручную или если не хочется максимальной точности выставить 1
    speed_of_right = speed_of_motors + K_ERROR * (speed_of_motors - speed2);            //
  
    if (speed_of_left > 255) {                                                          // Проверка на слишком большие значения
      speed_of_left = 255;                                                              //
                                                                                        //
    }                                                                                   //
    
    if (speed_of_right > 255) {                                                         // Проверка на слишком большие значения
      speed_of_right = 255;                                                             //
                                                                                        //
    }                                                                                   //

  }
  //------------------------------------------------------------------------------//
  //                          Конец движения по прямой                            //
  //------------------------------------------------------------------------------//

//------------------------------------------------------------------------------//
//                 Конец поиска препятствия датчиком расстояния                 //
//------------------------------------------------------------------------------//
  
}



//------------------------------------------------------------------------------//
//     Далее просто функции которые вызываются для воспроизведения музыки:      //
//------------------------------------------------------------------------------//
// Функция запуска мелодии
void playMelody() {
  currentNote = 0;
  melodyPlaying = true;
  lastNoteTime = millis();
  tone(SPEAKER, tones[0], durations[0]);
  
}



// Функция обновления мелодии
void updateMelody() {
  if (!melodyPlaying) return;
  
  if (millis() - lastNoteTime >= durations[currentNote] + pauses[currentNote]) {
    noTone(SPEAKER);
    currentNote++;
    
    if (currentNote >= COUNT_NOTES) {
      melodyPlaying = false;
      return;
    }
    
    tone(SPEAKER, tones[currentNote], durations[currentNote]);
    lastNoteTime = millis();
  }
}
