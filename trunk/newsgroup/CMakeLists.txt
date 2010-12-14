PROJECT(newsgroup)

ADD_EXECUTABLE(newsgroup main.cpp tf-idf.cpp pca.cpp )
target_link_libraries(newsgroup cxcore cv highgui cvaux ml)
