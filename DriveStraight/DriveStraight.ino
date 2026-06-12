#include <Wire.h>
#include <AS5600.h>



#define IN1 5                                   // direction of rotation
#define IN2 18                                  // speed of rotation
#define IN3 19                                  // speed of rotation
#define IN4 23                                  // direction of rotation
#define K_ERROR 3.5                             // coefficient of error



// Создаем объекты энкодеров, привязывая их к разным аппаратным шинам I2C ESP32
AS5600 enc1(&Wire);  // Первый энкодер (пины 21, 22)
AS5600 enc2(&Wire1); // Второй энкодер (пины 25, 26)



// Переменные для хранения предыдущих значений (нужны для расчета разницы)
int prev1 = 0;
int prev2 = 0;
unsigned long prevTime = 0;



void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  
  Serial.begin(115200); // Запускаем монитор порта
  
  // Инициализируем шины I2C на нужных пинах
  Wire.begin(21, 22);
  Wire1.begin(25, 26);
  
  // Запускаем энкодеры
  enc1.begin();
  enc2.begin();
  
  // Считываем начальные значения, чтобы при первом запуске loop
  // не было ложного огромного скачка скорости
  prev1 = enc1.readAngle();
  prev2 = enc2.readAngle();
  prevTime = micros(); // Засекаем начальное время в микросекундах
}



int speed_of_motors = 150;



void loop() {
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
  float speed1 = - (d1 * 360.0 / 4096.0) / (5.7 * dt);
  float speed2 = (d2 * 360.0 / 4096.0) / (5.7 * dt);
 
  // Сохраняем текущие значения как "предыдущие" для следующего цикла
  prev1 = curr1;
  prev2 = curr2;
  prevTime = now;
  
  // Небольшая задержка, чтобы не перегружать процессор и порт
  delay(20); 


  float speed_of_left = speed_of_motors + K_ERROR * (speed_of_motors - speed1);       // Вычисляем скорость для каждого мотора отдельно, чтобы он всегда ехал по прямой, для этого к выбранной скорости speed_of_motors прибавляем отклонение (сколько хотим - сколько измерили) умноженное на коэффициент ошибки, его можно подобрать вручную или если не хочется максимальной точности выставить 1
  float speed_of_right = speed_of_motors + K_ERROR * (speed_of_motors - speed2);      //

  if (speed_of_left > 255) {                                                          // Проверка на слишком большие значения
    speed_of_left = 255;                                                              //
                                                                                      //
  }                                                                                   //
  
  if (speed_of_right > 255) {                                                         // Проверка на слишком большие значения
    speed_of_right = 255;                                                             //
                                                                                      //
  }                                                                                   //
  
  digitalWrite(IN1, LOW);                                                             // Движение прямо
  analogWrite(IN2, speed_of_right);                                                   //
  digitalWrite(IN4, LOW);                                                             //
  analogWrite(IN3, speed_of_left);                                                    //









  


  // Выводим измеренные с помощью энкодеров и через "      " выставленные на моторы скорости в монитор порта (1 знак после запятой)
  Serial.print(speed1, 1);
  Serial.print(" ");                                // Разделитель
  Serial.print(speed2, 1);
  Serial.print("      ");                           // Разделитель
  Serial.print(speed_of_left, 1);
  Serial.print(" ");                                // Разделитель
  Serial.println(speed_of_right, 1);
  
}
