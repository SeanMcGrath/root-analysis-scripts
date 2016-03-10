#include "stdlib.h"
#include "argp.h"
#include <TFile.h>
#include <TApplication.h>
#include "TWaveScanner.h"
#include "TPeakIntegrator.h"

using namespace std;

// argp configuration
const char *argp_program_version = "fadc 0.1";
const char *argp_program_bug_address = "<srmcgrat@umass.edu>";
static char doc[] =
	"Utility for viewing and extracting the data contained in fADC125 .root files.\n"
	"COMMANDS:\n"
	"	view 		see plots of all the waveforms in the root file.\n"
	"	analyze		print a comma-separated list of pulse integrals\n"
	"			and leading-edge times to standard output.\n"
	"\n"
	"OPTIONS:\n";

static char args_doc[] = "COMMAND CHANNEL ROOT_FILE";

static struct argp_option options[] = {
	{"peakmethod",	'p', "METHOD",	0, "Use METHOD to find the peak in the waveform. METHOD must be one of { average | fractional }"},
	{"yrange",	'y', "Y_RANGE",	0, "set Y_RANGE as the maximum Y value of displayed plots."},
	{ 0 }
};

struct arguments
{
	char *command;
	int channel;
	char *rootFile;
	double yrange;
	enum PeakFindingMethod method;
};

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{
	struct arguments *arguments = (struct arguments *)state->input;

	switch (key)
	{
		case 'y':
			arguments->yrange = (double)strtof(arg, NULL);
			break;
		case 'p':
			if (strcmp(arg, "average") == 0) {
				arguments->method = byMean;
			}
			else if (strcmp(arg, "fractional") == 0) {
				arguments->method = byIncreases;
			}
			break;

		case ARGP_KEY_ARG:
			if (state->arg_num >= 3) {
				cout << "Too many arguments." << endl;
				argp_usage(state);
			}
			else if (state->arg_num == 0)
				arguments->command = arg;
			else if (state->arg_num == 1)
				if (strcmp(arg, "all") == 0)
					arguments->channel = -1;
				else
					arguments->channel = (int)strtol(arg, NULL, 10);
			else if (state->arg_num == 2)
				arguments->rootFile = arg;
			break;
		
		case ARGP_KEY_END:
			if (state->arg_num < 3)
				argp_usage(state);
			break;

		default:
			return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc };

// Main

TTree * getRawData(char *fileName)
{
	TFile *f = new TFile(fileName);
	TTree *Df125WindowRawData = (TTree*)f->Get("Df125WindowRawData");
	return Df125WindowRawData;
}

int main(int argc, char *argv[])
{
	struct arguments arguments;

	//defaults
	arguments.yrange = 2000.0;
	arguments.command = "";
	arguments.method = none;
	arguments.channel = 13;
	arguments.rootFile = "";
	
	argp_parse(&argp, argc, argv, ARGP_NO_EXIT, 0, &arguments);

	if (strcmp(arguments.command, "view") == 0) {
		TApplication theApp("tapp", &argc, argv);
		TTree *data = getRawData(arguments.rootFile);
		TCanvas *canvas = new TCanvas("c1");
		TWaveScanner scan;
		scan.SetAnalysisChannel(arguments.channel);
		scan.SetYAxisRange(arguments.yrange);
		scan.SetPeakFindingMethod(arguments.method);
		scan.SetCanvas(canvas);
		data->Process(&scan);
	}
	else if (strcmp(arguments.command, "analyze") == 0) {
		TTree *data = getRawData(arguments.rootFile);
		TPeakIntegrator integrator;
		integrator.SetAnalysisChannel(arguments.channel);
		integrator.SetPeakFindingMethod(arguments.method);
		data->Process(&integrator);
	}
	else if (strcmp(arguments.command, "") != 0)
		cout << "unknown command: " << arguments.command << endl;
	return 0;	
}
