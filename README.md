# TG-2210-3
Implementación de RNN LSTM para estimación de humedad del suelo

Este repositorio contiene los códigos implementados en el desarrollo del proyecto “Predicción de riego para cultivos utilizando aprendizaje de máquina en el borde”.

En la carpeta "Firmware MCU", se encuentran los códigos en C++ para la adquisición de datos de los sensores y la ejecución del modelo TinyML correspondiente a una RNN LSTM. En esta carpeta, se encuentra la carpeta "InferenciaLSTM_test", donde está el código para la plataforma arduino el cual no generó salidas satisfactorias debido a la librería "TensorFlow". Sin embargo, en la carpeta "InferenciaLSTM_funcional", se encuentra el código para la plataforma de desarrollo Espressif, el cual permite el funcionamiento y las inferencias de manera exitosa. 

La carpeta "Machine learning", contiene los códigos en Python para la limpieza y organización de los datos, el análisis gráfico de los datos, el entrenamiento de una RNN LSTM y la conversión a TensorFlow Lite del modelo para pasarlo al dispositivo embebido.

La carpeta "Plataforma web", contiene el código implementado para la creación de ésta en Node-RED
