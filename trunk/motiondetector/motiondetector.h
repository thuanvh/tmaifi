/* 
 * File:   motiondetector.h
 * Author: thuanvh
 *
 * Created on November 6, 2010, 10:53 PM
 */

#ifndef MOTIONDETECTOR_H
#define	MOTIONDETECTOR_H

#define ERR_DIR_MISSING "Directory parametre is missing."
#define ERR_DIR_OPEN "Could not open directory"
#define ERR_FILE_MISSING "File parametre is missing."
#define ERR_FILE_OPEN "Could not open file"
#define ERR_FUNC_MISSING "Function parametre is missing."

void MotionDetection(char* videoFile, int fps, int queuesize);

#endif	/* MOTIONDETECTOR_H */

