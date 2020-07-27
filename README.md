# TSSOO-taller03

* Autor: Sebastián Lillo Núñez

* Correo: sebastian.lillon@alumnos.uv.cl

A continuacion se presenta un desafio planeteado a modo de taller en donde se solicita procesar datos generados de forma aleatoria y almacenados en tipo de datos como arreglos
o vectores. Para esto se presentan dos modulos de procesamiento, uno trabaja de manera secuencial y la otra de forma paralela, indicando que se debe implementar con la API OpenMP,
además, de realizar la misma implementacion pero de forma similar y sin OMP, tal como fue en el taller02, con instancias de threads distintos, también conocidos como hilos, son 
formas de procesarparalelamente un misma acción dentro del programa, para asi agilizar minimizando los tiempos de ejecución de los procesamientos de datos. El modulo paralelo 
ya mencionado trabaja a su vez con dos componentes, el llenado de arreglo y la suma de los elementos del arreglo, para realizar estas acciones, se utilizan herramientas como 
vectores y la biblioteca chronos para evidenciar los tiempos y sacar conclusiones a partir de eso; por tanto este módulo trabajo con una cantidad de hilos definida por lo 
parametros de entrada, junto con la cantidad de datos y los rangos que pueden tomar los número aleatorios generados. A su vez se compara con un tercer modulo que realiza el mismo 
procesamiento de datos pero de manera secuencial para asi conseguir un contraste mayor en cuanto a los tiempos de ejecución se refiere, ya que estos son reducidos en procesos
paralelizados.
