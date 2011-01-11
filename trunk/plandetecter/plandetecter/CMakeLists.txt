PROJECT(plandetecter)

ADD_EXECUTABLE(plandetecter main.cpp plandetecter.cpp)

target_link_libraries(plandetecter cxcore cv highgui cvaux ml)