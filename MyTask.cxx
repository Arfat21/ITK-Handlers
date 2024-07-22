// 
//Find the type of document rev and move it from target to reference Using Action Handler.
//#include <iostream>
//#include <tccore/custom.h>
//#include <epm/epm.h>
//#include <tc/emh.h>
//#include <epm/epm_toolkit_tc_utils.h>
//#include <tccore/aom.h>
//#include <tccore/aom_prop.h>
//#include <tccore/grm.h>
//#include <res/res_itk.h>
//#include <form/form.h>
//#include <tccore/releasestatus.h>
//#include <tccore/item.h>
//#include<bom/bom.h>
//#include <time.h>
//
//#define DLLAPI _declspec(dllexport)
//
//extern "C" {
//	DLLAPI int MyTask_register_callbacks();
//}
//
//int MyTask_register_callbacks();
//int myFunc(int* decision, va_list args);
//int MyTaskLogic(EPM_action_message_t msg);
//void move_to_reference(tag_t root_task, tag_t DocumentRevision);
//
//int MyTask_register_callbacks() {
//	TC_write_syslog("Registering callback function of MyTask\n");
//	std::cout << "Callback function of MyTask is registered \n";
//	int status = ITK_ok;
//	status = CUSTOM_register_exit("MyTask", "USER_init_module", (CUSTOM_EXIT_ftn_t)myFunc);
//	if (status != ITK_ok) {
//		char *cErrorText = nullptr;
//		EMH_ask_error_text(status, &cErrorText);
//		TC_write_syslog("Error registering callback: %s\n", cErrorText);
//		MEM_free(cErrorText);
//	}
//	else {
//		TC_write_syslog("Callback registered successfully\n");
//	}
//	return status;
//}
//
//int myFunc(int* decision, va_list args) {
//	TC_write_syslog("Executing myFunc function of MyTask\n");
//	std::cout << "myFunc function of MyTask is called \n";
//	int status = ITK_ok;
//	*decision = ALL_CUSTOMIZATIONS;
//	status = EPM_register_action_handler("MoveDocumentToReference", "Move document revision to reference", MyTaskLogic);
//	if (status != ITK_ok) {
//		char *cErrorText = nullptr;
//		EMH_ask_error_text(status, &cErrorText);
//		TC_write_syslog("Error registering action handler: %s\n", cErrorText);
//		MEM_free(cErrorText);
//	}
//	else {
//		TC_write_syslog("Action handler registered successfully\n");
//	}
//	return status;
//}
//
//int MyTaskLogic(EPM_action_message_t msg) {
//	int status = ITK_ok;
//	EPM_decision_t decision = EPM_go;
//	TC_write_syslog("Executing custom action Handler (MyTask)\n");
//
//	tag_t root_task = NULLTAG;
//	status = EPM_ask_root_task(msg.task, &root_task);
//	if (status != ITK_ok) {
//		TC_write_syslog("Error getting root task: %d\n", status);
//		return status;
//	}
//	TC_write_syslog("Root task tag: %u\n", root_task);
//
//	int count = 0;
//	tag_t* attachments = NULL;
//	status = EPM_ask_attachments(root_task, EPM_target_attachment, &count, &attachments);
//	if (status != ITK_ok) {
//		TC_write_syslog("Error getting attachments: %d\n", status);
//		return status;
//	}
//	TC_write_syslog("Number of attachments: %d\n", count);
//
//	for (int i = 0; i < count; i++) {
//		char* object_type = NULL;
//		status = WSOM_ask_object_type2(attachments[i], &object_type);
//		if (status != ITK_ok) {
//			TC_write_syslog("Error getting object type for attachment %u: %d\n", attachments[i], status);
//			continue;  // Proceed to the next attachment
//		}
//
//		TC_write_syslog("Attachment %u is of type: %s\n", attachments[i], object_type);
//
//		if (tc_strcmp(object_type, "ItemRevision") == 0) {
//			TC_write_syslog("Processing ItemRevision\n");
//			char* item_type = NULL;
//			status = ITEM_ask_rev_type2(attachments[i], &item_type);
//			if (status != ITK_ok) {
//				TC_write_syslog("Error getting item type for attachment %u: %d\n", attachments[i], status);
//				MEM_free(object_type);
//				continue;  // Proceed to the next attachment
//			}
//			TC_write_syslog("Successfully retrieved item type %s for attachment %u\n", item_type, attachments[i]);
//
//			if (tc_strcmp(item_type, "Document") == 0) {
//				TC_write_syslog("Moving Document revision %u from target to reference\n", attachments[i]);
//				move_to_reference(root_task, attachments[i]);
//				TC_write_syslog("Successfully moved Document revision %u from target to reference\n", attachments[i]);
//			}
//			else {
//				TC_write_syslog("Item type is not Document: %s\n", item_type);
//			}
//			MEM_free(item_type);
//		}
//		else if (tc_strcmp(object_type, "DocumentRevision") == 0) {
//			TC_write_syslog("Moving Document revision %u from target to reference\n", attachments[i]);
//			move_to_reference(root_task, attachments[i]);
//			TC_write_syslog("Successfully moved Document revision %u from target to reference\n", attachments[i]);
//		}
//		else {
//			TC_write_syslog("Object type is not handled: %s\n", object_type);
//		}
//		MEM_free(object_type);
//	}
//
//	MEM_free(attachments);
//	return status;
//}
//
//void move_to_reference(tag_t root_task, tag_t DocumentRevision) {
//	int status = ITK_ok;
//
//	// Remove from target
//	tag_t attachments_to_remove[1] = { DocumentRevision };
//	status = EPM_remove_attachments(root_task, 1, attachments_to_remove);
//	if (status != ITK_ok) {
//		TC_write_syslog("Error removing attachment from target: %d\n", status);
//		return;
//	}
//	TC_write_syslog("Successfully removed Document revision %u from target\n", DocumentRevision);
//
//	// Add to reference
//	tag_t attachments_to_add[1] = { DocumentRevision };
//	int attachment_types[1] = { EPM_reference_attachment };
//	status = EPM_add_attachments(root_task, 1, attachments_to_add, attachment_types);
//	if (status != ITK_ok) {
//		TC_write_syslog("Error adding attachment to reference: %d\n", status);
//		return;
//	}
//	TC_write_syslog("Successfully added Document revision %u to reference\n", DocumentRevision);
//}


