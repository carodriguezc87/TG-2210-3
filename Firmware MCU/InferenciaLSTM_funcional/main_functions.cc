#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/micro/micro_log.h"

#include "tensorflow/lite/micro/tflite_bridge/micro_error_reporter.h"

#include "main_functions.h"
#include "model_data.h"
#include "constants.h"
#include "output_handler.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <esp_heap_caps.h>
#include <esp_timer.h>
#include <esp_log.h>

// Globals, used for compatibility with Arduino-style sketches.
namespace {
  const tflite::Model* model = nullptr;
  tflite::MicroInterpreter* interpreter = nullptr;
  TfLiteTensor* input = nullptr;
  TfLiteTensor* output = nullptr;

  #ifdef CONFIG_IDF_TARGET_ESP32S3
    constexpr int scratchBufSize = 39 * 1024;
  #else
    constexpr int scratchBufSize = 0;
  #endif
  // An area of memory to use for input, output, and intermediate arrays.
  constexpr int kTensorArenaSize = 60 * 1024 + scratchBufSize;
  uint8_t tensor_arena[kTensorArenaSize];
}

struct MinMaxScaler 
{
  float min;
  float max;
};

void inverseScaleData(float *data, int num_samples, float min, float max)
{
  *data = 0.5 * (*data + 1.0) * (max - min) + min;
}

int once=1;

void setup() {
	model = tflite::GetModel(LSTM_model);
	if (model->version() != TFLITE_SCHEMA_VERSION) 
  {
	  MicroPrintf(
				"Model provided is schema version %d not equal to supported "
						"version %d.", model->version(), TFLITE_SCHEMA_VERSION);
		return;
	}
	if (once){
	  // Pull in only the operation implementations we need.
		static tflite::MicroMutableOpResolver<4> op_resolver;
		if (op_resolver.AddStridedSlice() != kTfLiteOk) {
				return;
			}
		if (op_resolver.AddReshape() != kTfLiteOk) {
			return;
		}
		if (op_resolver.AddFullyConnected() != kTfLiteOk) {
			return;
		}
		if (op_resolver.AddUnidirectionalSequenceLSTM() != kTfLiteOk) {
			return;
		}
		// Build an interpreter to run the model with.
		static tflite::MicroInterpreter static_interpreter(
				model, op_resolver,tensor_arena, kTensorArenaSize);
		interpreter = &static_interpreter;
		once=0;
	}

	// Allocate memory from the tensor_arena for the model's tensors.
	TfLiteStatus allocate_status = interpreter->AllocateTensors();
	if (allocate_status != kTfLiteOk) 
  {
		MicroPrintf("AllocateTensors() failed");
		return;
	}

	// Obtain pointers to the model's input and output tensors.
	input = interpreter->input(0);
	output = interpreter->output(0);
}

bool riego(float pendiente, float promedio, float nivel_min){
  if (pendiente < 0 && (promedio < nivel_min))
    return 1;
  else if (pendiente > 0 && ((promedio < nivel_min) || (promedio > nivel_min)))
    return 0;
  else
    return 0;
}

long long total_time = 0;
long long start_time = 0;
int ind=0, nivel=0;
float dataIn[18][9];
float yA=0, yB=0, pendiente=0, promedio=0;;

void loop() {
	float *pmin;
	float *pmax;
	int k=0;
	int contador = 0;
	printf("\n");
	printf("Datos de entrada\n");
	for(int j=0; j<18;j++)
  {
		printf("[%i] ",(j+1));
		for(k=0;k<9;k++)
    {
			printf("%.2f, ",test[ind][j][k]);
		}
		printf("\n");
	}
	printf("\n");
	printf("Datos escalados\n");

	for(int j=0; j<18;j++)
  {
		printf("[%i] ",(j+1));
		for(k=0, pmin=&min_scalers[0],pmax=&max_scalers[0]; k<9; k++,pmin++,pmax++){
			dataIn[j][k] = 2.0 * (test[ind][j][k]- *pmin) / (*pmax - *pmin)-1.0;
			printf("%.2f, ",dataIn[j][k]);
		}
		printf("\n");
	}
	printf("\n");
	for (int i = 0; i < 18; i++) {
    for (int j = 0; j <9; j++) 
    {
      input->data.f[contador++] = dataIn[i][j];
	  }
	}

	long long start_time = esp_timer_get_time();
	// Run inference, and report any error
	TfLiteStatus invoke_status = interpreter->Invoke();
	if (invoke_status != kTfLiteOk) 
  {
		MicroPrintf("Invoke failed on x");
		return;
	}
	long long total_time = (esp_timer_get_time() - start_time);
	float out = output->data.f[0];
	inverseScaleData(&out, 1, min_scalers[3],max_scalers[3]);
  
	if (ind>1)printf("Inferencia para 10 + %i horas\n",(ind));
	else printf("Inferencia para 10 + %i hora\n",(ind));
	
  printf("Estimado de humedad del suelo: %f\n", out);
	printf("Inference time = %lldms\n", total_time / 1000);
	total_time = 0;
	if (ind==0) yA=out;
	ind++;
	promedio+=out;

	if (ind>4)
	{
		nivel=66;
		yB=out;
		ind=0;
		printf("\n");
		promedio/=5;
		pendiente=(yB - yA) / (5 - 1);
		printf("Pendiente: %.3f\n",pendiente);
		printf("Promedio de inferencias: %.3f\n",promedio);
		printf("Umbral minimo de humedad: %i\n",nivel);
		printf("\n");
		printf("Accion a realizar: ");
		if (riego(pendiente,promedio,nivel))
		    printf("Activar riego.\n");
		else
		    printf("No activar riego.\n");
		promedio=0;
		yB=0;
		pendiente=0;
	}
}
