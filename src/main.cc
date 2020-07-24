#include <global.hh>
#include <checkArgs.hpp>

uint64_t *g_OmpParalelo = nullptr;
uint64_t *g_numerosSerie = nullptr;
uint32_t sumaOmpParalelo = 0;
uint32_t sumaSerial = 0;

void llenarOmp(size_t tipoSumaLlenado, size_t hilos, size_t end, size_t limInf, size_t limSup)
{
	std::random_device device;
	std::mt19937 rng(device());
	std::uniform_int_distribution<uint32_t> unif(limInf, limSup);
	switch (tipoSumaLlenado)
	{
	case 0:
#pragma omp parallel for num_threads(hilos)
		for (size_t i = 0; i < end; ++i)
		{
			uint32_t number = unif(rng);
			g_OmpParalelo[i] = number;
		}
		break;
	case 1:
#pragma omp parallel for num_threads(1)
		for (size_t i = 0; i < end; ++i)
		{
			uint32_t number = unif(rng);
			g_numerosSerie[i] = number;
		}
		break;
	}
}

void sumarOmp(size_t tipoSuma, size_t hilos, size_t end)
{
	switch (tipoSuma)
	{
	case 0:
#pragma omp parallel for reduction(+ \
								   : sumaOmpParalelo) num_threads(hilos)
		for (size_t i = 0; i < end; ++i)
		{
			sumaOmpParalelo += g_OmpParalelo[i];
		}
		break;
	case 1:
#pragma omp parallel for reduction(+ \
								   : sumaSerial) num_threads(hilos)
		for (size_t i = 0; i < end; ++i)
		{
			sumaSerial += g_OmpParalelo[i];
		}
		break;
	}
}

int main(int argc, char **argv)
{
	uint64_t totalElementos;
	uint64_t numThreads;
	uint64_t numLimSup;
	uint64_t numLimInf;
	auto argumentos = (std::shared_ptr<checkArgs>)new checkArgs(argc, argv);
	totalElementos = argumentos->getArgs().tamArr;
	numThreads = argumentos->getArgs().numThreads;
	numLimSup = argumentos->getArgs().limSup;
	numLimInf = argumentos->getArgs().limInf;
	g_numerosSerie = new uint64_t[totalElementos];
	g_OmpParalelo = new uint64_t[totalElementos];

	auto start = std::chrono::high_resolution_clock::now();
	llenarOmp(0, numThreads, totalElementos, numLimInf, numLimSup);
	auto end = std::chrono::high_resolution_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	auto timepoTotalLlenadoOmp = elapsed.count();

	start = std::chrono::high_resolution_clock::now();
	llenarOmp(1, 1, totalElementos, numLimInf, numLimSup);
	end = std::chrono::high_resolution_clock::now();
	elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	auto tiempoTotalLlenadoSerial = elapsed.count();

	start = std::chrono::high_resolution_clock::now();
	sumarOmp(0, numThreads, totalElementos);
	end = std::chrono::high_resolution_clock::now();
	elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	auto tiempoTotalSumaOmp = elapsed.count();

	start = std::chrono::high_resolution_clock::now();
	sumarOmp(1, 1, totalElementos);
	end = std::chrono::high_resolution_clock::now();
	elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	auto tiempoTotalSumaSerial = elapsed.count();

	std::cout << "Elementos                     : " << totalElementos << std::endl;
	std::cout << "Threads                       : " << numThreads << std::endl;
	std::cout << "Limite Inferior               : " << numLimInf << std::endl;
	std::cout << "Limite Superior               : " << numLimSup << std::endl;
	std::cout << "Suma con OMP                  : " << sumaOmpParalelo << std::endl;
	std::cout << "Suma en Serie                 : " << sumaSerial << std::endl;
	std::cout << "Tiempo Total Llenado Serial   : " << tiempoTotalLlenadoSerial << " ms" << std::endl;
	std::cout << "Tiempo Total Suma Serial      : " << tiempoTotalSumaSerial << " ms" << std::endl;
	std::cout << "Tiempo Total Llenado OMP      : " << timepoTotalLlenadoOmp << " ms" << std::endl;
	std::cout << "Tiempo Total Suma OMP         : " << tiempoTotalSumaOmp << " ms" << std::endl;

	return (EXIT_SUCCESS);
}
