
#head file path
include_directories(.)
include_directories(util)


aux_source_directory(. SRC_LISTA)
aux_source_directory(util SRC_LISTB)


add_executable(oflex ${SRC_LISTA} ${SRC_LISTB})

list(REMOVE_ITEM SRC_LISTA "./main.cpp")

add_library(oflexlib ${SRC_LISTA} ${SRC_LISTB})
