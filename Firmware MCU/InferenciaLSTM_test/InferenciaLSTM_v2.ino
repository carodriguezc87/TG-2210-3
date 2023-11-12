#include <TensorFlowLite.h>

//#include "tensorflow/lite/experimental/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/experimental/micro/kernels/all_ops_resolver.h"
#include "tensorflow/lite/experimental/micro/micro_error_reporter.h"
#include "tensorflow/lite/experimental/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"
#include "data.h"

/*
//#include <TensorFlowLite_ESP32.h>
#include <ESP_TF.h>
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/kernels/micro_ops.h"
//#include "tensorflow/lite/micro/all_ops_resolver.h"
//#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "data.h"
*/
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

 // This pulls in all the operation implementations we need.
 static tflite::ops::micro::AllOpsResolver resolver;  
  /*static tflite::MicroOpResolver micro_mutable_op_resolver;
  micro_mutable_op_resolver.AddBuiltin(
    tflite::BuiltinOperator_FULLY_CONNECTED,
    tflite::ops::micro::Register_FULLY_CONNECTED(),
    1, 3);
  micro_mutable_op_resolver.AddBuiltin(
    tflite::BuiltinOperator_UNIDIRECTIONAL_SEQUENCE_LSTM(),
    tflite::ops::micro::Register_UNIDIRECTIONAL_SEQUENCE_LSTM());
    
  micro_mutable_op_resolver.AddBuiltin( 
    tflite::BuiltinOperator_STRIDED_SLICE(),
    tflite::ops::micro::Register_STRIDED_SLICE());
  */   
 // Build an interpreter to run the model with.
  static tflite::MicroInterpreter static_interpreter(
      model, resolver, tensor_arena, kTensorArenaSize, error_reporter);
 
  interpreter = &static_interpreter;
                                             
  // Asignar memoria  para los tensores del modelo.
  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  if (interpreter->AllocateTensors() != kTfLiteOk) 
  {
    Serial.println("Error al asignar espacio");
  }

  // Obtain pointers to the model's input and output tensors..
  input = interpreter->input(0);
  output = interpreter->output(0);

  //Shape for the LSTM model
  input->dims->data[0] = 1;
  input->dims->data[1] = 18;
  input->dims->data[2] = 9;

  //Copy test data to input tensors
  memcpy(input->data.f, test2, 162 * sizeof(float)); 

  
  TfLiteStatus invoke_status = interpreter->Invoke();
  if (invoke_status != kTfLiteOk) 
  {
    Serial.println("Invoke failed");
  }
/*
  if (interpreter->Invoke() != kTfLiteOk) {
    // Maneja el error de inferencia
    Serial.println("No se logró la inferencia");
  }
*/
  float out = output->data.f[0];
  //output = interpreter->output(0);
  Serial.print("Predicción del modelo: ");
  Serial.println(out);
}

void loop() {
  // put your main code here, to run repeatedly:

}
