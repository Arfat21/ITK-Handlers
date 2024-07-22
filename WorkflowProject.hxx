#pragma once

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
#include <time.h>

#define DLLAPI _declspec(dllexport)

extern "C" {
	DLLAPI int WorkflowProject_register_callbacks();
}

int WorkflowProject_register_callbacks();
int myFunc(int* decision, va_list args);
int MyTaskLogic(EPM_action_message_t msg);
void process_primary_revision(tag_t primary_revision);
void process_secondary_revision(tag_t secondary_revision);
void process_tertiary_revision(tag_t tertiary_revision);
void log_item_info(tag_t item_revision, const char* type);
void apply_status(tag_t item_revision, const char* status_name);
void process_related_revisions(tag_t parent_revision, const char* relation_type_name, const char* level);

