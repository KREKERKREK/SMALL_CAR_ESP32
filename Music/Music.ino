#define SPEAKER 2


const byte COUNT_NOTES = 25;

// Частоты нот для "Happy Birthday to You"
int tones[COUNT_NOTES] = {
  // Hap-py birth-day to you
  264, 264, 297, 264, 352, 330,
  // Hap-py birth-day to you
  264, 264, 297, 264, 396, 352,
  // Hap-py birth-day dear [Name]
  264, 264, 528, 440, 352, 330, 297,
  // Hap-py birth-day to you
  466, 466, 440, 352, 396, 352
  
};

// Длительности нот (миллисекунды)
int durations[COUNT_NOTES] = {
  // Hap-py birth-day to you
  350, 350, 700, 700, 700, 1400,
  // Hap-py birth-day to you
  350, 350, 700, 700, 700, 1400,
  // Hap-py birth-day dear [Name]
  350, 350, 700, 700, 700, 700, 1400,
  // Hap-py birth-day to you
  350, 350, 700, 700, 700, 1400
  
};

// Паузы между нотами (миллисекунды)
int pauses[COUNT_NOTES] = {
  50, 50, 50, 50, 50, 100,
  50, 50, 50, 50, 50, 100,
  50, 50, 50, 50, 50, 50, 100,
  50, 50, 50, 50, 50, 100
  
};

void setup() {
  pinMode(dynPin, OUTPUT);
  
}

void loop() {
  for (int i = 0; i < COUNT_NOTES; i++) {
    tone(dynPin, tones[i], durations[i]);
    delay(durations[i] + pauses[i]);
    noTone(dynPin);
    
  }
  
  // Пауза перед повторением
  delay(2000);
    
  }
  
}
