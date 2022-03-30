/*
-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
 Copyright (c) 2022 Cascoda Ltd
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
#ifndef DOXYGEN
/* Force Doxygen to document static inline */
#define STATIC static
#endif

/**
 * @file
 * 
 * KNX virtual Switching Actuator
 *
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
 *  handlers for the implemented methods (get/post):
 *   - get_[path]
 *     function that is being called when a GET is called on [path]
 *     set the global variables in the output
 *   - post_[path]
 *     function that is being called when a POST is called on [path]
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
 can be included without changing this code
 */
#include "oc_api.h"
#include "oc_core_res.h"
#include "port/oc_clock.h"
#include <signal.h>
/* test purpose only; commandline reset */
#include "api/oc_knx_dev.h"

#ifdef INCLUDE_EXTERNAL
/* import external definitions from header file*/
/* this file should be externally supplied */
#include "external_header.h"
#endif
#include "knx_iot_virtual_sa.h"

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

#define MY_NAME "KNX virtual Switching Actuator" /**< The name of the application */

#ifdef WIN32
/** windows specific code */
#include <windows.h>
STATIC CONDITION_VARIABLE cv; /**< event loop variable */
STATIC CRITICAL_SECTION cs;   /**< event loop variable */
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

#define btoa(x) ((x) ? "true" : "false")
volatile int quit = 0;  /**< stop variable, used by handle_signal */
bool g_reset = false;   /**< reset variable, set by commandline arguments */

volatile bool g_OnOff_1;   /**< global variable for OnOff_1 */
volatile bool g_InfoOnOff_1;   /**< global variable for InfoOnOff_1 */
volatile bool g_OnOff_2;   /**< global variable for OnOff_2 */
volatile bool g_InfoOnOff_2;   /**< global variable for InfoOnOff_2 */
volatile bool g_OnOff_3;   /**< global variable for OnOff_3 */
volatile bool g_InfoOnOff_3;   /**< global variable for InfoOnOff_3 */
volatile bool g_OnOff_4;   /**< global variable for OnOff_4 */
volatile bool g_InfoOnOff_4;   /**< global variable for InfoOnOff_4 */
 
volatile bool g_fault_OnOff_1;   /**< global variable for fault OnOff_1 */ 
volatile bool g_fault_OnOff_2;   /**< global variable for fault OnOff_2 */ 
volatile bool g_fault_OnOff_3;   /**< global variable for fault OnOff_3 */ 
volatile bool g_fault_OnOff_4;   /**< global variable for fault OnOff_4 */

void app_set_bool_variable(char* url, bool value) 
{
  if ( strcmp(url, "/p/1") == 0) { 
    g_OnOff_1 = value;   /**< global variable for OnOff_1 */
    return;
  } 
  if ( strcmp(url, "/p/2") == 0) { 
    g_InfoOnOff_1 = value;   /**< global variable for InfoOnOff_1 */
    return;
  } 
  if ( strcmp(url, "/p/3") == 0) { 
    g_OnOff_2 = value;   /**< global variable for OnOff_2 */
    return;
  } 
  if ( strcmp(url, "/p/4") == 0) { 
    g_InfoOnOff_2 = value;   /**< global variable for InfoOnOff_2 */
    return;
  } 
  if ( strcmp(url, "/p/5") == 0) { 
    g_OnOff_3 = value;   /**< global variable for OnOff_3 */
    return;
  } 
  if ( strcmp(url, "/p/6") == 0) { 
    g_InfoOnOff_3 = value;   /**< global variable for InfoOnOff_3 */
    return;
  } 
  if ( strcmp(url, "/p/7") == 0) { 
    g_OnOff_4 = value;   /**< global variable for OnOff_4 */
    return;
  } 
  if ( strcmp(url, "/p/8") == 0) { 
    g_InfoOnOff_4 = value;   /**< global variable for InfoOnOff_4 */
    return;
  } 
}

