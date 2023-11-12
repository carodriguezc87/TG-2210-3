#include <TensorFlowLite.h>

//#include "tensorflow/lite/experimental/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/experimental/micro/kernels/all_ops_resolver.h"
#include "tensorflow/lite/experimental/micro/micro_error_reporter.h"
#include "tensorflow/lite/experimental/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"
#include "data.h"

namespace 
{         
  tflite::ErrorReporter* error_reporter = nullptr;
  const tflite::Model* model = nullptr;
  tflite::MicroInterpreter* interpreter = nullptr;
  TfLiteTensor* input = nullptr;
  TfLiteTensor* output = nullptr;
  constexpr int kTensorArenaSize = 60 * 1024; // Espacio en DRAM = 61440 bytes
  uint8_t tensor_arena[kTensorArenaSize];
}  

const char* ver = "Version 0.6";
const char* sketch = "Inferencia_LSTM";

void setup() 
{
  Serial.begin(115200);
  while(Serial == false);                       
  Serial.println(F(ver));                       
  Serial.println(F(sketch));                    
  delay(1000);
  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;  
  
  model = tflite::GetModel(LSTM_model);  
  if (!model) 
  {
    Serial.println("Error al cargar el modelo TFLite");
  }

 // Llama a todas las operaciones necesarias para el modelo LSTM
 static tflite::ops::micro::AllOpsResolver resolver;  
 // Construye un interprete para correr el modelo.
  static tflite::MicroInterpreter static_interpreter(
      model, resolver, tensor_arena, kTensorArenaSize, error_reporter);
  interpreter = &static_interpreter;
                                             
  // Asigna memoria  para los tensores del modelo.
  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  if (interpreter->AllocateTensors() != kTfLiteOk) 
  {
    Serial.println("Error al asignar espacio");
  }

  // Obtiene los punteros para los tensores de entrada y salida del modelo
  input = interpreter->input(0);
  output = interpreter->output(0);

  //Da forma al tensor de entrada del modelo LSTM
  input->dims->data[0] = 1;
  input->dims->data[1] = 18;
  input->dims->data[2] = 9;

  //Copia los datos del arreglo "test2" al tensor de entrada
  memcpy(input->data.f, test2, 162 * sizeof(float)); 

  //Invoca la inferencia con el modelo
  TfLiteStatus invoke_status = interpreter->Invoke();
  if (invoke_status != kTfLiteOk) 
  {
    Serial.println("Invoke failed");
  }
  float out = output->data.f[0];
  Serial.print("Predicción del modelo: ");
  Serial.println(out);
}

void loop() {
}
