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

// URL defines
#define URL_ONOFF_1 "/p/o_1_1" // define URL OnOff_1 for /p/o_1_1
#define URL_INFOONOFF_1 "/p/o_2_2" // define URL InfoOnOff_1 for /p/o_2_2
#define URL_ONOFF_2 "/p/o_3_3" // define URL OnOff_2 for /p/o_3_3
#define URL_INFOONOFF_2 "/p/o_4_4" // define URL InfoOnOff_2 for /p/o_4_4
#define URL_ONOFF_3 "/p/o_5_5" // define URL OnOff_3 for /p/o_5_5
#define URL_INFOONOFF_3 "/p/o_6_6" // define URL InfoOnOff_3 for /p/o_6_6
#define URL_ONOFF_4 "/p/o_7_7" // define URL OnOff_4 for /p/o_7_7
#define URL_INFOONOFF_4 "/p/o_8_8" // define URL InfoOnOff_4 for /p/o_8_8



/**
 * Callback invoked by the stack when a successfull put is done
 *
 * @param[in] url the url of the put
 *
 */
typedef void (*oc_put_cb_t)(char* url);

/**
 * @brief The put callback
 *
 */
typedef struct oc_put_struct_t
{
  oc_put_cb_t cb; /**< the put callback, e.g. when something has changed */
} oc_put_struct_t;

/**
 * @brief set the put callback (on application level)
 * 
 * @param cb the callback
 */
void app_set_put_cb(oc_put_cb_t cb);

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


// Getters/Setters for bool
/**
 * @brief Checks if the url depicts a bool
 * 
 * @param url the url 
 * @return true: url conveys a bool
 */
bool app_is_bool_url(char* url);

/**
 * @brief Set a bool
 * 
 * @param url the url for the bool to set
 * @param in value to set
 */
void app_set_bool_variable(char* url, bool value);

/**
 * @brief Get a bool
 * 
 * @param url the url for the bool to get
 * @return boolean variable
 */
bool app_retrieve_bool_variable(char *url);
 

/**
 * @brief checks if the url represents a parameter
 *
 * @param url the url
 * @return true the url represents a parameter
 */
bool app_is_url_parameter(char* url);

/**
 * @brief retrieves the url of a parameter
 * index starts at 1
 * @param index the index to retrieve the url from
 * @return the url or NULL
 */
char* app_get_parameter_url(int index);

/**
 * @brief retrieves the name of a parameter
 * index starts at 1
 * @param index the index to retrieve the parameter name from
 * @return the name or NULL
 */
char* app_get_parameter_name(int index);

/**
 * @brief sets the fault state of the url/data point 
 * the caller needs to know if the resource/data point implements a fault situation
 * 
 * @param url the url of the resource/data point
 * @param value the boolean fault value to be set
 */
void app_set_fault_variable(char* url, bool value);

/**
 * @brief checks if the url is in use (e.g. used in the Group Object Table)
 * 
 * @param url the url of the resource/data point
 * @return true: entry in Group Object Table has the URL
 * @return false: No entry in Group Object Table has the URL
 */
bool app_is_url_in_use(char* url);

/**
 * @brief function to report if the (oscore) security is turn on for this instance
 * 
 * @return true is secure
 * @return false is not secure
 */
bool app_is_secure();

/**
 * @brief retrieves the password for showing on screen
 * 
 * @return password (as string)
 */
char* app_get_password();

/**
 * @brief function to set the input string to upper case
 *
 * @param str the string to make upper case
 *
 */
void app_str_to_upper(char *str);

/**
 * @brief Function to be used as a callback for when a button is pressed.
 *        This function will toggle the value of the url, and send a message.
 * 
 * @param url the url of the resource/data point
 */
void dev_btn_toggle_cb(char *url);

#ifdef __cplusplus
}
#endif
