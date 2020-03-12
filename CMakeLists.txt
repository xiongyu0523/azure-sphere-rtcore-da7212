#  Copyright (c) Codethink Ltd. All rights reserved.
#  Licensed under the MIT License.

CMAKE_MINIMUM_REQUIRED(VERSION 3.11)
PROJECT(azure-sphere-rtcore-da7212 C)

# include
include_directories(${CMAKE_SOURCE_DIR} 
					${CMAKE_SOURCE_DIR}/lib ${CMAKE_SOURCE_DIR}/lib/mt3620
					${CMAKE_SOURCE_DIR}/codec ${CMAKE_SOURCE_DIR}/codec/da7212 ${CMAKE_SOURCE_DIR}/codec/port)

add_compile_definitions(NDEBUG)

# Create executable
ADD_EXECUTABLE(${PROJECT_NAME} 
               main.c 
			   lib/VectorTable.c lib/GPT.c lib/GPIO.c lib/UART.c lib/Print.c lib/I2S.c lib/I2CMaster.c
			   codec/fsl_codec_common.c codec/da7212/fsl_dialog7212.c codec/port/da7212/fsl_codec_adapter.c codec/port/mt3620/fsl_codec_i2c.c)
TARGET_LINK_LIBRARIES(${PROJECT_NAME})
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINK_DEPENDS ${CMAKE_SOURCE_DIR}/linker.ld)

# Add MakeImage post-build command
INCLUDE("${AZURE_SPHERE_MAKE_IMAGE_FILE}")

TARGET_LINK_LIBRARIES(${PROJECT_NAME} PUBLIC -L${CMAKE_SOURCE_DIR})
