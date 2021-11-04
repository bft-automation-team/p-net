/*********************************************************************
 *        _       _         _
 *  _ __ | |_  _ | |  __ _ | |__   ___
 * | '__|| __|(_)| | / _` || '_ \ / __|
 * | |   | |_  _ | || (_| || |_) |\__ \
 * |_|    \__|(_)|_| \__,_||_.__/ |___/
 *
 * www.rt-labs.com
 * Copyright 2021 rt-labs AB, Sweden.
 *
 * This software is dual-licensed under GPLv3 and a commercial
 * license. See the file LICENSE.md distributed with this software for
 * full license information.
 ********************************************************************/

#include "app_data.h"
#include "app_utils.h"
#include "app_gsdml.h"
#include "app_log.h"
#include "sampleapp_common.h"
#include "osal.h"
#include "pnal.h"
#include <pnet_api.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define APP_DATA_DEFAULT_OUTPUT_DATA 0

/* Parameters data
 * Todo: Data is always in pnio data format. Add conversion to uint32_t.
 */
uint32_t app_param = 0;

/* Process data */
uint8_t inputdata[APP_GSDML_INPUT_DATA_SIZE] = {0};
uint8_t outputdata[APP_GSDML_OUTPUT_DATA_SIZE] = {0};
uint16_t previous_output_state[APP_GSDML_OUTPUT_DATA_SIZE / 2] = {0};


/**
 * Reverts values of a byte.
 *
 * @param b        In:    Input byte
 * @return         Reversed byte
 */
unsigned char reverse_byte(unsigned char b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}

/**
 * Set output state.
 *
 * Compares new state with previous state, to minimize system calls.
 *
 * @param led_state        In:    New LED state
 */
static void app_handle_output (uint16_t * output_data)
{
   for (int i = 0; i < APP_GSDML_OUTPUT_DATA_SIZE / 2; i++) {
      if (output_data[i] != previous_output_state[i]) {
         app_set_output_state(output_data);
         for (int j = 0; j < APP_GSDML_OUTPUT_DATA_SIZE / 2; j++) {
            previous_output_state[j] = output_data[j];
         }
         return;
      }
   }
}

uint8_t * app_data_get_input_data (
   uint32_t submodule_id,
   uint16_t * plexus_outputs,
   uint16_t * size,
   uint8_t * iops)
{
   if (size == NULL || iops == NULL)
   {
      return NULL;
   }

   if (submodule_id != APP_GSDML_SUBMOD_ID_ANALOG_DIGITAL_IN_OUT)
   {
      /* Automated RT Tester scenario 2 - unsupported (sub)module */
      *iops = PNET_IOXS_BAD;
      return NULL;
   }
   
   struct timespec spec;
   clock_gettime(CLOCK_REALTIME, &spec);

   /* Prepare input data.
    * Most 2 significant bytes: Digital inputs (16 bits)
    * Lowest 32 bits: Analog inputs (16 uint16_t)    
    */
   // conversion from original data type to bytes, for communication
   for (int i = 0; i < APP_GSDML_INPUT_DATA_SIZE_DIGITAL / 2; i++) {
      inputdata[(i*2)] = plexus_outputs[i] >> 8;
      inputdata[(i*2) + 1] = plexus_outputs[i] & 0xFF;
      APP_LOG_DEBUG ("* %d app_data_get_input_data => Digital input %d "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN" (%d)\n", spec.tv_sec, i,
         BYTE_TO_BINARY(plexus_outputs[i]>>8), BYTE_TO_BINARY(plexus_outputs[i]), plexus_outputs[i]);
   }

   for (int i = 0; i < APP_GSDML_INPUT_DATA_SIZE_ANALOG / 2; i++) {
      inputdata[APP_GSDML_INPUT_DATA_SIZE_DIGITAL + (i*2)] = (plexus_outputs[(APP_GSDML_INPUT_DATA_SIZE_DIGITAL / 2) + i] >> 8);
      inputdata[APP_GSDML_INPUT_DATA_SIZE_DIGITAL + (i*2) + 1] = plexus_outputs[(APP_GSDML_INPUT_DATA_SIZE_DIGITAL / 2) + i] & 0xFF;
      // if (i == 0) {
      //    APP_LOG_DEBUG ("* app_data_get_input_data => Analog input "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN" (%d)\n",
      //       BYTE_TO_BINARY(plexus_outputs[(APP_GSDML_INPUT_DATA_SIZE_DIGITAL / 2) + i]>>8),
      //       BYTE_TO_BINARY(plexus_outputs[(APP_GSDML_INPUT_DATA_SIZE_DIGITAL / 2) + i]),
      //       plexus_outputs[(APP_GSDML_INPUT_DATA_SIZE_DIGITAL / 2) + i]);
      //    APP_LOG_DEBUG ("* app_data_get_input_data => Analog input "BYTE_TO_BINARY_PATTERN" (%d) "BYTE_TO_BINARY_PATTERN" (%d)\n",
      //       BYTE_TO_BINARY(inputdata[APP_GSDML_INPUT_DATA_SIZE_DIGITAL + (i*2)]),
      //       inputdata[APP_GSDML_INPUT_DATA_SIZE_DIGITAL + (i*2)],
      //       BYTE_TO_BINARY(inputdata[APP_GSDML_INPUT_DATA_SIZE_DIGITAL + (i*2) + 1]),
      //       inputdata[APP_GSDML_INPUT_DATA_SIZE_DIGITAL + (i*2) + 1]);
      // }
   }

   *size = APP_GSDML_INPUT_DATA_SIZE;
   *iops = PNET_IOXS_GOOD;

   return inputdata;
}

