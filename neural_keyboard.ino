#include "gesture_model_data.h"   // Your TFLite model as C array
#include <BleKeyboard.h>
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include <"FlowLite.h">
#include <"tensortensorflow/lite/micro/micro_error_reporter.h">
#include <"tensorflow/lite/micro/micro_interpreter.h">
#include <"tensorflow/lite/schema/schema_generated.h">
#include <"tensorflow/lite/version.h">

// BLE keyboard object
BleKeyboard bleKeyboard;

// Adjust to your model size
constexpr int kTensorArenaSize = 60 * 1024;
uint8_t tensor_arena[kTensorArenaSize];

const char* GESTURE_LABELS[] = {"LUCK", "POINT", "SHAKE_OPEN", "SUPER", "TILT_LEFT", "TILT_RIGHT"};
const int NUM_CLASSES = sizeof(GESTURE_LABELS) / sizeof(GESTURE_LABELS[0]);

tflite::MicroErrorReporter micro_error_reporter;
tflite::ErrorReporter* error_reporter = &micro_error_reporter;
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter;
TfLiteTensor* input;
TfLiteTensor* output;

void setup() {
  Serial.begin(115200);
  bleKeyboard.begin();

  model = tflite::GetModel(g_model);
  if (model->version() != TFLITE_SCHEMA_VERSION) {
    Serial.println("Model version mismatch");
    while (1);
  }

  static tflite::AllOpsResolver resolver;
  interpreter = new tflite::MicroInterpreter(model, resolver, tensor_arena, kTensorArenaSize, error_reporter);

  TfLiteStatus alloc_status = interpreter->AllocateTensors();
  if (alloc_status != kTfLiteOk) {
    Serial.println("Tensor allocation failed");
    while (1);
  }

  input = interpreter->input(0);
  output = interpreter->output(0);

  Serial.println("Setup complete");
}

void loop() {
  // --- Replace with your actual sensor acquisition ---
  float flex1 = 0, flex2 = 0, flex3 = 0;
  float ax = 0, ay = 0, az = 0, gx = 0, gy = 0, gz = 0;

  // Example:
  // flex1 = analogRead(FLEX1_PIN);

  // --- Prepare model input ---
  input->data.f[0] = flex1;
  input->data.f[1] = flex2;
  input->data.f[2] = flex3;
  input->data.f[3] = ax;
  input->data.f[4] = ay;
  input->data.f[5] = az;
  input->data.f[6] = gx;
  input->data.f[7] = gy;
  input->data.f[8] = gz;

  if (interpreter->Invoke() != kTfLiteOk) {
    Serial.println("Invoke failed");
    delay(100);
    return;
  }

  // Find highest output
  int predicted = 0;
  float max_score = output->data.f[0];
  for (int i = 1; i < NUM_CLASSES; i++) {
    if (output->data.f[i] > max_score) {
      max_score = output->data.f[i];
      predicted = i;
    }
  }

  // BLE Keyboard mapping
  if (bleKeyboard.isConnected()) {
    switch (predicted) {
      case 0: bleKeyboard.write('L'); break;             // LUCK -> 'L'
      case 1: bleKeyboard.write('A'); break;             // POINT -> 'A'
      case 2: bleKeyboard.write(KEY_TAB);                // SHAKE_OPEN -> TAB
              delay(50);
              bleKeyboard.write(' ');                    // SHAKE_OPEN -> Space
              break;
      case 3: bleKeyboard.write('3'); break;             // SUPER -> '3'
      case 4: bleKeyboard.write(KEY_RETURN); break;      // TILT_LEFT -> Enter
      case 5: bleKeyboard.write(KEY_BACKSPACE); break;   // TILT_RIGHT -> Backspace
    }
  }

  delay(200); // Adjust as needed

  Serial.print("Gesture: ");
  Serial.println(GESTURE_LABELS[predicted]);
}
