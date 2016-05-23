#include "MicServer.h"
#include "kp.h"

JNIEXPORT jint JNICALL Java_MicServer_connectSmartSpace
  (JNIEnv *env, jclass clazz, jstring hostname, jstring ip, jint port) {

	const char *hostname_ = (*env) -> GetStringUTFChars(env, hostname, NULL);
	const char *ip_ = (*env) -> GetStringUTFChars(env, ip, NULL);

	if(sslog_ss_init_session_with_parameters(hostname_, ip_, (int)port) != 0)
		return -1;

	register_ontology();

	if (ss_join(sslog_get_ss_info(), "MicrophoneService") == -1)
		return -1;

	return 0;
}

char* generateUuid(char *uuid) {

	int rand_val = 0, rand_length = 1, i = 0, postfix_length = 6;
	char *result = (char*) malloc (sizeof(char) * strlen(uuid) + postfix_length + 2);

	for(; i < postfix_length; rand_length *= 10, i++);

	do {
		srand(time(NULL));
		rand_val = rand() % rand_length;
		sprintf(result, "%s-%d", uuid, rand_val);
	} while(sslog_ss_exists_uuid(result) == 1);

	return result;
}

JNIEXPORT jint JNICALL Java_MicServer_publishData
  (JNIEnv *env, jclass clazz, jstring ip, jstring port) {
    
    const char *ip_ = (*env) -> GetStringUTFChars(env, ip, NULL);
    const char *port_ = (*env) -> GetStringUTFChars(env, port, NULL);
    
    individual_t *individual = sslog_new_individual(CLASS_MICROPHONESERVICE);
    if(individual == NULL)
	return -1;

    sslog_set_individual_uuid(individual, 
			      "http://www.cs.karelia.ru/smartroom#microphone-service");
    
    sslog_ss_remove_property_all(individual, PROPERTY_IP);
    sslog_ss_remove_property_all(individual, PROPERTY_PORT);
    
    if(sslog_ss_add_property(individual, PROPERTY_IP, (void *)ip_) != 0 )
	return -1;
    
    if(sslog_ss_add_property(individual, PROPERTY_PORT, (void *)port_) != 0 )
	return -1;

    if(sslog_ss_insert_individual(individual) != 0)
	return -1;

    return 0;
}

JNIEXPORT void JNICALL Java_MicServer_deletePublishedData
  (JNIEnv *env, jclass clazz) {
    
    list_t *list = sslog_ss_get_individual_by_class_all(CLASS_MICROPHONESERVICE);
    individual_t *individual;

    if(list != NULL) {
	list_head_t* pos = NULL;
	list_for_each(pos, &list->links) {
		list_t* node = list_entry(pos, list_t, links);
		individual = (individual_t*)(node->data);
		sslog_ss_populate_individual(individual);
	}
	
    } else
	return;
    
    sslog_ss_remove_property_all(individual, PROPERTY_IP);
    sslog_ss_remove_property_all(individual, PROPERTY_PORT);
}
