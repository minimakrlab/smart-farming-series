// ============================================================
//  MiniMakrLab — Smart Water Tank Monitor v1.0
//  PIN MAP:
//  TRIG      → Pin 6
//  ECHO      → Pin 5
//  RED LED   → Pin 2  (LOW level  — below 30%)
//  YELLOW LED→ Pin 3  (MED level  — 30% to 60%)
//  GREEN LED → Pin 1  (HIGH level — above 60%)
//  BUZZER    → Pin 4
// ============================================================

// ── Ultrasonic pins (your original) ──
int trigPin = 6;
int echoPin = 5;

// ── LED pins (your specified) ─────────
int ledRed    = 2;   // LOW  — below 30%
int ledYellow = 3;   // MED  — 30% to 60%
int ledGreen  = 1;   // HIGH — above 60%
int buzzer    = 4;

// ── Your original variables ───────────
int duration, distance;

// ── Tank configuration ────────────────
// Measure your actual tank and update:
int tankEmpty = 15;  // cm — distance when tank is EMPTY
int tankFull  =  3;  // cm — distance when tank is FULL

// ── Level thresholds ──────────────────
int highLevel = 60;  // % above this → GREEN
int lowLevel  = 30;  // % below this → RED + buzzer

// ══════════════════════════════════════
void setup() {
  Serial.begin(115200);

  // Your original pin setup
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // LED + buzzer setup
  pinMode(ledRed,    OUTPUT);
  pinMode(ledYellow, OUTPUT);
  pinMode(ledGreen,  OUTPUT);
  pinMode(buzzer,    OUTPUT);

  // All off at start
  digitalWrite(ledRed,    LOW);
  digitalWrite(ledYellow, LOW);
  digitalWrite(ledGreen,  LOW);
  digitalWrite(buzzer,    LOW);

  Serial.println("============================");
  Serial.println(" MiniMakrLab Tank Monitor   ");
  Serial.println("============================");
  Serial.println("RED    → Pin 2 (LOW  <30%)  ");
  Serial.println("YELLOW → Pin 3 (MED  30-60%)");
  Serial.println("GREEN  → Pin 1 (HIGH >60%)  ");
  Serial.println("============================");

  // Startup blink — RED → YELLOW → GREEN
  startupBlink();
}

// ══════════════════════════════════════
void loop() {

  // ── YOUR ORIGINAL ULTRASONIC CODE ──

  // Clear trigger
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Send a 10µs pulse
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read echo duration
  duration = pulseIn(echoPin, HIGH, 30000); // timeout 30ms

  // Convert to distance (cm)
  distance = duration * 0.034 / 2;

  // Print result (your original output)
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // ── ADDED: Calculate water level % ──
  int level = calculateLevel(distance);

  Serial.print("Water Level: ");
  Serial.print(level);
  Serial.println("%");

  // ── ADDED: Update LEDs ───────────────
  updateLEDs(level);

  // ── ADDED: Buzzer alert if LOW ───────
  updateBuzzer(level);

  // Print status
  printStatus(level);

  delay(500); // your original delay
}

// ══════════════════════════════════════
// CALCULATE LEVEL %
// ══════════════════════════════════════
int calculateLevel(int dist) {
  // Closer distance = more water = higher %
  int lvl = map(dist, tankEmpty, tankFull, 0, 100);
  return constrain(lvl, 0, 100);
}

// ══════════════════════════════════════
// UPDATE LEDs BASED ON LEVEL
// ══════════════════════════════════════
void updateLEDs(int level) {
  // Turn all off first
  digitalWrite(ledRed,    LOW);
  digitalWrite(ledYellow, LOW);
  digitalWrite(ledGreen,  LOW);

  if (level > highLevel) {
    // HIGH — Green ON
    digitalWrite(ledGreen, HIGH);
  }
  else if (level > lowLevel) {
    // MEDIUM — Yellow ON
    digitalWrite(ledYellow, HIGH);
  }
  else {
    // LOW — Red ON
    digitalWrite(ledRed, HIGH);
  }
}

// ══════════════════════════════════════
// BUZZER — 3 SHORT BEEPS WHEN LOW
// ══════════════════════════════════════
void updateBuzzer(int level) {
  if (level <= lowLevel) {
    for (int i = 0; i < 3; i++) {
      digitalWrite(buzzer, HIGH);
      delay(150);
      digitalWrite(buzzer, LOW);
      delay(150);
    }
  }
}

// ══════════════════════════════════════
// PRINT STATUS TO SERIAL MONITOR
// ══════════════════════════════════════
void printStatus(int level) {
  Serial.print("Status: ");
  if (level > highLevel) {
    Serial.println("GREEN  — Tank HIGH (above 60%)");
  } else if (level > lowLevel) {
    Serial.println("YELLOW — Tank MEDIUM (30-60%)");
  } else {
    Serial.println("RED    — Tank LOW! Refill needed!");
  }
  Serial.println("----------------------------");
}

// ══════════════════════════════════════
// STARTUP BLINK — RED > YELLOW > GREEN
// ══════════════════════════════════════
void startupBlink() {
  Serial.println("Starting up...");

  digitalWrite(ledRed, HIGH);
  delay(400);
  digitalWrite(ledRed, LOW);

  digitalWrite(ledYellow, HIGH);
  delay(400);
  digitalWrite(ledYellow, LOW);

  digitalWrite(ledGreen, HIGH);
  delay(400);
  digitalWrite(ledGreen, LOW);

  Serial.println("Ready!");
}

// ══════════════════════════════════════
// HOW TO CALIBRATE YOUR TANK:
// ══════════════════════════════════════
// 1. Empty your tank completely
//    → Check Serial Monitor distance reading
//    → Set tankEmpty = that value
//
// 2. Fill your tank completely
//    → Check Serial Monitor distance reading
//    → Set tankFull = that value
//
// 3. Upload code again → done!
//
// Example:
//   Empty tank reads 28cm → tankEmpty = 28
//   Full tank reads  3cm  → tankFull  = 3
// ══════════════════════════════════════
