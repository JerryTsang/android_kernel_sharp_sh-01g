/* Copyright (c) 2013-2016, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/slab.h>
#include <sound/apr_audio-v2.h>
#include <sound/q6asm-v2.h>
#include <sound/compress_params.h>
#include "msm-audio-effects-q6-v2.h"

#define GET_NEXT(ptr, upper_limit, rc)                                  \
({                                                                      \
	if (((ptr) + 1) > (upper_limit)) {                              \
		pr_err("%s: param list out of boundary\n", __func__);   \
		(rc) = -EINVAL;                                         \
	}                                                               \
	((rc) == 0) ? *(ptr)++ :  -EINVAL;                              \
})

#define CHECK_PARAM_LEN(len, max_len, tag, rc)                          \
do {                                                                    \
	if ((len) > (max_len)) {                                        \
		pr_err("%s: params length overflows\n", (tag));         \
		(rc) = -EINVAL;                                         \
	}                                                               \
} while (0)


int msm_audio_effects_virtualizer_handler(struct audio_client *ac,
				struct virtualizer_params *virtualizer,
				long *values)
{
	long *param_max_offset = values + MAX_PP_PARAMS_SZ - 1;
	char *params = NULL;
	int rc = 0;
	int devices = GET_NEXT(values, param_max_offset, rc);
	int num_commands = GET_NEXT(values, param_max_offset, rc);
	int *updt_params, i, prev_enable_flag;
	uint32_t params_length = (MAX_INBAND_PARAM_SZ);

	pr_debug("%s\n", __func__);
	if (!ac || (devices == -EINVAL) || (num_commands == -EINVAL)) {
		pr_err("%s: cannot set audio effects\n", __func__);
		return -EINVAL;
	}
	params = kzalloc(params_length, GFP_KERNEL);
	if (!params) {
		pr_err("%s, params memory alloc failed\n", __func__);
		return -ENOMEM;
	}
	pr_debug("%s: device: %d\n", __func__, devices);
	updt_params = (int *)params;
	params_length = 0;
	for (i = 0; i < num_commands; i++) {
		uint32_t command_id =
			GET_NEXT(values, param_max_offset, rc);
		uint32_t command_config_state =
			GET_NEXT(values, param_max_offset, rc);
		uint32_t index_offset =
			GET_NEXT(values, param_max_offset, rc);
		uint32_t length =
			GET_NEXT(values, param_max_offset, rc);
		switch (command_id) {
		case VIRTUALIZER_ENABLE:
			pr_debug("%s: VIRTUALIZER_ENABLE\n", __func__);
			if (length != 1 || index_offset != 0) {
				pr_err("no valid params\n");
				rc = -EINVAL;
				goto invalid_config;
			}
			prev_enable_flag = virtualizer->enable_flag;
			virtualizer->enable_flag =
				GET_NEXT(values, param_max_offset, rc);
			if (prev_enable_flag != virtualizer->enable_flag) {
				params_length += COMMAND_PAYLOAD_SZ +
					VIRTUALIZER_ENABLE_PARAM_SZ;
				CHECK_PARAM_LEN(params_length,
						MAX_INBAND_PARAM_SZ,
						"VIRT ENABLE", rc);
				if (rc != 0)
					break;
				*updt_params++ =
				AUDPROC_MODULE_ID_VIRTUALIZER;
				*updt_params++ =
				AUDPROC_PARAM_ID_VIRTUALIZER_ENABLE;
				*updt_params++ =
				VIRTUALIZER_ENABLE_PARAM_SZ;
				*updt_params++ =
				virtualizer->enable_flag;
			}
			break;
		case VIRTUALIZER_STRENGTH:
			pr_debug("%s: VIRTUALIZER_STRENGTH\n", __func__);
			if (length != 1 || index_offset != 0) {
				pr_err("no valid params\n");
				rc = -EINVAL;
				goto invalid_config;
			}
			virtualizer->strength =
				GET_NEXT(values, param_max_offset, rc);
			if (command_config_state == CONFIG_SET) {
				params_length += COMMAND_PAYLOAD_SZ +
					VIRTUALIZER_STRENGTH_PARAM_SZ;
				CHECK_PARAM_LEN(params_length,
						MAX_INBAND_PARAM_SZ,
						"VIRT STRENGTH", rc);
				if (rc != 0)
					break;
				*updt_params++ =
					AUDPROC_MODULE_ID_VIRTUALIZER;
				*updt_params++ =
					AUDPROC_PARAM_ID_VIRTUALIZER_STRENGTH;
				*updt_params++ =
					VIRTUALIZER_STRENGTH_PARAM_SZ;
				*updt_params++ =
					virtualizer->strength;
			}
			break;
		case VIRTUALIZER_OUT_TYPE:
			pr_debug("%s: VIRTUALIZER_OUT_TYPE\n", __func__);
			if (length != 1 || index_offset != 0) {
				pr_err("no valid params\n");
				rc = -EINVAL;
				goto invalid_config;
			}
			virtualizer->out_type =
				GET_NEXT(values, param_max_offset, rc);
			if (command_config_state == CONFIG_SET) {
				params_length += COMMAND_PAYLOAD_SZ +
					VIRTUALIZER_OUT_TYPE_PARAM_SZ;
				CHECK_PARAM_LEN(params_length,
						MAX_INBAND_PARAM_SZ,
						"VIRT OUT_TYPE", rc);
				if (rc != 0)
					break;
				*updt_params++ =
					AUDPROC_MODULE_ID_VIRTUALIZER;
				*updt_params++ =
					AUDPROC_PARAM_ID_VIRTUALIZER_OUT_TYPE;
				*updt_params++ =
					VIRTUALIZER_OUT_TYPE_PARAM_SZ;
				*updt_params++ =
					virtualizer->out_type;
			}
			break;
		case VIRTUALIZER_GAIN_ADJUST:
			pr_debug("%s: VIRTUALIZER_GAIN_ADJUST\n", __func__);
			if (length != 1 || index_offset != 0) {
				pr_err("no valid params\n");
				rc = -EINVAL;
				goto invalid_config;
			}
			virtualizer->gain_adjust =
				GET_NEXT(values, param_max_offset, rc);
			if (command_config_state == CONFIG_SET) {
				params_length += COMMAND_PAYLOAD_SZ +
					VIRTUALIZER_GAIN_ADJUST_PARAM_SZ;
				CHECK_PARAM_LEN(params_length,
						MAX_INBAND_PARAM_SZ,
						"VIRT GAIN_ADJUST", rc);
				if (rc != 0)
					break;
				*updt_params++ =
				AUDPROC_MODULE_ID_VIRTUALIZER;
				*updt_params++ =
				AUDPROC_PARAM_ID_VIRTUALIZER_GAIN_ADJUST;
				*updt_params++ =
				VIRTUALIZER_GAIN_ADJUST_PARAM_SZ;
				*updt_params++ =
				virtualizer->gain_adjust;
			}
			break;
		default:
			pr_err("%s: Invalid command to set config\n", __func__);
			break;
		}
	}
        if (params_length)
		q6asm_send_audio_effects_params(ac, params,
						params_length);
invalid_config:
	kfree(params);
	return rc;
}

int msm_audio_effects_reverb_handler(struct audio_client *ac,
				     struct reverb_params *reverb,
				     long *values)
{
	long *param_max_offset = values + MAX_PP_PARAMS_SZ - 1;
	char *params = NULL;
	int rc = 0;
	int devices = GET_NEXT(values, param_max_offset, rc);
	int num_commands = GET_NEXT(values, param_max_offset, rc);
	int *updt_params, i, prev_enable_flag;
	uint32_t params_length = (MAX_INBAND_PARAM_SZ);

	pr_debug("%s\n", __func__);
	if (!ac || (devices == -EINVAL) || (num_commands == -EINVAL)) {
		pr_err("%s: cannot set audio effects\n", __func__);
		return -EINVAL;
	}
	params = kzalloc(params_length, GFP_KERNEL);
	if (!params) {
		pr_err("%s, params memory alloc failed\n", __func__);
		return -ENOMEM;
	}
	pr_debug("%s: device: %d\n", __func__, devices);
	updt_params = (int *)params;
	params_length = 0;
	for (i = 0; i < num_commands; i++) {
		uint32_t command_id =
			GET_NEXT(values, param_max_offset, rc);
		uint32_t command_config_state =
			GET_NEXT(values, param_max_offset, rc);
		uint32_t index_offset =
			GET_NEXT(values, param_max_offset, rc);
		uint32_t length =
			GET_NEXT(values, param_max_offset, rc);
		switch (command_id) {
		case REVERB_ENABLE:
			pr_debug("%s: REVERB_ENABLE\n", __func__);
			if (length != 1 || index_offset != 0) {
				pr_err("no valid params\n");
				rc = -EINVAL;
				goto invalid_config;
			}
			prev_enable_flag = reverb->enable_flag;
			reverb->enable_flag =
				GET_NEXT(values, param_max_offset, rc);
			if (prev_enable_flag != reverb->enable_flag) {
				params_length += COMMAND_PAYLOAD_SZ +
					REVERB_ENABLE_PARAM_SZ;
				CHECK_PARAM_LEN(params_length,
						MAX_INBAND_PARAM_SZ,
						"REVERB_ENABLE", rc);
				if (rc != 0)
					break;
				*updt_params++ =
					AUDPROC_MODULE_ID_REVERB;
				*updt_params++ =
					AUDPROC_PARAM_ID_REVERB_ENABLE;
				*updt_params++ =
					REVERB_ENABLE_PARAM_SZ;
				*updt_params++ =
					reverb->enable_flag;
			}
			break;
		case REVERB_MODE:
			pr_debug("%s: REVERB_MODE\n", __func__);
			if (length != 1 || index_offset != 0) {
				pr_err("no valid params\n");
				rc = -EINVAL;
				goto invalid_config;
			}
			reverb->mode =
				GET_NEXT(values, param_max_offset, rc);
			if (command_config_state == CONFIG_SET) {
				params_length += COMMAND_PAYLOAD_SZ +
					REVERB_MODE_PARAM_SZ;
				CHECK_PARAM_LEN(params_length,
						MAX_INBAND_PARAM_SZ,
						"REVERB_MODE", rc);
				if (rc != 0)
					break;
				*updt_params++ =
					AUDPROC_MODULE_ID_REVERB;
				*updt_params++ =
					AUDPROC_PARAM_ID_REVERB_MODE;
				*updt_params++ =
					REVERB_MODE_PARAM_SZ;
				*updt_params++ =
					reverb->mode;
			}
			break;
		case REVERB_PRESET:
			pr_debug("%s: REVERB_PRESET\n", __func__);
			if (length != 1 || index_offset != 0) {
				pr_err("no valid params\n");
				rc = -EINVAL;
				goto invalid_config;
			}
			reverb->preset =
				GET_NEXT(values, param_max_offset, rc);
			if (command_config_state == CONFIG_SET) {
				params_length += COMMAND_PAYLOAD_SZ +
					REVERB_PRESET_PARAM_SZ;
				CHECK_PARAM_LEN(params_length,
						MAX_INBAND_PARAM_SZ,
						"REVERB_PRESET", rc);
				if (rc != 0)
					break;
				*updt_params++ =
					AUDPROC_MODULE_ID_REVERB;
				*updt_params++ =
					AUDPROC_PARAM_ID_REVERB_PRESET;
				*updt_params++ =
					REVERB_PRESET_PARAM_SZ;
				*updt_params++ =
					reverb->preset;
			}
			break;
		case REVERB_WET_MIX:
			pr_debug("%s: REVERB_WET_MIX\n", __func__);
			if (length != 1 || index_offset != 0) {
				pr_err("no valid params\n");
				rc = -EINVAL;
				goto invalid_config;
			}
			reverb->wet_mix =
				GET_NEXT(values, param_max_offset, rc);
			if (command_config_state == CONFIG_SET) {
				params_length += COMMAND_PAYLOAD_SZ +
					REVERB_WET_MIX_PARAM_SZ;
				CHECK_PARAM_LEN(params_length,
						MAX_INBAND_PARAM_SZ,
						"REVERB_WET_MIX", rc);
				if (rc != 0)
					break;
				*updt_params++ =
					AUDPROC_MODULE_ID_REVERB;
				*updt_params++ =
					AUDPROC_PARAM_ID_REVERB_WET_MIX;
				*updt_params++ =
					REVERB_WET_MIX_PARAM_SZ;
				*updt_params++ =
					reverb->wet_mix;
			}
			break;
		case REVERB_GAIN_ADJUST:
			pr_debug("%s: REVERB_GAIN_ADJUST\n", __func__);
			if (length != 1 || index_offset != 0) {
				pr_err("no valid params\n");
				rc = -EINVAL;
				goto invalid_config;
			}
			reverb->gain_adjust =
				GET_NEXT(values, param_max_offset, rc);
			if (command_config_state == CONFIG_SET) {
				params_length += COMMAND_PAYLOAD_SZ +
					REVERB_GAIN_ADJUST_PARAM_SZ;
				CHECK_PARAM_LEN(params_length,
						MAX_INBAND_PARAM_SZ,
						"REVERB_GAIN_ADJUST", rc);
				if (rc != 0)
					break;
				*updt_params++ =
					AUDPROC_MODULE_ID_REVERB;
				*updt_params++ =
					AUDPROC_PARAM_ID_REVERB_GAIN_ADJUST;
				*updt_params++ =
					REVERB_GAIN_ADJUST_PARAM_SZ;
				*updt_params++ =
					reverb->gain_adjust;
			}
			break;
		case REVERB_ROOM_LEVEL:
			pr_debug("%s: REVERB_ROOM_LEVEL\n", __func__);
			if (length != 1 || index_offset != 0) {
				pr_err("no valid params\n");
				rc = -EINVAL;
				goto invalid_config;
			}
			reverb->room_level =
				GET_NEXT(values, param_max_offset, rc);
			if (command_config_state == CONFIG_SET) {
				params_length += COMMAND_PAYLOAD_SZ +
					REVERB_ROOM_LEVEL_PARAM_SZ;
				CHECK_PARAM_LEN(params_length,
						MAX_INBAND_PARAM_SZ,
						"REVERB_ROOM_LEVEL", rc);
				if (rc != 0)
					break;
				*updt_params++ =
					AUDPROC_MODULE_ID_REVERB;
				*updt_params++ =
					AUDPROC_PARAM_ID_REVERB_ROOM_LEVEL;
				*updt_params++ =
					REVERB_ROOM_LEVEL_PARAM_SZ;
				*updt_params++ =
					reverb->room_level;
			}
			break;
		case REVERB_ROOM_HF_LEVEL:
			pr_debug("%s: REVERB_ROOM_HF_LEVEL\n", __func__);
			if (length != 1 || index_offset != 0) {
				pr_err("no valid params\n");
				rc = -EINVAL;
				goto invalid_config;
			}
			reverb->room_hf_level =
				GET_NEXT(values, param_max_offset, rc);
			if (command_config_state == CONFIG_SET) {
				params_length += COMMAND_PAYLOAD_SZ +
					REVERB_ROOM_HF_LEVEL_PARAM_SZ;
				CHECK_PARAM_LEN(params_length,
						MAX_INBAND_PARAM_SZ,
						"REVERB_ROOM_HF_LEVEL", rc);
				if (rc != 0)
					break;
				*updt_params++ =
					AUDPROC_MODULE_ID_REVERB;
				*updt_params++ =
					AUDPROC_PARAM_ID_REVERB_ROOM_HF_LEVEL;
				*updt_params++ =
					REVERB_ROOM_HF_LEVEL_PARAM_SZ;
				*updt_params++ =
					reverb->room_hf_level;
			}
			break;
		case REVERB_DECAY_TIME:
			pr_debug("%s: REVERB_DECAY_TIME\n", __func__);
			if (length != 1 || index_offset != 0) {
				pr_err("no valid params\n");
				rc = -EINVAL;
				goto invalid_config;
			}
			reverb->decay_time =
				GET_NEXT(values, param_max_offset, rc);
			if (command_config_state == CONFIG_SET) {
				params_length += COMMAND_PAYLOAD_SZ +
					REVERB_DECAY_TIME_PARAM_SZ;
				CHECK_PARAM_LEN(params_length,
						MAX_INBAND_PARAM_SZ,
						"REVERB_DECAY_TIME", rc);
				if (rc != 0)
					break;
				*updt_params++ =
					AUDPROC_MODULE_ID_REVERB;
				*updt_params++ =
					AUDPROC_PARAM_ID_REVERB_DECAY_TIME;
				*updt_params++ =
					REVERB_DECAY_TIME_PARAM_SZ;
				*updt_params++ =
					reverb->decay_time;
			}
			break;
		case REVERB_DECAY_HF_RATIO:
			pr_debug("%s: REVERB_DECAY_HF_RATIO\n", __func__);
			if (length != 1 || index_offset != 0) {
				pr_err("no valid params\n");
				rc = -EINVAL;
				goto invalid_config;
			}
			reverb->decay_hf_ratio =
				GET_NEXT(values, param_max_offset, rc);
			if (command_config_state == CONFIG_SET) {
				params_length += COMMAND_PAYLOAD_SZ +
					REVERB_DECAY_HF_RATIO_PARAM_SZ;
				CHECK_PARAM_LEN(params_length,
						MAX_INBAND_PARAM_SZ,
						"REVERB_DECAY_HF_RATIO", rc);
				if (rc != 0)
					break;
				*updt_params++ =
					AUDPROC_MODULE_ID_REVERB;
				*updt_params++ =
					AUDPROC_PARAM_ID_REVERB_DECAY_HF_RATIO;
				*updt_params++ =
					REVERB_DECAY_HF_RATIO_PARAM_SZ;
				*updt_params++ =
					reverb->decay_hf_ratio;
			}
			break;
		case REVERB_REFLECTIONS_LEVEL:
			pr_debug("%s: REVERB_REFLECTIONS_LEVEL\n", __func__);
			if (length != 1 || index_offset != 0) {
				pr_err("no valid params\n");
				rc = -EINVAL;
				goto invalid_config;
			}
			reverb->reflections_level =
				GET_NEXT(values, param_max_offset, rc);
			if (command_config_state == CONFIG_SET) {
				params_length += COMMAND_PAYLOAD_SZ +
					REVERB_REFLECTIONS_LEVEL_PARAM_SZ;
				CHECK_PARAM_LEN(params_length,
						MAX_INBAND_PARAM_SZ,
						"REVERB_REFLECTIONS_LEVEL", rc);
				if (rc != 0)
					break;
				*updt_params++ =
				AUDPROC_MODULE_ID_REVERB;
				*updt_params++ =
				AUDPROC_PARAM_ID_REVERB_REFLECTIONS_LEVEL;
				*updt_params++ =
				REVERB_REFLECTIONS_LEVEL_PARAM_SZ;
				*updt_params++ =
				reverb->reflections_level;
			}
			break;
		case REVERB_REFLECTIONS_DELAY:
			pr_debug("%s: REVERB_REFLECTIONS_DELAY\n", __func__);
			if (length != 1 || index_offset != 0) {
				pr_err("no valid params\n");
				rc = -EINVAL;
				goto invalid_config;
			}
			reverb->reflections_delay =
				GET_NEXT(values, param_max_offset, rc);
			if (command_config_state == CONFIG_SET) {
				params_length += COMMAND_PAYLOAD_SZ +
					REVERB_REFLECTIONS_DELAY_PARAM_SZ;
				CHECK_PARAM_LEN(params_length,
						MAX_INBAND_PARAM_SZ,
						"REVERB_REFLECTIONS_DELAY", rc);
				if (rc != 0)
					break;
				*updt_params++ =
				AUDPROC_MODULE_ID_REVERB;
				*updt_params++ =
				AUDPROC_PARAM_ID_REVERB_REFLECTIONS_DELAY;
				*updt_params++ =
				REVERB_REFLECTIONS_DELAY_PARAM_SZ;
				*updt_params++ =
				reverb->reflections_delay;
			}
			break;
		case REVERB_LEVEL:
			pr_debug("%s: REVERB_LEVEL\n", __func__);
			if (length != 1 || index_offset != 0) {
				pr_err("no valid params\n");
				rc = -EINVAL;
				goto invalid_config;
			}
			reverb->level =
				GET_NEXT(values, param_max_offset, rc);
			if (command_config_state == CONFIG_SET) {
				params_length += COMMAND_PAYLOAD_SZ +
					REVERB_LEVEL_PARAM_SZ;
				CHECK_PARAM_LEN(params_length,
						MAX_INBAND_PARAM_SZ,
						"REVERB_LEVEL", rc);
				if (rc != 0)
					break;
				*updt_params++ =
					AUDPROC_MODULE_ID_REVERB;
				*updt_params++ =
					AUDPROC_PARAM_ID_REVERB_LEVEL;
				*updt_params++ =
					REVERB_LEVEL_PARAM_SZ;
				*updt_params++ =
					reverb->level;
			}
			break;
		case REVERB_DELAY:
			pr_debug("%s: REVERB_DELAY\n", __func__);
			if (length != 1 || index_offset != 0) {
				pr_err("no valid params\n");
				rc = -EINVAL;
				goto invalid_config;
			}
			reverb->delay =
				GET_NEXT(values, param_max_offset, rc);
			if (command_config_state == CONFIG_SET) {
				params_length += COMMAND_PAYLOAD_SZ +
					REVERB_DELAY_PARAM_SZ;
				CHECK_PARAM_LEN(params_length,
						MAX_INBAND_PARAM_SZ,
						"REVERB_DELAY", rc);
				if (rc != 0)
					break;
				*updt_params++ =
					AUDPROC_MODULE_ID_REVERB;
				*updt_params++ =
					AUDPROC_PARAM_ID_REVERB_DELAY;
				*updt_params++ =
					REVERB_DELAY_PARAM_SZ;
				*updt_params++ =
					reverb->delay;
			}
			break;
		case REVERB_DIFFUSION:
			pr_debug("%s: REVERB_DIFFUSION\n", __func__);
			if (length != 1 || index_offset != 0) {
				pr_err("no valid params\n");
				rc = -EINVAL;
				goto invalid_config;
			}
			reverb->diffusion =
				GET_NEXT(values, param_max_offset, rc);
			if (command_config_state == CONFIG_SET) {
				params_length += COMMAND_PAYLOAD_SZ +
					REVERB_DIFFUSION_PARAM_SZ;
				CHECK_PARAM_LEN(params_length,
						MAX_INBAND_PARAM_SZ,
						"REVERB_DIFFUSION", rc);
				if (rc != 0)
					break;
				*updt_params++ =
					AUDPROC_MODULE_ID_REVERB;
				*updt_params++ =
					AUDPROC_PARAM_ID_REVERB_DIFFUSION;
				*updt_params++ =
					REVERB_DIFFUSION_PARAM_SZ;
				*updt_params++ =
					reverb->diffusion;
			}
			break;
		case REVERB_DENSITY:
			pr_debug("%s: REVERB_DENSITY\n", __func__);
			if (length != 1 || index_offset != 0) {
				pr_err("no valid params\n");
				rc = -EINVAL;
				goto invalid_config;
			}
			reverb->density =
				GET_NEXT(values, param_max_offset, rc);
			if (command_config_state == CONFIG_SET) {
				params_length += COMMAND_PAYLOAD_SZ +
					REVERB_DENSITY_PARAM_SZ;
				CHECK_PARAM_LEN(params_length,
						MAX_INBAND_PARAM_SZ,
						"REVERB_DENSITY", rc);
				if (rc != 0)
					break;
				*updt_params++ =
					AUDPROC_MODULE_ID_REVERB;
				*updt_params++ =
					AUDPROC_PARAM_ID_REVERB_DENSITY;
				*updt_params++ =
					REVERB_DENSITY_PARAM_SZ;
				*updt_params++ =
					reverb->density;
			}
			break;
		default:
			pr_err("%s: Invalid command to set config\n", __func__);
			break;
		}
	}
        if (params_length)
		q6asm_send_audio_effects_params(ac, params,
						params_length);
invalid_config:
	kfree(params);
	return rc;
}

int msm_audio_effects_bass_boost_handler(struct audio_client *ac,
					struct bass_boost_params *bass_boost,
					long *values)
{
	long *param_max_offset = values + MAX_PP_PARAMS_SZ - 1;
	char *params = NULL;
	int rc = 0;
	int devices = GET_NEXT(values, param_max_offset, rc);
	int num_commands = GET_NEXT(values, param_max_offset, rc);
	int *updt_params, i, prev_enable_flag;
	uint32_t params_length = (MAX_INBAND_PARAM_SZ);

	pr_debug("%s\n", __func__);
	if (!ac || (devices == -EINVAL) || (num_commands == -EINVAL)) {
		pr_err("%s: cannot set audio effects\n", __func__);
		return -EINVAL;
	}
	params = kzalloc(params_length, GFP_KERNEL);
	if (!params) {
		pr_err("%s, params memory alloc failed\n", __func__);
		return -ENOMEM;
	}
	pr_debug("%s: device: %d\n", __func__, devices);
	updt_params = (int *)params;
	params_length = 0;
	for (i = 0; i < num_commands; i++) {
		uint32_t command_id =
			GET_NEXT(values, param_max_offset, rc);
		uint32_t command_config_state =
			GET_NEXT(values, param_max_offset, rc);
		uint32_t index_offset =
			GET_NEXT(values, param_max_offset, rc);
		uint32_t length =
			GET_NEXT(values, param_max_offset, rc);
		switch (command_id) {
		case BASS_BOOST_ENABLE:
			pr_debug("%s: BASS_BOOST_ENABLE\n", __func__);
			if (length != 1 || index_offset != 0) {
				pr_err("no valid params\n");
				rc = -EINVAL;
				goto invalid_config;
			}
			prev_enable_flag = bass_boost->enable_flag;
			bass_boost->enable_flag =
				GET_NEXT(values, param_max_offset, rc);
			if (prev_enable_flag != bass_boost->enable_flag) {
				params_length += COMMAND_PAYLOAD_SZ +
					BASS_BOOST_ENABLE_PARAM_SZ;
				CHECK_PARAM_LEN(params_length,
						MAX_INBAND_PARAM_SZ,
						"BASS_BOOST_ENABLE", rc);
				if (rc != 0)
					break;
				*updt_params++ =
					AUDPROC_MODULE_ID_BASS_BOOST;
				*updt_params++ =
					AUDPROC_PARAM_ID_BASS_BOOST_ENABLE;
				*updt_params++ =
					BASS_BOOST_ENABLE_PARAM_SZ;
				*updt_params++ =
					bass_boost->enable_flag;
			}
			break;
		case BASS_BOOST_MODE:
			pr_debug("%s: BASS_BOOST_MODE\n", __func__);
			if (length != 1 || index_offset != 0) {
				pr_err("no valid params\n");
				rc = -EINVAL;
				goto invalid_config;
			}
			bass_boost->mode =
				GET_NEXT(values, param_max_offset, rc);
			if (command_config_state == CONFIG_SET) {
				params_length += COMMAND_PAYLOAD_SZ +
					BASS_BOOST_MODE_PARAM_SZ;
				CHECK_PARAM_LEN(params_length,
						MAX_INBAND_PARAM_SZ,
						"BASS_BOOST_MODE", rc);
				if (rc != 0)
					break;
				*updt_params++ =
					AUDPROC_MODULE_ID_BASS_BOOST;
				*updt_params++ =
					AUDPROC_PARAM_ID_BASS_BOOST_MODE;
				*updt_params++ =
					BASS_BOOST_MODE_PARAM_SZ;
				*updt_params++ =
					bass_boost->mode;
			}
			break;
		case BASS_BOOST_STRENGTH:
			pr_debug("%s: BASS_BOOST_STRENGTH\n", __func__);
			if (length != 1 || index_offset != 0) {
				pr_err("no valid params\n");
				rc = -EINVAL;
				goto invalid_config;
			}
			bass_boost->strength =
				GET_NEXT(values, param_max_offset, rc);
			if (command_config_state == CONFIG_SET) {
				params_length += COMMAND_PAYLOAD_SZ +
					BASS_BOOST_STRENGTH_PARAM_SZ;
				CHECK_PARAM_LEN(params_length,
						MAX_INBAND_PARAM_SZ,
						"BASS_BOOST_STRENGTH", rc);
				if (rc != 0)
					break;
				*updt_params++ =
					AUDPROC_MODULE_ID_BASS_BOOST;
				*updt_params++ =
					AUDPROC_PARAM_ID_BASS_BOOST_STRENGTH;
				*updt_params++ =
					BASS_BOOST_STRENGTH_PARAM_SZ;
				*updt_params++ =
					bass_boost->strength;
			}
			break;
		default:
			pr_err("%s: Invalid command to set config\n", __func__);
			break;
		}
	}
        if (params_length)
		q6asm_send_audio_effects_params(ac, params,
						params_length);
invalid_config:
	kfree(params);
	return rc;
}

int msm_audio_effects_popless_eq_handler(struct audio_client *ac,
					 struct eq_params *eq,
					 long *values)
{
	long *param_max_offset = values + MAX_PP_PARAMS_SZ - 1;
	char *params = NULL;
	int rc = 0;
	int devices = GET_NEXT(values, param_max_offset, rc);
	int num_commands = GET_NEXT(values, param_max_offset, rc);
	int *updt_params, i, prev_enable_flag;
	uint32_t params_length = (MAX_INBAND_PARAM_SZ);

	pr_debug("%s\n", __func__);
	if (!ac || (devices == -EINVAL) || (num_commands == -EINVAL)) {
		pr_err("%s: cannot set audio effects\n", __func__);
		return -EINVAL;
	}
	params = kzalloc(params_length, GFP_KERNEL);
	if (!params) {
		pr_err("%s, params memory alloc failed\n", __func__);
		return -ENOMEM;
	}
	pr_debug("%s: device: %d\n", __func__, devices);
	updt_params = (int *)params;
	params_length = 0;
	for (i = 0; i < num_commands; i++) {
		uint32_t command_id =
			GET_NEXT(values, param_max_offset, rc);
		uint32_t command_config_state =
			GET_NEXT(values, param_max_offset, rc);
		uint32_t index_offset =
			GET_NEXT(values, param_max_offset, rc);
		uint32_t length =
			GET_NEXT(values, param_max_offset, rc);
		uint32_t idx;
		int j;
		switch (command_id) {
		case EQ_ENABLE:
			pr_debug("%s: EQ_ENABLE\n", __func__);
			if (length != 1 || index_offset != 0) {
				pr_err("no valid params\n");
				rc = -EINVAL;
				goto invalid_config;
			}
			prev_enable_flag = eq->enable_flag;
			eq->enable_flag =
				GET_NEXT(values, param_max_offset, rc);
			pr_debug("%s: prev_enable_flag : %d, eq.enable_flag : %d",
				__func__, prev_enable_flag, eq->enable_flag);
			if (prev_enable_flag != eq->enable_flag) {
				params_length += COMMAND_PAYLOAD_SZ +
					EQ_ENABLE_PARAM_SZ;
				CHECK_PARAM_LEN(params_length,
						MAX_INBAND_PARAM_SZ,
						"EQ_ENABLE", rc);
				if (rc != 0)
					break;
				*updt_params++ =
					AUDPROC_MODULE_ID_POPLESS_EQUALIZER;
				*updt_params++ =
					AUDPROC_PARAM_ID_EQ_ENABLE;
				*updt_params++ =
					EQ_ENABLE_PARAM_SZ;
				*updt_params++ =
					eq->enable_flag;
			}
			break;
		case EQ_CONFIG:
			pr_debug("%s: EQ_CONFIG\n", __func__);
			if (length < EQ_CONFIG_PARAM_LEN || index_offset != 0) {
				pr_err("no valid params\n");
				rc = -EINVAL;
				goto invalid_config;
			}
			for (idx = 0; idx < MAX_EQ_BANDS; idx++)
				eq->per_band_cfg[idx].band_idx = -1;
			eq->config.eq_pregain =
				GET_NEXT(values, param_max_offset, rc);
			eq->config.preset_id =
				GET_NEXT(values, param_max_offset, rc);
			eq->config.num_bands =
				GET_NEXT(values, param_max_offset, rc);
			if (eq->config.num_bands > MAX_EQ_BANDS) {
				pr_err("invalid num of bands\n");
				rc = -EINVAL;
				goto invalid_config;
			}
			if (eq->config.num_bands &&
			    (((length - EQ_CONFIG_PARAM_LEN)/
				EQ_CONFIG_PER_BAND_PARAM_LEN)
				!= eq->config.num_bands)) {
				pr_err("invalid length to set config per band\n");
				rc = -EINVAL;
				goto invalid_config;
			}
			for (j = 0; j < eq->config.num_bands; j++) {
				idx = GET_NEXT(values, param_max_offset, rc);
				eq->per_band_cfg[idx].band_idx = idx;
				eq->per_band_cfg[idx].filter_type =
					GET_NEXT(values, param_max_offset, rc);
				eq->per_band_cfg[idx].freq_millihertz =
					GET_NEXT(values, param_max_offset, rc);
				eq->per_band_cfg[idx].gain_millibels =
					GET_NEXT(values, param_max_offset, rc);
				eq->per_band_cfg[idx].quality_factor =
					GET_NEXT(values, param_max_offset, rc);
			}
			if (command_config_state == CONFIG_SET) {
				int config_param_length = EQ_CONFIG_PARAM_SZ +
					(EQ_CONFIG_PER_BAND_PARAM_SZ*
					 eq->config.num_bands);
				params_length += COMMAND_PAYLOAD_SZ +
						config_param_length;
				CHECK_PARAM_LEN(params_length,
						MAX_INBAND_PARAM_SZ,
						"EQ_CONFIG", rc);
				if (rc != 0)
					break;
				*updt_params++ =
					AUDPROC_MODULE_ID_POPLESS_EQUALIZER;
				*updt_params++ =
					AUDPROC_PARAM_ID_EQ_CONFIG;
				*updt_params++ =
					config_param_length;
				*updt_params++ =
					eq->config.eq_pregain;
				*updt_params++ =
					eq->config.preset_id;
				*updt_params++ =
					eq->config.num_bands;
				for (idx = 0; idx < MAX_EQ_BANDS; idx++) {
					if (eq->per_band_cfg[idx].band_idx < 0)
						continue;
					*updt_params++ =
					eq->per_band_cfg[idx].filter_type;
					*updt_params++ =
					eq->per_band_cfg[idx].freq_millihertz;
					*updt_params++ =
					eq->per_band_cfg[idx].gain_millibels;
					*updt_params++ =
					eq->per_band_cfg[idx].quality_factor;
					*updt_params++ =
					eq->per_band_cfg[idx].band_idx;
				}
			}
			break;
		case EQ_BAND_INDEX:
			pr_debug("%s: EQ_BAND_INDEX\n", __func__);
			if (length != 1 || index_offset != 0) {
				pr_err("no valid params\n");
				rc = -EINVAL;
				goto invalid_config;
			}
			idx = GET_NEXT(values, param_max_offset, rc);
			if (idx > MAX_EQ_BANDS) {
				pr_err("invalid band index\n");
				rc = -EINVAL;
				goto invalid_config;
			}
			eq->band_index = idx;
			if (command_config_state == CONFIG_SET) {
				params_length += COMMAND_PAYLOAD_SZ +
					EQ_BAND_INDEX_PARAM_SZ;
				CHECK_PARAM_LEN(params_length,
						MAX_INBAND_PARAM_SZ,
						"EQ_BAND_INDEX", rc);
				if (rc != 0)
					break;
				*updt_params++ =
					AUDPROC_MODULE_ID_POPLESS_EQUALIZER;
				*updt_params++ =
					AUDPROC_PARAM_ID_EQ_BAND_INDEX;
				*updt_params++ =
					EQ_BAND_INDEX_PARAM_SZ;
				*updt_params++ =
					eq->band_index;
			}
			break;
		case EQ_SINGLE_BAND_FREQ:
			pr_debug("%s: EQ_SINGLE_BAND_FREQ\n", __func__);
			if (length != 1 || index_offset != 0) {
				pr_err("no valid params\n");
				rc = -EINVAL;
				goto invalid_config;
			}
			if (eq->band_index > MAX_EQ_BANDS) {
				pr_err("invalid band index to set frequency\n");
				break;
			}
			eq->freq_millihertz =
				GET_NEXT(values, param_max_offset, rc);
			if (command_config_state == CONFIG_SET) {
				params_length += COMMAND_PAYLOAD_SZ +
					EQ_SINGLE_BAND_FREQ_PARAM_SZ;
				CHECK_PARAM_LEN(params_length,
						MAX_INBAND_PARAM_SZ,
						"EQ_SINGLE_BAND_FREQ", rc);
				if (rc != 0)
					break;
				*updt_params++ =
					AUDPROC_MODULE_ID_POPLESS_EQUALIZER;
				*updt_params++ =
					AUDPROC_PARAM_ID_EQ_SINGLE_BAND_FREQ;
				*updt_params++ =
					EQ_SINGLE_BAND_FREQ_PARAM_SZ;
				*updt_params++ =
					eq->freq_millihertz;
			}
			break;
		default:
			pr_err("%s: Invalid command to set config\n", __func__);
			break;
		}
	}
        if (params_length)
		q6asm_send_audio_effects_params(ac, params,
						params_length);
invalid_config:
	kfree(params);
	return rc;
}
