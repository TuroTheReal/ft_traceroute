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
				fprintf(stderr, "traceroute: unrecognized option '%s'\n", argv[i]);
				print_help();
				exit(1);
			}
		}
		else if (argv[i][0] == '-' && argv[i][1] != '\0') {
			// Vérifier que c'est une option courte valide (1 seul caractère après -)
			if (argv[i][2] != '\0' && argv[i][1] != '-' && ((argv[i][1] == 'n') || (argv[i][1] == 'h') || (argv[i][1] == 'V'))) {
				// C'est une chaîne comme -help, -ttl, etc.
				fprintf(stderr, "traceroute: unrecognized option '%s'\n", argv[i]);
				print_help();
				exit(1);
			}
		}
	}
}

void parse_args(int argc, char** argv, t_trace *trace) {
	int opt;
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
			case 'V': print_version(); exit(0);
			case 'n': {
				char *endptr;
				trace->count = strtol(optarg, &endptr, 10);
				if (*endptr != '\0') {
					fprintf(stderr, "traceroute: invalid argument: '%s'\n", optarg);
					exit(1);
				}
				else if (trace->count <= 0 || trace->count >= INT32_MAX) {
					fprintf(stderr, "traceroute: invalid argument: '%s': out of range: 1 <= value <= 2147483647\n", optarg);
					exit(1);
				}
				break;
			}
			case 'm': {
				char *endptr;
				trace->interval = strtod(optarg, &endptr);

				if (*endptr != '\0' || trace->interval < 0)
					trace->interval = 0;

				if (trace->interval < 0.2 && getuid() != 0) {
					fprintf(stderr, "traceroute: cannot flood; minimal interval allowed is 200ms\n");
					exit(1);
				}
				break;
			}
			case 'q': {
				char *endptr;
				trace->timeout = strtod(optarg, &endptr);
				if (*endptr != '\0' || (trace->timeout <= 0 || trace->timeout > INT32_MAX)) {
					if (*endptr != '\0')
						fprintf(stderr, "traceroute: invalid argument: '%s'\n", optarg);
					else if (trace->timeout <= 0)
						fprintf(stderr, "traceroute: option value too small: '%s'\n", optarg);
					else
						fprintf(stderr, "traceroute: option value too big: '%s'\n", optarg);
					exit(1);
				}
				break;
			}
			case 'i': {
				char *endptr;
				trace->ttl = strtol(optarg, &endptr, 10);
				if (*endptr != '\0') {
					fprintf(stderr, "traceroute: invalid argument: '%s'\n", optarg);
					exit(1);
				}
				else if (trace->ttl == 0){
					fprintf(stderr, "traceroute: cannot set unicast time-to-live: invalid argument: '%s'\n", optarg);
					exit(1);
				}
				if (trace->ttl < 0 || trace->ttl > 255){
					fprintf(stderr, "traceroute: invalid argument: '%s': out of range: 0 <= value <= 255\n", optarg);
					exit(1);
				}
				break;
			}
			case '?':
				if (optopt == 0) {
					// -?
					print_help();
					exit(0);
				} else {
					// Option invalide
					fprintf(stderr, "traceroute: invalid option -- '%c'\n", optopt);
					print_help();
					exit(1);
				}
				break;
			case 'h':
				print_help();
				exit(0);
			default:
				fprintf(stderr, "traceroute: bad option '-%c'\n", optopt);
				exit(1);
		}
	}

	if (optind >= argc || argv[optind][0] == '-') {
		fprintf(stderr, "traceroute: missing or invalid hostname\n");
		fprintf(stderr, "Try 'trace --help' or 'trace --usage' for more information.\n");
		exit(2);
	}
	trace->hostname = argv[optind];

	if (optind + 1 < argc) {
		fprintf(stderr, "Too many arguments: ");
		for (int i = optind + 1; i < argc; i++)
			fprintf(stderr, "'%s' ", argv[i]);
		fprintf(stderr, "\n");
		exit(3);
	}
}