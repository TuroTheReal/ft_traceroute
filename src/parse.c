#include "ft_traceroute.h"

void validate_options(int argc, char **argv) {
	for (int i = 1; i < argc; i++) {
		if (argv[i][0] == '-' && argv[i][1] == '-') {
			// Options longues valides
			if (strcmp(argv[i], "--port") != 0 &&
				strcmp(argv[i], "--max-hops") != 0 &&
				strcmp(argv[i], "--queries") != 0 &&
				strcmp(argv[i], "--help") != 0 &&
				strcmp(argv[i], "--version") != 0 &&
				strcmp(argv[i], "--wait") != 0 &&
				strcmp(argv[i], "--interface") != 0) {
				fprintf(stderr, "unrecognized option '%s'\n", argv[i]);
				print_help();
				exit(1);
			}
		}
		else if (argv[i][0] == '-' && argv[i][1] != '\0') {
			// Vérifier que c'est une option courte valide (1 seul caractère après -)
			if (argv[i][2] != '\0' && argv[i][1] != '-' &&
				((argv[i][1] == 'h') || (argv[i][1] == 'V'))) {
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
		{"port", required_argument, NULL, 'p'},
		{"max-hops", required_argument, NULL, 'm'},
		{"queries", required_argument, NULL, 'q'},
		{"interface", required_argument, NULL, 'i'},
		{"wait", required_argument, NULL, 'w'},
		{"version", no_argument, NULL, 'V'},
		{"help", no_argument, NULL, 'h'},
		{0, 0, 0, 0}
	};

	validate_options(argc, argv);
	opterr = 0;

	while ((opt = getopt_long(argc, argv, "V?p:m:q:i:w:", long_options, NULL)) != -1) {
		switch (opt) {
			case 'V':
				print_version();
				exit(0);

			case 'p':{
				char *endptr;
				int res = strtol(optarg, &endptr, 10);
				if (*endptr != '\0') {
					fprintf(stderr, "Cannot handle `-%c' option with arg `%s' (argc %d)\n",
							opt, optarg, optind - 1);
					exit(1);
				}
				if (res < 0 || res > 65535)
					fprintf(stderr, "Cannot handle port %d, set to default\n", res);
				else
					trace->base_port = res;
				break;
			}

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

			case 'w': {
				double waittime = 5.0, here = 0.0, near = 0.0;
				char *endptr;

				// Parser la première valeur (waittime)
				waittime = strtod(optarg, &endptr);

				// Si c'est suivi d'une virgule, parser here
				if (*endptr == ',') {
					endptr++;
					here = strtod(endptr, &endptr);

					// Si c'est suivi d'une autre virgule, parser near
					if (*endptr == ',') {
						endptr++;
						near = strtod(endptr, &endptr);
					}
				}

				// Vérifier qu'il n'y a pas de caractères invalides à la fin
				if (*endptr != '\0') {
					fprintf(stderr, "Cannot handle `-%c' option with arg `%s' (argc %d)\n",
							opt, optarg, optind - 1);
					exit(1);
				}

				// Valider les valeurs
				if (waittime < 0 || here < 0 || near < 0) {
					fprintf(stderr, "bad wait specifications `%s'\n", optarg);
					exit(1);
				}

				trace->waittime = waittime;
				trace->here = here;
				trace->near = near;
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
				}
				else if (optopt) {
					if (optopt == 'm' || optopt == 'q' || optopt == 'i' || optopt == 'p' || optopt == 'w') {
						fprintf(stderr, "Option `-%c' (argc %d) requires an argument: `-%c ", optopt, optind - 1, optopt);
						if (optopt == 'm')
							fprintf(stderr, "max_ttl'\n");
						else if (optopt == 'q')
							fprintf(stderr, "nqueries'\n");
						else if (optopt == 'i')
							fprintf(stderr, "device'\n");
						else if (optopt == 'p')
							fprintf(stderr, "port'\n");
						else if (optopt == 'w')
							fprintf(stderr, "MAX,HERE,NEAR'\n");
						exit(1);
					}
					else {
						fprintf(stderr, "Bad option `-%c' (argc %d)\n", optopt, optind);
						exit(1);
					}
				}
				else {
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
				}
				else {
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
		if (max_ttl_value < 0) {
			fprintf(stderr, "min hops cannot be less than 0\n");
			exit(1);
		}
		if (max_ttl_value == 0) {
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
		if (nprobes_value <= 0) {
			fprintf(stderr, "no less than 1 probes per hop\n");
			exit(1);
		}
		if (nprobes_value > 10) {
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