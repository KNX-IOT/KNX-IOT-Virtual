/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 Copyright (c) 2022-2023 Cascoda Ltd
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
*/

/**
 * @file
 * 
 * KNX virtual Push Button
 
 * ## Application Design
 *
 * support functions:
 *
 * - app_init
 *   initializes the stack values.
 * - register_resources
 *   function that registers all endpoints,
 *   e.g. sets the GET/PUT/POST/DELETE
 *      handlers for each end point
 *
 * - main
 *   starts the stack, with the registered resources.
 *   can be compiled out with NO_MAIN
 *
 *  handlers for the implemented methods (get/put):
 *   - get_[path]
 *     function that is being called when a GET is called on [path]
 *     set the global variables in the output
 *   - put_[path]
 *     function that is being called when a PUT is called on [path]
 *     if input data is of the correct type
 *       updates the global variables
 *
 * ## stack specific defines
 * - __linux__
 *   build for Linux
 * - WIN32
 *   build for Windows
 * - OC_OSCORE
 *   oscore is enabled as compile flag
 * ## File specific defines
 * - NO_MAIN
 *   compile out the function main()
 * - INCLUDE_EXTERNAL
 *   includes header file "external_header.h", so that other tools/dependencies
 *   can be included without changing this code
 * - KNX_GUI
 *   build the GUI with console option, so that all 
 *   logging can be seen in the command window
 */
#include "oc_api.h"
#include "oc_core_res.h"
#include "oc_rep.h"
#include "oc_helpers.h"
#include "api/oc_knx_fp.h"
#include "port/oc_clock.h"
#include <signal.h>
/* test purpose only; commandline reset */
#include "api/oc_knx_dev.h"
#ifdef OC_SPAKE
#include "security/oc_spake2plus.h"
#endif
#ifdef INCLUDE_EXTERNAL
/* import external definitions from header file*/
/* this file should be externally supplied */
#include "external_header.h"
#endif
#include "knx_iot_virtual_pb.h"

#include <stdlib.h>
#include <ctype.h>

#ifdef __linux__
/** linux specific code */
#include <pthread.h>
#ifndef NO_MAIN
static pthread_mutex_t mutex;
static pthread_cond_t cv;
static struct timespec ts;
#endif /* NO_MAIN */
#endif

#include <stdio.h> /* defines FILENAME_MAX */

#define MY_NAME "KNX virtual Push Button" /**< The name of the application */
#define APP_MAX_STRING 30

#ifdef WIN32
/** windows specific code */
#include <windows.h>
static CONDITION_VARIABLE cv; /**< event loop variable */
static CRITICAL_SECTION cs;   /**< event loop variable */
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

#define btoa(x) ((x) ? "true" : "false")
volatile int quit = 0;  /**< stop variable, used by handle_signal */
bool g_reset = false;   /**< reset variable, set by commandline arguments */
char g_serial_number[20] = "00FA10010400";





volatile bool g_OnOff_1;   /**< global variable for OnOff_1 */
volatile bool g_InfoOnOff_1;   /**< global variable for InfoOnOff_1 */
volatile bool g_OnOff_2;   /**< global variable for OnOff_2 */
volatile bool g_InfoOnOff_2;   /**< global variable for InfoOnOff_2 */
volatile bool g_OnOff_3;   /**< global variable for OnOff_3 */
volatile bool g_InfoOnOff_3;   /**< global variable for InfoOnOff_3 */
volatile bool g_OnOff_4;   /**< global variable for OnOff_4 */
volatile bool g_InfoOnOff_4;   /**< global variable for InfoOnOff_4 */
 
volatile bool g_fault_InfoOnOff_1;   /**< global variable for fault InfoOnOff_1 */ 
volatile bool g_fault_InfoOnOff_2;   /**< global variable for fault InfoOnOff_2 */ 
volatile bool g_fault_InfoOnOff_3;   /**< global variable for fault InfoOnOff_3 */ 
volatile bool g_fault_InfoOnOff_4;   /**< global variable for fault InfoOnOff_4 */




// BOOLEAN code

/**
 * @brief function to check if the url is represented by a boolean
 *
 * @param true = url value is a boolean
 * @param false = url is not a boolean
 */
bool app_is_bool_url(char* url)
{
  if ( strcmp(url, URL_ONOFF_1) == 0) { 
    return true; /**< OnOff_1 is a boolean */
  }
  if ( strcmp(url, URL_INFOONOFF_1) == 0) { 
    return true; /**< InfoOnOff_1 is a boolean */
  }
  if ( strcmp(url, URL_ONOFF_2) == 0) { 
    return true; /**< OnOff_2 is a boolean */
  }
  if ( strcmp(url, URL_INFOONOFF_2) == 0) { 
    return true; /**< InfoOnOff_2 is a boolean */
  }
  if ( strcmp(url, URL_ONOFF_3) == 0) { 
    return true; /**< OnOff_3 is a boolean */
  }
  if ( strcmp(url, URL_INFOONOFF_3) == 0) { 
    return true; /**< InfoOnOff_3 is a boolean */
  }
  if ( strcmp(url, URL_ONOFF_4) == 0) { 
    return true; /**< OnOff_4 is a boolean */
  }
  if ( strcmp(url, URL_INFOONOFF_4) == 0) { 
    return true; /**< InfoOnOff_4 is a boolean */
  }
  return false;
}

/**
 * @brief sets the global boolean variable at the url
 *
 * @param url the url indicating the global variable
 * @param value the value to be set
 */
void app_set_bool_variable(char* url, bool value) 
{
  if ( strcmp(url, URL_ONOFF_1) == 0) { 
    g_OnOff_1 = value; /**< global variable for OnOff_1 */
    return;
  } 
  if ( strcmp(url, URL_INFOONOFF_1) == 0) { 
    g_InfoOnOff_1 = value; /**< global variable for InfoOnOff_1 */
    return;
  } 
  if ( strcmp(url, URL_ONOFF_2) == 0) { 
    g_OnOff_2 = value; /**< global variable for OnOff_2 */
    return;
  } 
  if ( strcmp(url, URL_INFOONOFF_2) == 0) { 
    g_InfoOnOff_2 = value; /**< global variable for InfoOnOff_2 */
    return;
  } 
  if ( strcmp(url, URL_ONOFF_3) == 0) { 
    g_OnOff_3 = value; /**< global variable for OnOff_3 */
    return;
  } 
  if ( strcmp(url, URL_INFOONOFF_3) == 0) { 
    g_InfoOnOff_3 = value; /**< global variable for InfoOnOff_3 */
    return;
  } 
  if ( strcmp(url, URL_ONOFF_4) == 0) { 
    g_OnOff_4 = value; /**< global variable for OnOff_4 */
    return;
  } 
  if ( strcmp(url, URL_INFOONOFF_4) == 0) { 
    g_InfoOnOff_4 = value; /**< global variable for InfoOnOff_4 */
    return;
  } 
}

/**
 * @brief retrieve the global boolean variable at the url
 *
 * @param url the url indicating the global variable
 * @return the value of the variable
 */
bool app_retrieve_bool_variable(char* url) 
{
  if ( strcmp(url, URL_ONOFF_1) == 0) { 
    return g_OnOff_1; /**< global variable for OnOff_1 */
  } 
  if ( strcmp(url, URL_INFOONOFF_1) == 0) { 
    return g_InfoOnOff_1; /**< global variable for InfoOnOff_1 */
  } 
  if ( strcmp(url, URL_ONOFF_2) == 0) { 
    return g_OnOff_2; /**< global variable for OnOff_2 */
  } 
  if ( strcmp(url, URL_INFOONOFF_2) == 0) { 
    return g_InfoOnOff_2; /**< global variable for InfoOnOff_2 */
  } 
  if ( strcmp(url, URL_ONOFF_3) == 0) { 
    return g_OnOff_3; /**< global variable for OnOff_3 */
  } 
  if ( strcmp(url, URL_INFOONOFF_3) == 0) { 
    return g_InfoOnOff_3; /**< global variable for InfoOnOff_3 */
  } 
  if ( strcmp(url, URL_ONOFF_4) == 0) { 
    return g_OnOff_4; /**< global variable for OnOff_4 */
  } 
  if ( strcmp(url, URL_INFOONOFF_4) == 0) { 
    return g_InfoOnOff_4; /**< global variable for InfoOnOff_4 */
  } 
}

// FAULT code

/**
 * @brief set the fault (boolean) variable at the url
 *
 * @param url the url indicating the fault variable
 * @param value the value of the fault variable
 */
void app_set_fault_variable(char* url, bool value)
{
}

/**
 * @brief retrieve the fault (boolean) variable at the url
 *
 * @param url the url indicating the fault variable
 * @return the value of the fault variable
 */