//try to read argument and value from custom handler where you will be passing object type and value and based on that it should move to reference folder
 //object type Design now

#include <iostream>
#include <tccore/custom.h>
#include <epm/epm.h>
#include <tc/emh.h>
#include <epm/epm_toolkit_tc_utils.h>
#include <tccore/aom.h>
#include <tccore/aom_prop.h>
#include <tccore/grm.h>
#include <res/res_itk.h>
#include <form/form.h>
#include <tccore/releasestatus.h>
#include <tccore/item.h>
#include<bom/bom.h>
#include <time.h>

#define DLLAPI _declspec(dllexport)

extern "C" {
	DLLAPI int MyTask_register_callbacks();
}

int MyTask_register_callbacks();
int myFunc(int* decision, va_list args);
int MyTaskLogic(EPM_action_message_t msg);
void move_to_reference(tag_t root_task, tag_t DocumentRevision);

int MyTask_register_callbacks() {
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
	else {
		TC_write_syslog("Callback registered successfully\n");
	}
	return status;
}

int myFunc(int* decision, va_list args) {
	TC_write_syslog("Executing myFunc function of MyTask\n");
	std::cout << "myFunc function of MyTask is called \n";
	int status = ITK_ok;
	*decision = ALL_CUSTOMIZATIONS;
	status = EPM_register_action_handler("DynamicRefrenece", "Move document revision to reference", MyTaskLogic);
	if (status != ITK_ok) {
		char *cErrorText = nullptr;
		EMH_ask_error_text(status, &cErrorText);
		TC_write_syslog("Error registering action handler: %s\n", cErrorText);
		MEM_free(cErrorText);
	}
	else {
		TC_write_syslog("Action handler registered successfully\n");
	}
	return status;
}


