#include "WorkflowProject.hxx"

int WorkflowProject_register_callbacks() {
	TC_write_syslog("Registering callback function of MyTask\n");
	std::cout << "Callback function of MyTask is registered \n";
	int status = ITK_ok;
	status = CUSTOM_register_exit("MyTask", "USER_init_module", (CUSTOM_EXIT_ftn_t)myFunc);
	if (status != ITK_ok) {
		char *cErrorText = nullptr;
		EMH_ask_error_text(status, &cErrorText);
		TC_write_syslog("Error registering callback: %s\n", cErrorText);
		MEM_free(cErrorText);
	}
	return status;
}

int myFunc(int* decision, va_list args) {
	TC_write_syslog("Executing myFunc function of MyTask\n");
	std::cout << "myFunc function of MyTask is called \n";
	int status = ITK_ok;
	*decision = ALL_CUSTOMIZATIONS;
	status = EPM_register_action_handler("ArfatTask1", "Set property of master form", MyTaskLogic);
	if (status != ITK_ok) {
		char *cErrorText = nullptr;
		EMH_ask_error_text(status, &cErrorText);
		TC_write_syslog("Error registering action handler: %s\n", cErrorText);
		MEM_free(cErrorText);
	}
	return status;
}

int MyTaskLogic(EPM_action_message_t msg) {
	int status = ITK_ok;
	EPM_decision_t decision = EPM_go;
	TC_write_syslog("Executing custom action Handler (MyTask)\n");

	tag_t root_task = NULLTAG;
	status = EPM_ask_root_task(msg.task, &root_task);
	if (status != ITK_ok) {
		TC_write_syslog("Error getting root task: %d\n", status);
		return status;
	}
	TC_write_syslog("Root task tag: %u\n", root_task);

	int count = 0;
	tag_t* attachments = NULL;
	status = EPM_ask_attachments(root_task, EPM_target_attachment, &count, &attachments);
	if (status != ITK_ok) {
		TC_write_syslog("Error getting attachments: %d\n", status);
		return status;
	}
	TC_write_syslog("Number of attachments: %d\n", count);

	for (int i = 0; i < count; i++) {
		char* object_type = NULL;
		WSOM_ask_object_type2(attachments[i], &object_type);
		if (tc_strcmp(object_type, "ItemRevision") == 0) {
			TC_write_syslog("Processing ItemRevision %u\n", attachments[i]);
			process_primary_revision(attachments[i]);
		}
		MEM_free(object_type);
	}

	MEM_free(attachments);
	return status;
}

void process_primary_revision(tag_t primary_revision) {
	int status = ITK_ok;

	// Retrieve and log primary item information
	tag_t primary_item;
	status = ITEM_ask_item_of_rev(primary_revision, &primary_item);
	if (status != ITK_ok) {
		TC_write_syslog("Error getting item of primary revision: %d\n", status);
		return;
	}
	log_item_info(primary_item, "Primary");

	// Apply "Approved" status to primary item revision
	apply_status(primary_revision, "Approved");

	// Process secondary revisions attached to the primary revision
	process_related_revisions(primary_revision, "IMAN_specification", "Secondary");
}

void process_secondary_revision(tag_t secondary_revision) {
	int status = ITK_ok;

	// Retrieve and log secondary item information
	tag_t secondary_item;
	status = ITEM_ask_item_of_rev(secondary_revision, &secondary_item);
	if (status != ITK_ok) {
		TC_write_syslog("Error getting item of secondary revision: %d\n", status);
		return;
	}
	log_item_info(secondary_item, "Secondary");

	// Apply "Validation" status to the secondary revision
	apply_status(secondary_revision, "Validation");

	// Process tertiary revisions attached to the secondary revision
	process_related_revisions(secondary_revision, "IMAN_specification", "Tertiary");
}

void process_tertiary_revision(tag_t tertiary_revision) {
	int status = ITK_ok;

	// Retrieve and log tertiary item information
	tag_t tertiary_item;
	status = ITEM_ask_item_of_rev(tertiary_revision, &tertiary_item);
	if (status != ITK_ok) {
		TC_write_syslog("Error getting item of tertiary revision: %d\n", status);
		return;
	}
	log_item_info(tertiary_item, "Tertiary");

	// Apply "Approved" status to tertiary revisions
	apply_status(tertiary_revision, "Approved");
}

void process_related_revisions(tag_t parent_revision, const char* relation_type_name, const char* level) {
	int status = ITK_ok;
	int count = 0;
	tag_t* related_revisions = NULL;
	tag_t relation_type = NULLTAG;

	// Find the relation type by its name
	status = GRM_find_relation_type("IMAN_specification", &relation_type);
	if (status != ITK_ok) {
		TC_write_syslog("Error finding relation type '%s': %d\n", relation_type_name, status);
		return;
	}

	// List secondary objects using the identified relation type
	status = GRM_list_secondary_objects_only(parent_revision, relation_type, &count, &related_revisions);
	if (status != ITK_ok) {
		TC_write_syslog("Error getting %s revisions: %d\n", level, status);
		return;
	}
	TC_write_syslog("Number of %s revisions: %d\n", level, count);

	for (int i = 0; i < count; i++) {
		if (tc_strcmp(level, "Secondary") == 0) {
			process_secondary_revision(related_revisions[i]);
		}
		else if (tc_strcmp(level, "Tertiary") == 0) {
			process_tertiary_revision(related_revisions[i]);
		}
	}

	MEM_free(related_revisions);
}

void log_item_info(tag_t item, const char* level) {
	char* item_id = NULL;
	char* item_name = NULL;

	ITEM_ask_id2(item, &item_id);
	ITEM_ask_name2(item, &item_name);

	TC_write_syslog("%s Item ID: %s, Item Name: %s\n", level, item_id, item_name);

	MEM_free(item_id);
	MEM_free(item_name);
}

void apply_status(tag_t item_revision, const char* status_name) {
	int status = ITK_ok;

	// Lock the item revision
	status = AOM_lock(item_revision);
	if (status != ITK_ok) {
		TC_write_syslog("Error locking item revision: %d\n", status);
		return;
	}

	// Create the release status
	tag_t release_status = NULLTAG;
	status = RELSTAT_create_release_status(status_name, &release_status);
	if (status != ITK_ok) {
		TC_write_syslog("Error creating release status '%s': %d\n", status_name, status);
		AOM_unlock(item_revision);
		return;
	}

	// Apply the release status to the item revision
	status = RELSTAT_add_release_status(release_status, 1, &item_revision, FALSE);
	if (status == ITK_ok) {
		TC_write_syslog("Successfully applied status '%s' to item revision\n", status_name);
	}
	else {
		TC_write_syslog("Failed to apply status '%s' to item revision: %d\n", status_name, status);
	}

	// Unlock the item revision
	AOM_unlock(item_revision);
}