bool app_retrieve_fault_variable(char* url)
{ 
  if ( strcmp(url, URL_INFOONOFF_1) == 0) { 
    return g_fault_InfoOnOff_1;   /**< global variable for InfoOnOff_1 */
  } 
  if ( strcmp(url, URL_INFOONOFF_2) == 0) { 
    return g_fault_InfoOnOff_2;   /**< global variable for InfoOnOff_2 */
  } 
  if ( strcmp(url, URL_INFOONOFF_3) == 0) { 
    return g_fault_InfoOnOff_3;   /**< global variable for InfoOnOff_3 */
  } 
  if ( strcmp(url, URL_INFOONOFF_4) == 0) { 
    return g_fault_InfoOnOff_4;   /**< global variable for InfoOnOff_4 */
  }
  return false;
}

// PARAMETER code

bool app_is_url_parameter(char* url)
{
  return false;
}

char* app_get_parameter_url(int index)
{
  return NULL;
}

char* app_get_parameter_name(int index)
{
  return NULL;
}

bool app_is_secure()
{
#ifdef OC_OSCORE
  return true;
#else
  return false;
#endif /* OC_OSCORE */
}

static oc_put_struct_t app_put = { NULL };

void
app_set_put_cb(oc_put_cb_t cb)
{
  app_put.cb = cb;
}

oc_put_struct_t *
oc_get_put_cb(void)
{
  return &app_put;
}

void do_put_cb(char* url) 
{
  oc_put_struct_t *my_cb = oc_get_put_cb();
  if (my_cb && my_cb->cb) {
    my_cb->cb(url);
  }
}

#ifdef __cplusplus
extern "C" {
#endif
int app_initialize_stack();
void signal_event_loop(void);
void register_resources(void);
int app_init(void);
#ifdef __cplusplus
}
#endif

// DEVBOARD code

/**
 * @brief devboard button toggle callback
 *
 */
// we no longer have app_set_bool_variable
//void dev_btn_toggle_cb(char *url)
//{
//  PRINT_APP("Handling %s\n", url);
//  bool val = app_retrieve_bool_variable(url);
//  if (val == true)
//  {
//    val = false;
//  }
//  else
//  {
//    val = true;
//  }
//  app_set_bool_variable(url, val);
//  oc_do_s_mode_with_scope(5, url, "w");
//}

/**
 * @brief s-mode response callback
 * will be called when a response is received on an s-mode read request
 *
 * @param url the url
 * @param rep the full response
 * @param rep_value the parsed value of the response
 */
void
oc_add_s_mode_response_cb(char *url, oc_rep_t *rep, oc_rep_t *rep_value)
{
  (void)rep;
  (void)rep_value;

  PRINT("oc_add_s_mode_response_cb %s\n", url);
}



/**
 * @brief function to set the input string to upper case
 *
 * @param str the string to make upper case
 *
 */
void app_str_to_upper(char *str){
    while (*str != '\0')
    {
        *str = toupper(*str);
        str++;
    }
}

/**
 * @brief function to set up the device.
 *
 * sets the:
 * - manufacturer     : cascoda
 * - serial number    : 00FA10010400
 * - base path
 * - knx spec version 
 * - hardware version : [0, 4, 0]
 * - firmware version : [0, 4, 0]
 * - hardware type    : 000000000001
 * - device model     : KNX virtual - PB
 *
 */
int
app_init(void)
{
  int ret = oc_init_platform("cascoda", NULL, NULL);
  char serial_number_uppercase[20];

  /* set the application name, version, base url, device serial number */
  ret |= oc_add_device(MY_NAME, "1.0.0", "//", g_serial_number, NULL, NULL);

  oc_device_info_t *device = oc_core_get_device_info(0);

  
  /* set the hardware version 0.4.0 */
  oc_core_set_device_hwv(0, 0, 4, 0);
  
  
  /* set the firmware version 0.4.0 */
  oc_core_set_device_fwv(0, 0, 4, 0);
  

  /* set the hardware type*/
  //                         123456789012
  oc_core_set_device_hwt(0, "000000000001");

  /* set the model */
  oc_core_set_device_model(0, "KNX virtual - PB");

  oc_set_s_mode_response_cb(oc_add_s_mode_response_cb);
#define PASSWORD "ABY8B77J50YXMUDW3DG4"
#ifdef OC_SPAKE
  if (strlen(oc_spake_get_password()) == 0)
    oc_spake_set_password(PASSWORD);


  strncpy(serial_number_uppercase, oc_string(device->serialnumber), 19);
  app_str_to_upper(serial_number_uppercase);
  printf("\n === QR Code: KNX:S:%s;P:%s ===\n", serial_number_uppercase, oc_spake_get_password());
#endif

  return ret;
}

/**
 * @brief returns the password
 */
char* app_get_password()
{
  return PASSWORD;
}

// data point (objects) handling


/**
 * @brief CoAP GET method for data point "OnOff_1" resource at url URL_ONOFF_1 ("/p/o_1_1").
 * resource types: ['urn:knx:dpa.421.61']
 * function is called to initialize the return values of the GET method.
 * initialization of the returned values are done from the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the interface used for this call
 * @param user_data the user data.
 */
void
get_OnOff_1(oc_request_t *request, oc_interface_mask_t interfaces,
               void *user_data)
{
  (void)user_data; /* variable not used */

  /* MANUFACTORER: SENSOR add here the code to talk to the HW if one implements a
     sensor. the call to the HW needs to fill in the global variable before it
     returns to this function here. alternative is to have a callback from the
     hardware that sets the global variables.
  */
  bool error_state = false; /* the error state, the generated code */

  PRINT("-- Begin get_OnOff_1 %s \n", URL_ONOFF_1);
  /* check if the accept header is CBOR */
  if (oc_check_accept_header(request, APPLICATION_CBOR) == false) {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
    return;
  }

  // check the query parameter m with the various values
  char *m;
  char* m_key;
  size_t m_key_len;
  size_t m_len = (int)oc_get_query_value(request, "m", &m);
  if (m_len != -1) {
    PRINT("  Query param: %.*s",(int)m_len, m);
    oc_init_query_iterator();
    size_t device_index = request->resource->device;
    oc_device_info_t *device = oc_core_get_device_info(device_index);
    if (device != NULL) {
      oc_rep_begin_root_object();
      while (oc_iterate_query(request, &m_key, &m_key_len, &m, &m_len) != -1) {
        // unique identifier
        if ((strncmp(m, "id", m_len) == 0) |
            (strncmp(m, "*", m_len) == 0) ) {
          char mystring[100];
          snprintf(mystring,99,"urn:knx:sn:%s%s",oc_string(device->serialnumber),
           oc_string(request->resource->uri));
          oc_rep_i_set_text_string(root, 9, mystring);
        }
        // resource types
        if ((strncmp(m, "rt", m_len) == 0) |
            (strncmp(m, "*", m_len) == 0) ) {
          oc_rep_set_text_string(root, rt, "urn:knx:dpa.421.61");
        }
        // interfaces
        if ((strncmp(m, "if", m_len) == 0) |
            (strncmp(m, "*", m_len) == 0) ) {
          oc_rep_set_text_string(root, if, "if.s");
        }
        if ((strncmp(m, "dpt", m_len) == 0) |
            (strncmp(m, "*", m_len) == 0) ) {
          oc_rep_set_text_string(root, dpt, oc_string(request->resource->dpt));
        }
        // ga
        if ((strncmp(m, "ga", m_len) == 0) |
            (strncmp(m, "*", m_len) == 0) ) {
          int index = oc_core_find_group_object_table_url(oc_string(request->resource->uri));
          if (index > -1) {
             oc_group_object_table_t* got_table_entry = oc_core_get_group_object_table_entry(index);
             if (got_table_entry) {
               oc_rep_set_int_array(root, ga, got_table_entry->ga, got_table_entry->ga_len);
             }
          }
        }
        if ((strncmp(m, "desc", m_len) == 0) |
            (strncmp(m, "*", m_len) == 0) ) {
          oc_rep_set_text_string(root, desc, "On/Off push button 1");
        }
      } /* query iterator */
      oc_rep_end_root_object();
    } else {
      /* device is NULL */
      oc_send_cbor_response(request, OC_STATUS_BAD_OPTION);
    }
    oc_send_cbor_response(request, OC_STATUS_OK);
    return;
  }
  oc_rep_begin_root_object();
  oc_rep_i_set_boolean(root, 1, g_OnOff_1);
  oc_rep_end_root_object(); 
  
  if (g_err) {
    error_state = true;
  }
  PRINT("CBOR encoder size %d\n", oc_rep_get_encoded_payload_size());
  if (error_state == false) {
    oc_send_cbor_response(request, OC_STATUS_OK);
  } else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_OnOff_1\n");
}



/**
 * @brief CoAP GET method for data point "InfoOnOff_1" resource at url URL_INFOONOFF_1 ("/p/o_2_2").
 * resource types: ['urn:knx:dpa.421.51']
 * function is called to initialize the return values of the GET method.
 * initialization of the returned values are done from the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the interface used for this call
 * @param user_data the user data.
 */
