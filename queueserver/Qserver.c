#include "../SmartSlog_lib/SmartRoomOntology.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#define SSLOG_EXTERN

#ifndef _KBHIT_H_
#define _KBHIT_H_
 
#include <sys/select.h>
 
int kbhit()
{
    struct timeval tv;
    fd_set read_fd;
 
    tv.tv_sec=0;
    tv.tv_usec=0;
    FD_ZERO(&read_fd);
    FD_SET(0,&read_fd);
 
    if(select(1, &read_fd, NULL, NULL, &tv) == -1)
        return 0;
 
    if(FD_ISSET(0,&read_fd))
        return 1;
 
    return 0;
}
 
#endif 

void requestsHandler(subscription_t *);
void cntrRequestsHandler(subscription_t *);
bool isValidIpAddress(char *ipAddress);
void clean_triples_from_ss();

//global variables
int firstreq = 1;
int lastreq = 1; 
char buffer [33] = "";
char* headuuid="NaN";
	
char* headisbusy = "false";
	
char s_true[10] = "true";
char s_false[10]  = "false";
	
bool reqAdded = true;

individual_t *head;
	
int main()
{
    //local variables	
	/*
	char buffer [33] = "";
	char* headuuid="NaN";
	
	char* headisbusy = "false";
	
	char s_true[10] = "true";
	char s_false[10]  = "false";
	
	bool reqAdded = true;
	*/
	
	individual_t *temp_req;
	//sslog_ss_init_session();
	sslog_ss_init_session_with_parameters("X", "127.0.0.1", 10341);
	//sslog_ss_init_session_with_parameters("X", "194.85.173.9", 10011);
	register_ontology();
        
	if (ss_join(sslog_get_ss_info(), "QueueServerKP") == -1) {
		printf("ERROR: Can't join to SS\n");
		return 0;
	}
	
	head = sslog_new_individual(CLASS_QUEUEHEAD);

	if (head == NULL) {
		printf("\nError: %s\n", get_error_text());
		return 1;
	}	
			
    sslog_ss_init_individual_with_uuid(head, "QueueHead");
	
	individual_t *server = sslog_new_individual(CLASS_QUEUESERVICE);
	
	
	if (server == NULL) {
		printf("\nError: %s\n", get_error_text());
		return 1;
	}	
			
    sslog_ss_init_individual_with_uuid(server, "QueueService");
	
	if(sslog_ss_add_property(server, PROPERTY_FIRSTREQ, "1") == -1){
        	printf("Error: %s\n", get_error_text());
		return 1;
	}
	
	if(sslog_ss_add_property(server, PROPERTY_LASTREQ, "1") == -1){
        	printf("Error: %s\n", get_error_text());
		return 1;
	}
	
	//request subscription initialization
	
	subscription_t *reqsub = sslog_new_subscription(true);
	sslog_sbcr_add_class(reqsub, CLASS_REQUEST);
	sslog_sbcr_set_changed_handler	(reqsub, requestsHandler);
	sslog_sbcr_subscribe(reqsub);
	if(!sslog_sbcr_is_active(reqsub)){
		printf ("LOG: req subscription not active\n ");
	}
	
	/*
	if(sslog_sbcr_subscribe(reqsub) != SSLOG_ERROR_NO){
		printf(ERROR: UNABLE TO SUBSCRIBE REQUESTS\n);
		return 1;
	}
	*/
	//controll request subscription initialization
	
	subscription_t *cnreqsub = sslog_new_subscription(false);
	sslog_sbcr_add_class(cnreqsub, CLASS_CONTROLLREQUEST);
	sslog_sbcr_subscribe(cnreqsub);
	if(!sslog_sbcr_is_active(cnreqsub)){
		printf ("LOG: req subscription not active\n ");
	}
	/*
    if(sslog_sbcr_subscribe(cnreqsub) != SSLOG_ERROR_NO){
		printf(ERROR: UNABLE TO SUBSCRIBE CONTROLL REQUESTS\n);
		return 1;
	}  
*/	
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
	printf("LOG: head inserted succesfully");

	
	printf("LOG: Succesfully initialized server.\n");
	printf("---------------------------------------------\n");
	printf("|-------PRESS e TO EXIT---------------------|\n");
	printf("---------------------------------------------\n");
	
	subscription_changes_data_t* req_ch_data = NULL;
		SSLOG_EXTERN list_t* req_ch_list = NULL;
		SSLOG_EXTERN list_t* rmreq_ch_list = NULL;
		
		subscription_changes_data_t* cntr_req_ch_data = NULL;
		SSLOG_EXTERN list_t* cntr_req_ch_list = NULL;
	
	while (true) {
		
		if (getchar() == 'e' )
		printf("LOG: Exiting");
				
		
		
		
		
		
		//
		//Check if server is free and set new speaker if needed
		//
		
		prop_val_t *iB_val = sslog_get_property(head,PROPERTY_ISBUSY->name );
		char *temp_isBusy = strdup((char *) iB_val->prop_value);
		if (!strcmp(headisbusy, s_false) && reqAdded){
			printf("LOG: HEAD IS FREE\n");
			printf("LOG: SEARCHING FOR NEW REQUESTS\n");
			individual_t* min_req;
			int position = 9999999;
			list_t* candidates = sslog_ss_get_individual_by_class_all(CLASS_REQUEST);
			if(!list_is_empty(candidates)){
				list_head_t* pos = NULL;
				list_for_each(pos, &candidates->links ){
					list_t* node = list_entry(pos, list_t, links);
					individual_t* temp_individual = (individual_t*)(node->data);
					
					prop_val_t* pos = sslog_get_property(temp_individual, PROPERTY_REQUESTPLACE->name);
				
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
					
				}
				
				if(min_req!= NULL){
					printf("LOG: FOUND NEW REQ\n");
					prop_val_t* new_speaker = sslog_get_property(min_req, PROPERTY_REQUESTUSERNAME->name);
					printf("LOG: GETTING NEW SPEAKERNAME\n");
					if(new_speaker!=NULL){
						char* new_speaker_name = strdup((char *) new_speaker->prop_value);
						printf("LOG: SETTING NEW SPEAKERNAME\n");
						set_property_by_name(head,PROPERTY_HEADUSERNAME->name, new_speaker_name );
						printf("LOG: SETTING NEW HEAD-> isBusy\n");
						set_property_by_name(head, PROPERTY_ISBUSY->name, s_true);
						printf("LOG: SETTING NEW ISBUSY\n");
						headisbusy = s_true;
						printf("LOG: SETTING NEW FIRST REQ\n");
						firstreq = position;
						printf("LOG: SETTING NEW HEAD uuid\n");
						headuuid = min_req->uuid; 
					}
					else{
						printf("LOG: REQ WITH EMPTY USERNAME\n");
						if(firstreq != lastreq)
						firstreq++;
						else reqAdded = false;
					}
				}
				else{
					printf("LOG: NO POSSIBLE REQ TO INSERT\n");
					reqAdded = false;
				}
			}
		}
		
		/*
		printf("LOG: Initializing req");
		individual_t *req = sslog_new_individual(CLASS_REQUEST);
		if (req == NULL) {
		printf("\nError: %s\n", get_error_text());
		return 0;
		}*/
		
		
		
		system("sleep 2");
		//sslog_ss_init_individual_with_uuid(req, "request 1");
		/*if(sslog_ss_insert_individual(req) == -1){
				printf("Error: %s\n", get_error_text());
				return 1;
			}*/
		//system("sleep 5");
		}
	

        //clean_triples_from_ss();
	
	sslog_sbcr_unsubscribe(reqsub);
	sslog_sbcr_unsubscribe(cnreqsub);
	
	sslog_ss_remove_individual(head);
	sslog_ss_remove_individual(server);

	
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

void cntrRequestsHandler(subscription_t *cnreqsub){
	
	subscription_changes_data_t* cntr_req_ch_data = NULL;
	SSLOG_EXTERN list_t* cntr_req_ch_list = NULL;
		
	cntr_req_ch_data = sslog_sbcr_get_changes_last(cnreqsub);
		cntr_req_ch_list=sslog_sbcr_ch_get_individual_by_action(cntr_req_ch_data, ACTION_INSERT) ;
		if(!list_is_empty(cntr_req_ch_list)){
			printf("LOG: Some controll requests were inserted.\n");
			sslog_sbcr_ch_print(cntr_req_ch_data);
			
			list_head_t* pos = NULL;
			list_for_each(pos, &cntr_req_ch_list->links ){
				list_t* node = list_entry(pos, list_t, links);
				char* temp_uuid= (char*)(node->data);
				individual_t* temp_individual = sslog_repo_get_individual_by_uuid(temp_uuid);
				lastreq++;
				char temp_str[50];
				sprintf(buffer, "%d", temp_str);
				set_property_by_name(temp_individual, PROPERTY_HASSTATE->name, temp_str);
				
			}
			
			list_free(cntr_req_ch_list);
		}
}

void requestsHandler(subscription_t *reqsub){
	printf("LOG: processing request handler\n");
	
	subscription_changes_data_t* req_ch_data = NULL;
	SSLOG_EXTERN list_t* req_ch_list = NULL;
	SSLOG_EXTERN list_t* rmreq_ch_list = NULL;
	
	req_ch_data = sslog_sbcr_get_changes_last(reqsub);
		sslog_sbcr_ch_print(req_ch_data);
		rmreq_ch_list = sslog_sbcr_ch_get_individual_by_action(req_ch_data, ACTION_REMOVE); 	
		if(!list_is_empty(rmreq_ch_list)){
			printf("\nLOG: Some request were removed.\n");
			sslog_sbcr_ch_print(req_ch_data);
			list_head_t* pos = NULL;
			list_for_each(pos, &rmreq_ch_list->links ){
				list_t* node = list_entry(pos, list_t, links);
				char* temp_uuid= (char*)(node->data);
				if(temp_uuid != NULL ){
					printf("	\nLOG: Processing request\n");
					if(!strcmp(temp_uuid, headuuid )){
						printf ("		LOG:User was in the head. releasing head\n");
						set_property_by_name(head,PROPERTY_ISBUSY->name, s_false );
						headuuid = "NaN";
						firstreq++;
						headisbusy = s_false;
						
					}
					
				}
				
			}
				
			//list_free(rmreq_ch_list);
		}
		
		//processing inserted requests
		req_ch_list = sslog_sbcr_ch_get_individual_by_action(req_ch_data, ACTION_INSERT);
		if(!list_is_empty(req_ch_list)){
			printf("\nLOG: Some requests were inserted.\n");
			sslog_sbcr_ch_print(req_ch_data);
			reqAdded = true;
			
			list_head_t* pos = NULL;
			list_for_each(pos, &req_ch_list->links ){
				
				list_t* node = list_entry(pos, list_t, links);
				char* temp_uuid= (char*)(node->data);
				
				if(temp_uuid != NULL ){
					individual_t* temp_req = sslog_repo_get_individual_by_uuid(temp_uuid);	
					printf("	\nLOG: Processing request\n");
					char plbuff[10] = "";
					lastreq ++;
					sprintf(plbuff, "%d", lastreq);
					set_property_by_name(temp_req,PROPERTY_REQUESTPLACE->name, plbuff );
					set_property_by_name(head, PROPERTY_LASTREQ->name, plbuff);
					printf("	\nLOG: Setting up place: %d\n", lastreq);
						
					}
					
				
				
			}
			
			//list_free(req_ch_list);
		};
}
