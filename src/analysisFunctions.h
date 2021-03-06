#ifndef analysisFunctions_cxx
#define analysisFunctions_cxx
#include <vector>

enum PeakFindingMethod
{
	byIncreases,
	byMean,
	byConstFraction,
	none
};

struct PeakFindingOptions
{
	double threshold;
	double threshold2;
	int iterations;
	int baselineSamples;
};

// Analysis functions
int mean(std::vector<unsigned int> * vec);
unsigned int peakMax(std::vector<unsigned int> * vec);
unsigned int * FindPeakByMean(std::vector<unsigned int> * data);
unsigned int * FindPeakByIncreases(std::vector<unsigned int> * data, double threshold, int iterations);
unsigned int * FindPeakByConstantFraction(std::vector<unsigned int> * data, double threshold);
unsigned int * FindPeak(
		std::vector<unsigned int> * data, 
		enum PeakFindingMethod method,
		struct PeakFindingOptions options);

#endif