void
get_InfoOnOff_1(oc_request_t *request, oc_interface_mask_t interfaces,
               void *user_data)
{
  (void)user_data; /* variable not used */

  /* MANUFACTORER: SENSOR add here the code to talk to the HW if one implements a
     sensor. the call to the HW needs to fill in the global variable before it
     returns to this function here. alternative is to have a callback from the
     hardware that sets the global variables.
  */
  bool error_state = false; /* the error state, the generated code */

  PRINT("-- Begin get_InfoOnOff_1 %s \n", URL_INFOONOFF_1);
  /* check if the accept header is CBOR */
  if (oc_check_accept_header(request, APPLICATION_CBOR) == false) {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
    return;
  }

  // check the query parameter m with the various values
  char *m;
  char* m_key;
  size_t m_key_len;
  size_t m_len = (int)oc_get_query_value(request, "m", &m);
  if (m_len != -1) {
    PRINT("  Query param: %.*s",(int)m_len, m);
    oc_init_query_iterator();
    size_t device_index = request->resource->device;
    oc_device_info_t *device = oc_core_get_device_info(device_index);
    if (device != NULL) {
      oc_rep_begin_root_object();
      while (oc_iterate_query(request, &m_key, &m_key_len, &m, &m_len) != -1) {
        // unique identifier
        if ((strncmp(m, "id", m_len) == 0) |
            (strncmp(m, "*", m_len) == 0) ) {
          char mystring[100];
          snprintf(mystring,99,"urn:knx:sn:%s%s",oc_string(device->serialnumber),
           oc_string(request->resource->uri));
          oc_rep_i_set_text_string(root, 9, mystring);
        }
        // resource types
        if ((strncmp(m, "rt", m_len) == 0) |
            (strncmp(m, "*", m_len) == 0) ) {
          oc_rep_set_text_string(root, rt, "urn:knx:dpa.421.51");
        }
        // interfaces
        if ((strncmp(m, "if", m_len) == 0) |
            (strncmp(m, "*", m_len) == 0) ) {
          oc_rep_set_text_string(root, if, "if.a");
        }
        if ((strncmp(m, "dpt", m_len) == 0) |
            (strncmp(m, "*", m_len) == 0) ) {
          oc_rep_set_text_string(root, dpt, oc_string(request->resource->dpt));
        }
        // ga
        if ((strncmp(m, "ga", m_len) == 0) |
            (strncmp(m, "*", m_len) == 0) ) {
          int index = oc_core_find_group_object_table_url(oc_string(request->resource->uri));
          if (index > -1) {
             oc_group_object_table_t* got_table_entry = oc_core_get_group_object_table_entry(index);
             if (got_table_entry) {
               oc_rep_set_int_array(root, ga, got_table_entry->ga, got_table_entry->ga_len);
             }
          }
        }
        if ((strncmp(m, "desc", m_len) == 0) |
            (strncmp(m, "*", m_len) == 0) ) {
          oc_rep_set_text_string(root, desc, "Feedback 1");
        }
      } /* query iterator */
      oc_rep_end_root_object();
    } else {
      /* device is NULL */
      oc_send_cbor_response(request, OC_STATUS_BAD_OPTION);
    }
    oc_send_cbor_response(request, OC_STATUS_OK);
    return;
  }
  oc_rep_begin_root_object();
  oc_rep_i_set_boolean(root, 1, g_InfoOnOff_1);
  oc_rep_end_root_object(); 
  
  if (g_err) {
    error_state = true;
  }
  PRINT("CBOR encoder size %d\n", oc_rep_get_encoded_payload_size());
  if (error_state == false) {
    oc_send_cbor_response(request, OC_STATUS_OK);
  } else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_InfoOnOff_1\n");
}

 
/**
 * @brief CoAP PUT method for data point "InfoOnOff_1" resource at url "/p/o_2_2".
 * resource types: ['urn:knx:dpa.421.51']
 * The function has as input the request body, which are the input values of the
 * PUT method.
 * The input values (as a set) are checked if all supplied values are correct.
 * If the input values are correct, they will be assigned to the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the used interfaces during the request.
 * @param user_data the supplied user data.
 */
void
put_InfoOnOff_1(oc_request_t *request, oc_interface_mask_t interfaces,
                void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = true;
  PRINT("-- Begin put_InfoOnOff_1:\n");

  oc_rep_t *rep = NULL;
  /* handle the different requests e.g. via s-mode or normal CoAP call*/
  if (oc_is_redirected_request(request)) {
    PRINT("  redirected request..\n");
  }
  rep = request->request_payload;
  /* loop over all the entries in the request */
  /* handle the type of payload correctly. */
  error_state = true;
  while (rep != NULL) {
    /* handle the type of payload correctly. */
    if ((rep->iname == 1) && (rep->type == OC_REP_BOOL)) {
      PRINT("  put_InfoOnOff_1 received : %d\n", rep->value.boolean);
      g_InfoOnOff_1 = rep->value.boolean;
      error_state = false;
      break;
    }
  }

  if (error_state == false){
      oc_send_cbor_response(request, OC_STATUS_CHANGED); 
      do_put_cb(URL_INFOONOFF_1);
      PRINT("-- End put_InfoOnOff_1\n");
      return;
  }
  /* request data was not recognized, so it was a bad request */
  oc_send_response(request, OC_STATUS_BAD_REQUEST);
  PRINT("-- End put_InfoOnOff_1\n");
}

/**
 * @brief CoAP GET method for data point "OnOff_2" resource at url URL_ONOFF_2 ("/p/o_3_3").
 * resource types: ['urn:knx:dpa.421.61']
 * function is called to initialize the return values of the GET method.
 * initialization of the returned values are done from the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the interface used for this call
 * @param user_data the user data.
 */
void
get_OnOff_2(oc_request_t *request, oc_interface_mask_t interfaces,
               void *user_data)
{
  (void)user_data; /* variable not used */

  /* MANUFACTORER: SENSOR add here the code to talk to the HW if one implements a
     sensor. the call to the HW needs to fill in the global variable before it
     returns to this function here. alternative is to have a callback from the
     hardware that sets the global variables.
  */
  bool error_state = false; /* the error state, the generated code */

  PRINT("-- Begin get_OnOff_2 %s \n", URL_ONOFF_2);
  /* check if the accept header is CBOR */
  if (oc_check_accept_header(request, APPLICATION_CBOR) == false) {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
    return;
  }

  // check the query parameter m with the various values
  char *m;
  char* m_key;
  size_t m_key_len;
  size_t m_len = (int)oc_get_query_value(request, "m", &m);
  if (m_len != -1) {
    PRINT("  Query param: %.*s",(int)m_len, m);
    oc_init_query_iterator();
    size_t device_index = request->resource->device;
    oc_device_info_t *device = oc_core_get_device_info(device_index);
    if (device != NULL) {
      oc_rep_begin_root_object();
      while (oc_iterate_query(request, &m_key, &m_key_len, &m, &m_len) != -1) {
        // unique identifier
        if ((strncmp(m, "id", m_len) == 0) |
            (strncmp(m, "*", m_len) == 0) ) {
          char mystring[100];
          snprintf(mystring,99,"urn:knx:sn:%s%s",oc_string(device->serialnumber),
           oc_string(request->resource->uri));
          oc_rep_i_set_text_string(root, 9, mystring);
        }
        // resource types
        if ((strncmp(m, "rt", m_len) == 0) |
            (strncmp(m, "*", m_len) == 0) ) {
          oc_rep_set_text_string(root, rt, "urn:knx:dpa.421.61");
        }
        // interfaces
        if ((strncmp(m, "if", m_len) == 0) |
            (strncmp(m, "*", m_len) == 0) ) {
          oc_rep_set_text_string(root, if, "if.s");
        }
        if ((strncmp(m, "dpt", m_len) == 0) |
            (strncmp(m, "*", m_len) == 0) ) {
          oc_rep_set_text_string(root, dpt, oc_string(request->resource->dpt));
        }
        // ga
        if ((strncmp(m, "ga", m_len) == 0) |
            (strncmp(m, "*", m_len) == 0) ) {
          int index = oc_core_find_group_object_table_url(oc_string(request->resource->uri));
          if (index > -1) {
             oc_group_object_table_t* got_table_entry = oc_core_get_group_object_table_entry(index);
             if (got_table_entry) {
               oc_rep_set_int_array(root, ga, got_table_entry->ga, got_table_entry->ga_len);
             }
          }
        }
        if ((strncmp(m, "desc", m_len) == 0) |
            (strncmp(m, "*", m_len) == 0) ) {
          oc_rep_set_text_string(root, desc, "On/Off push button 2");
        }
      } /* query iterator */
      oc_rep_end_root_object();
    } else {
      /* device is NULL */
      oc_send_cbor_response(request, OC_STATUS_BAD_OPTION);
    }
    oc_send_cbor_response(request, OC_STATUS_OK);
    return;
  }
  oc_rep_begin_root_object();
  oc_rep_i_set_boolean(root, 1, g_OnOff_2);
  oc_rep_end_root_object(); 
  
  if (g_err) {
    error_state = true;
  }
  PRINT("CBOR encoder size %d\n", oc_rep_get_encoded_payload_size());
  if (error_state == false) {
    oc_send_cbor_response(request, OC_STATUS_OK);
  } else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_OnOff_2\n");
}