bool app_retrieve_bool_variable(char* url) 
{
  if ( strcmp(url, "/p/1") == 0) { 
    return g_OnOff_1;   /**< global variable for OnOff_1 */
  }
  if ( strcmp(url, "/p/2") == 0) { 
    return g_InfoOnOff_1;   /**< global variable for InfoOnOff_1 */
  }
  if ( strcmp(url, "/p/3") == 0) { 
    return g_OnOff_2;   /**< global variable for OnOff_2 */
  }
  if ( strcmp(url, "/p/4") == 0) { 
    return g_InfoOnOff_2;   /**< global variable for InfoOnOff_2 */
  }
  if ( strcmp(url, "/p/5") == 0) { 
    return g_OnOff_3;   /**< global variable for OnOff_3 */
  }
  if ( strcmp(url, "/p/6") == 0) { 
    return g_InfoOnOff_3;   /**< global variable for InfoOnOff_3 */
  }
  if ( strcmp(url, "/p/7") == 0) { 
    return g_OnOff_4;   /**< global variable for OnOff_4 */
  }
  if ( strcmp(url, "/p/8") == 0) { 
    return g_InfoOnOff_4;   /**< global variable for InfoOnOff_4 */
  }
  return false;
}



void app_set_fault_variable(char* url, bool value)
{
 
  if ( strcmp(url, "/p/1") == 0) { 
    g_fault_OnOff_1 = value;   /**< global variable for OnOff_1 */
  } 
  if ( strcmp(url, "/p/3") == 0) { 
    g_fault_OnOff_2 = value;   /**< global variable for OnOff_2 */
  } 
  if ( strcmp(url, "/p/5") == 0) { 
    g_fault_OnOff_3 = value;   /**< global variable for OnOff_3 */
  } 
  if ( strcmp(url, "/p/7") == 0) { 
    g_fault_OnOff_4 = value;   /**< global variable for OnOff_4 */
  }
}

bool app_retrieve_fault_variable(char* url)
{
 
  if ( strcmp(url, "/p/1") == 0) { 
    return g_fault_OnOff_1;   /**< global variable for OnOff_1 */
  } 
  if ( strcmp(url, "/p/3") == 0) { 
    return g_fault_OnOff_2;   /**< global variable for OnOff_2 */
  } 
  if ( strcmp(url, "/p/5") == 0) { 
    return g_fault_OnOff_3;   /**< global variable for OnOff_3 */
  } 
  if ( strcmp(url, "/p/7") == 0) { 
    return g_fault_OnOff_4;   /**< global variable for OnOff_4 */
  }
  return false;
}

bool app_is_secure()
{
#ifdef OC_OSCORE
  return true;
#else
  return false;
#endif /* OC_OSCORE */
}

static oc_post_struct_t app_post = { NULL };

void
app_set_post_cb(oc_post_cb_t cb)
{
  app_post.cb = cb;
}

oc_post_struct_t *
oc_get_post_cb(void)
{
  return &app_post;
}

