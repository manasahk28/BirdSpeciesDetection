# TinyML Bird Species Detection Using ESP32 & INMP44 Microphone

This project presents a lightweight, real-time bird species detection system using TinyML, the ESP32-WROOM microcontroller, and an INMP44 I2S microphone. The system listens to ambient bird sounds, sends audio data to a Flask inference server, and predicts the bird species with confidence scores. The model is trained in TensorFlow and exported as a `.tflite` file optimized for microcontrollers.

---

## 🎯 Project Overview

Birds are important indicators of environmental health, yet identifying species manually through sound is time-consuming and requires expertise. This project aims to build a portable, low-cost, energy-efficient setup that classifies bird vocalizations in real time.

We classify four bird species:
- Sparrow  
- Crow  
- Parrot  
- Peacock  

The model is trained using Kaggle audio datasets, converted to TensorFlow Lite, and deployed through a Flask backend.

---

## ✨ Key Features

- Real-time audio capture via INMP44 microphone  
- Mel Spectrogram–based audio preprocessing  
- Lightweight TensorFlow Lite model  
- Flask server for inference  
- Wi-Fi communication using ESP32  
- On-device display of prediction + confidence  
- Suitable for field deployment  

---

## 📁 System Architecture

### **Hardware Components**

| Component | Description |
|----------|-------------|
| ESP32-WROOM | Microcontroller for audio capture + Wi-Fi |
| INMP44 Microphone | I2S digital microphone for sound input |
| Power Supply | 5V USB or battery |
| Breadboard & Wires | Circuit connections |

### **Software Components**

- Google Colab  
- Python  
- Librosa / NumPy / Matplotlib  
- TensorFlow Lite  
- Flask server  
- Arduino IDE  
- WiFiClient & HTTP libraries  

---

## 🔧 Hardware Connections

| INMP44 Pin | ESP32 Pin |
|------------|-----------|
| VDD | 3.3V |
| GND | GND |
| SCK | GPIO 2 |
| WS (LRCK) | GPIO 15 |
| SD | GPIO 13 |

---

## 🧠 Methodology

1. **Dataset Collection**
   - Bird audio recordings downloaded from Kaggle  
   - Cleaned, trimmed, normalized  

2. **Feature Extraction**
   - Mel Spectrograms generated using Librosa  
   - Converted into fixed-size arrays for the model  

3. **Model Training**
   - CNN trained on 4 species  
   - Accuracy & loss evaluated  
   - Model converted to `.tflite`  

4. **Server Deployment**
   - TFLite model loaded in Flask  
   - `/predict` endpoint handles inference  

5. **ESP32 Integration**
   - Records audio using INMP44  
   - Sends raw audio buffer to server  
   - Receives predicted species & confidence  

6. **Real-Time Output**
   - Displayed via serial monitor or OLED display  

---

## 🚀 How It Works

1. ESP32 captures bird sound  
2. Audio sent to Flask server via Wi-Fi  
3. Server preprocesses → runs TinyML inference  
4. Response returned: `{ species, confidence }`  
5. ESP32 displays the final output  

---

## 📊 Model Performance

| Metric | Result |
|--------|--------|
| Training Accuracy | ~98% |
| Validation Accuracy | ~97% |
| Training Loss | ~98% |

---
## 📁 **Full Project Folder Structure**
```text
TinyML-Bird-Detection/
│
├── esp32/
│   ├── src/
│   │   └── Bird_Species_Detectioin.ino
│   └── model/
│       └── bird_model.tflite
│
├── server/
│   ├── app.py
│   └── requirements.txt
│
├── Birds/
│   ├── Parrot/
│   ├── Crow/
│   ├── Peacock/
│   └── Sparrow/
│
├── models/
│   ├── IOT_model.ipynb
│   └── saved_models/
│       ├── best_model.h5
│       └── bird_model.tflite│
└── README.md
```

## 🌿 **Installation Steps**

1. **Clone the Repository**
```bash
git clone https://github.com/manasahk28/BirdSpeciesDetection.git
cd TinyML-Bird-Detection
```

2. **Install Server Dependencies**
```bash
cd server
pip install -r requirements.txt
```

3. **Run Flask Server**
```bash
python app.py
```

4. **Set Up ESP32**
- Open `esp32/src/Bird_Species_Detection.ino` in Arduino IDE  
- Install required libraries:
  - WiFi.h  
  - ArduinoJson  
  - I2S driver  
- Update WiFi SSID, password, and server URL inside `wifi_config.h`
- Select **ESP32 Dev Module** → Upload code

5. **Connect I2S Microphone**
- Connect SCK, WS, SD, VCC, and GND as per I2S configuration
- Power the ESP32 with USB or external supply

6. **Start Testing**
- ESP32 records live audio  
- Sends it to server `/predict` endpoint  
- Receives bird species + confidence score  

---

## 📥 API Endpoint

### **POST /predict**

**Input:**  
Raw audio bytes recorded from INMP44.

**Output (JSON):**
```json
{
  "species": "Parrot",
  "confidence": 0.87
}
```
---

💡 **Innovations**

- Low-power on-edge ML pipeline  
- Digital I2S microphone for low-noise, high-clarity audio capture  
- No dependency on heavy cloud services  
- Ideal for remote ecological and biodiversity monitoring  

---

📚 **Future Enhancements**

- Add more bird species  
- Fully on-device inference (remove Flask server)  
- GPS integration for location-tagged detections  
- Solar-powered outdoor deployment  
- LoRa or LoRaWAN for long-range, low-power communication  

---

📌 **Conclusion**

The TinyML bird species detection system beautifully blends embedded machine learning with IoT hardware, making real-time ecological monitoring both accessible and efficient.  
With its low power needs, portable design, and reliable performance, the system contributes meaningfully to biodiversity research and opens the door to scalable, sustainable environmental monitoring solutions.
