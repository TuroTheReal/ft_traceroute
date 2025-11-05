#include "ft_traceroute.h"

void validate_options(int argc, char **argv) {
	for (int i = 1; i < argc; i++) {
		if (argv[i][0] == '-' && argv[i][1] == '-') {
			// Options longues valides
			if (strcmp(argv[i], "--help") != 0 &&
				strcmp(argv[i], "--version") != 0) {
				fprintf(stderr, "unrecognized option '%s'\n", argv[i]);
				print_help();
				exit(2);
			}
		}
		else if (argv[i][0] == '-' && argv[i][1] != '\0') {
			// Vérifier que c'est une option courte valide (1 seul caractère après -)
			if (argv[i][2] != '\0' && argv[i][1] != '-' &&
				((argv[i][1] == 'h') || (argv[i][1] == 'V'))) {
				// C'est une chaîne comme -help, -ttl, etc.
				fprintf(stderr, "unrecognized option '%s'\n", argv[i]);
				print_help();
				exit(2);
			}
		}
	}
}

void parse_args(int argc, char** argv, t_trace *trace) {
	int opt;

	static struct option long_options[] = {
		{"version", no_argument, NULL, 'V'},
		{"help", no_argument, NULL, 'h'},
		{0, 0, 0, 0}
	};

	validate_options(argc, argv);
	opterr = 0;

	while ((opt = getopt_long(argc, argv, "V?h", long_options, NULL)) != -1) {
		switch (opt) {
			case 'V':
				print_version();
				exit(0);

			case 'h':
				print_help();
				exit(0);

			case '?':
				if (!optopt){
					print_help();
					exit(0);
				}
				fprintf(stderr, "unrecognized option '%s'\n", argv[optind - 1]);
				fprintf(stderr, "Try 'traceroute --help' for more information.\n");
				exit(2);

			default:
				fprintf(stderr, "unrecognized option '%s'\n", argv[optind - 1]);
				fprintf(stderr, "Try 'traceroute --help' for more information.\n");
				exit(2);
		}
	}

	if (optind >= argc) {
		fprintf(stderr, "Specify \"host\" missing argument.\n");
		exit(1);
	}

	if (argv[optind][0] == '-') {
		fprintf(stderr, "invalid option -- '%s'\n", argv[optind]);
		fprintf(stderr, "Try 'traceroute --help' for more information.\n");
		exit(1);
	}

	trace->hostname = argv[optind];

	if (optind + 1 < argc) {
		fprintf(stderr, "extra operand");
		for (int i = optind + 1; i < argc; i++) {
			fprintf(stderr, " '%s'", argv[i]);
		}
		fprintf(stderr, "\nTry 'traceroute --help' for more information.\n");
		exit(2);
	}

}