/**
 * @brief CoAP GET method for data point "InfoOnOff_2" resource at url URL_INFOONOFF_2 ("/p/o_4_4").
 * resource types: ['urn:knx:dpa.421.51']
 * function is called to initialize the return values of the GET method.
 * initialization of the returned values are done from the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the interface used for this call
 * @param user_data the user data.
 */
void
get_InfoOnOff_2(oc_request_t *request, oc_interface_mask_t interfaces,
               void *user_data)
{
  (void)user_data; /* variable not used */

  /* MANUFACTORER: SENSOR add here the code to talk to the HW if one implements a
     sensor. the call to the HW needs to fill in the global variable before it
     returns to this function here. alternative is to have a callback from the
     hardware that sets the global variables.
  */
  bool error_state = false; /* the error state, the generated code */

  PRINT("-- Begin get_InfoOnOff_2 %s \n", URL_INFOONOFF_2);
  /* check if the accept header is CBOR */
  if (oc_check_accept_header(request, APPLICATION_CBOR) == false) {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
    return;
  }

  // check the query parameter m with the various values
  char *m;
  char* m_key;
  size_t m_key_len;
  size_t m_len = (int)oc_get_query_value(request, "m", &m);
  if (m_len != -1) {
    PRINT("  Query param: %.*s",(int)m_len, m);
    oc_init_query_iterator();
    size_t device_index = request->resource->device;
    oc_device_info_t *device = oc_core_get_device_info(device_index);
    if (device != NULL) {
      oc_rep_begin_root_object();
      while (oc_iterate_query(request, &m_key, &m_key_len, &m, &m_len) != -1) {
        // unique identifier
        if ((strncmp(m, "id", m_len) == 0) |
            (strncmp(m, "*", m_len) == 0) ) {
          char mystring[100];
          snprintf(mystring,99,"urn:knx:sn:%s%s",oc_string(device->serialnumber),
           oc_string(request->resource->uri));
          oc_rep_i_set_text_string(root, 9, mystring);
        }
        // resource types
        if ((strncmp(m, "rt", m_len) == 0) |
            (strncmp(m, "*", m_len) == 0) ) {
          oc_rep_set_text_string(root, rt, "urn:knx:dpa.421.51");
        }
        // interfaces
        if ((strncmp(m, "if", m_len) == 0) |
            (strncmp(m, "*", m_len) == 0) ) {
          oc_rep_set_text_string(root, if, "if.a");
        }
        if ((strncmp(m, "dpt", m_len) == 0) |
            (strncmp(m, "*", m_len) == 0) ) {
          oc_rep_set_text_string(root, dpt, oc_string(request->resource->dpt));
        }
        // ga
        if ((strncmp(m, "ga", m_len) == 0) |
            (strncmp(m, "*", m_len) == 0) ) {
          int index = oc_core_find_group_object_table_url(oc_string(request->resource->uri));
          if (index > -1) {
             oc_group_object_table_t* got_table_entry = oc_core_get_group_object_table_entry(index);
             if (got_table_entry) {
               oc_rep_set_int_array(root, ga, got_table_entry->ga, got_table_entry->ga_len);
             }
          }
        }
        if ((strncmp(m, "desc", m_len) == 0) |
            (strncmp(m, "*", m_len) == 0) ) {
          oc_rep_set_text_string(root, desc, "Feedback 2");
        }
      } /* query iterator */
      oc_rep_end_root_object();
    } else {
      /* device is NULL */
      oc_send_cbor_response(request, OC_STATUS_BAD_OPTION);
    }
    oc_send_cbor_response(request, OC_STATUS_OK);
    return;
  }
  oc_rep_begin_root_object();
  oc_rep_i_set_boolean(root, 1, g_InfoOnOff_2);
  oc_rep_end_root_object(); 
  
  if (g_err) {
    error_state = true;
  }
  PRINT("CBOR encoder size %d\n", oc_rep_get_encoded_payload_size());
  if (error_state == false) {
    oc_send_cbor_response(request, OC_STATUS_OK);
  } else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_InfoOnOff_2\n");
}

 
/**
 * @brief CoAP PUT method for data point "InfoOnOff_2" resource at url "/p/o_4_4".
 * resource types: ['urn:knx:dpa.421.51']
 * The function has as input the request body, which are the input values of the
 * PUT method.
 * The input values (as a set) are checked if all supplied values are correct.
 * If the input values are correct, they will be assigned to the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the used interfaces during the request.
 * @param user_data the supplied user data.
 */
void
put_InfoOnOff_2(oc_request_t *request, oc_interface_mask_t interfaces,
                void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = true;
  PRINT("-- Begin put_InfoOnOff_2:\n");

  oc_rep_t *rep = NULL;
  /* handle the different requests e.g. via s-mode or normal CoAP call*/
  if (oc_is_redirected_request(request)) {
    PRINT("  redirected request..\n");
  }
  rep = request->request_payload;
  /* loop over all the entries in the request */
  /* handle the type of payload correctly. */
  error_state = true;
  while (rep != NULL) {
    /* handle the type of payload correctly. */
    if ((rep->iname == 1) && (rep->type == OC_REP_BOOL)) {
      PRINT("  put_InfoOnOff_2 received : %d\n", rep->value.boolean);
      g_InfoOnOff_2 = rep->value.boolean;
      error_state = false;
      break;
    }
  }

  if (error_state == false){
      oc_send_cbor_response(request, OC_STATUS_CHANGED); 
      do_put_cb(URL_INFOONOFF_2);
      PRINT("-- End put_InfoOnOff_2\n");
      return;
  }
  /* request data was not recognized, so it was a bad request */
  oc_send_response(request, OC_STATUS_BAD_REQUEST);
  PRINT("-- End put_InfoOnOff_2\n");
}

/**
 * @brief CoAP GET method for data point "OnOff_3" resource at url URL_ONOFF_3 ("/p/o_5_5").
 * resource types: ['urn:knx:dpa.421.61']
 * function is called to initialize the return values of the GET method.
 * initialization of the returned values are done from the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the interface used for this call
 * @param user_data the user data.
 */
void
get_OnOff_3(oc_request_t *request, oc_interface_mask_t interfaces,
               void *user_data)
{
  (void)user_data; /* variable not used */

  /* MANUFACTORER: SENSOR add here the code to talk to the HW if one implements a
     sensor. the call to the HW needs to fill in the global variable before it
     returns to this function here. alternative is to have a callback from the
     hardware that sets the global variables.
  */
  bool error_state = false; /* the error state, the generated code */

  PRINT("-- Begin get_OnOff_3 %s \n", URL_ONOFF_3);
  /* check if the accept header is CBOR */
  if (oc_check_accept_header(request, APPLICATION_CBOR) == false) {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
    return;
  }

  // check the query parameter m with the various values
  char *m;
  char* m_key;
  size_t m_key_len;
  size_t m_len = (int)oc_get_query_value(request, "m", &m);
  if (m_len != -1) {
    PRINT("  Query param: %.*s",(int)m_len, m);
    oc_init_query_iterator();
    size_t device_index = request->resource->device;
    oc_device_info_t *device = oc_core_get_device_info(device_index);
    if (device != NULL) {
      oc_rep_begin_root_object();
      while (oc_iterate_query(request, &m_key, &m_key_len, &m, &m_len) != -1) {
        // unique identifier
        if ((strncmp(m, "id", m_len) == 0) |
            (strncmp(m, "*", m_len) == 0) ) {
          char mystring[100];
          snprintf(mystring,99,"urn:knx:sn:%s%s",oc_string(device->serialnumber),
           oc_string(request->resource->uri));
          oc_rep_i_set_text_string(root, 9, mystring);
        }
        // resource types
        if ((strncmp(m, "rt", m_len) == 0) |
            (strncmp(m, "*", m_len) == 0) ) {
          oc_rep_set_text_string(root, rt, "urn:knx:dpa.421.61");
        }
        // interfaces
        if ((strncmp(m, "if", m_len) == 0) |
            (strncmp(m, "*", m_len) == 0) ) {
          oc_rep_set_text_string(root, if, "if.s");
        }
        if ((strncmp(m, "dpt", m_len) == 0) |
            (strncmp(m, "*", m_len) == 0) ) {
          oc_rep_set_text_string(root, dpt, oc_string(request->resource->dpt));
        }
        // ga
        if ((strncmp(m, "ga", m_len) == 0) |
            (strncmp(m, "*", m_len) == 0) ) {
          int index = oc_core_find_group_object_table_url(oc_string(request->resource->uri));
          if (index > -1) {
             oc_group_object_table_t* got_table_entry = oc_core_get_group_object_table_entry(index);
             if (got_table_entry) {
               oc_rep_set_int_array(root, ga, got_table_entry->ga, got_table_entry->ga_len);
             }
          }
        }
        if ((strncmp(m, "desc", m_len) == 0) |
            (strncmp(m, "*", m_len) == 0) ) {
          oc_rep_set_text_string(root, desc, "On/Off push button 3");
        }
      } /* query iterator */
      oc_rep_end_root_object();
    } else {
      /* device is NULL */
      oc_send_cbor_response(request, OC_STATUS_BAD_OPTION);
    }
    oc_send_cbor_response(request, OC_STATUS_OK);
    return;
  }
  oc_rep_begin_root_object();
  oc_rep_i_set_boolean(root, 1, g_OnOff_3);
  oc_rep_end_root_object(); 
  
  if (g_err) {
    error_state = true;
  }
  PRINT("CBOR encoder size %d\n", oc_rep_get_encoded_payload_size());
  if (error_state == false) {
    oc_send_cbor_response(request, OC_STATUS_OK);
  } else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_OnOff_3\n");
}



