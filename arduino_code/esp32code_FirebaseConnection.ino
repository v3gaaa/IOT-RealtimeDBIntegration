#include <Arduino.h>
//Bibliotecas para la conexion con firebase
#include <WiFi.h>
#include <FirebaseESP32.h>
//Biblioteca para el sensor de ultra sonico
#include <NewPing.h>
// Bibliotecas para el sensor PIR movimiento
#include <Adafruit_Sensor.h>
//Biblioteca sensor temperatura
#include <DHT.h>

// Incluye los archivos de ayuda de Firebase
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

// Definir el SSID y la contraseña de tu red Wi-Fi
#define WIFI_SSID "SSID"
#define WIFI_PASSWORD "PASSWORD"

// Define la clave API
#define API_KEY "API KEY"

// Define la URL de la base de datos en tiempo real
#define DATABASE_URL "URL"

// Define las credenciales de inicio de sesión del usuario
#define USER_EMAIL "EMAIL"
#define USER_PASSWORD "PASSWORD"

// Definición de pines para el sensor ultrasónico
#define TRIG_PIN 2  // Pin de trigger conectado al pin D2
#define ECHO_PIN 4  // Pin de echo conectado al pin D4
#define MAX_DISTANCE 200  // Distancia máxima de medición en centímetros

// Configura el objeto NewPing para el sensor ultrasónico
NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);

// Define el PIN para el sensor PIR movimiento
#define PIR_PIN 5

// Variables para el sensor PIR movimiento
int pirState = LOW;
int val = 0;

// Definición del PIN donde está conectado el sensor DHT de temperatura
#define DHT_PIN 18  // Pin D18 donde se conecta el sensor DHT
#define DHT_TYPE DHT11  // Cambia esto a DHT22 o DHT21 si estás utilizando un modelo de sensor DHT diferente
DHT dht(DHT_PIN, DHT_TYPE);

// Define objetos Firebase Data, Firebase Auth y Firebase Config
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;

unsigned long count = 0;


void setup() {
  // Inicializa la comunicación serial a 115200 baudios para la depuración
  Serial.begin(115200);

  // Conecta al Wi-Fi utilizando las credenciales proporcionadas
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // Imprime la versión del cliente Firebase
  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  // Asigna la clave API requerida
  config.api_key = API_KEY;

  // Asigna las credenciales de inicio de sesión del usuario
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  // Asigna la URL de la base de datos en tiempo real requerida
  config.database_url = DATABASE_URL;

  // Asigna la función de devolución de llamada para la tarea de generación de tokens de larga duración
  config.token_status_callback = tokenStatusCallback; // Ver addons/TokenHelper.h

  // Controla la reconexión de Wi-Fi, establecer en 'false' si será controlada por tu código o una biblioteca de terceros
  Firebase.reconnectNetwork(true);

  // Configura el tamaño del búfer SSL (necesario desde la versión 4.4.x con BearSSL)
  fbdo.setBSSLBufferSize(4096 /* Tamaño del búfer Rx en bytes de 512 a 16384 */, 1024 /* Tamaño del búfer Tx en bytes de 512 a 16384 */);

  // Inicializa Firebase con la configuración y autenticación
  Firebase.begin(&config, &auth);

  // Establece la cantidad de dígitos decimales para números de punto flotante en Firebase
  Firebase.setDoubleDigits(5);

  // Configura el PIN del sensor PIR de movimiento como entrada
  pinMode(PIR_PIN, INPUT);

  // Inicializa el sensor DHT temperatura 
  dht.begin();
}

void loop() {
  // Firebase.ready() debe llamarse repetidamente para manejar tareas de autenticación

  if (Firebase.ready() && (millis() - sendDataPrevMillis > 3000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();

    // Mide la distancia con el sensor ultrasónico
    unsigned int distance = sonar.ping_cm();

    // Lee los datos de temperatura del sensor DHT
    float temperature = dht.readTemperature();  // Cambia a readHumidity() si también deseas la humedad

    // Lee el valor del sensor PIR movimiento
    val = digitalRead(PIR_PIN);

    // Envía la distancia medida a Firebase Realtime Database
    if (Firebase.setInt(fbdo, "/sensores/distancia", distance)) {
      Serial.printf("Distancia enviada a Firebase: %u cm\n", distance);
    } else {
      Serial.println("Error al enviar la distancia a Firebase");
      Serial.println(fbdo.errorReason().c_str());
    }

    // Envía el valor del sensor PIR a Firebase
    if (Firebase.setInt(fbdo, "/sensores/movimiento", val)) {
      Serial.printf("Valor del sensor PIR enviado a Firebase: %d\n", val);
    } else {
      Serial.println("Error al enviar el valor del sensor PIR a Firebase");
      Serial.println(fbdo.errorReason().c_str());
    }

    // Envía los datos de temperatura a Firebase si la lectura es válida
    if (!isnan(temperature)) {
      if (Firebase.setFloat(fbdo, "/sensores/temperatura", temperature)) {
        Serial.printf("Temperatura enviada a Firebase: %.2f °C\n", temperature);
      } else {
        Serial.println("Error al enviar la temperatura a Firebase");
        Serial.println(fbdo.errorReason().c_str());
      }
    } else {
      Serial.println("Error al leer la temperatura del sensor DHT");
    }

    Serial.println();

    count++;
  }
}