void do_post_cb(char* url) 
{
  oc_post_struct_t *my_cb = oc_get_post_cb();
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

/**
 * @brief s-mode response callback
 * will be called when a response is received on an s-mode read request
 *
 * @param url the url
 * @param rep the full response
 * @param rep_value the parsed value of the response
 */
STATIC void
oc_add_s_mode_response_cb(char *url, oc_rep_t *rep, oc_rep_t *rep_value)
{
  (void)rep;
  (void)rep_value;

  PRINT("oc_add_s_mode_response_cb %s\n", url);
}

/**
 * @brief function to set up the device.
 *
 * sets the:
 * - manufacturer  : cascoda
 * - serial number : 0004000
 * - base path
 * - knx spec version
 * - hardware version
 * - firmware version
 * - hardware type
 * - device model
 *
 */
STATIC int
app_init(void)
{
  int ret = oc_init_platform("cascoda", NULL, NULL);

  /* set the application name, version, base url, device serial number */
  ret |= oc_add_device(MY_NAME, "1.0.0", "//", "0004000", NULL, NULL);

  oc_device_info_t *device = oc_core_get_device_info(0);
  PRINT("Serial Number: %s\n", oc_string(device->serialnumber));

  /* set the hardware version 0.1.2 */
  oc_core_set_device_hwv(0, 0, 1, 2);

  /* set the firmware version 3.4.5 */
  oc_core_set_device_fwv(0, 3, 4, 5);

  /* set the hardware type*/
  oc_core_set_device_hwt(0, "Windows");

  /* set the model */
  oc_core_set_device_model(0, "KNX virtual - SA");

  oc_set_s_mode_response_cb(oc_add_s_mode_response_cb);

  return ret;
}

/**
 * @brief GET method for "OnOff_1" resource at "/p/1".
 *
 * function is called to initialize the return values of the GET method.
 * initialization of the returned values are done from the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the interface used for this call
 * @param user_data the user data.
 */
STATIC void
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

  PRINT("-- Begin get_OnOff_1 /p/1 \n");
  /* check if the accept header is CBOR */
  if (request->accept != APPLICATION_CBOR) {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
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
 * @brief POST method for "OnOff_1" resource at "/p/1".
 *
 * The function has as input the request body, which are the input values of the
 * POST method.
 * The input values (as a set) are checked if all supplied values are correct.
 * If the input values are correct, they will be assigned to the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the used interfaces during the request.
 * @param user_data the supplied user data.
 */
STATIC void
post_OnOff_1(oc_request_t *request, oc_interface_mask_t interfaces,
                void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = false;
  PRINT("-- Begin post_OnOff_1:\n");

  oc_rep_t *rep = NULL;
  /* handle the different requests e.g. via s-mode or normal CoAP call*/
  if (oc_is_s_mode_request(request)) {
    PRINT(" S-MODE\n");
    /* retrieve the value of the s-mode payload */
    rep = oc_s_mode_get_value(request);
  } else {
    /* the regular payload */
    rep = request->request_payload;
  }
  
  while (rep != NULL) {
    /* handle the type of payload correctly. */
    if ((rep->iname == 1) && (rep->type == OC_REP_BOOL)) {
      PRINT("  post_OnOff_1 received : %d\n", rep->value.boolean);
      g_OnOff_1 = rep->value.boolean;
      oc_send_cbor_response(request, OC_STATUS_CHANGED);
      /* MANUFACTORER: add here the code to talk to the HW if one implements an
       actuator. The call to the HW needs to fill in the global variable before it
       returns to this function here. Alternative is to have a callback from the
       hardware that sets the global variables.
     */
      /* update the status information of InfoOnOff_1*/
      if (g_fault_OnOff_1 == false) { 
        PRINT("  No Fault update feedback to %d'\n", g_OnOff_1);
        /* no fault hence update the feedback with the current state of the actuator */
        g_InfoOnOff_1 = g_OnOff_1;
      } else {
        /* fault hence update the feedback with "false" */
        PRINT("  Fault'\n");
        g_InfoOnOff_1 = false;
      }
      /* send the status information to '/p/2' with flag 'w'*/
      PRINT("  Send status to '/p/2' with flag: 'w'\n");
      oc_do_s_mode_with_scope(2, "/p/2", "w");
      oc_do_s_mode_with_scope(5, "/p/2", "w");
      do_post_cb("/p/1");
      PRINT("-- End post_OnOff_1\n");
      return;
    }
    rep = rep->next;
  }

  oc_send_response(request, OC_STATUS_BAD_REQUEST);
  PRINT("-- End post_OnOff_1\n");
}
/**
 * @brief GET method for "InfoOnOff_1" resource at "/p/2".
 *
 * function is called to initialize the return values of the GET method.
 * initialization of the returned values are done from the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the interface used for this call
 * @param user_data the user data.
 */
STATIC void
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

  PRINT("-- Begin get_InfoOnOff_1 /p/2 \n");
  /* check if the accept header is CBOR */
  if (request->accept != APPLICATION_CBOR) {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
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
 * @brief GET method for "OnOff_2" resource at "/p/3".
 *
 * function is called to initialize the return values of the GET method.
 * initialization of the returned values are done from the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the interface used for this call
 * @param user_data the user data.
 */
STATIC void
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

  PRINT("-- Begin get_OnOff_2 /p/3 \n");
  /* check if the accept header is CBOR */
  if (request->accept != APPLICATION_CBOR) {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
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
 * @brief POST method for "OnOff_2" resource at "/p/3".
 *
 * The function has as input the request body, which are the input values of the
 * POST method.
 * The input values (as a set) are checked if all supplied values are correct.
 * If the input values are correct, they will be assigned to the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the used interfaces during the request.
 * @param user_data the supplied user data.
 */
STATIC void
post_OnOff_2(oc_request_t *request, oc_interface_mask_t interfaces,
                void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = false;
  PRINT("-- Begin post_OnOff_2:\n");

  oc_rep_t *rep = NULL;
  /* handle the different requests e.g. via s-mode or normal CoAP call*/
  if (oc_is_s_mode_request(request)) {
    PRINT(" S-MODE\n");
    /* retrieve the value of the s-mode payload */
    rep = oc_s_mode_get_value(request);
  } else {
    /* the regular payload */
    rep = request->request_payload;
  }
  
  while (rep != NULL) {
    /* handle the type of payload correctly. */
    if ((rep->iname == 1) && (rep->type == OC_REP_BOOL)) {
      PRINT("  post_OnOff_2 received : %d\n", rep->value.boolean);
      g_OnOff_2 = rep->value.boolean;
      oc_send_cbor_response(request, OC_STATUS_CHANGED);
      /* MANUFACTORER: add here the code to talk to the HW if one implements an
       actuator. The call to the HW needs to fill in the global variable before it
       returns to this function here. Alternative is to have a callback from the
       hardware that sets the global variables.
     */
      /* update the status information of InfoOnOff_2*/
      if (g_fault_OnOff_2 == false) { 
        PRINT("  No Fault update feedback to %d'\n", g_OnOff_2);
        /* no fault hence update the feedback with the current state of the actuator */
        g_InfoOnOff_2 = g_OnOff_2;
      } else {
        /* fault hence update the feedback with "false" */
        PRINT("  Fault'\n");
        g_InfoOnOff_2 = false;
      }
      /* send the status information to '/p/4' with flag 'w'*/
      PRINT("  Send status to '/p/4' with flag: 'w'\n");
      oc_do_s_mode_with_scope(2, "/p/4", "w");
      oc_do_s_mode_with_scope(5, "/p/4", "w");
      do_post_cb("/p/3");
      PRINT("-- End post_OnOff_2\n");
      return;
    }
    rep = rep->next;
  }

  oc_send_response(request, OC_STATUS_BAD_REQUEST);
  PRINT("-- End post_OnOff_2\n");
}
/**
 * @brief GET method for "InfoOnOff_2" resource at "/p/4".
 *
 * function is called to initialize the return values of the GET method.
 * initialization of the returned values are done from the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the interface used for this call
 * @param user_data the user data.
 */
STATIC void
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

  PRINT("-- Begin get_InfoOnOff_2 /p/4 \n");
  /* check if the accept header is CBOR */
  if (request->accept != APPLICATION_CBOR) {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
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
 * @brief GET method for "OnOff_3" resource at "/p/5".
 *
 * function is called to initialize the return values of the GET method.
 * initialization of the returned values are done from the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the interface used for this call
 * @param user_data the user data.
 */
STATIC void
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

  PRINT("-- Begin get_OnOff_3 /p/5 \n");
  /* check if the accept header is CBOR */
  if (request->accept != APPLICATION_CBOR) {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
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
 * @brief POST method for "OnOff_3" resource at "/p/5".
 *
 * The function has as input the request body, which are the input values of the
 * POST method.
 * The input values (as a set) are checked if all supplied values are correct.
 * If the input values are correct, they will be assigned to the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the used interfaces during the request.
 * @param user_data the supplied user data.
 */
STATIC void
post_OnOff_3(oc_request_t *request, oc_interface_mask_t interfaces,
                void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = false;
  PRINT("-- Begin post_OnOff_3:\n");

  oc_rep_t *rep = NULL;
  /* handle the different requests e.g. via s-mode or normal CoAP call*/
  if (oc_is_s_mode_request(request)) {
    PRINT(" S-MODE\n");
    /* retrieve the value of the s-mode payload */
    rep = oc_s_mode_get_value(request);
  } else {
    /* the regular payload */
    rep = request->request_payload;
  }
  
  while (rep != NULL) {
    /* handle the type of payload correctly. */
    if ((rep->iname == 1) && (rep->type == OC_REP_BOOL)) {
      PRINT("  post_OnOff_3 received : %d\n", rep->value.boolean);
      g_OnOff_3 = rep->value.boolean;
      oc_send_cbor_response(request, OC_STATUS_CHANGED);
      /* MANUFACTORER: add here the code to talk to the HW if one implements an
       actuator. The call to the HW needs to fill in the global variable before it
       returns to this function here. Alternative is to have a callback from the
       hardware that sets the global variables.
     */
      /* update the status information of InfoOnOff_3*/
      if (g_fault_OnOff_3 == false) { 
        PRINT("  No Fault update feedback to %d'\n", g_OnOff_3);
        /* no fault hence update the feedback with the current state of the actuator */
        g_InfoOnOff_3 = g_OnOff_3;
      } else {
        /* fault hence update the feedback with "false" */
        PRINT("  Fault'\n");
        g_InfoOnOff_3 = false;
      }
      /* send the status information to '/p/6' with flag 'w'*/
      PRINT("  Send status to '/p/6' with flag: 'w'\n");
      oc_do_s_mode_with_scope(2, "/p/6", "w");
      oc_do_s_mode_with_scope(5, "/p/6", "w");
      do_post_cb("/p/5");
      PRINT("-- End post_OnOff_3\n");
      return;
    }
    rep = rep->next;
  }

  oc_send_response(request, OC_STATUS_BAD_REQUEST);
  PRINT("-- End post_OnOff_3\n");
}
/**
 * @brief GET method for "InfoOnOff_3" resource at "/p/6".
 *
 * function is called to initialize the return values of the GET method.
 * initialization of the returned values are done from the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the interface used for this call
 * @param user_data the user data.
 */
STATIC void
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

  PRINT("-- Begin get_InfoOnOff_3 /p/6 \n");
  /* check if the accept header is CBOR */
  if (request->accept != APPLICATION_CBOR) {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
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
 * @brief GET method for "OnOff_4" resource at "/p/7".
 *
 * function is called to initialize the return values of the GET method.
 * initialization of the returned values are done from the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the interface used for this call
 * @param user_data the user data.
 */
STATIC void
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

  PRINT("-- Begin get_OnOff_4 /p/7 \n");
  /* check if the accept header is CBOR */
  if (request->accept != APPLICATION_CBOR) {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
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
 * @brief POST method for "OnOff_4" resource at "/p/7".
 *
 * The function has as input the request body, which are the input values of the
 * POST method.
 * The input values (as a set) are checked if all supplied values are correct.
 * If the input values are correct, they will be assigned to the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the used interfaces during the request.
 * @param user_data the supplied user data.
 */
STATIC void
post_OnOff_4(oc_request_t *request, oc_interface_mask_t interfaces,
                void *user_data)
{
  (void)interfaces;
  (void)user_data;
  bool error_state = false;
  PRINT("-- Begin post_OnOff_4:\n");

  oc_rep_t *rep = NULL;
  /* handle the different requests e.g. via s-mode or normal CoAP call*/
  if (oc_is_s_mode_request(request)) {
    PRINT(" S-MODE\n");
    /* retrieve the value of the s-mode payload */
    rep = oc_s_mode_get_value(request);
  } else {
    /* the regular payload */
    rep = request->request_payload;
  }
  
  while (rep != NULL) {
    /* handle the type of payload correctly. */
    if ((rep->iname == 1) && (rep->type == OC_REP_BOOL)) {
      PRINT("  post_OnOff_4 received : %d\n", rep->value.boolean);
      g_OnOff_4 = rep->value.boolean;
      oc_send_cbor_response(request, OC_STATUS_CHANGED);
      /* MANUFACTORER: add here the code to talk to the HW if one implements an
       actuator. The call to the HW needs to fill in the global variable before it
       returns to this function here. Alternative is to have a callback from the
       hardware that sets the global variables.
     */
      /* update the status information of InfoOnOff_4*/
      if (g_fault_OnOff_4 == false) { 
        PRINT("  No Fault update feedback to %d'\n", g_OnOff_4);
        /* no fault hence update the feedback with the current state of the actuator */
        g_InfoOnOff_4 = g_OnOff_4;
      } else {
        /* fault hence update the feedback with "false" */
        PRINT("  Fault'\n");
        g_InfoOnOff_4 = false;
      }
      /* send the status information to '/p/8' with flag 'w'*/
      PRINT("  Send status to '/p/8' with flag: 'w'\n");
      oc_do_s_mode_with_scope(2, "/p/8", "w");
      oc_do_s_mode_with_scope(5, "/p/8", "w");
      do_post_cb("/p/7");
      PRINT("-- End post_OnOff_4\n");
      return;
    }
    rep = rep->next;
  }

  oc_send_response(request, OC_STATUS_BAD_REQUEST);
  PRINT("-- End post_OnOff_4\n");
}
/**
 * @brief GET method for "InfoOnOff_4" resource at "/p/8".
 *
 * function is called to initialize the return values of the GET method.
 * initialization of the returned values are done from the global property
 * values. 
 *
 * @param request the request representation.
 * @param interfaces the interface used for this call
 * @param user_data the user data.
 */
STATIC void
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

  PRINT("-- Begin get_InfoOnOff_4 /p/8 \n");
  /* check if the accept header is CBOR */
  if (request->accept != APPLICATION_CBOR) {
    oc_send_response(request, OC_STATUS_BAD_OPTION);
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
 * @brief register all the data point resources to the stack
 * this function registers all data point level resources:
 * - each resource path is bind to a specific function for the supported methods
 *  (GET, POST)
 * - each resource is
 *   - secure
 *   - observable
 *   - discoverable through well-known/core
 *   - used interfaces as: dpa.xxx.yyy 
 *      - xxx : function block number
 *      - yyy : data point function number
 */
STATIC void
register_resources(void)
{
  PRINT("Register Resource 'OnOff_1' with local path \"/p/1\"\n");
  oc_resource_t *res_OnOff_1 =
    oc_new_resource("OnOff_1", "/p/1", 2, 0);
  oc_resource_bind_resource_type(res_OnOff_1, "urn:knx:dpa.417.61");
  oc_resource_bind_resource_type(res_OnOff_1, "DPT_Switch");
  oc_resource_bind_content_type(res_OnOff_1, APPLICATION_CBOR);
  oc_resource_bind_resource_interface(res_OnOff_1, OC_IF_A); /* if.a */ 
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
  oc_resource_set_request_handler(res_OnOff_1, OC_POST, post_OnOff_1, NULL); 
  oc_add_resource(res_OnOff_1);
  PRINT("Register Resource 'InfoOnOff_1' with local path \"/p/2\"\n");
  oc_resource_t *res_InfoOnOff_1 =
    oc_new_resource("InfoOnOff_1", "/p/2", 2, 0);
  oc_resource_bind_resource_type(res_InfoOnOff_1, "urn:knx:dpa.417.51");
  oc_resource_bind_resource_type(res_InfoOnOff_1, "DPT_Switch");
  oc_resource_bind_content_type(res_InfoOnOff_1, APPLICATION_CBOR);
  oc_resource_bind_resource_interface(res_InfoOnOff_1, OC_IF_S); /* if.s */ 
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
  oc_add_resource(res_InfoOnOff_1);
  PRINT("Register Resource 'OnOff_2' with local path \"/p/3\"\n");
  oc_resource_t *res_OnOff_2 =
    oc_new_resource("OnOff_2", "/p/3", 2, 0);
  oc_resource_bind_resource_type(res_OnOff_2, "urn:knx:dpa.417.61");
  oc_resource_bind_resource_type(res_OnOff_2, "DPT_Switch");
  oc_resource_bind_content_type(res_OnOff_2, APPLICATION_CBOR);
  oc_resource_bind_resource_interface(res_OnOff_2, OC_IF_A); /* if.a */ 
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
  oc_resource_set_request_handler(res_OnOff_2, OC_POST, post_OnOff_2, NULL); 
  oc_add_resource(res_OnOff_2);
  PRINT("Register Resource 'InfoOnOff_2' with local path \"/p/4\"\n");
  oc_resource_t *res_InfoOnOff_2 =
    oc_new_resource("InfoOnOff_2", "/p/4", 2, 0);
  oc_resource_bind_resource_type(res_InfoOnOff_2, "urn:knx:dpa.417.51");
  oc_resource_bind_resource_type(res_InfoOnOff_2, "DPT_Switch");
  oc_resource_bind_content_type(res_InfoOnOff_2, APPLICATION_CBOR);
  oc_resource_bind_resource_interface(res_InfoOnOff_2, OC_IF_S); /* if.s */ 
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
  oc_add_resource(res_InfoOnOff_2);
  PRINT("Register Resource 'OnOff_3' with local path \"/p/5\"\n");
  oc_resource_t *res_OnOff_3 =
    oc_new_resource("OnOff_3", "/p/5", 2, 0);
  oc_resource_bind_resource_type(res_OnOff_3, "urn:knx:dpa.417.61");
  oc_resource_bind_resource_type(res_OnOff_3, "DPT_Switch");
  oc_resource_bind_content_type(res_OnOff_3, APPLICATION_CBOR);
  oc_resource_bind_resource_interface(res_OnOff_3, OC_IF_A); /* if.a */ 
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
  oc_resource_set_request_handler(res_OnOff_3, OC_POST, post_OnOff_3, NULL); 
  oc_add_resource(res_OnOff_3);
  PRINT("Register Resource 'InfoOnOff_3' with local path \"/p/6\"\n");
  oc_resource_t *res_InfoOnOff_3 =
    oc_new_resource("InfoOnOff_3", "/p/6", 2, 0);
  oc_resource_bind_resource_type(res_InfoOnOff_3, "urn:knx:dpa.417.51");
  oc_resource_bind_resource_type(res_InfoOnOff_3, "DPT_Switch");
  oc_resource_bind_content_type(res_InfoOnOff_3, APPLICATION_CBOR);
  oc_resource_bind_resource_interface(res_InfoOnOff_3, OC_IF_S); /* if.s */ 
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
  oc_add_resource(res_InfoOnOff_3);
  PRINT("Register Resource 'OnOff_4' with local path \"/p/7\"\n");
  oc_resource_t *res_OnOff_4 =
    oc_new_resource("OnOff_4", "/p/7", 2, 0);
  oc_resource_bind_resource_type(res_OnOff_4, "urn:knx:dpa.417.61");
  oc_resource_bind_resource_type(res_OnOff_4, "DPT_Switch");
  oc_resource_bind_content_type(res_OnOff_4, APPLICATION_CBOR);
  oc_resource_bind_resource_interface(res_OnOff_4, OC_IF_A); /* if.a */ 
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
  oc_resource_set_request_handler(res_OnOff_4, OC_POST, post_OnOff_4, NULL); 
  oc_add_resource(res_OnOff_4);
  PRINT("Register Resource 'InfoOnOff_4' with local path \"/p/8\"\n");
  oc_resource_t *res_InfoOnOff_4 =
    oc_new_resource("InfoOnOff_4", "/p/8", 2, 0);
  oc_resource_bind_resource_type(res_InfoOnOff_4, "urn:knx:dpa.417.51");
  oc_resource_bind_resource_type(res_InfoOnOff_4, "DPT_Switch");
  oc_resource_bind_content_type(res_InfoOnOff_4, APPLICATION_CBOR);
  oc_resource_bind_resource_interface(res_InfoOnOff_4, OC_IF_S); /* if.s */ 
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
  oc_add_resource(res_InfoOnOff_4);
}

/**
 * @brief initiate preset for device
 * current implementation: device reset as command line argument
 * @param device_index the device identifier of the list of devices
 * @param data the supplied data.
 */
STATIC void
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
 * @brief application reset
 *
 * @param device_index the device identifier of the list of devices
 * @param reset_value the knx reset value
 * @param data the supplied data.
 */
STATIC void
reset_cb(size_t device_index, int reset_value, void *data)
{
  (void)device_index;

  PRINT("reset_cb %d\n", reset_value);
}

/**
 * @brief restart the device (application depended)
 *
 * @param device_index the device identifier of the list of devices
 * @param data the supplied data.
 */
STATIC void
restart_cb(size_t device_index, void *data)
{
  (void)device_index;
  (void)data;

  PRINT("-----restart_cb -------\n");
  exit(0);
}

/**
 * @brief set the host name on the device (application depended)
 *
 * @param device_index the device identifier of the list of devices
 * @param host_name the host name to be set on the device
 * @param data the supplied data.
 */
STATIC void
hostname_cb(size_t device_index, oc_string_t host_name, void *data)
{
  (void)device_index;
  (void)data;

  PRINT("-----host name ------- %s\n", oc_string(host_name));
}

/**
 * @brief software update callback
 *
 * @param device_index the device index
 * @param offset the offset of the image
 * @param payload the image data
 * @param len the length of the image data
 * @param data the user data
 */
STATIC void
swu_cb(size_t device_index, size_t offset, uint8_t *payload, size_t len,
       void *data)
{
  (void)device_index;
  char *fname = (char *)data;
  PRINT(" swu_cb %s block=%d size=%d \n", fname, (int)offset, (int)len);

  FILE *fp = fopen(fname, "rw");
  fseek(fp, offset, SEEK_SET);
  size_t written = fwrite(payload, len, 1, fp);
  if (written != len) {
    PRINT(" swu_cb returned %d != %d (expected)\n", (int)written, (int)len);
  }
  fclose(fp);
}

/**
 * @brief initializes the global variables
 * registers and starts the handler
 */
STATIC void
initialize_variables(void)
{
  /* initialize global variables for resources */
  /* if wanted read them from persistent storage */

  g_OnOff_1 = true;   /**< global variable for OnOff_1 */ 
  g_InfoOnOff_1 = true;   /**< global variable for InfoOnOff_1 */ 
  g_OnOff_2 = true;   /**< global variable for OnOff_2 */ 
  g_InfoOnOff_2 = true;   /**< global variable for InfoOnOff_2 */ 
  g_OnOff_3 = true;   /**< global variable for OnOff_3 */ 
  g_InfoOnOff_3 = true;   /**< global variable for InfoOnOff_3 */ 
  g_OnOff_4 = true;   /**< global variable for OnOff_4 */ 
  g_InfoOnOff_4 = true;   /**< global variable for InfoOnOff_4 */ 
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
  PRINT("\tstorage at 'knx_iot_virtual_sa' \n");
  oc_storage_config("./knx_iot_virtual_sa");


  /*initialize the variables */
  initialize_variables();

  /* initializes the handlers structure */
  STATIC oc_handler_t handler = { .init = app_init,
                                  .signal_event_loop = signal_event_loop,
                                  .register_resources = register_resources,
                                  .requests_entry = NULL };

  /* set the application callbacks */
  oc_set_hostname_cb(hostname_cb, NULL);
  oc_set_reset_cb(reset_cb, NULL);
  oc_set_restart_cb(restart_cb, NULL);
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
STATIC void
signal_event_loop(void)
{

#ifndef NO_MAIN
  WakeConditionVariable(&cv);
#endif /* NO_MAIN */
}
#endif /* WIN32 */

#ifndef NO_MAIN

#ifdef __linux__
/**
 * @brief signal the event loop (Linux)
 * wakes up the main function to handle the next callback
 */
STATIC void
signal_event_loop(void)
{
  pthread_mutex_lock(&mutex);
  pthread_cond_signal(&cv);
  pthread_mutex_unlock(&mutex);
}
#endif /* __linux__ */

/**
 * @brief handle Ctrl-C
 * @param signal the captured signal
 */
STATIC void
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
STATIC void
print_usage()
{
  PRINT("Usage:\n");
  PRINT("no arguments : starts the server\n");
  PRINT("-help  : this message\n");
  PRINT("reset  : does an full reset of the device\n");
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
    printf("argv[%d] = %s\n", i, argv[i]);
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
