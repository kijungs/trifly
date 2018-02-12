#include "option.hpp"

/*************************************************
*		Initial values							 *
**************************************************/
char*		Option::inFileName = nullptr;
char*		Option::outPath = nullptr;

double			Option::tolerance(0.2);
unsigned int	Option::budget(1E05);
unsigned int	Option::trial(3);


const char short_options[] = "i:l:s:t:u:";

static struct option long_options[] = {
	{"budget", required_argument, 0, SIZE_BUDGET},
	{"tolerance", required_argument, 0, SIZE_TOLERANCE}, 
	{"trial", required_argument, 0, SIZE_TRIAL}, 
	{0, 0, 0, 0}
};

bool Option::parse(int argc, char **argv)
{

	int opt = 0;
	int option_index = 0;
	while(1)
	{
		opt = getopt_long(argc, argv, short_options, long_options, &option_index);
		if (-1 == opt)
			break;

		switch(opt)
		{
			case SIZE_BUDGET:
				Option::budget = atoi(optarg);
				break;

			case SIZE_TOLERANCE:
				Option::tolerance = atof(optarg);
				break;

			case SIZE_TRIAL:
				Option::trial = atoi(optarg);
				break;

			default:
				if (long_options[option_index].flag != 0)
					break;

				cerr << "Invalid parameter " << argv[optind] << " " << optarg << endl;
				exit(1);
		}
	}

	if (optind < argc - 1)
	{
		Option::inFileName = argv[optind++];
		Option::outPath = argv[optind++];
	}
	else
	{
		cout << "Invalid parameter" << endl;
		exit(1);
	}

	return true;

}

void Option::print()
{

	cout << Option::inFileName << "\t" << Option::outPath << "\t" << Option::budget << "\t" << Option::tolerance << "\t" << Option::trial << endl;
	cout << endl;


	return;
}