/**
 * @brief CoAP GET method for data point "InfoOnOff_3" resource at url URL_INFOONOFF_3 ("/p/o_6_6").
 * resource types: ['urn:knx:dpa.421.51']
 * function is called to initialize the return values of the GET method.
 * initialization of the returned values are done from the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the interface used for this call
 * @param user_data the user data.
 */
void
get_InfoOnOff_3(oc_request_t *request, oc_interface_mask_t interfaces,
               void *user_data)
{
  (void)user_data; /* variable not used */

  /* MANUFACTORER: SENSOR add here the code to talk to the HW if one implements a
     sensor. the call to the HW needs to fill in the global variable before it
     returns to this function here. alternative is to have a callback from the
     hardware that sets the global variables.
  */
  bool error_state = false; /* the error state, the generated code */

  PRINT("-- Begin get_InfoOnOff_3 %s \n", URL_INFOONOFF_3);
  /* check if the accept header is CBOR */
  if (oc_check_accept_header(request, APPLICATION_CBOR) == false) {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
    return;
  }

  // check the query parameter m with the various values
  char *m;
  char* m_key;
  size_t m_key_len;
  size_t m_len = (int)oc_get_query_value(request, "m", &m);
  if (m_len != -1) {
    PRINT("  Query param: %.*s",(int)m_len, m);
    oc_init_query_iterator();
    size_t device_index = request->resource->device;
    oc_device_info_t *device = oc_core_get_device_info(device_index);
    if (device != NULL) {
      oc_rep_begin_root_object();
      while (oc_iterate_query(request, &m_key, &m_key_len, &m, &m_len) != -1) {
        // unique identifier
        if ((strncmp(m, "id", m_len) == 0) |
            (strncmp(m, "*", m_len) == 0) ) {
          char mystring[100];
          snprintf(mystring,99,"urn:knx:sn:%s%s",oc_string(device->serialnumber),
           oc_string(request->resource->uri));
          oc_rep_i_set_text_string(root, 9, mystring);
        }
        // resource types
        if ((strncmp(m, "rt", m_len) == 0) |
            (strncmp(m, "*", m_len) == 0) ) {
          oc_rep_set_text_string(root, rt, "urn:knx:dpa.421.51");
        }
        // interfaces
        if ((strncmp(m, "if", m_len) == 0) |
            (strncmp(m, "*", m_len) == 0) ) {
          oc_rep_set_text_string(root, if, "if.a");
        }
        if ((strncmp(m, "dpt", m_len) == 0) |
            (strncmp(m, "*", m_len) == 0) ) {
          oc_rep_set_text_string(root, dpt, oc_string(request->resource->dpt));
        }
        // ga
        if ((strncmp(m, "ga", m_len) == 0) |
            (strncmp(m, "*", m_len) == 0) ) {
          int index = oc_core_find_group_object_table_url(oc_string(request->resource->uri));
          if (index > -1) {
             oc_group_object_table_t* got_table_entry = oc_core_get_group_object_table_entry(index);
             if (got_table_entry) {
               oc_rep_set_int_array(root, ga, got_table_entry->ga, got_table_entry->ga_len);
             }
          }
        }
        if ((strncmp(m, "desc", m_len) == 0) |
            (strncmp(m, "*", m_len) == 0) ) {
          oc_rep_set_text_string(root, desc, "Feedback 3");
        }
      } /* query iterator */
      oc_rep_end_root_object();
    } else {
      /* device is NULL */
      oc_send_cbor_response(request, OC_STATUS_BAD_OPTION);
    }
    oc_send_cbor_response(request, OC_STATUS_OK);
    return;
  }
  oc_rep_begin_root_object();
  oc_rep_i_set_boolean(root, 1, g_InfoOnOff_3);
  oc_rep_end_root_object(); 
  
  if (g_err) {
    error_state = true;
  }
  PRINT("CBOR encoder size %d\n", oc_rep_get_encoded_payload_size());
  if (error_state == false) {
    oc_send_cbor_response(request, OC_STATUS_OK);
  } else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_InfoOnOff_3\n");
}

 
/**
 * @brief CoAP PUT method for data point "InfoOnOff_3" resource at url "/p/o_6_6".
 * resource types: ['urn:knx:dpa.421.51']
 * The function has as input the request body, which are the input values of the
 * PUT method.
 * The input values (as a set) are checked if all supplied values are correct.
 * If the input values are correct, they will be assigned to the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the used interfaces during the request.
 * @param user_data the supplied user data.
 */
void
put_InfoOnOff_3(oc_request_t *request, oc_interface_mask_t interfaces,
                void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = true;
  PRINT("-- Begin put_InfoOnOff_3:\n");

  oc_rep_t *rep = NULL;
  /* handle the different requests e.g. via s-mode or normal CoAP call*/
  if (oc_is_redirected_request(request)) {
    PRINT("  redirected request..\n");
  }
  rep = request->request_payload;
  /* loop over all the entries in the request */
  /* handle the type of payload correctly. */
  error_state = true;
  while (rep != NULL) {
    /* handle the type of payload correctly. */
    if ((rep->iname == 1) && (rep->type == OC_REP_BOOL)) {
      PRINT("  put_InfoOnOff_3 received : %d\n", rep->value.boolean);
      g_InfoOnOff_3 = rep->value.boolean;
      error_state = false;
      break;
    }
  }

  if (error_state == false){
      oc_send_cbor_response(request, OC_STATUS_CHANGED); 
      do_put_cb(URL_INFOONOFF_3);
      PRINT("-- End put_InfoOnOff_3\n");
      return;
  }
  /* request data was not recognized, so it was a bad request */
  oc_send_response(request, OC_STATUS_BAD_REQUEST);
  PRINT("-- End put_InfoOnOff_3\n");
}

/**
 * @brief CoAP GET method for data point "OnOff_4" resource at url URL_ONOFF_4 ("/p/o_7_7").
 * resource types: ['urn:knx:dpa.421.61']
 * function is called to initialize the return values of the GET method.
 * initialization of the returned values are done from the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the interface used for this call
 * @param user_data the user data.
 */
void
get_OnOff_4(oc_request_t *request, oc_interface_mask_t interfaces,
               void *user_data)
{
  (void)user_data; /* variable not used */

  /* MANUFACTORER: SENSOR add here the code to talk to the HW if one implements a
     sensor. the call to the HW needs to fill in the global variable before it
     returns to this function here. alternative is to have a callback from the
     hardware that sets the global variables.
  */
  bool error_state = false; /* the error state, the generated code */

  PRINT("-- Begin get_OnOff_4 %s \n", URL_ONOFF_4);
  /* check if the accept header is CBOR */
  if (oc_check_accept_header(request, APPLICATION_CBOR) == false) {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
    return;
  }

  // check the query parameter m with the various values
  char *m;
  char* m_key;
  size_t m_key_len;
  size_t m_len = (int)oc_get_query_value(request, "m", &m);
  if (m_len != -1) {
    PRINT("  Query param: %.*s",(int)m_len, m);
    oc_init_query_iterator();
    size_t device_index = request->resource->device;
    oc_device_info_t *device = oc_core_get_device_info(device_index);
    if (device != NULL) {
      oc_rep_begin_root_object();
      while (oc_iterate_query(request, &m_key, &m_key_len, &m, &m_len) != -1) {
        // unique identifier
        if ((strncmp(m, "id", m_len) == 0) |
            (strncmp(m, "*", m_len) == 0) ) {
          char mystring[100];
          snprintf(mystring,99,"urn:knx:sn:%s%s",oc_string(device->serialnumber),
           oc_string(request->resource->uri));
          oc_rep_i_set_text_string(root, 9, mystring);
        }
        // resource types
        if ((strncmp(m, "rt", m_len) == 0) |
            (strncmp(m, "*", m_len) == 0) ) {
          oc_rep_set_text_string(root, rt, "urn:knx:dpa.421.61");
        }
        // interfaces
        if ((strncmp(m, "if", m_len) == 0) |
            (strncmp(m, "*", m_len) == 0) ) {
          oc_rep_set_text_string(root, if, "if.s");
        }
        if ((strncmp(m, "dpt", m_len) == 0) |
            (strncmp(m, "*", m_len) == 0) ) {
          oc_rep_set_text_string(root, dpt, oc_string(request->resource->dpt));
        }
        // ga
        if ((strncmp(m, "ga", m_len) == 0) |
            (strncmp(m, "*", m_len) == 0) ) {
          int index = oc_core_find_group_object_table_url(oc_string(request->resource->uri));
          if (index > -1) {
             oc_group_object_table_t* got_table_entry = oc_core_get_group_object_table_entry(index);
             if (got_table_entry) {
               oc_rep_set_int_array(root, ga, got_table_entry->ga, got_table_entry->ga_len);
             }
          }
        }
        if ((strncmp(m, "desc", m_len) == 0) |
            (strncmp(m, "*", m_len) == 0) ) {
          oc_rep_set_text_string(root, desc, "On/Off push button 4");
        }
      } /* query iterator */
      oc_rep_end_root_object();
    } else {
      /* device is NULL */
      oc_send_cbor_response(request, OC_STATUS_BAD_OPTION);
    }
    oc_send_cbor_response(request, OC_STATUS_OK);
    return;
  }
  oc_rep_begin_root_object();
  oc_rep_i_set_boolean(root, 1, g_OnOff_4);
  oc_rep_end_root_object(); 
  
  if (g_err) {
    error_state = true;
  }
  PRINT("CBOR encoder size %d\n", oc_rep_get_encoded_payload_size());
  if (error_state == false) {
    oc_send_cbor_response(request, OC_STATUS_OK);
  } else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_OnOff_4\n");
}