int app_data_set_output_data (
   uint32_t submodule_id,
   uint8_t * data,
   uint16_t size)
{
   if (data != NULL && size == APP_GSDML_OUTPUT_DATA_SIZE)
   {
      struct timespec spec;
      clock_gettime(CLOCK_REALTIME, &spec);
      
      if (submodule_id == APP_GSDML_SUBMOD_ID_ANALOG_DIGITAL_IN_OUT)
      {
         uint16_t data_for_plexus_input[APP_GSDML_OUTPUT_DATA_SIZE / 2] = {0};
         memcpy (outputdata, data, size);    // outputdata comes from Codesys PNIO mapped OUTPUTS!

         for (int i = 0; i < APP_GSDML_OUTPUT_DATA_SIZE / 2; i++) {
            data_for_plexus_input[i] = ((uint16_t)data[(i * 2)] << 8) | data[(i * 2) + 1];
            // APP_LOG_DEBUG ("* value %d = %d \n", i, data_for_plexus_input[i]);
            // if (i == 2) {
            //    uint16_t analog_output = data_for_plexus_input[i];
            //    APP_LOG_DEBUG ("* %d app_data_set_output_data => Output %d "BYTE_TO_BINARY_PATTERN" "BYTE_TO_BINARY_PATTERN" (%d)\n", spec.tv_sec, i,
            //       BYTE_TO_BINARY(outputdata[i]), BYTE_TO_BINARY(outputdata[i+1]), analog_output);
            // }
         }
         app_handle_output (data_for_plexus_input);
         return 0;
      }
   }
   return -1;
}

int app_data_set_default_outputs (void)
{
   for (int i = 0; i < APP_GSDML_OUTPUT_DATA_SIZE; i++) {
      outputdata[i] = APP_DATA_DEFAULT_OUTPUT_DATA;
   }

   uint16_t data_for_plexus_input[APP_GSDML_OUTPUT_DATA_SIZE / 2] = {0};
   app_handle_output (data_for_plexus_input);
   
   return 0;
}

int app_data_write_parameter (
   uint32_t submodule_id,
   uint32_t index,
   const uint8_t * data,
   uint16_t length)
{
   const app_gsdml_param_t * par_cfg;

   par_cfg = app_gsdml_get_parameter_cfg (submodule_id, index);
   if (par_cfg == NULL)
   {
      APP_LOG_WARNING (
         "PLC write request unsupported submodule/parameter. "
         "Submodule id: %u Index: %u\n",
         (unsigned)submodule_id,
         (unsigned)index);
      return -1;
   }

   if (length != par_cfg->length)
   {
      APP_LOG_WARNING (
         "PLC write request unsupported length. "
         "Index: %u Length: %u Expected length: %u\n",
         (unsigned)index,
         (unsigned)length,
         par_cfg->length);
      return -1;
   }

   if (index == APP_GSDM_PARAMETER_IDX)
   {
      memcpy (&app_param, data, sizeof (length));
   }
   APP_LOG_DEBUG ("  Writing %s\n", par_cfg->name);
   app_log_print_bytes (APP_LOG_LEVEL_DEBUG, data, length);

   return 0;
}

int app_data_read_parameter (
   uint32_t submodule_id,
   uint32_t index,
   uint8_t ** data,
   uint16_t * length)
{
   const app_gsdml_param_t * par_cfg;

   par_cfg = app_gsdml_get_parameter_cfg (submodule_id, index);
   if (par_cfg == NULL)
   {
      APP_LOG_WARNING (
         "PLC read request unsupported submodule/parameter. "
         "Submodule id: %u Index: %u\n",
         (unsigned)submodule_id,
         (unsigned)index);
      return -1;
   }

   if (*length < par_cfg->length)
   {
      APP_LOG_WARNING (
         "PLC read request unsupported length. "
         "Index: %u Length: %u Expected length: %u\n",
         (unsigned)index,
         (unsigned)*length,
         par_cfg->length);
      return -1;
   }

   APP_LOG_DEBUG ("  Reading %s\n", par_cfg->name);
   if (index == APP_GSDM_PARAMETER_IDX)
   {
      *data = (uint8_t *)&app_param;
      *length = sizeof (app_param);
   }

   app_log_print_bytes (APP_LOG_LEVEL_DEBUG, *data, *length);

   return 0;
}