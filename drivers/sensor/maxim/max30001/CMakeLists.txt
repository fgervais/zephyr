# Copyright (c) 2025 Croxel Inc.
# Copyright (c) 2025 CogniPilot Foundation
# SPDX-License-Identifier: Apache-2.0

zephyr_library()
zephyr_library_include_directories(.)
zephyr_library_sources(
	max30001.c
)
zephyr_library_sources_ifdef(CONFIG_SENSOR_ASYNC_API
	max30001_decoder.c
)
zephyr_library_sources_ifdef(CONFIG_MAX30001_TRIGGER
	max30001_trigger.c
)
zephyr_library_sources_ifdef(CONFIG_MAX30001_STREAM
	max30001_stream.c
)