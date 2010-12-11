PROJECT(didact12)

ADD_EXECUTABLE(didact12 main.cpp)
ADD_EXECUTABLE(cube cube.cpp)
target_link_libraries(didact12 GL GLU glut X11 Xmu Xi m)
target_link_libraries(cube GL GLU glut X11 Xmu Xi m)
#TARGET_LINK_LIBRARIES(GL GLU glut X11 Xmu Xi m)
#link_libraries(GL GLU glut X11 Xmu Xi m)
