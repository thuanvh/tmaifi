PROJECT(jolirobot)

ADD_EXECUTABLE(jolirobot main.cpp)

target_link_libraries(jolirobot GL GLU glut X11 Xmu Xi m jpeg tiff) 


add_subdirectory(dicdact)
# # add_subdirectory(redbook)