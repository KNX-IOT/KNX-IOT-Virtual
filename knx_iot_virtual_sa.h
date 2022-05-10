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
/**
 * @file
 *
 * header file for the generated application.
 * header file contains functions to use the generated application with an external main.
 * e.g. if the c code is compiled without main then 
 * these functions can be used to call all generated code
 *
 */

#include "oc_api.h"
#include "oc_core_res.h"
#include "port/oc_clock.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Callback invoked by the stack when a successfull post is done
 *
 * @param[in] url the url of the post
 *
 */
typedef void (*oc_post_cb_t)(char* url);

/**
 * @brief The post callback
 *
 */
typedef struct oc_post_struct_t
{
  oc_post_cb_t cb; /**< the post callback, e.g. when something has changed */
} oc_post_struct_t;

/**
 * @brief set the post callback (on application level)
 * 
 * @param cb the callback
 */
void app_set_post_cb(oc_post_cb_t cb);

/**
 * @brief initialize the stack
 * 
 * @return int 0 == success
 */
int app_initialize_stack();

/**
 * @brief sets the serial number
 * should be called before app_initialize_stack()
 * 
 * @param serial_number the serial number as string
 * @return int 0 == success
 */
int app_set_serial_number(char* serial_number);



bool app_is_bool_url(char* url);

/**
 * @brief retrieve the boolean variable of the url/data point
 * the caller needs to know if the resource/data point is conveying a boolean
 * "/p/o_1" of OnOff_1 
 * "/p/o_2" of InfoOnOff_1 
 * "/p/o_3" of OnOff_2 
 * "/p/o_4" of InfoOnOff_2 
 * "/p/o_5" of OnOff_3 
 * "/p/o_6" of InfoOnOff_3 
 * "/p/o_7" of OnOff_4 
 * "/p/o_8" of InfoOnOff_4 
 * 
 * @param url the url of the resource/data point
 * @return true value is true
 * @return false value is false or error.
 */
bool app_retrieve_bool_variable(char* url);

/**
 * @brief set the boolean variable that belongs to the url 
 * The caller needs to know if the resource/data point is conveying a boolean
 * "/p/o_1" of OnOff_1
 * "/p/o_3" of OnOff_2
 * "/p/o_5" of OnOff_3
 * "/p/o_7" of OnOff_4
 * 
 * @param url the url of the resource/data point
 * @param value the boolean value to be set
 */
void app_set_bool_variable(char* url, bool value);

bool app_is_int_url(char* url);

/**
 * @brief retrieve the int variable of the url/data point
 * the caller needs to know if the resource/data point is conveying a integer
 * 
 * @param url the url of the resource/data point
 * @return true value is true
 * @return false value is false or error.
 */
int app_retrieve_int_variable(char* url);

/**
 * @brief set the integer variable that belongs to the url 
 * The caller needs to know if the resource/data point is conveying a integer
 * 
 * @param url the url of the resource/data point
 * @param value the boolean value to be set
 */
void app_set_integer_variable(char* url, int value);

/**
 * @brief function to check if the url is represented by a string
 *
 * @param true = url value is a string
 * @param false = url is not a string
 */

bool app_is_string_url(char* url);
/**
 * @brief sets the global string variable at the url
 *
 * @param url the url indicating the global variable
 * @param value the value to be set
 */
 
void app_set_string_variable(char* url, char* value);
/**
 * @brief retrieve the global string variable at the url
 *
 * @param url the url indicating the global variable
 * @return the value of the variable
 */
char* app_retrieve_string_variable(char* url);

/**
 * @brief checks if the url represents a parameter
 *
 * @param url the url
 * @return true the url represents a parameter
 */
bool app_is_url_parameter(char* url);

char* app_get_parameter_url(int index);

char* app_get_parameter_name(int index);



/**
 * @brief retrieve the fault state of the url/data point
 * the caller needs to know if the resource/data point implements a fault situation
 * 
 * @param url the url of the resource/data point
 * @return true value is true
 * @return false value is false or error.
 */
bool app_retrieve_fault_variable(char* url);

/**
 * @brief sets the fault state of the url/data point 
 * the caller needs to know if the resource/data point implements a fault situation
 * 
 * @param url the url of the resource/data point
 * @param value the boolean fault value to be set
 */
void app_set_fault_variable(char* url, bool value);

/**
 * @brief function to report if the (oscore) security is turn on for this instance
 * 
 * @return true is secure
 * @return false is not secure
 */
bool app_is_secure();


#ifdef __cplusplus
}
#endif
