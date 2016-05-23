#include "../SmartSlog_lib/SmartRoomOntology.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#define SSLOG_EXTERN

bool isValidIpAddress(char *ipAddress);
void clean_triples_from_ss();
int main()
{
    //local variables
	int firstreq = 1;
	int lastreq = 1; 
	
	char buffer [33] = "";
	char* headuuid="";
	
	char s_true[10] = "true";
	char s_false[10]  = "false";
	
	individual_t *temp_req;
	//sslog_ss_init_session();
	sslog_ss_init_session_with_parameters("X", "127.0.0.1", 10341);
	register_ontology();
        
	if (ss_join(sslog_get_ss_info(), "QueueServer KP") == -1) {
		printf("Can't join to SS\n");
		return 0;
	}
	
	individual_t *head = sslog_new_individual(CLASS_QUEUEHEAD);

	if (head == NULL) {
		printf("\nError: %s\n", get_error_text());
		return 1;
	}	
	
			
        sslog_ss_init_individual_with_uuid(head, "QueueHead");

	//request subscription initialization
	
	subscription_t *reqsub = sslog_new_subscription(false);
	sslog_sbcr_add_class(reqsub, CLASS_REQUEST);
	if(sslog_sbcr_subscribe(reqsub) != SSLOG_ERROR_NO){
		printf(ERROR: UNABLE TO SUBSCRIBE REQUESTS\n);
		return 1;
	}
	//controll request subscription initialization
	
	subscription_t *cnreqsub = sslog_new_subscription(false);
	sslog_sbcr_add_class(cnreqsub, CLASS_CONTROLLREQUEST);
    if(sslog_sbcr_subscribe(cnreqsub) != SSLOG_ERROR_NO){
		printf(ERROR: UNABLE TO SUBSCRIBE CONTROLL REQUESTS\n);
		return 1;
	}    
	//Base head initialization and insertion
	
	if(sslog_ss_add_property(head, PROPERTY_HEADUSERNAME, s_false) == -1){
        	printf("Error: %s\n", get_error_text());
		return 1;
	}
	
	if(sslog_ss_add_property(head, PROPERTY_ISBUSY, s_false) == -1){
        	printf("Error: %s\n", get_error_text());
		return 1;
	}

	if(sslog_ss_insert_individual(head) == -1){
        	printf("Error: %s\n", get_error_text());
		return 1;
	}

	
	printf("LOG: Succesfully initialized server.\n");
	while (true) {
		subscription_changes_data_t* req_ch_data = NULL;
		SSLOG_EXTERN list_t* req_ch_list = NULL;
		
		subscription_changes_data_t* cntr_req_ch_data = NULL;
		SSLOG_EXTERN list_t* cntr_req_ch_list = NULL;
		
		
		//processing new requests;
		req_ch_data = sslog_sbcr_get_changes_last(reqsub);
		req_ch_list = sslog_sbcr_ch_get_individual_by_action(req_ch_data, ACTION_REMOVE); 	
		if(!list_is_empty(req_ch_list)){
			printf("LOG: Some request were removed.\n");
			sslog_sbcr_ch_print(req_ch_data);
			list_head_t* pos = NULL;
			list_for_each(pos, &req_ch_list->links ){
				list_t* node = list_entry(pos, list_t, links);
				char* temp_uuid= (char*)(node->data);
				if(temp_uuid != NULL ){
					printf("	LOG: Processing request\n");
					if(!strcmp(temp_uuid, headuuid )){
						printf ("		LOG:User was in the head. releasing head\n");
						set_property_by_name(head,PROPERTY_ISBUSY->name, s_false );
						headuuid = "NaN";
						
					}
					
				}
				
			}
				
			list_free(req_ch_list);
		}
		
		
		req_ch_list = sslog_sbcr_ch_get_individual_by_action(req_ch_data, ACTION_INSERT);
		/*if(!list_is_empty(req_ch_list)){
			printf("LOG: Some requests were inserted.\n");
			req_ch_list = sslog_sbcr_ch_get_individual_all(req_ch_data);
			sslog_sbcr_ch_print(req_ch_data);
			
			itoa(i,buffer,10);
			set_property_by_name(temp_individual, PROPERTY_HASSTATE->name, )
			
			list_free(req_ch_list);
		}*/
		
		
		cntr_req_ch_data = sslog_sbcr_get_changes_last(cnreqsub);
		cntr_req_ch_list=sslog_sbcr_ch_get_individual_by_action(cntr_req_ch_data, ACTION_INSERT) ;
		if(!list_is_empty(cntr_req_ch_list)){
			printf("LOG: Some controll requests were inserted.\n");
			sslog_sbcr_ch_print(cntr_req_ch_data);
			
			list_head_t* pos = NULL;
			list_for_each(pos, &req_ch_list->links ){
				list_t* node = list_entry(pos, list_t, links);
				char* temp_uuid= (char*)(node->data);
				individual_t* temp_individual = sslog_repo_get_individual_by_uuid(temp_uuid);
				lastreq++;
				char temp_str[50];
				sprintf(buffer, "%d", temp_str);
				set_property_by_name(temp_individual, PROPERTY_HASSTATE->name, temp_str);
				
			}
			
			list_free(req_ch_list);
		}
		
		
		//
		//Check if server is free set new speaker if needed
		//
		prop_val_t *iB_val = sslog_get_property(head,PROPERTY_ISBUSY->name );
		char *temp_isBusy = strdup((char *) iB_val->prop_value);
		if (!strcmp(temp_isBusy, s_false)){
			individual_t* min_req;
			int position;
			list_t* candidates = sslog_ss_get_individual_by_class_all(CLASS_REQUEST);
			if(!list_is_empty(candidates)){
				list_head_t* pos = NULL;
				list_for_each(pos, &candidates->links ){
					list_t* node = list_entry(pos, list_t, links);
					individual_t* temp_individual = (individual_t*)(node->data);
				// To Do: change position property
					prop_val_t* pos = sslog_get_property(temp_individual, PROPERTY_HASSTATE->name);
				
					if (pos !=NULL ){
						char* posstr = strdup((char *) pos->prop_value);
						int posint = atoi(posstr);
						if(posint != NULL 
						&& posint > firstreq
						&& posint < position){
							position = posint;
							min_req = temp_individual;
							
						}
					}
					prop_val_t* new_speaker = sslog_get_property(min_req, PROPERTY_REQUESTUSERNAME->name);
					char* new_speaker_name = strdup((char *) new_speaker->prop_value);
					set_property_by_name(head,PROPERTY_HEADUSERNAME->name, new_speaker_name );
					set_property_by_name(head, PROPERTY_ISBUSY->name, s_true);
					firstreq = position;
					headuuid = min_req->uuid;
				}
			}
		}
		
	}

        //clean_triples_from_ss();
	
	sslog_sbcr_unsubscribe(reqsub);
	sslog_sbcr_unsubscribe(cnreqsub);
	
	ss_leave(sslog_get_ss_info());
	sslog_ss_leave_session_all();
	sslog_repo_clean_all();

	printf("\nKP leave SS...\n");

	return 0;
}
bool isValidIpAddress(char *ipAddress)
{
    struct sockaddr_in sa;
    int result = inet_pton(AF_INET, ipAddress, &(sa.sin_addr));
    return result != 0;
}
