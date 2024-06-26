cmake_minimum_required(VERSION 3.16)

set(STM32_CUBE_F4_PATH "~/Tools/STM32/STM32CubeF4")
set(CMSIS_INCLUDE_DIRS ${STM32_CUBE_F4_PATH}/CMSIS)
set(CMSIS_SOURCES ${STM32_CUBE_F4_PATH}/CMSIS)
set(CMAKE_TOOLCHAIN_FILE "~/Tools/STM32/stm32-cmake/cmake/stm32_gcc.cmake")

set(PROJECT_NAME neuroslave_sea)
project(${PROJECT_NAME} C ASM)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

set(HAL_COMP_LIST LL_RCC RCC LL_GPIO LL_SPI LL_UTILS GPIO CORTEX STM32F4)
set(CMSIS_COMP_LIST STM32F4)
find_package(CMSIS COMPONENTS "${CMSIS_COMP_LIST}" REQUIRED)
find_package(HAL COMPONENTS "${HAL_COMP_LIST}" REQUIRED)

set(W5500_PATH "~/Tools/Network/ioLibrary_Driver/Ethernet")
set(PROJECT_DIRS ${W5500_PATH} ${W5500_PATH}/W5500)
list(APPEND PROJECT_DIRS "../")

set(SOURCE_LIST neuromain.c)
list(APPEND SOURCE_LIST ${W5500_PATH}/wizchip_conf.c ${W5500_PATH}/socket.c ${W5500_PATH}/W5500/w5500.c)

list(APPEND SOURCE_LIST ../eth/server_w5500.c ../spi/spi_ll_easy_api.c)

# STM32F4-Discovery
add_executable(${PROJECT_NAME} ${SOURCE_LIST})
target_compile_definitions(${PROJECT_NAME} PRIVATE USE_FULL_LL_DRIVER=1 _WIZCHIP_=5500)
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_DIRS})
target_link_libraries(${PROJECT_NAME}
    HAL::STM32::F4::LL_RCC
    HAL::STM32::F4::LL_GPIO
    HAL::STM32::F4::LL_SPI
    HAL::STM32::F4::LL_UTILS 
    CMSIS::STM32::F401CC
    STM32::NoSys 
)
#target_link_libraries(${PROJECT_NAME}
#        HAL::STM32::F4::RCC
#        HAL::STM32::F4::GPIO
#        HAL::STM32::F4::CORTEX
#        CMSIS::STM32::F401CC
#)
stm32_print_size_of_target(${PROJECT_NAME})

