#include <iostream>
#include <thread>
#include <random>
#include <time.h>
#include <Windows.h>
#include <fstream>

using namespace std;

struct neuron
{
	double value;
	double error;
	void act()
	{
		value = (1 / (1 + pow(2.71828, -value)));
	}

};
class network
{
public:
	int layers;
	neuron** neurons;
	double*** weights;
	int* size;
	int threadsNum;

	double sigm_pro(double x)
	{
		if ((fabs(x-1) < 1e-9) || ((fabs(x) < 1e-9))) 
			return 0.0;
		double res =x * (1.0 - x);
			return res;
	}
	double predict(double x)
	{
		if (x >= 0.8)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	void setLayers(int n, int* p)
	{
		srand(time(0));
		layers = n;
		neurons = new neuron * [n];
		weights = new double** [n - 1];
		size = new int[n];
		for (int i = 0; i < n; i++)
		{
			size[i] = p[i];
			neurons[i] = new neuron[p[i]];
			if (i < n - 1)
			{
				weights[i] = new double* [p[i]];
				for (int j = 0; j < p[i]; j++)
				{
					weights[i][j] = new double[p[i + 1]];
					for (int k = 0; k < p[i + 1]; k++)
					{
						weights[i][j][k] = ((rand() % 100)) * 0.01 / size[i];
					}
				}
			}
		}
	}

	void setInput(double* p)
	{
		for (int i = 0; i < size[0]; i++)
		{
			neurons[0][i].value = p[i];
		}
	}
	void show()
	{
		setlocale(LC_ALL, "ru");
		cout << "Ядер процессора: " << thread::hardware_concurrency() << endl;
		cout << "Нейронная сеть имеет архитектуру: ";
		for (int i = 0; i < layers; i++)
		{
			cout << size[i];
			if (i < layers - 1)
			{
				cout << " - ";
			}
		}
		cout << endl;
		for (int i = 0; i < layers; i++)
		{
			cout << "\n#Слой " << i + 1 << "\n\n";
			for (int j = 0; j < size[i]; j++)
			{
				cout << "Нейрон #" << j + 1 << ": \n";
				cout << "Значение: " << neurons[i][j].value << endl;
				if (i < layers - 1)
				{
					cout << "Веса: \n";
					for (int k = 0; k < size[i + 1]; k++)
					{
						cout << "#" << k + 1 << ": ";
						cout << weights[i][j][k] << endl;
					}
				}
			}
		}
	}

	void LayersCleaner(int LayerNumber, int start, int stop)
	{
		srand(time(0));
		for (int i = 0; i < stop; i++)
		{
			neurons[LayerNumber][i].value = 0;
		}
	}

	void ForwardFeeder(int LayerNumber, int start, int stop)
	{
		for (int j = start; j < stop; j++)
		{
			for (int k = 0; k < size[LayerNumber - 1]; k++)
			{
				neurons[LayerNumber][j].value += neurons[LayerNumber - 1][k].value * weights[LayerNumber - 1][k][j];

			}
			neurons[LayerNumber][j].act();
		}
	}

	double ForwardFeed()
	{
		setlocale(LC_ALL, "ru");
		for (int i = 1; i < layers; i++)
		{
			if (threadsNum == 1)
			{
				thread th1([&]()
					{
						LayersCleaner(i, 0, size[i]);
					});
			}
			if (threadsNum == 2)
			{
				thread th1([&]()
					{
						LayersCleaner(i, 0, (floor(size[i] / 2)));
					});
				thread th2([&]()
					{
						LayersCleaner(i, int(floor(size[i] / 2)), size[i]);
					});
				th1.join();
				th2.join();
			}
			if (threadsNum == 4)
			{
				if (size[i] == 1)
				{
					cout << "Выполняю очистку слоя 1-м ядром...\n";
					thread th1([&]()
						{
							LayersCleaner(i, 0, size[i]);
					});

				th1.join();
				}
			
				if ((size[i] == 2) or (size[i] == 3))
				{
					cout << "Выполняю очистку слоя 2-МЯ ядрами...\n";
					thread th1([&]()
						{
							LayersCleaner(i, 0, (floor(size[i] / 2)));
						});
					thread th2([&]()
						{
							LayersCleaner(i, int(floor(size[i] / 2)), size[i]);
						});
					th1.join();
					th2.join();
				}
			
					if (size[i] >= 4)
					{
						int start1 = 0; int stop1 = int(size[i] / 4);
						int start2 = int(size[i] / 4); int stop2 = int(size[i] / 2);
						int start3 = int(size[i] / 2); int stop3 = int(size[i] - floor(size[i] / 4));
						int start4 = int(size[i] - floor(size[i] / 4)); int stop4 = size[i];
						thread th1([&]() {LayersCleaner(i, start1, stop1); });
						thread th2([&]() {LayersCleaner(i, start2, stop2); });
						thread th3([&]() {LayersCleaner(i, start3, stop3); });
						thread th4([&]() {LayersCleaner(i, start4, stop4); });
						th1.join();
						th2.join();
						th3.join();
						th4.join();
					}
			}
		
			if (threadsNum == 1)
			{
				thread th1([&]()
					{
						ForwardFeeder(i, 0, size[i]);
					});
			}
			if (threadsNum == 2)
			{
				thread th1([&]()
				{
					ForwardFeeder(i, 0, (floor(size[i] / 2)));
				});
				thread th2([&]()
				{
					ForwardFeeder(i, int(floor(size[i] / 2)), size[i]);
				});
				th1.join();
				th2.join();
			}
			if (threadsNum == 4)
			{
				
		
				if ((size[i] == 2) or (size[i] == 3))
				{
					cout << "Выполняю очистку слоя 2-МЯ ядрами...\n";
					thread th1([&]()
						{
							ForwardFeeder(i, 0, (floor(size[i] / 2)));
						});
					thread th2([&]()
						{
							ForwardFeeder(i, int(floor(size[i] / 2)), size[i]);
						});
					th1.join();
					th2.join();
				}
			
				if (size[i] >= 4)
				{
					int start1 = 0; int stop1 = int(size[i] / 4);
					int start2 = int(size[i] / 4); int stop2 = int(size[i] / 2);
					int start3 = int(size[i] / 2); int stop3 = int(size[i] - floor(size[i] / 4));
					int start4 = int(size[i] - floor(size[i] / 4)); int stop4 = size[i];
					thread th1([&]() {ForwardFeeder(i, start1, stop1); });
					thread th2([&]() {ForwardFeeder(i, start2, stop2); });
					thread th3([&]() {ForwardFeeder(i, start3, stop3); });
					thread th4([&]() {ForwardFeeder(i, start4, stop4); });
					th1.join();
					th2.join();
					th3.join();
					th4.join();
				}
			}
		}
		double max = 0;
		double prediction = 0;
		for (int i = 0; i < size[layers - 1]; i++)
		{
			if (neurons[layers - 1][i].value > max)
			{
				max = neurons[layers - 1][i].value;
				prediction = i;
			}
		}
		return prediction;

	}


};





