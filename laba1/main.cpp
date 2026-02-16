#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <chrono>
#include <limits>

using namespace std;

const int N = 70000;
const int M = 10;
const int K = 11;
const int MAX_ITER = 100;

vector<vector<double>> generateObjects()
{
    vector<vector<double>> objects(N, vector<double>(M));

    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, 100.0);

    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < M; j++)
        {
            objects[i][j] = dis(gen);
        }
    }

    return objects;
}

vector<vector<double>> initializeCenters(const vector<vector<double>>& objects)
{
    vector<vector<double>> centers(K, vector<double>(M));

    for (int i = 0; i < K; i++)
    {
        centers[i] = objects[i];
    }

    return centers;
}

double distanceSquared(const vector<double>& a, const vector<double>& b)
{
    double sum = 0.0;

    for (int i = 0; i < M; i++)
    {
        sum += (a[i] - b[i]) * (a[i] - b[i]);
    }

    return sum;
}

bool assignObjectsToClusters(
    const vector<vector<double>>& objects,
    const vector<vector<double>>& centers,
    vector<int>& clusterAssignment)
{
    bool changed = false;

    for (int i = 0; i < N; i++)
    {
        double minDist = numeric_limits<double>::max();
        int bestCluster = 0;

        for (int k = 0; k < K; k++)
        {
            double dist = distanceSquared(objects[i], centers[k]);

            if (dist < minDist)
            {
                minDist = dist;
                bestCluster = k;
            }
        }

        if (clusterAssignment[i] != bestCluster)
        {
            clusterAssignment[i] = bestCluster;
            changed = true;
        }
    }

    return changed;
}

void updateCenters(
    const vector<vector<double>>& objects,
    vector<vector<double>>& centers,
    const vector<int>& clusterAssignment)
{
    vector<vector<double>> newCenters(K, vector<double>(M, 0.0));
    vector<int> numElementsInCluster(K, 0);

    for (int i = 0; i < N; i++)
    {
        int c = clusterAssignment[i];
        numElementsInCluster[c]++;

        for (int j = 0; j < M; j++)
        {
            newCenters[c][j] += objects[i][j];
        }
    }

    for (int k = 0; k < K; k++)
    {
        if (numElementsInCluster[k] > 0)
        {
            for (int j = 0; j < M; j++)
            {
                centers[k][j] = newCenters[k][j] / numElementsInCluster[k];
            }
        }
    }
}

int main()
{
    cout << "Объектов: " << N << endl;
    cout << "Атрибутов элемента: " << M << endl;
    cout << "Кластеров: " << K << endl;

    vector<vector<double>> objects = generateObjects();
    vector<vector<double>> centers = initializeCenters(objects);
    vector<int> clusterAssignment(N, -1);

    int iteration = 0;
    bool changed = true;

    auto start = chrono::high_resolution_clock::now();

    while (changed && iteration < MAX_ITER)
    {
        iteration++;

        changed = assignObjectsToClusters(objects, centers, clusterAssignment);
        updateCenters(objects, centers, clusterAssignment);
    }

    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;

    cout << "Итераций: " << iteration << endl;
    cout << "Время выполнения (CPU): " << elapsed.count() << " секунд" << endl;

    cout << "\nРазмеры кластеров:\n";

    vector<int> finalCounts(K, 0);

    for (int i = 0; i < N; i++)
    {
        finalCounts[clusterAssignment[i]]++;
    }

    for (int k = 0; k < K; k++)
    {
        cout << "Кластер " << k << ": " << finalCounts[k] << endl;
    }

    cout << endl;

    for (int k = 0; k < K; k++)
    {
        cout << "Центроид " << k << ": " << endl;

        for (int j = 0; j < M; j++)
        {
            cout << centers[k][j] << " ";
        }
        cout << endl;
    }

    return 0;
}
