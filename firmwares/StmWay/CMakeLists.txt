cmake_minimum_required(VERSION 3.16)
set(STM32_CUBE_F4_PATH "~/Tools/STM32/STM32CubeF4")
set(CMAKE_TOOLCHAIN_FILE "~/Tools/STM32/stm32-cmake/cmake/stm32_gcc.cmake")

set(PROJECT_NAME neuroslave_sea)
project(${PROJECT_NAME} C ASM)
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

find_package(CMSIS COMPONENTS STM32F4 REQUIRED)
find_package(HAL COMPONENTS STM32F4 LL_RCC LL_GPIO REQUIRED)
#find_package(CMSIS COMPONENTS STM32L0 STM32F1 STM32F4 REQUIRED)
#find_package(HAL COMPONENTS STM32L0 STM32F1 STM32F4 LL_RCC LL_GPIO REQUIRED)

# STM32F4-Discovery
add_executable(${PROJECT_NAME} neuromain.c)
target_link_libraries(${PROJECT_NAME}
    HAL::STM32::F4::LL_RCC
    HAL::STM32::F4::LL_GPIO
    CMSIS::STM32::F407VG
    STM32::NoSys 
)
stm32_print_size_of_target(${PROJECT_NAME})