/**
 * @brief CoAP GET method for data point "InfoOnOff_4" resource at url URL_INFOONOFF_4 ("/p/o_8_8").
 * resource types: ['urn:knx:dpa.421.51']
 * function is called to initialize the return values of the GET method.
 * initialization of the returned values are done from the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the interface used for this call
 * @param user_data the user data.
 */
void
get_InfoOnOff_4(oc_request_t *request, oc_interface_mask_t interfaces,
               void *user_data)
{
  (void)user_data; /* variable not used */

  /* MANUFACTORER: SENSOR add here the code to talk to the HW if one implements a
     sensor. the call to the HW needs to fill in the global variable before it
     returns to this function here. alternative is to have a callback from the
     hardware that sets the global variables.
  */
  bool error_state = false; /* the error state, the generated code */

  PRINT("-- Begin get_InfoOnOff_4 %s \n", URL_INFOONOFF_4);
  /* check if the accept header is CBOR */
  if (oc_check_accept_header(request, APPLICATION_CBOR) == false) {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
    return;
  }

  // check the query parameter m with the various values
  char *m;
  char* m_key;
  size_t m_key_len;
  size_t m_len = (int)oc_get_query_value(request, "m", &m);
  if (m_len != -1) {
    PRINT("  Query param: %.*s",(int)m_len, m);
    oc_init_query_iterator();
    size_t device_index = request->resource->device;
    oc_device_info_t *device = oc_core_get_device_info(device_index);
    if (device != NULL) {
      oc_rep_begin_root_object();
      while (oc_iterate_query(request, &m_key, &m_key_len, &m, &m_len) != -1) {
        // unique identifier
        if ((strncmp(m, "id", m_len) == 0) |
            (strncmp(m, "*", m_len) == 0) ) {
          char mystring[100];
          snprintf(mystring,99,"urn:knx:sn:%s%s",oc_string(device->serialnumber),
           oc_string(request->resource->uri));
          oc_rep_i_set_text_string(root, 9, mystring);
        }
        // resource types
        if ((strncmp(m, "rt", m_len) == 0) |
            (strncmp(m, "*", m_len) == 0) ) {
          oc_rep_set_text_string(root, rt, "urn:knx:dpa.421.51");
        }
        // interfaces
        if ((strncmp(m, "if", m_len) == 0) |
            (strncmp(m, "*", m_len) == 0) ) {
          oc_rep_set_text_string(root, if, "if.a");
        }
        if ((strncmp(m, "dpt", m_len) == 0) |
            (strncmp(m, "*", m_len) == 0) ) {
          oc_rep_set_text_string(root, dpt, oc_string(request->resource->dpt));
        }
        // ga
        if ((strncmp(m, "ga", m_len) == 0) |
            (strncmp(m, "*", m_len) == 0) ) {
          int index = oc_core_find_group_object_table_url(oc_string(request->resource->uri));
          if (index > -1) {
             oc_group_object_table_t* got_table_entry = oc_core_get_group_object_table_entry(index);
             if (got_table_entry) {
               oc_rep_set_int_array(root, ga, got_table_entry->ga, got_table_entry->ga_len);
             }
          }
        }
        if ((strncmp(m, "desc", m_len) == 0) |
            (strncmp(m, "*", m_len) == 0) ) {
          oc_rep_set_text_string(root, desc, "Feedback 4");
        }
      } /* query iterator */
      oc_rep_end_root_object();
    } else {
      /* device is NULL */
      oc_send_cbor_response(request, OC_STATUS_BAD_OPTION);
    }
    oc_send_cbor_response(request, OC_STATUS_OK);
    return;
  }
  oc_rep_begin_root_object();
  oc_rep_i_set_boolean(root, 1, g_InfoOnOff_4);
  oc_rep_end_root_object(); 
  
  if (g_err) {
    error_state = true;
  }
  PRINT("CBOR encoder size %d\n", oc_rep_get_encoded_payload_size());
  if (error_state == false) {
    oc_send_cbor_response(request, OC_STATUS_OK);
  } else {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
  }
  PRINT("-- End get_InfoOnOff_4\n");
}

 
/**
 * @brief CoAP PUT method for data point "InfoOnOff_4" resource at url "/p/o_8_8".
 * resource types: ['urn:knx:dpa.421.51']
 * The function has as input the request body, which are the input values of the
 * PUT method.
 * The input values (as a set) are checked if all supplied values are correct.
 * If the input values are correct, they will be assigned to the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the used interfaces during the request.
 * @param user_data the supplied user data.
 */
void
put_InfoOnOff_4(oc_request_t *request, oc_interface_mask_t interfaces,
                void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = true;
  PRINT("-- Begin put_InfoOnOff_4:\n");

  oc_rep_t *rep = NULL;
  /* handle the different requests e.g. via s-mode or normal CoAP call*/
  if (oc_is_redirected_request(request)) {
    PRINT("  redirected request..\n");
  }
  rep = request->request_payload;
  /* loop over all the entries in the request */
  /* handle the type of payload correctly. */
  error_state = true;
  while (rep != NULL) {
    /* handle the type of payload correctly. */
    if ((rep->iname == 1) && (rep->type == OC_REP_BOOL)) {
      PRINT("  put_InfoOnOff_4 received : %d\n", rep->value.boolean);
      g_InfoOnOff_4 = rep->value.boolean;
      error_state = false;
      break;
    }
  }

  if (error_state == false){
      oc_send_cbor_response(request, OC_STATUS_CHANGED); 
      do_put_cb(URL_INFOONOFF_4);
      PRINT("-- End put_InfoOnOff_4\n");
      return;
  }
  /* request data was not recognized, so it was a bad request */
  oc_send_response(request, OC_STATUS_BAD_REQUEST);
  PRINT("-- End put_InfoOnOff_4\n");
}


// parameters handling



/**
 * @brief register all the data point resources to the stack
 * this function registers all data point level resources:
 * - each resource path is bind to a specific function for the supported methods
 *  (GET, PUT)
 * - each resource is
 *   - secure
 *   - observable
 *   - discoverable through well-known/core
 *   - used interfaces as: dpa.xxx.yyy 
 *      - xxx : function block number
 *      - yyy : data point function number
 */
