// ESP32-C3 Super Mini - Automatic Plant Watering System

// ─── Configuration ───────────────────────────────────────
const int SOIL_PIN     = 2;      // Analog input
const int RELAY_PIN    = 7;      // Relay control

const int DRY_VALUE    = 4000;   // Raw ADC when soil is DRY
const int WET_VALUE    = 2000;   // Raw ADC when soil is WET

// Watering settings
const int DRY_THRESHOLD   = 60;  // Water ON below this % moisture
const int WET_THRESHOLD   = 80;  // Water OFF above this % moisture
const unsigned long PUMP_TIMEOUT = 30000; // Max pump ON time (30 sec safety)
const unsigned long READ_INTERVAL = 2000; // Read sensor every 2 seconds

// ─── Relay Logic (adjust if your relay is Active LOW) ────
#define RELAY_ON   HIGH  // Change to LOW if relay triggers inverted
#define RELAY_OFF  LOW

// ─── Globals ─────────────────────────────────────────────
bool pumpRunning = false;
unsigned long pumpStartTime = 0;
unsigned long lastReadTime  = 0;

// ─────────────────────────────────────────────────────────

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, RELAY_OFF); // Pump OFF on startup

  analogReadResolution(12); // ESP32-C3: 12-bit ADC (0–4095)
  analogSetAttenuation(ADC_11db); // Full range 0–3.3V

  Serial.println("=== Plant Watering System Started ===");
  Serial.print("Dry threshold : "); Serial.print(DRY_THRESHOLD); Serial.println("%");
  Serial.print("Wet threshold : "); Serial.print(WET_THRESHOLD); Serial.println("%");
}

// ─────────────────────────────────────────────────────────

int readMoisturePercent() {
  // Average 10 readings to reduce noise
  long sum = 0;
  for (int i = 0; i < 10; i++) {
    sum += analogRead(SOIL_PIN);
    delay(10);
  }
  int raw = sum / 10;

  // Map raw to percentage (higher raw = drier soil)
  int percent = map(raw, DRY_VALUE, WET_VALUE, 0, 100);
  percent = constrain(percent, 0, 100);

  Serial.print("Raw ADC: "); Serial.print(raw);
  Serial.print("  |  Moisture: "); Serial.print(percent); Serial.println("%");

  return percent;
}

void pumpON() {
  if (!pumpRunning) {
    digitalWrite(RELAY_PIN, RELAY_ON);
    pumpRunning   = true;
    pumpStartTime = millis();
    Serial.println(">>> PUMP ON — Watering started");
  }
}

void pumpOFF() {
  if (pumpRunning) {
    digitalWrite(RELAY_PIN, RELAY_OFF);
    pumpRunning = false;
    Serial.println(">>> PUMP OFF — Watering stopped");
  }
}

// ─────────────────────────────────────────────────────────

void loop() {
  unsigned long now = millis();

  // Safety timeout — force pump off if running too long
  if (pumpRunning && (now - pumpStartTime >= PUMP_TIMEOUT)) {
    Serial.println("!!! SAFETY TIMEOUT — Pump forced OFF");
    pumpOFF();
  }

  // Read sensor at interval
  if (now - lastReadTime >= READ_INTERVAL) {
    lastReadTime = now;

    int moisture = readMoisturePercent();

    if (!pumpRunning && moisture < DRY_THRESHOLD) {
      pumpON();
    } else if (pumpRunning && moisture >= WET_THRESHOLD) {
      pumpOFF();
    }
  }
}
