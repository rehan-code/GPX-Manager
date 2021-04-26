#ifndef GPX_PARSER_H
#define GPX_PARSER_H

#include "GPXParser.h"

/**Creates a List of all the Waypoints in the GPXdoc
 * @return the List containing all the Waypoints
 */
List *createWptList (xmlNode *wptNode, int option);

/**Creates a List of all the Routes in the GPXdoc
 * @return the List containing all the Routes
 */
List *createRteList (xmlNode *rteNode);

/**Creates a List of all the Track Ssgments in the GPXdoc
 * @return the List containing all the Track Segments
 */
List *createTrksegList (xmlNode *trksegNode);

/**Creates a List of all the Tracks in the GPXdoc
 * @return the List containing all the Tracks
 */
List *createTrkList (xmlNode *trkNode);

/**Calculates the number of GPXData elements from a Waypoint List
 * @return the integer number of the number of GPXData value elements
 */
int getNumWptGPXData(List *wptList);

/**Calculates the number of GPXData elements from a Rout List
 * @return the integer number of the number of GPXData value elements
 */
int getNumRteGPXData(List *rteList);

/**Calculates the number of GPXData elements from a Track List
 * @return the integer number of the number of GPXData value elements
 */
int getNumTrkGPXData(List *trkList);

/**Calculates the strlen to malloc the string for the corresponding to string function
 * @return the strlen integer to malloc the ToString function of a Waypoint
 */
int getWptstrlen(void *data);

/**Calculates the strlen to malloc the string for the corresponding to string function
 * @return the strlen integer to malloc the ToString function of a Route
 */
int getRtestrlen(void *data);

/**Calculates the strlen to malloc the string for the corresponding to string function
 * @return the strlen integer to malloc the ToString function of a TrackSegment
 */
int getTrksegstrlen(void *data);

/**Calculates the strlen to malloc the string for the corresponding to string function
 * @return the strlen integer to malloc the ToString function of a Track
 */
int getTrkstrlen(void *data);

/**Calculates the strlen to malloc the string for the corresponding to string function
 * @return the strlen integer to malloc the ToString function of GPXDoc
 */
int getGPXDocstrlen(GPXdoc* doc);

#endif