
#include "../SmartSlog_lib/SmartRoomOntology.h"

/* Print menu for user */
static void print_menu()
{
	printf("\n--- Menu ---");
	printf("\n1 - Send request.");
	printf("\n2 - Send Controll Request.");
	printf("\n0 - Exit.\n");
}

/* Removes all triples from thr smart space */
static void clean_ss()
{
	printf("\n\n --- Clean ---\n");

	ss_triple_t *triples = NULL;
	ss_add_triple(&triples, SS_RDF_SIB_ANY, SS_RDF_SIB_ANY, SS_RDF_SIB_ANY, SS_RDF_TYPE_URI, SS_RDF_TYPE_URI);

	ss_remove(sslog_get_ss_info(), triples);
	ss_delete_triples(triples);
}

int main()
{
	//sslog_ss_init_session();
	sslog_ss_init_session_with_parameters("X", "127.0.0.1", 10341);
	//sslog_ss_init_session_with_parameters("X", "194.85.173.9", 10010);
	register_ontology();

	if (ss_join(sslog_get_ss_info(), "Queue Test") == -1) {
		printf("Can't join to SS\n");
		return 0;
	}

	printf("LOG: Initializing req");
	individual_t *req = sslog_new_individual(CLASS_REQUEST);
	if (req == NULL) {
		printf("\nError: %s\n", get_error_text());
		return 0;
	}

	sslog_ss_init_individual_with_uuid(req, "request 1");

	while (true) {
		print_menu();

		char c = getc(stdin);

		if (c == '0') {
			break;
		} else if (c == '1') {
			printf("LOG: Initializing req");
			if (req == NULL) {
				printf("\nError: %s\n", get_error_text());
				return 0;
			}
			
			printf("inserting individual in ss");
			if(sslog_ss_insert_individual(req) == -1){
				printf("Error: %s\n", get_error_text());
				return 1;
			}
		
		} else if (c == '2') {
		
			
		}
	}

    clean_ss();

	//free(calling);

	ss_leave(sslog_get_ss_info());
	sslog_ss_leave_session_all();
	sslog_repo_clean_all();

	printf("\nKP leave SS...\n");

	return 0;
}