// Function to move an attachment to the reference folder
void move_to_reference(tag_t root_task, tag_t DocumentRevision) {
	int status = ITK_ok;

	// Remove from target
	tag_t attachments_to_remove[1] = { DocumentRevision };
	status = EPM_remove_attachments(root_task, 1, attachments_to_remove);
	if (status != ITK_ok) {
		TC_write_syslog("Error removing attachment from target: %d\n", status);
		return;
	}
	TC_write_syslog("Successfully removed %u from target\n", DocumentRevision);

	// Add to reference
	tag_t attachments_to_add[1] = { DocumentRevision };
	int attachment_types[1] = { EPM_reference_attachment };
	status = EPM_add_attachments(root_task, 1, attachments_to_add, attachment_types);
	if (status != ITK_ok) {
		TC_write_syslog("Error adding attachment to reference: %d\n", status);
		return;
	}
	TC_write_syslog("Successfully added %u to reference\n", DocumentRevision);
}

// Main logic function for the custom task
int MyTaskLogic(EPM_action_message_t msg) {
	int status = ITK_ok;
	EPM_decision_t decision = EPM_go;
	char* object_type_to_move = NULL;

	TC_write_syslog("Executing custom action Handler (MyTask)\n");

	// Read arguments from the custom handler
	int no_of_arguments = TC_number_of_arguments(msg.arguments);
	if (no_of_arguments > 0) {
		for (int i = 0; i < no_of_arguments; i++) {
			char *cArgument_name = NULL, *cArgument_value = NULL;
			status = ITK_ask_argument_named_value(TC_next_argument(msg.arguments), &cArgument_name, &cArgument_value);
			if (status != ITK_ok) {
				TC_write_syslog("Error getting argument named value: %d\n", status);
				continue;  // Skip this argument and continue to the next
			}
			if (tc_strcmp(cArgument_name, "object_type") == 0) {
				size_t length = strlen(cArgument_value) + 1;
				object_type_to_move = (char*)malloc(length);
				if (object_type_to_move != NULL) {
					tc_strcpy(object_type_to_move, cArgument_value);
				}
				MEM_free(cArgument_name);
				MEM_free(cArgument_value);
				break;
			}
			MEM_free(cArgument_name);
			MEM_free(cArgument_value);
		}
	}

	if (object_type_to_move == NULL) {
		TC_write_syslog("No object type specified to move.\n");
		return ITK_ok;  // Nothing to do if no object type is specified
	}

	tag_t root_task = NULLTAG;
	status = EPM_ask_root_task(msg.task, &root_task);
	if (status != ITK_ok) {
		TC_write_syslog("Error getting root task: %d\n", status);
		free(object_type_to_move);
		return status;
	}
	TC_write_syslog("Root task tag: %u\n", root_task);

	int count = 0;
	tag_t* attachments = NULL;
	status = EPM_ask_attachments(root_task, EPM_target_attachment, &count, &attachments);
	if (status != ITK_ok) {
		TC_write_syslog("Error getting attachments: %d\n", status);
		free(object_type_to_move);
		return status;
	}
	TC_write_syslog("Number of attachments: %d\n", count);

	for (int i = 0; i < count; i++) {
		char* object_type = NULL;
		status = WSOM_ask_object_type2(attachments[i], &object_type);
		if (status != ITK_ok) {
			TC_write_syslog("Error getting object type for attachment %u: %d\n", attachments[i], status);
			continue;  // Proceed to the next attachment
		}

		TC_write_syslog("Attachment %u is of type: %s\n", attachments[i], object_type);

		if (tc_strcmp(object_type, object_type_to_move) == 0) {
			TC_write_syslog("Moving %s revision %u from target to reference\n", object_type, attachments[i]);
			move_to_reference(root_task, attachments[i]);
			TC_write_syslog("Successfully moved %s revision %u from target to reference\n", object_type, attachments[i]);
		}
		else {
			TC_write_syslog("Object type %s is not the specified type %s\n", object_type, object_type_to_move);
		}

		MEM_free(object_type);
	}

	free(object_type_to_move);
	MEM_free(attachments);
	return status;
}