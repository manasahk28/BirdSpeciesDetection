#include <WiFi.h>
#include <HTTPClient.h>
#include <driver/i2s.h>

// ====== WiFi credentials ======
const char* ssid = "NanuNanduNimdu";
const char* password = "Itspersonal";

// ====== Flask Server URL ======
const char* serverURL = "http://10.99.105.191:5000/predict";

// ====== I2S Microphone Pins ======
#define I2S_WS 15   // LRCLK
#define I2S_SD 13   // DOUT from INMP441
#define I2S_SCK 2   // BCLK
#define I2S_PORT I2S_NUM_0

// ====== Bird Class Names ======
String birdNames[] = {"parrot", "peacock", "sparrow", "crow"};

// ====== Recording configuration ======
const int bufferSize = 16000; // 1s at 16kHz, mono, 16-bit = 16000 samples
int16_t samples[bufferSize];
int16_t downsampledBuffer[bufferSize];
const int downsampleFactor = 16; // Adjust for smaller payloads (8, 16, etc.)

void setup() {
  Serial.begin(115200);
  delay(1000);

  // ====== WiFi Connection ======
  Serial.print("🔗 Connecting to WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }

  Serial.println("\n✅ WiFi connected!");
  Serial.print("📡 ESP32 IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("🔎 Server URL: ");
  Serial.println(serverURL);

  // ====== I2S Configuration ======
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = 16000,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 512,
    .use_apll = false
  };

  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_SD
  };

  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_PORT, &pin_config);
  i2s_zero_dma_buffer(I2S_PORT);
  i2s_start(I2S_PORT);

  Serial.println("--------------------------------------");
  Serial.println("Type 'start' to record bird sound 🎙️");
  Serial.println("--------------------------------------");
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();
    if (input == "start") {
      recordAndPredict();
      Serial.println("--------------------------------------");
      Serial.println("Type 'start' to try again.");
      Serial.println("--------------------------------------");
    }
  }
}

void recordAndPredict() {
  Serial.println("🎤 Please make a sound near the microphone...");
  delay(500);
  Serial.println("🎙️ Recording for ~1 second...");

  i2s_zero_dma_buffer(I2S_PORT);

  // ====== Record Audio ======
  size_t bytesRead = 0;
  size_t maxBytes = bufferSize * sizeof(int16_t);
  esp_err_t res = i2s_read(I2S_PORT, (void*)samples, maxBytes, &bytesRead, portMAX_DELAY);

  if (res != ESP_OK) {
    Serial.println("❌ I2S read failed with error: " + String((int)res));
    return;
  }

  size_t actualSamples = bytesRead / sizeof(int16_t);
  Serial.println("Recorded samples: " + String(actualSamples));

  // ====== Check Energy ======
  double energy = 0.0;
  for (size_t i = 0; i < actualSamples; i++) {
    energy += abs((long)samples[i]);
  }
  energy /= max((size_t)1, actualSamples);
  Serial.println("Energy: " + String(energy));

  if (energy < 30.0) {
    Serial.println("🔇 No sound detected. Please try again.");
    return;
  } else {
    Serial.println("🎧 Sound detected! Preparing to send to server...");
  }

  // ====== Downsample safely ======
  int j = 0;
  for (size_t i = 0; i < actualSamples; i += downsampleFactor) {
    downsampledBuffer[j++] = samples[i];
  }

  int downsampledSamples = j;
  int payloadSize = downsampledSamples * sizeof(int16_t);
  Serial.printf("Payload size (bytes): %d (mod2=%d)\n", payloadSize, payloadSize % 2);

  // ====== Send to Flask Server ======
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.setTimeout(15000);
    http.begin(serverURL);
    http.addHeader("Content-Type", "application/octet-stream");

    int httpResponseCode = http.POST((uint8_t*)downsampledBuffer, payloadSize);

    if (httpResponseCode > 0) {
      Serial.println("HTTP code: " + String(httpResponseCode));
      String response = http.getString();
      Serial.println("🌐 Server response: " + response);
    } else {
      Serial.println("⚠️ HTTP Error: " + String(httpResponseCode));
      if (httpResponseCode == -1) {
        Serial.println(" -> Connection failed. Check Flask host, IP, or Wi-Fi.");
      }
    }

    http.end();
  } else {
    Serial.println("❌ WiFi not connected.");
  }
}