void
register_resources(void)
{
  PRINT("Register Resource 'OnOff_1' with local path \"%s\"\n", URL_ONOFF_1);
  oc_resource_t *res_OnOff_1 =
    oc_new_resource("OnOff_1", URL_ONOFF_1, 1, 0);
  oc_resource_bind_resource_type(res_OnOff_1, "urn:knx:dpa.421.61");
  oc_resource_bind_dpt(res_OnOff_1, "urn:knx:dpt.switch");
  oc_resource_bind_content_type(res_OnOff_1, APPLICATION_CBOR);
  oc_resource_bind_resource_interface(res_OnOff_1, OC_IF_S); /* if.s */ 
  oc_resource_set_function_block_instance(res_OnOff_1, 1); /* instance 1 */ 
  oc_resource_set_discoverable(res_OnOff_1, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  /* oc_resource_set_periodic_observable(res_OnOff_1, 1); */
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is
    called. this function must be called when the value changes, preferable on
    an interrupt when something is read from the hardware. */
  oc_resource_set_observable(res_OnOff_1, true);
  oc_resource_set_request_handler(res_OnOff_1, OC_GET, get_OnOff_1, NULL);
  oc_add_resource(res_OnOff_1);
  PRINT("Register Resource 'InfoOnOff_1' with local path \"%s\"\n", URL_INFOONOFF_1);
  oc_resource_t *res_InfoOnOff_1 =
    oc_new_resource("InfoOnOff_1", URL_INFOONOFF_1, 1, 0);
  oc_resource_bind_resource_type(res_InfoOnOff_1, "urn:knx:dpa.421.51");
  oc_resource_bind_dpt(res_InfoOnOff_1, "urn:knx:dpt.switch");
  oc_resource_bind_content_type(res_InfoOnOff_1, APPLICATION_CBOR);
  oc_resource_bind_resource_interface(res_InfoOnOff_1, OC_IF_A); /* if.a */ 
  oc_resource_set_function_block_instance(res_InfoOnOff_1, 1); /* instance 1 */ 
  oc_resource_set_discoverable(res_InfoOnOff_1, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  /* oc_resource_set_periodic_observable(res_InfoOnOff_1, 1); */
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is
    called. this function must be called when the value changes, preferable on
    an interrupt when something is read from the hardware. */
  oc_resource_set_observable(res_InfoOnOff_1, true);
  oc_resource_set_request_handler(res_InfoOnOff_1, OC_GET, get_InfoOnOff_1, NULL);
  oc_resource_set_request_handler(res_InfoOnOff_1, OC_PUT, put_InfoOnOff_1, NULL); 
  oc_add_resource(res_InfoOnOff_1);
  PRINT("Register Resource 'OnOff_2' with local path \"%s\"\n", URL_ONOFF_2);
  oc_resource_t *res_OnOff_2 =
    oc_new_resource("OnOff_2", URL_ONOFF_2, 1, 0);
  oc_resource_bind_resource_type(res_OnOff_2, "urn:knx:dpa.421.61");
  oc_resource_bind_dpt(res_OnOff_2, "urn:knx:dpt.switch");
  oc_resource_bind_content_type(res_OnOff_2, APPLICATION_CBOR);
  oc_resource_bind_resource_interface(res_OnOff_2, OC_IF_S); /* if.s */ 
  oc_resource_set_function_block_instance(res_OnOff_2, 2); /* instance 2 */ 
  oc_resource_set_discoverable(res_OnOff_2, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  /* oc_resource_set_periodic_observable(res_OnOff_2, 1); */
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is
    called. this function must be called when the value changes, preferable on
    an interrupt when something is read from the hardware. */
  oc_resource_set_observable(res_OnOff_2, true);
  oc_resource_set_request_handler(res_OnOff_2, OC_GET, get_OnOff_2, NULL);
  oc_add_resource(res_OnOff_2);
  PRINT("Register Resource 'InfoOnOff_2' with local path \"%s\"\n", URL_INFOONOFF_2);
  oc_resource_t *res_InfoOnOff_2 =
    oc_new_resource("InfoOnOff_2", URL_INFOONOFF_2, 1, 0);
  oc_resource_bind_resource_type(res_InfoOnOff_2, "urn:knx:dpa.421.51");
  oc_resource_bind_dpt(res_InfoOnOff_2, "urn:knx:dpt.switch");
  oc_resource_bind_content_type(res_InfoOnOff_2, APPLICATION_CBOR);
  oc_resource_bind_resource_interface(res_InfoOnOff_2, OC_IF_A); /* if.a */ 
  oc_resource_set_function_block_instance(res_InfoOnOff_2, 2); /* instance 2 */ 
  oc_resource_set_discoverable(res_InfoOnOff_2, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  /* oc_resource_set_periodic_observable(res_InfoOnOff_2, 1); */
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is
    called. this function must be called when the value changes, preferable on
    an interrupt when something is read from the hardware. */
  oc_resource_set_observable(res_InfoOnOff_2, true);
  oc_resource_set_request_handler(res_InfoOnOff_2, OC_GET, get_InfoOnOff_2, NULL);
  oc_resource_set_request_handler(res_InfoOnOff_2, OC_PUT, put_InfoOnOff_2, NULL); 
  oc_add_resource(res_InfoOnOff_2);
  PRINT("Register Resource 'OnOff_3' with local path \"%s\"\n", URL_ONOFF_3);
  oc_resource_t *res_OnOff_3 =
    oc_new_resource("OnOff_3", URL_ONOFF_3, 1, 0);
  oc_resource_bind_resource_type(res_OnOff_3, "urn:knx:dpa.421.61");
  oc_resource_bind_dpt(res_OnOff_3, "urn:knx:dpt.switch");
  oc_resource_bind_content_type(res_OnOff_3, APPLICATION_CBOR);
  oc_resource_bind_resource_interface(res_OnOff_3, OC_IF_S); /* if.s */ 
  oc_resource_set_function_block_instance(res_OnOff_3, 3); /* instance 3 */ 
  oc_resource_set_discoverable(res_OnOff_3, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  /* oc_resource_set_periodic_observable(res_OnOff_3, 1); */
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is
    called. this function must be called when the value changes, preferable on
    an interrupt when something is read from the hardware. */
  oc_resource_set_observable(res_OnOff_3, true);
  oc_resource_set_request_handler(res_OnOff_3, OC_GET, get_OnOff_3, NULL);
  oc_add_resource(res_OnOff_3);
  PRINT("Register Resource 'InfoOnOff_3' with local path \"%s\"\n", URL_INFOONOFF_3);
  oc_resource_t *res_InfoOnOff_3 =
    oc_new_resource("InfoOnOff_3", URL_INFOONOFF_3, 1, 0);
  oc_resource_bind_resource_type(res_InfoOnOff_3, "urn:knx:dpa.421.51");
  oc_resource_bind_dpt(res_InfoOnOff_3, "urn:knx:dpt.switch");
  oc_resource_bind_content_type(res_InfoOnOff_3, APPLICATION_CBOR);
  oc_resource_bind_resource_interface(res_InfoOnOff_3, OC_IF_A); /* if.a */ 
  oc_resource_set_function_block_instance(res_InfoOnOff_3, 3); /* instance 3 */ 
  oc_resource_set_discoverable(res_InfoOnOff_3, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  /* oc_resource_set_periodic_observable(res_InfoOnOff_3, 1); */
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is
    called. this function must be called when the value changes, preferable on
    an interrupt when something is read from the hardware. */
  oc_resource_set_observable(res_InfoOnOff_3, true);
  oc_resource_set_request_handler(res_InfoOnOff_3, OC_GET, get_InfoOnOff_3, NULL);
  oc_resource_set_request_handler(res_InfoOnOff_3, OC_PUT, put_InfoOnOff_3, NULL); 
  oc_add_resource(res_InfoOnOff_3);
  PRINT("Register Resource 'OnOff_4' with local path \"%s\"\n", URL_ONOFF_4);
  oc_resource_t *res_OnOff_4 =
    oc_new_resource("OnOff_4", URL_ONOFF_4, 1, 0);
  oc_resource_bind_resource_type(res_OnOff_4, "urn:knx:dpa.421.61");
  oc_resource_bind_dpt(res_OnOff_4, "urn:knx:dpt.switch");
  oc_resource_bind_content_type(res_OnOff_4, APPLICATION_CBOR);
  oc_resource_bind_resource_interface(res_OnOff_4, OC_IF_S); /* if.s */ 
  oc_resource_set_function_block_instance(res_OnOff_4, 4); /* instance 4 */ 
  oc_resource_set_discoverable(res_OnOff_4, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  /* oc_resource_set_periodic_observable(res_OnOff_4, 1); */
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is
    called. this function must be called when the value changes, preferable on
    an interrupt when something is read from the hardware. */
  oc_resource_set_observable(res_OnOff_4, true);
  oc_resource_set_request_handler(res_OnOff_4, OC_GET, get_OnOff_4, NULL);
  oc_add_resource(res_OnOff_4);
  PRINT("Register Resource 'InfoOnOff_4' with local path \"%s\"\n", URL_INFOONOFF_4);
  oc_resource_t *res_InfoOnOff_4 =
    oc_new_resource("InfoOnOff_4", URL_INFOONOFF_4, 1, 0);
  oc_resource_bind_resource_type(res_InfoOnOff_4, "urn:knx:dpa.421.51");
  oc_resource_bind_dpt(res_InfoOnOff_4, "urn:knx:dpt.switch");
  oc_resource_bind_content_type(res_InfoOnOff_4, APPLICATION_CBOR);
  oc_resource_bind_resource_interface(res_InfoOnOff_4, OC_IF_A); /* if.a */ 
  oc_resource_set_function_block_instance(res_InfoOnOff_4, 4); /* instance 4 */ 
  oc_resource_set_discoverable(res_InfoOnOff_4, true);
  /* periodic observable
     to be used when one wants to send an event per time slice
     period is 1 second */
  /* oc_resource_set_periodic_observable(res_InfoOnOff_4, 1); */
  /* set observable
     events are send when oc_notify_observers(oc_resource_t *resource) is
    called. this function must be called when the value changes, preferable on
    an interrupt when something is read from the hardware. */
  oc_resource_set_observable(res_InfoOnOff_4, true);
  oc_resource_set_request_handler(res_InfoOnOff_4, OC_GET, get_InfoOnOff_4, NULL);
  oc_resource_set_request_handler(res_InfoOnOff_4, OC_PUT, put_InfoOnOff_4, NULL); 
  oc_add_resource(res_InfoOnOff_4);

}

/**
 * @brief initiate preset for device
 * current implementation: device reset as command line argument
 * @param device_index the device identifier of the list of devices
 * @param data the supplied data.
 */
void
factory_presets_cb(size_t device_index, void *data)
{
  (void)device_index;
  (void)data;

  if (g_reset) {
    PRINT("factory_presets_cb: resetting device\n");
    oc_knx_device_storage_reset(device_index, 2);
  }
}

/**
 * @brief set the host name on the device (application depended)
 *
 * @param device_index the device identifier of the list of devices
 * @param host_name the host name to be set on the device
 * @param data the supplied data.
 */
void
hostname_cb(size_t device_index, oc_string_t host_name, void *data)
{
  (void)device_index;
  (void)data;

  PRINT("-----host name ------- %s\n", oc_string(host_name));
}

static oc_event_callback_retval_t send_delayed_response(void *context)
{
  oc_separate_response_t *response = (oc_separate_response_t *)context;

  if (response->active)
  {
    oc_set_separate_response_buffer(response);
    oc_send_separate_response(response, OC_STATUS_CHANGED);
    PRINT_APP("Delayed response sent\n");
  }
  else
  {
    PRINT_APP("Delayed response NOT active\n");
  }

  return OC_EVENT_DONE;
}

/**
 * @brief software update callback
 *
 * @param device the device index
 * @param response the instance of an internal struct that is used to track
 *       		   the state of the separate response
 * @param binary_size the full size of the binary
 * @param offset the offset of the image
 * @param payload the image data
 * @param len the length of the image data
 * @param data the user data
 */
void swu_cb(size_t device,
            oc_separate_response_t *response,
            size_t binary_size,
            size_t offset,
            uint8_t *payload,
            size_t len,
            void *data)
{
  (void)device;
  (void)binary_size;
  char filename[] = "./downloaded.bin";
  PRINT(" swu_cb %s block=%d size=%d \n", filename, (int)offset, (int)len);

  FILE *write_ptr = fopen("downloaded_bin", "ab");
  size_t r = fwrite(payload, sizeof(*payload), len, write_ptr);
  fclose(write_ptr);

  oc_set_delayed_callback(response, &send_delayed_response, 0);
}

/**
 * @brief initializes the global variables
 * for the resources 
 * for the parameters
 */
void
initialize_variables(void)
{
  /* initialize global variables for resources */
  /* if wanted read them from persistent storage */
  //g_OnOff_1 = false;   /**< global variable for OnOff_1 */ 
  //g_InfoOnOff_1 = false;   /**< global variable for InfoOnOff_1 */ 
  //g_OnOff_2 = false;   /**< global variable for OnOff_2 */ 
  //g_InfoOnOff_2 = false;   /**< global variable for InfoOnOff_2 */ 
  //g_OnOff_3 = false;   /**< global variable for OnOff_3 */ 
  //g_InfoOnOff_3 = false;   /**< global variable for InfoOnOff_3 */ 
  //g_OnOff_4 = false;   /**< global variable for OnOff_4 */ 
  //g_InfoOnOff_4 = false;   /**< global variable for InfoOnOff_4 */ 
  /* parameter variables */
  uint8_t oc_storage_buf[32];
  long ret;
  bool err;

}

int app_set_serial_number(char* serial_number)
{
  strncpy(g_serial_number, serial_number, 20);
  return 0;
}

int app_initialize_stack()
{
  int init;
  char *fname = "my_software_image";

  PRINT("KNX-IOT Server name : \"%s\"\n", MY_NAME);

  /* show the current working folder */
  char buff[FILENAME_MAX];
  char *retbuf = NULL;
  retbuf = GetCurrentDir(buff, FILENAME_MAX);
  if (retbuf != NULL) {
    PRINT("Current working dir: %s\n", buff);
  }

  /*
   The storage folder depends on the build system
   the folder is created in the makefile, with $target as name with _cred as
   post fix.
  */
#ifdef WIN32
  char storage[400];
  sprintf(storage,"./knx_iot_virtual_pb_%s",g_serial_number);  
  PRINT("\tstorage at '%s' \n",storage);
  oc_storage_config(storage);
#else
  PRINT("\tstorage at 'knx_iot_virtual_pb_creds' \n");
  oc_storage_config("./knx_iot_virtual_pb_creds");
#endif
  


  /*initialize the variables */
  initialize_variables();

  /* initializes the handlers structure */
  static oc_handler_t handler = { .init = app_init,
                                  .signal_event_loop = signal_event_loop,
                                  .register_resources = register_resources,
                                  .requests_entry = NULL };

  /* set the application callbacks */
  oc_set_hostname_cb(hostname_cb, NULL);
  oc_set_factory_presets_cb(factory_presets_cb, NULL);
  oc_set_swu_cb(swu_cb, (void *)fname);

  /* start the stack */
  init = oc_main_init(&handler);

  if (init < 0) {
    PRINT("oc_main_init failed %d, exiting.\n", init);
    return init;
  }

#ifdef OC_OSCORE
  PRINT("OSCORE - Enabled\n");
#else
  PRINT("OSCORE - Disabled\n");
#endif /* OC_OSCORE */

  oc_device_info_t *device = oc_core_get_device_info(0);
  PRINT("serial number: %s\n", oc_string(device->serialnumber));
  oc_endpoint_t *my_ep = oc_connectivity_get_endpoints(0);
  if (my_ep != NULL) {
    PRINTipaddr(*my_ep);
    PRINT("\n");
  }
  PRINT("Server \"%s\" running, waiting on incoming "
        "connections.\n",
        MY_NAME);
  return 0;
}

#ifdef WIN32
/**
 * @brief signal the event loop (windows version)
 * wakes up the main function to handle the next callback
 */
void
signal_event_loop(void)
{

#ifndef NO_MAIN
  WakeConditionVariable(&cv);
#endif /* NO_MAIN */
}
#endif /* WIN32 */

#ifdef __linux__
/**
 * @brief signal the event loop (Linux)
 * wakes up the main function to handle the next callback
 */
void
signal_event_loop(void)
{
#ifndef NO_MAIN
  pthread_mutex_lock(&mutex);
  pthread_cond_signal(&cv);
  pthread_mutex_unlock(&mutex);
#endif /* NO_MAIN */
}
#endif /* __linux__ */


#ifndef NO_MAIN

/**
 * @brief handle Ctrl-C
 * @param signal the captured signal
 */
static void
handle_signal(int signal)
{
  (void)signal;
  signal_event_loop();
  quit = 1;
}

/**
 * @brief print usage and quits
 *
 */
static void
print_usage()
{
  PRINT("Usage:\n");
  PRINT("no arguments : starts the server\n");
  PRINT("-help  : this message\n");
  PRINT("reset  : does an full reset of the device\n");
  PRINT("-s <serial number> : sets the serial number of the device\n");
  exit(0);
}
/**
 * @brief main application.
 * initializes the global variables
 * registers and starts the handler
 * handles (in a loop) the next event.
 * shuts down the stack
 */
int
main(int argc, char *argv[])
{
  oc_clock_time_t next_event;
  bool do_send_s_mode = false;

#ifdef KNX_GUI
  WinMain(GetModuleHandle(NULL), NULL, GetCommandLine(), SW_SHOWNORMAL);
#endif

#ifdef WIN32
  /* windows specific */
  InitializeCriticalSection(&cs);
  InitializeConditionVariable(&cv);
  /* install Ctrl-C */
  signal(SIGINT, handle_signal);
#endif
#ifdef __linux__
  /* Linux specific */
  struct sigaction sa;
  sigfillset(&sa.sa_mask);
  sa.sa_flags = 0;
  sa.sa_handler = handle_signal;
  /* install Ctrl-C */
  sigaction(SIGINT, &sa, NULL);
#endif

  for (int i = 0; i < argc; i++) {
    PRINT_APP("argv[%d] = %s\n", i, argv[i]);
  }
  if (argc > 1) {
    if (strcmp(argv[1], "reset") == 0) {
      PRINT(" internal reset\n");
      g_reset = true;
    }
    if (strcmp(argv[1], "-help") == 0) {
      print_usage();
    }
  }
  if (argc > 2) {
     if (strcmp(argv[1], "-s") == 0) {
        // serial number
        PRINT("serial number %s\n", argv[2]);
        app_set_serial_number(argv[2]);
     }
  }

  /* do all initialization */
  app_initialize_stack();

#ifdef WIN32
  /* windows specific loop */
  while (quit != 1) {
    next_event = oc_main_poll();
    if (next_event == 0) {
      SleepConditionVariableCS(&cv, &cs, INFINITE);
    } else {
      oc_clock_time_t now = oc_clock_time();
      if (now < next_event) {
        SleepConditionVariableCS(
          &cv, &cs, (DWORD)((next_event - now) * 1000 / OC_CLOCK_SECOND));
      }
    }
  }
#endif

#ifdef __linux__
  /* Linux specific loop */
  while (quit != 1) {
    next_event = oc_main_poll();
    pthread_mutex_lock(&mutex);
    if (next_event == 0) {
      pthread_cond_wait(&cv, &mutex);
    } else {
      ts.tv_sec = (next_event / OC_CLOCK_SECOND);
      ts.tv_nsec = (next_event % OC_CLOCK_SECOND) * 1.e09 / OC_CLOCK_SECOND;
      pthread_cond_timedwait(&cv, &mutex, &ts);
    }
    pthread_mutex_unlock(&mutex);
  }
#endif

  /* shut down the stack */
  oc_main_shutdown();
  return 0;
}
#endif /* NO_MAIN */
