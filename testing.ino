#include "gesture_model_data.h"
#include "tensorflow/lite/micro/all_ops_resolver.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"

constexpr int kTensorArenaSize = 60 * 1024; // Adjust to your model's need
uint8_t tensor_arena[kTensorArenaSize];

const char* GESTURE_LABELS[] = {
  "LUCK", "POINT", "SHAKE_OPEN", "SUPER", "TILT_LEFT", "TILT_RIGHT"
};
const int NUM_CLASSES = sizeof(GESTURE_LABELS) / sizeof(GESTURE_LABELS[0]);

tflite::MicroErrorReporter micro_error_reporter;
tflite::ErrorReporter* error_reporter = &micro_error_reporter;
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter;
TfLiteTensor* input;
TfLiteTensor* output;

void setup() {
  Serial.begin(115200); // Serial for communication

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
  // TODO: Replace with actual sensor readings
  float flex1 = 0, flex2 = 0, flex3 = 0;
  float ax = 0, ay = 0, az = 0, gx = 0, gy = 0, gz = 0;
  // For demo/testing, just leave as zero or add random values if needed

  // Prepare model input (order must match training)
  input->data.f[0] = flex1;
  input->data.f[1] = flex2;
  input->data.f[2] = flex3;
  input->data.f[3] = ax;
  input->data.f[4] = ay;
  input->data.f[5] = az;
  input->data.f[6] = gx;
  input->data.f[7] = gy;
  input->data.f[8] = gz;

  // Run inference
  if (interpreter->Invoke() != kTfLiteOk) {
    Serial.println("Invoke failed");
    delay(100);
    return;
  }

  // Find prediction (max value in output)
  int predicted = 0;
  float max_score = output->data.f[0];
  for (int i = 1; i < NUM_CLASSES; i++) {
    if (output->data.f[i] > max_score) {
      max_score = output->data.f[i];
      predicted = i;
    }
  }

  // Serial output - send gesture name
  Serial.println(GESTURE_LABELS[predicted]);
  delay(200); // Adjust as needed
}
