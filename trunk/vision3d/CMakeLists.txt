PROJECT(vision3d)

ADD_EXECUTABLE(vision3d main.cpp vision.cpp)

#target_link_libraries(vision3d cxcore cv highgui cvaux ml)
target_link_libraries(vision3d opencv_core opencv_highgui opencv_features2d)