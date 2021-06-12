#include "kMeans.h"
#include <QDebug>

using namespace std;
#define CONVERGENCE_THRESHOLD 1e-8

// Calcilate Euclid distance between two points in n-dimensional space
static double getDistance(const vector<double>& a, const vector<double>& b) {
	double temp = 0.0;
	auto it1 = a.begin();
	auto it2 = b.begin();
	for (; (it1 != a.end()) && (it2 != b.end()); it1++, it2++)
	{
		temp += (*it1 - *it2) * (*it1 - *it2);
	}
	return sqrt(temp);
}

// Find nearest cluster for the given point
int whichIsNearest(const vector<vector<double>>& centroids, const vector<double>& pt) {
	double minDistance = 0;
	int minLabel = 0;
	for (int i = 0; i<centroids.size(); i++) {
		double tempDistance = getDistance(centroids[i], pt);
		if (i == 0 || tempDistance < minDistance) {
			minDistance = tempDistance;
			minLabel = i;
		}
	}
	return minLabel;
}

// Calculate kMeans
vector<vector<double>> kMeansCentroids(vector<vector<double>> points, int nFeatures, int K) {
	// Total number of points
	int nSamples = points.size();

	// Randomize initial centroids
	vector<vector<double>> centroids;
	for (int k = 0; k < K; k++)
	{
		int rand_int = rand() % nSamples;
		centroids.push_back(points[rand_int]);
	}

	// Create empty vector for each cluster
	vector<vector<int> > cluster;
	for (int k = 0; k < K; k++) {
		vector<int> vectemp;
		cluster.push_back(vectemp);
	}

	// Iteration counter
	int counter = 0;

	// Iteratively find better centroids
	while (1) {
		// Clear each cluster
		for (int k = 0; k < K; k++)
		{
			cluster[k].clear();
		}

		// Set convergence flag to TRUE
		bool converge = true;

		// For every sample, find which cluster it belongs to,
		// By comparing the distance between it and each clusters' centroid.
		for (int m = 0; m < nSamples; m++) {
			int which = whichIsNearest(centroids, points[m]);
			cluster[which].push_back(m);
		}

		// For every cluster, re-calculate its centroid.
		for (int k = 0; k < K; k++) {
			int clusterSize = cluster[k].size();

			vector<double> vectemp = vector<double>(nFeatures, 0);
			for (int i = 0; i < clusterSize; i++) {
				for (int j = 0; j < nFeatures; j++)
					vectemp[j] = vectemp[j] + points[cluster[k][i]][j] / (double)clusterSize;
			}
			// If centroid position changed set convergence flag to false
			if (getDistance(centroids[k], vectemp) >= CONVERGENCE_THRESHOLD)
				converge = false;
			centroids[k] = vectemp;
		}
		
		// If convergence achieved break the loop
		if (converge) break;
		++counter;
	}


	// Return cluster centroid vectors
	return centroids;
}






