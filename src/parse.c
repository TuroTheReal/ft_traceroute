#include "ft_traceroute.h"

void validate_options(int argc, char **argv) {
	for (int i = 1; i < argc; i++) {
		if (argv[i][0] == '-' && argv[i][1] == '-') {
			// Options longues valides
			if (strcmp(argv[i], "--no-dns") != 0 &&
				strcmp(argv[i], "--max-hops") != 0 &&
				strcmp(argv[i], "--queries") != 0 &&
				strcmp(argv[i], "--help") != 0 &&
				strcmp(argv[i], "--version") != 0 &&
				strcmp(argv[i], "--interface") != 0) {
				fprintf(stderr, "unrecognized option '%s'\n", argv[i]);
				print_help();
				exit(1);
			}
		}
		else if (argv[i][0] == '-' && argv[i][1] != '\0') {
			// Vérifier que c'est une option courte valide (1 seul caractère après -)
			if (argv[i][2] != '\0' && argv[i][1] != '-' &&
				((argv[i][1] == 'n') || (argv[i][1] == 'h') || (argv[i][1] == 'V'))) {
				// C'est une chaîne comme -help, -ttl, etc.
				fprintf(stderr, "unrecognized option '%s'\n", argv[i]);
				print_help();
				exit(1);
			}
		}
	}
}

void parse_args(int argc, char** argv, t_trace *trace) {
	int opt;
	int max_ttl_provided = 0;
	int nprobes_provided = 0;
	long max_ttl_value = 0;
	long nprobes_value = 0;

	static struct option long_options[] = {
		{"no-dns", no_argument, NULL, 'n'},
		{"max-hops", required_argument, NULL, 'm'},
		{"queries", required_argument, NULL, 'q'},
		{"interface", required_argument, NULL, 'i'},
		{"version", no_argument, NULL, 'V'},
		{"help", no_argument, NULL, 'h'},
		{0, 0, 0, 0}
	};

	validate_options(argc, argv);
	opterr = 0;  // Désactiver msg erreur auto getopt

	while ((opt = getopt_long(argc, argv, "V?nm:q:i:", long_options, NULL)) != -1) {
		switch (opt) {
			case 'V':
				print_version();
				exit(0);

			case 'n':
				trace->no_dns = 1;
				break;

			case 'm': {
				char *endptr;
				max_ttl_value = strtol(optarg, &endptr, 10);
				if (*endptr != '\0') {
					fprintf(stderr, "Cannot handle `-%c' option with arg `%s' (argc %d)\n",
							opt, optarg, optind - 1);
					exit(1);
				}
				max_ttl_provided = 1;
				break;
			}

			case 'q': {
				char *endptr;
				nprobes_value = strtol(optarg, &endptr, 10);
				if (*endptr != '\0') {
					fprintf(stderr, "Cannot handle `-%c' option with arg `%s' (argc %d)\n",
							opt, optarg, optind - 1);
					exit(1);
				}
				nprobes_provided = 1;
				break;
			}

			case 'i':
				trace->interface = optarg;
				break;

			case '?':
				if (optopt == '?') {
					print_help();
					exit(0);
				} else if (optopt) {
					if (optopt == 'm' || optopt == 'q' || optopt == 'i') {
						fprintf(stderr, "Option `-%c' (argc %d) requires an argument: `-%c ",
								optopt, optind - 1, optopt);
						if (optopt == 'm')
							fprintf(stderr, "max_ttl'\n");
						else if (optopt == 'q')
							fprintf(stderr, "nqueries'\n");
						else if (optopt == 'i')
							fprintf(stderr, "device'\n");
						exit(1);
					} else {
						fprintf(stderr, "Bad option `-%c' (argc %d)\n", optopt, optind);
						exit(1);
					}
				} else {
					fprintf(stderr, "unrecognized option '%s'\n", argv[optind - 1]);
					fprintf(stderr, "Try 'traceroute --help' for more information.\n");
					exit(1);
				}
				break;

			case 'h':
				print_help();
				exit(0);

			default:
				if (optopt) {
					fprintf(stderr, "invalid option -- '%c'\n", optopt);
				} else {
					fprintf(stderr, "unrecognized option '%s'\n", argv[optind - 1]);
				}
				fprintf(stderr, "Try 'traceroute --help' for more information.\n");
				exit(1);
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

	if (max_ttl_provided) {
		if (max_ttl_value <= 0) {
			fprintf(stderr, "first hop out of range\n");
			exit(1);
		}
		if (max_ttl_value > 255) {
			fprintf(stderr, "max hops cannot be more than 255\n");
			exit(1);
		}
		trace->max_ttl = max_ttl_value;
	}

	if (nprobes_provided) {
		if (nprobes_value <= 0 || nprobes_value > 10) {
			fprintf(stderr, "no more than 10 probes per hop\n");
			exit(1);
		}
		trace->nprobes = nprobes_value;
	}

		if (optind + 1 < argc) {
		fprintf(stderr, "extra operand");
		for (int i = optind + 1; i < argc; i++) {
			fprintf(stderr, " '%s'", argv[i]);
		}
		fprintf(stderr, "\nTry 'traceroute --help' for more information.\n");
		exit(1);
	}

}