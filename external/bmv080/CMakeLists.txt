# Bibliotheken als "Importiert" definieren
add_library(bosch_bmv080 STATIC IMPORTED)
set_target_properties(bosch_bmv080 PROPERTIES 
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib_bmv080.a")

add_library(bosch_postprocessor STATIC IMPORTED)
set_target_properties(bosch_postprocessor PROPERTIES 
    IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/lib_postProcessor.a")

# Die Komponente bei ESP-IDF registrieren
idf_component_register(
    SRCS "bmv080_wrapper.cpp"
    INCLUDE_DIRS "."
    REQUIRES esphome
)

# Die Bibliotheken fest mit deinem Wrapper verknüpfen
target_link_libraries(${COMPONENT_LIB} PRIVATE bosch_bmv080 bosch_postprocessor)