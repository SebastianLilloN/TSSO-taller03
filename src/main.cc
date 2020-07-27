#include <global.hh>
#include <checkArgs.hpp>

uint64_t *g_numerosRandom = nullptr;
uint64_t *g_numerosParalelos = nullptr;
uint64_t *g_numerosSerie = nullptr;
uint64_t *g_sumaParalelo = nullptr;
uint64_t *g_numerosOmp = nullptr;

void llenarArreglo(size_t start, size_t end, uint32_t limInf, uint32_t limSup)
{
	for (size_t i = start; i < end; ++i)
	{
		g_numerosParalelos[i] = g_numerosRandom[i];
	}
}

void sumaParcial(size_t limInf, size_t limSup, size_t pos)
{
	for (size_t j = limInf; j < limSup; ++j)
	{
		g_sumaParalelo[pos] += g_numerosParalelos[j];
	}
}

int main(int argc, char **argv)
{
	uint64_t totalElementos;
	uint64_t numThreads;
	uint64_t numLimInf;
	uint64_t numLimSup;
	std::vector<std::thread *> threadsLlenado;
	std::vector<std::thread *> threadsSum;
	auto argumentos = (std::shared_ptr<checkArgs>)new checkArgs(argc, argv);
	totalElementos = argumentos->getArgs().tamArr;
	numThreads = argumentos->getArgs().numThreads;
	numLimInf = argumentos->getArgs().limInf;
	numLimSup = argumentos->getArgs().limSup;
	uint64_t sumaParalela = 0;
	uint64_t sumaSerial = 0;
	uint32_t sumaOmp = 0;

	std::srand(std::time(0));
	std::random_device device;
	std::mt19937 rng(device());
	std::uniform_int_distribution<> unif(numLimInf, numLimSup);

	g_numerosRandom = new uint64_t[totalElementos];
	g_numerosParalelos = new uint64_t[totalElementos];
	g_sumaParalelo = new uint64_t[totalElementos];
	g_numerosSerie = new uint64_t[totalElementos];
	g_numerosOmp = new uint64_t[totalElementos];

	for (size_t i = 0; i < totalElementos; ++i)
	{
		uint64_t randNumber = unif(rng);
		g_numerosRandom[i] = randNumber;
	}

	for (size_t i = 0; i < numThreads; ++i)
	{
		threadsLlenado.push_back(new std::thread((std::ref(llenarArreglo)),
												 i * (totalElementos) / numThreads,
												 (i + 1) * (totalElementos / numThreads), numLimInf, numLimSup));
	}

	auto start = std::chrono::high_resolution_clock::now();
	for (auto &thF : threadsLlenado)
	{
		thF->join();
	}
	auto end = std::chrono::high_resolution_clock::now();
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	auto tiempoTotalLlenadoParalelo = elapsed.count();

	start = std::chrono::high_resolution_clock::now();
	for (size_t i = 0; i < totalElementos; ++i)
	{
		g_numerosSerie[i] = g_numerosRandom[i];
	}
	end = std::chrono::high_resolution_clock::now();
	elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	auto tiempoTotalLlenadoSerial = elapsed.count();

	for (size_t i = 0; i < numThreads; ++i)
	{
		threadsSum.push_back(new std::thread(std::ref(sumaParcial),
											 i * (totalElementos) / numThreads,
											 (i + 1) * (totalElementos / numThreads), i));
	}

	start = std::chrono::high_resolution_clock::now();
	for (auto &thS : threadsSum)
	{
		thS->join();
	}

	for (size_t k = 0; k < numThreads; ++k)
	{
		sumaParalela += g_sumaParalelo[k];
	}
	end = std::chrono::high_resolution_clock::now();
	elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	auto tiempoTotalSumaParalelo = elapsed.count();

	start = std::chrono::high_resolution_clock::now();
	for (size_t i = 0; i < totalElementos; i++)
	{
		sumaSerial += g_numerosSerie[i];
	}
	end = std::chrono::high_resolution_clock::now();
	elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	auto tiempoTotalSumaSerial = elapsed.count();

	// auto vectorLineal1 = new uint32_t[totalElementos];

	start = std::chrono::system_clock::now();

#pragma omp parallel for num_threads(numThreads)
	for (size_t i = 0; i < totalElementos; ++i)
	{
		g_numerosOmp[i] = g_numerosRandom[i];
	}
	end = std::chrono::high_resolution_clock::now();
	elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	auto tiempoTotalLlenadoOmp = elapsed.count();

	start = std::chrono::system_clock::now();
#pragma omp parallel for reduction(+ \
								   : sumaOmp) num_threads(numThreads)
	for (size_t i = 0; i < totalElementos; ++i)
	{
		sumaOmp += g_numerosOmp[i];
	}
	end = std::chrono::high_resolution_clock::now();
	elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
	auto tiempoTotalSumaOmp = elapsed.count();

	std::cout << "Elementos                     : " << totalElementos << std::endl;
	std::cout << "Threads                       : " << numThreads << std::endl;
	std::cout << "Limite Inferior               : " << numLimInf << std::endl;
	std::cout << "Limite Superior               : " << numLimSup << std::endl;
	std::cout << "Suma en Paralelo              : " << sumaParalela << std::endl;
	std::cout << "Suma en Serie                 : " << sumaSerial << std::endl;
	std::cout << "Suma OMP                      : " << sumaOmp << std::endl;
	std::cout << "Tiempo Total Llenado Serial   : " << tiempoTotalLlenadoSerial << " ms" << std::endl;
	std::cout << "Tiempo Total Suma Serial      : " << tiempoTotalSumaSerial << " ms" << std::endl;
	std::cout << "Tiempo Total Llenado Paralelo : " << tiempoTotalLlenadoParalelo << " ms" << std::endl;
	std::cout << "Tiempo Total Suma Paralela    : " << tiempoTotalSumaParalelo << " ms" << std::endl;
	std::cout << "Tiempo Total Llenado OMP      : " << tiempoTotalLlenadoOmp << " ms" << std::endl;
	std::cout << "Tiempo Total Suma OMP         : " << tiempoTotalSumaOmp << " ms" << std::endl;
	return (EXIT_SUCCESS);
}