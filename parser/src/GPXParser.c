#include "GPXParser.h"
#include "GPXHelpers.h"

/**Student Name: Rehan Nagoor Mohideen
 * Student ID: 1100592
 */


/* ******************************* My helper functions *************************** */

/**Creates a List of all the Waypoints in the GPXdoc
 * @return the List containing all the Waypoints
 */
List *createWptList (xmlNode *wptNode, int option) {
    List *list = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
    Waypoint *wpt;
    xmlNode *tempNode;
    xmlNode *childNode;
    xmlAttr *attr;
    GPXData *gpxData;
    char type[10];

    if (option == 2) {
        strcpy(type, "rtept");
    } else if (option == 3) {
        strcpy(type, "trkpt");
    } else {
        strcpy(type, "wpt");
    }

    for (tempNode = wptNode; tempNode != NULL; tempNode = tempNode->next) {
        if (tempNode->type == XML_ELEMENT_NODE && strcmp((char *)tempNode->name, type)==0) {
            wpt = malloc(sizeof(Waypoint));
            for (attr = tempNode->properties; attr != NULL; attr = attr->next) {//get properties and loop through each
                if (strcmp((char *)attr->name, "lat")==0) {//if the property name is lat assign it to latitude
                    wpt->latitude = atof((char *)attr->children->content);
                }
                if (strcmp((char *)attr->name, "lon")==0) {
                    wpt->longitude = atof((char *)attr->children->content);
                }
            }

            wpt->name = NULL;
            wpt->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);
            for (childNode = tempNode->children; childNode != NULL; childNode = childNode->next) {
                if (childNode->type == XML_ELEMENT_NODE && strcmp((char *)childNode->name,"name")==0 && childNode->children != NULL && childNode->children->content != NULL) {
                    wpt->name = malloc(strlen((char *)childNode->children->content)+1);
                    strcpy(wpt->name, (char *)childNode->children->content);
                } else if (childNode->type == XML_ELEMENT_NODE && childNode->children != NULL && childNode->children->content != NULL) {//handle linking of other Data by allocating memory and assigning the variables
                    gpxData = malloc(sizeof(GPXData)+strlen((char *)childNode->children->content)+1);
                    strcpy(gpxData->name, (char *)childNode->name);
                    strcpy(gpxData->value, (char *)childNode->children->content);
                    insertBack(wpt->otherData, gpxData);
                }
            }
            if (wpt->name == NULL) {
                wpt->name = malloc(sizeof(char )*2);
                strcpy((char *)wpt->name, "");
            }
            
            insertBack(list, wpt);
        }
    }

    return list;
}

/**Creates a List of all the Routes in the GPXdoc
 * @return the List containing all the Routes
 */
List *createRteList (xmlNode *rteNode) {
    List *list = initializeList(&routeToString, &deleteRoute, &compareRoutes);
    Route *rte;
    xmlNode *tempNode;
    xmlNode *childNode;
    GPXData *gpxData;

    for (tempNode = rteNode; tempNode != NULL; tempNode = tempNode->next) {
        if (tempNode->type == XML_ELEMENT_NODE && strcmp((char *)tempNode->name,"rte")==0) {
            rte = malloc(sizeof(Route));

            rte->name = NULL;
            rte->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);
            rte->waypoints = createWptList(tempNode->children, 2);
            for (childNode = tempNode->children; childNode != NULL; childNode = childNode->next) {
                if (childNode->type == XML_ELEMENT_NODE && strcmp((char *)childNode->name,"name")==0 && childNode->children != NULL && childNode->children->content != NULL) {
                    rte->name = malloc(strlen((char *)childNode->children->content)+1);
                    strcpy(rte->name, (char *)childNode->children->content);

                } else if (childNode->type == XML_ELEMENT_NODE && strcmp((char *)childNode->name,"rtept")!=0 && childNode->children != NULL && childNode->children->content != NULL) {//handle allocating and assigning otherData
                    gpxData = malloc(sizeof(GPXData)+strlen((char *)childNode->children->content)+1);
                    strcpy(gpxData->name, (char *)childNode->name);
                    strcpy(gpxData->value, (char *)childNode->children->content);
                    insertBack(rte->otherData, gpxData);
                }
            }
            if (rte->name == NULL) {
                rte->name = malloc(sizeof(char )*2);
                strcpy((char *)rte->name, "");
            }
            
            insertBack(list, rte);
        }
    }

    return list;
}

/**Creates a List of all the Track Ssgments in the GPXdoc
 * @return the List containing all the Track Segments
 */
List *createTrksegList (xmlNode *trksegNode) {
    List *list = initializeList(&trackSegmentToString, &deleteTrackSegment, &compareTrackSegments);
    TrackSegment *trkseg;
    xmlNode *tempNode;

    for (tempNode = trksegNode; tempNode != NULL; tempNode = tempNode->next) {
        if (tempNode->type == XML_ELEMENT_NODE && strcmp((char *)tempNode->name,"trkseg")==0) {
            trkseg = malloc(sizeof(TrackSegment));
            trkseg->waypoints = createWptList(tempNode->children, 3);
            insertBack(list, trkseg);
        }
    }

    return list;
}

/**Creates a List of all the Tracks in the GPXdoc
 * @return the List containing all the Tracks
 */
List *createTrkList (xmlNode *trkNode) {
    List *list = initializeList(&trackToString, &deleteTrack, &compareTracks);
    Track *trk;
    xmlNode *tempNode;
    xmlNode *childNode;
    GPXData *gpxData;

    for (tempNode = trkNode; tempNode != NULL; tempNode = tempNode->next) {
        if (tempNode->type == XML_ELEMENT_NODE && strcmp((char *)tempNode->name,"trk")==0) {
            trk = malloc(sizeof(Track));

            trk->name = NULL;
            trk->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);
            trk->segments = createTrksegList(tempNode->children);
            for (childNode = tempNode->children; childNode != NULL; childNode = childNode->next) {
                if (childNode->type == XML_ELEMENT_NODE && strcmp((char *)childNode->name,"name")==0 && childNode->children != NULL && childNode->children->content != NULL) {
                    trk->name = malloc(strlen((char *)childNode->children->content)+1);
                    strcpy(trk->name, (char *)childNode->children->content);

                } else if (childNode->type == XML_ELEMENT_NODE && strcmp((char *)childNode->name,"trkseg")!=0 && childNode->children != NULL && childNode->children->content != NULL) {//handle allocating and assigning otherData
                    gpxData = malloc(sizeof(GPXData)+strlen((char *)childNode->children->content)+1);
                    strcpy(gpxData->name, (char *)childNode->name);
                    strcpy(gpxData->value, (char *)childNode->children->content);
                    insertBack(trk->otherData, gpxData);
                }
            }
            if (trk->name == NULL) {
                trk->name = malloc(sizeof(char )*2);
                strcpy((char *)trk->name, "");
            }
            
            insertBack(list, trk);
        }
    }

    return list;
}

/**Calculates the number of GPXData elements from a Waypoint List
 * @return the integer number of the number of GPXData value elements
 */
int getNumWptGPXData(List *wptList) {
    if (wptList == NULL) {
        return 0;
    }
    int total = 0;
    ListIterator iter = createIterator(wptList);
    void * elem;
    while ((elem = nextElement(&iter)) != NULL){
		Waypoint* tmp = (Waypoint*)elem;
        if (strcmp((char *)tmp->name, "")!=0){
            total++;
        }
		total += tmp->otherData->length;
	}
    return total;
}

/**Calculates the number of GPXData elements from a Rout List
 * @return the integer number of the number of GPXData value elements
 */
int getNumRteGPXData(List *rteList) {
    if (rteList == NULL) {
        return 0;
    }
    int total = 0;
    ListIterator iter = createIterator(rteList);
    void * elem;
    while ((elem = nextElement(&iter)) != NULL){
		Route* tmp = (Route*)elem;
        if (strcmp((char *)tmp->name, "")!=0){
            total++;
        }
		total += tmp->otherData->length;
        total += getNumWptGPXData(tmp->waypoints);
	}
    return total;
}

/**Calculates the number of GPXData elements from a Track List
 * @return the integer number of the number of GPXData value elements
 */
int getNumTrkGPXData(List *trkList) {
    if (trkList == NULL) {
        return 0;
    }
    int total = 0;
    ListIterator iter = createIterator(trkList);
    void * elem;
    void * elem2;
    while ((elem = nextElement(&iter)) != NULL){
		Track* tmp = (Track*)elem;
        if (strcmp((char *)tmp->name, "")!=0){
            total++;
        }
		total += tmp->otherData->length;
        
        ListIterator iter2 = createIterator(tmp->segments);
        while ((elem2 = nextElement(&iter2)) != NULL){
            TrackSegment* tmp2 = (TrackSegment*)elem2;
            total += getNumWptGPXData(tmp2->waypoints);
        }
	}
    return total;
}

/**Calculates the strlen to malloc the string for the corresponding to string function
 * @return the strlen integer to malloc the ToString function of a Waypoint
 */
int getWptstrlen(void *data) {
    if (data == NULL) {
        return 0;
    }
    Waypoint *wpt = (Waypoint *)data;
    int total = strlen((char *)wpt->name);
    ListIterator iter = createIterator(wpt->otherData);
    void * elem;
    while ((elem = nextElement(&iter)) != NULL){
		GPXData* tmp = (GPXData*)elem;
        total += 256 + strlen((char *)tmp->value) + 20;
	}
    return total+70;
}

/**Calculates the strlen to malloc the string for the corresponding to string function
 * @return the strlen integer to malloc the ToString function of a Route
 */
int getRtestrlen(void *data) {
    if (data == NULL) {
        return 0;
    }
    Route *rte = (Route *)data;
    int total = strlen((char *)rte->name);
    ListIterator iter = createIterator(rte->waypoints);
    void * elem;
    while ((elem = nextElement(&iter)) != NULL){
		Waypoint* tmp = (Waypoint*)elem;
		total += getWptstrlen(tmp);
	}
    iter = createIterator(rte->otherData);
    while ((elem = nextElement(&iter)) != NULL){
		GPXData* tmp = (GPXData*)elem;
        total += 256 + strlen((char *)tmp->value) + 20;
	}
    return total+60;
}

/**Calculates the strlen to malloc the string for the corresponding to string function
 * @return the strlen integer to malloc the ToString function of a TrackSegment
 */
int getTrksegstrlen(void *data) {
    if (data == NULL) {
        return 0;
    }
    int total = 0;
    TrackSegment *trk = (TrackSegment *)data;
    ListIterator iter = createIterator(trk->waypoints);
    void * elem;
    while ((elem = nextElement(&iter)) != NULL){
		Waypoint* tmp = (Waypoint*)elem;
		total += getWptstrlen(tmp);
	}
    return total+35;
}

/**Calculates the strlen to malloc the string for the corresponding to string function
 * @return the strlen integer to malloc the ToString function of a Track
 */
int getTrkstrlen(void *data) {
    if (data == NULL) {
        return 0;
    }
    Track *trk = (Track *)data;
    int total = strlen((char *)trk->name);
    ListIterator iter = createIterator(trk->segments);
    void * elem;
    while ((elem = nextElement(&iter)) != NULL){
		TrackSegment* tmp = (TrackSegment*)elem;
		total += getTrksegstrlen(tmp);
	}
    iter = createIterator(trk->otherData);
    while ((elem = nextElement(&iter)) != NULL){
		GPXData* tmp = (GPXData*)elem;
        total += 256 + strlen((char *)tmp->value) + 20;
	}
    return total+60;
}

/**Calculates the strlen to malloc the string for the corresponding to string function
 * @return the strlen integer to malloc the ToString function of GPXDoc
 */
int getGPXDocstrlen(GPXdoc* doc) {
    int total = strlen((char *)doc->creator) + 256;
    ListIterator iter = createIterator(doc->waypoints);
    void * elem;
    while ((elem = nextElement(&iter)) != NULL){
		Waypoint* tmp = (Waypoint*)elem;
		total += getWptstrlen(tmp);
	}
    iter = createIterator(doc->routes);
    while ((elem = nextElement(&iter)) != NULL){
		Route* tmp = (Route*)elem;
        total += getRtestrlen(tmp);
	}
    iter = createIterator(doc->tracks);
    while ((elem = nextElement(&iter)) != NULL){
		Track* tmp = (Track*)elem;
        total += getTrkstrlen(tmp);
	}
    return total+190;
}

/* ******************************* Functions *************************** */
/** Function to create an GPX object based on the contents of an GPX file.
 *@pre File name cannot be an empty string or NULL.
       File represented by this name must exist and must be readable.
 *@post Either:
        A valid GPXdoc has been created and its address was returned
		or 
		An error occurred, and NULL was returned
 *@return the pinter to the new struct or NULL
 *@param fileName - a string containing the name of the GPX file
**/
GPXdoc* createGPXdoc(char* fileName) {
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;
    xmlNode *cur_node = NULL;

    /*
     * this initialize the library and check potential ABI mismatches
     * between the version it was compiled for and the actual shared
     * library used.
     */
    LIBXML_TEST_VERSION

    /*parse the file and get the DOM */
    doc = xmlReadFile(fileName, NULL, 0);

    if (doc == NULL) {
        return NULL;
    }

    /*Get the root element node */
    root_element = xmlDocGetRootElement(doc);
    
    /*allocating space and content of the root GPX element and its properties*/
    GPXdoc *rootDoc = malloc(sizeof(GPXdoc));
    xmlAttr *attr;
    strcpy(rootDoc->namespace, (char *)root_element->ns->href);
    for (attr = root_element->properties; attr != NULL; attr = attr->next) {
        if (strcmp((char *)attr->name, "version")==0) {
            rootDoc->version = atof((char *)attr->children->content);
        }
        if (strcmp((char *)attr->name, "creator")==0) {
            rootDoc->creator = malloc(strlen((char *)attr->children->content)+1);
            strcpy(rootDoc->creator, (char *)attr->children->content);
        }
    }

    cur_node = root_element->children;
    rootDoc->waypoints = createWptList(cur_node, 1);
    rootDoc->routes = createRteList(cur_node);
    rootDoc->tracks = createTrkList(cur_node);
    
    /*free the document */
    xmlFreeDoc(doc);

    /*
     *Free the global variables that may
     *have been allocated by the parser.
     */
    xmlCleanupParser();

    return rootDoc;
}

/** Function to create a string representation of an GPX object.
 *@pre GPX object exists, is not null, and is valid
 *@post GPX has not been modified in any way, and a string representing the GPX contents has been created
 *@return a string contaning a humanly readable representation of an GPX object
 *@param obj - a pointer to an GPX struct
**/
char* GPXdocToString(GPXdoc* doc) {
    char *string = malloc(getGPXDocstrlen(doc));
    sprintf(string, "----------------GPX doc-----------------\nNamespace: %s\nVersion: %.2f\nCreator: %s\n\n\n-----------Waypoints----------\n", doc->namespace, doc->version, doc->creator);
    ListIterator iter = createIterator(doc->waypoints);
    void * elem;
    char* str;
	while ((elem = nextElement(&iter)) != NULL){
		Waypoint* tmpName = (Waypoint*)elem;
		str = doc->waypoints->printData(tmpName);
        strcat(string, str);
        free(str);
	}

    iter = createIterator(doc->routes);
    strcat(string, "\n------------Routes------------\n");
    while ((elem = nextElement(&iter)) != NULL){
		Route* tmp = (Route*)elem;
		str = doc->routes->printData(tmp);
        strcat(string, str);
        free(str);
	}
    
    iter = createIterator(doc->tracks);
    strcat(string, "\n------------Tracks------------\n");
    while ((elem = nextElement(&iter)) != NULL){
		Track* trk = (Track*)elem;
		str = doc->tracks->printData(trk);
        strcat(string, str);
        free(str);
	}

    return string;
}

/** Function to delete doc content and free all the memory.
 *@pre GPX object exists, is not null, and has not been freed
 *@post GPX object had been freed
 *@return none
 *@param obj - a pointer to an GPX struct
**/
void deleteGPXdoc(GPXdoc* doc) {
    if (doc != NULL) {
        free(doc->creator);
        freeList(doc->waypoints);
        freeList(doc->routes);
        freeList(doc->tracks);
        free(doc);
    }
}

/* For the five "get..." functions below, return the count of specified entities from the file.  
They all share the same format and only differ in what they have to count.
 
 *@pre GPX object exists, is not null, and has not been freed
 *@post GPX object has not been modified in any way
 *@return the number of entities in the GPXdoc object
 *@param obj - a pointer to an GPXdoc struct
 */


//Total number of waypoints in the GPX file
int getNumWaypoints(const GPXdoc* doc) { 
    return doc==NULL ? 0 : doc->waypoints->length;
}

//Total number of routes in the GPX file
int getNumRoutes(const GPXdoc* doc) {
    return doc==NULL ? 0 : doc->routes->length;
}

//Total number of tracks in the GPX file
int getNumTracks(const GPXdoc* doc) {
    return doc==NULL ? 0 : doc->tracks->length;
}

//Total number of segments in all tracks in the document
int getNumSegments(const GPXdoc* doc) {
    if (doc == NULL) {
        return 0;
    }
    int total = 0;
    ListIterator iter = createIterator(doc->tracks);
    void * elem;
    while ((elem = nextElement(&iter)) != NULL){
		Track* tmp = (Track*)elem;
		total += tmp->segments->length;
	}
    return total;
}

//Total number of GPXData elements in the document
int getNumGPXData(const GPXdoc* doc) {
    if (doc == NULL) {
        return 0;
    }
    int total = 0;
    //get the number of names and otherdata fromm all elements in the waypoint list
    total += getNumWptGPXData(doc->waypoints);
    total += getNumRteGPXData(doc->routes);
    total += getNumTrkGPXData(doc->tracks);
    return total;
}

// Function that returns a waypoint with the given name.  If more than one exists, return the first one.  
// Return NULL if the waypoint does not exist
Waypoint* getWaypoint(const GPXdoc* doc, char* name) {
    if (doc == NULL || name == NULL) {
        return NULL;
    }
    ListIterator iter = createIterator(doc->waypoints);
    void * elem;
    while ((elem = nextElement(&iter)) != NULL){
		Waypoint* tmp = (Waypoint*)elem;
		if (strcmp(name, (char *)tmp->name)==0) {
            return tmp;
        }
	}
    return NULL;
}
// Function that returns a track with the given name.  If more than one exists, return the first one. 
// Return NULL if the track does not exist 
Track* getTrack(const GPXdoc* doc, char* name) {
    if (doc == NULL || name == NULL) {
        return NULL;
    }
    ListIterator iter = createIterator(doc->tracks);
    void * elem;
    while ((elem = nextElement(&iter)) != NULL){
		Track* tmp = (Track*)elem;
		if (strcmp(name, (char *)tmp->name)==0) {
            return tmp;
        }
	}
    return NULL;
}
// Function that returns a route with the given name.  If more than one exists, return the first one.  
// Return NULL if the route does not exist
Route* getRoute(const GPXdoc* doc, char* name) {
    if (doc == NULL || name == NULL) {
        return NULL;
    }
    ListIterator iter = createIterator(doc->routes);
    void * elem;
    while ((elem = nextElement(&iter)) != NULL){
		Route* tmp = (Route*)elem;
		if (strcmp(name, (char *)tmp->name)==0) {
            return tmp;
        }
	}
    return NULL;
}


/* ******************************* List helper functions  - MUST be implemented *************************** */

void deleteGpxData( void* data) {
    if (data == NULL){
		return;
	}
    GPXData *gpxData = (GPXData *)data;
    free(gpxData);
}
char* gpxDataToString( void* data) {
    if (data == NULL){
		return NULL;
	}
    GPXData *gpxData = (GPXData *)data;
    char *string = malloc(strlen((char *)gpxData->name) + strlen((char *)gpxData->value) +20);
    sprintf(string, "name: %s\nvalue: %s\n", gpxData->name, gpxData->value);
    return string;
}
int compareGpxData(const void *first, const void *second) {
    if (first == NULL || second == NULL){
		return -1;
	}
    GPXData *gpxData1 = (GPXData *)first;
    GPXData *gpxData2 = (GPXData *)second;
    return(strcmp(gpxData1->name, gpxData2->name));
}

void deleteWaypoint(void* data) {
    if (data == NULL){
		return;
	}
    Waypoint *wpt = (Waypoint *)data;
    free(wpt->name);
    freeList(wpt->otherData);
    free(wpt);
}
char* waypointToString( void* data) {
    if (data == NULL){
		return NULL;
	}
    Waypoint *wpt = (Waypoint *)data;
    char *string = malloc(getWptstrlen(wpt));
    sprintf(string, "Name: %s\nLatitude: %f\nLongitude: %f\nOther Data:\n", wpt->name, wpt->latitude, wpt->longitude);
    //get all the otherData strings
    ListIterator iter = createIterator(wpt->otherData);
    void * elem;
	while ((elem = nextElement(&iter)) != NULL){
		Waypoint* tmp = (Waypoint*)elem;
		char* str = wpt->otherData->printData(tmp);
        strcat(string, str);
        free(str);
	}
    strcat(string, "\n");

    return string;
}
int compareWaypoints(const void *first, const void *second) {
    if (first == NULL || second == NULL){
		return -1;
	}
    Waypoint *wpt1 = (Waypoint *)first;
    Waypoint *wpt2 = (Waypoint *)second;
    return(strcmp(wpt1->name, wpt2->name));
}

void deleteRoute(void* data) {
    if (data == NULL){
		return;
	}
    Route *rte = (Route *)data;
    free(rte->name);
    freeList(rte->waypoints);
    freeList(rte->otherData);
    free(rte);
}
char* routeToString(void* data) {
    if (data == NULL){
		return NULL;
	}
    Route *rte = (Route *)data;
    char *string = malloc(getRtestrlen(rte));
    sprintf(string, "Name: %s\n\n           Waypoints:\n", rte->name);
    //get all the otherData strings
    ListIterator iter = createIterator(rte->waypoints);
    void * elem;
    while ((elem = nextElement(&iter)) != NULL){
		Route* tmp = (Route*)elem;
		char* str = rte->waypoints->printData(tmp);
        strcat(string, str);
        free(str);
	}

    iter = createIterator(rte->otherData);
    strcat(string, "          Other Data:\n");
	while ((elem = nextElement(&iter)) != NULL){
		Route* tmp = (Route*)elem;
		char* str = rte->otherData->printData(tmp);
        strcat(string, str);
        free(str);
	}
    strcat(string, "\n");

    return string;
}
int compareRoutes(const void *first, const void *second) {
    if (first == NULL || second == NULL){
		return -1;
	}
    Route *rte1 = (Route *)first;
    Route *rte2 = (Route *)second;
    return(strcmp(rte1->name, rte2->name));
}

void deleteTrackSegment(void* data) {
    if (data == NULL){
		return;
	}
    TrackSegment *trkseg = (TrackSegment *)data;
    freeList(trkseg->waypoints);
    free(trkseg);
}
char* trackSegmentToString(void* data) {
    if (data == NULL){
		return NULL;
	}
    TrackSegment *trkseg = (TrackSegment *)data;
    char *string = malloc(getTrksegstrlen(trkseg));
    sprintf(string, "   Track Segment Waypoints:\n");
    //get all the otherData strings
    ListIterator iter = createIterator(trkseg->waypoints);
    void * elem;
    while ((elem = nextElement(&iter)) != NULL){
		TrackSegment* tmp = (TrackSegment*)elem;
		char* str = trkseg->waypoints->printData(tmp);
        strcat(string, str);
        free(str);
	}

    return string;
}
int compareTrackSegments(const void *first, const void *second) {
    if (first == NULL || second == NULL){
		return -1;
	}
    //TrackSegment *trkseg1 = (TrackSegment *)first;
    //TrackSegment *trkseg2 = (TrackSegment *)second;
    return(0);
}

void deleteTrack(void* data) {
    if (data == NULL){
		return;
	}
    Track *trk = (Track *)data;
    free(trk->name);
    freeList(trk->segments);
    freeList(trk->otherData);
    free(trk);
}
char* trackToString(void* data) {
    if (data == NULL){
		return NULL;
	}
    Track *trk = (Track *)data;
    char *string = malloc(getTrkstrlen(trk));
    sprintf(string, "Name: %s\n\n        Tracksegments:\n", trk->name);
    //get all the otherData strings
    ListIterator iter = createIterator(trk->segments);
    void * elem;
    while ((elem = nextElement(&iter)) != NULL){
		Track* tmp = (Track*)elem;
		char* str = trk->segments->printData(tmp);
        strcat(string, str);
        free(str);
	}

    iter = createIterator(trk->otherData);
    strcat(string, "          Other Data:\n");
	while ((elem = nextElement(&iter)) != NULL){
		Track* tmp = (Track*)elem;
		char* str = trk->otherData->printData(tmp);
        strcat(string, str);
        free(str);
	}

    return string;
}
int compareTracks(const void *first, const void *second) {
    if (first == NULL || second == NULL){
		return -1;
	}
    Track *trk1 = (Track *)first;
    Track *trk2 = (Track *)second;
    return(strcmp(trk1->name, trk2->name));
}

/* ******************************* A2 Helper Functions functions *************************** */
/** 
 * @return 1 on success and 0 on invalid GPX Data structure
 */
int insertGPXDatatoXMLtree(xmlNodePtr node, List *gpxDataList) {
    if (node == NULL || gpxDataList == NULL) {
        return 0;
    }
    
    ListIterator iter = createIterator(gpxDataList);
    void * elem;
    
    while ((elem = nextElement(&iter)) != NULL) {
        GPXData* tmp = (GPXData*)elem;
        if (strcmp(tmp->name, "")==0 || strcmp(tmp->value, "")==0) {
            return 0;
        }
        xmlNewChild(node, NULL, BAD_CAST tmp->name, BAD_CAST tmp->value);
    }
    return 1;
}

int insertWpttoXMLtree(xmlNodePtr root_node, List *wptList, int option) {
    if (root_node == NULL || wptList == NULL) {
        return 0;
    }
    char temp[10];
    char type[10];
    if (option == 2) {
        strcpy(type, "rtept");
    } else if (option == 3) {
        strcpy(type, "trkpt");
    } else {
        strcpy(type, "wpt");
    }
    ListIterator iter = createIterator(wptList);
    void * elem;

    while ((elem = nextElement(&iter)) != NULL){
		Waypoint* tmp = (Waypoint*)elem;
        if (tmp->name == NULL) {
            return 0;
        }

        xmlNodePtr node = xmlNewChild(root_node, NULL, BAD_CAST type, NULL);
        //adding latitude property
        sprintf(temp, "%lf", tmp->latitude);
        xmlNewProp(node, BAD_CAST "lat", BAD_CAST temp);
        //adding longitude property
        sprintf(temp, "%lf", tmp->longitude);
        xmlNewProp(node, BAD_CAST "lon", BAD_CAST temp);
        //adding name property if it isnt empty
        if (strcmp(tmp->name, "")!=0) {
            xmlNewChild(node, NULL, BAD_CAST "name", BAD_CAST tmp->name);
        }
        //adding any remaining GPXData
        int res = insertGPXDatatoXMLtree(node, tmp->otherData);
        if (res == 0) {
            return 0;
        }
        
	}
    return 1;
}

int insertRtetoXMLtree(xmlNodePtr root_node, List *rteList) {
    if (root_node == NULL || rteList == NULL) {
        return 0;
    }
    void * elem;
    ListIterator iter = createIterator(rteList);

    while ((elem = nextElement(&iter)) != NULL){
		Route* tmp = (Route*)elem;
        if (tmp->name == NULL) {
            return 0;
        }

        xmlNodePtr node = xmlNewChild(root_node, NULL, BAD_CAST "rte", NULL);
        //adding name property if it isnt empty
        if (strcmp(tmp->name, "")!=0) {
            xmlNewChild(node, NULL, BAD_CAST "name", BAD_CAST tmp->name);
        }
        //adding any other GPXData
        int res = insertGPXDatatoXMLtree(node, tmp->otherData);
        //adding the waypoints
        res += insertWpttoXMLtree(node, tmp->waypoints, 2);
        

        if (res != 2) {
            return 0;
        }
	}
    return 1;
}

int insertTrktoXMLtree(xmlNodePtr root_node, List *trkList) {
    if (root_node == NULL || trkList == NULL) {
        return 0;
    }
    void * elem;
    ListIterator iter = createIterator(trkList);

    while ((elem = nextElement(&iter)) != NULL){
		Track* tmp = (Track*)elem;
        if (tmp->name == NULL || tmp->segments == NULL) {
            return 0;
        }

        xmlNodePtr node = xmlNewChild(root_node, NULL, BAD_CAST "trk", NULL);
        //adding name property if it isnt empty
        if (strcmp(tmp->name, "")!=0) {
            xmlNewChild(node, NULL, BAD_CAST "name", BAD_CAST tmp->name);
        }

        //adding any other GPXData
        int res = insertGPXDatatoXMLtree(node, tmp->otherData);
        if (res == 0) {
            return 0;
        }

        //adding the trk segments
        void * elem2;
        ListIterator iter2 = createIterator(tmp->segments);

        while ((elem2 = nextElement(&iter2)) != NULL){
		    TrackSegment* tmp2 = (TrackSegment*)elem2;
            xmlNodePtr trksegnode = xmlNewChild(node, NULL, BAD_CAST "trkseg", NULL);
            int res = insertWpttoXMLtree(trksegnode, tmp2->waypoints, 3);
            if (res == 0) {
                return 0;
            }
        }
	}
    return 1;
}

xmlDoc *gpxDoctoXml(GPXdoc *gpxDoc) {
    xmlDocPtr doc = NULL;       /* document pointer */
    xmlNodePtr root_node = NULL;/* node pointers */
    char temp[10];
    int res = 0;

    if (gpxDoc == NULL || strcmp(gpxDoc->namespace, "")==0 || gpxDoc->creator == NULL || strcmp(gpxDoc->creator, "")==0) {
        return NULL;
    }

    // Creates a new document, a node and set it as a root node
    doc = xmlNewDoc(BAD_CAST "1.0");
    root_node = xmlNewNode(NULL, BAD_CAST "gpx");
    xmlDocSetRootElement(doc, root_node);

    //add the root properties
    sprintf(temp, "%.1lf", gpxDoc->version);
    xmlNewProp(root_node, BAD_CAST "version", BAD_CAST temp);
    xmlNewProp(root_node, BAD_CAST "creator", BAD_CAST gpxDoc->creator);
    xmlSetNs(root_node, xmlNewNs(root_node, BAD_CAST gpxDoc->namespace, NULL));

    //add the waypoints
    res += insertWpttoXMLtree(root_node, gpxDoc->waypoints, 1);
    //add the routes
    res += insertRtetoXMLtree(root_node, gpxDoc->routes);
    //add the tracks
    res += insertTrktoXMLtree(root_node, gpxDoc->tracks);

    if (res != 3) {
        xmlFreeDoc(doc);
        doc = NULL;
    }

    /*
     *Free the global variables that may
     *have been allocated by the parser.
     */
    xmlCleanupParser();

    // /*
    //  * this is to debug memory for regression tests
    //  */
    // xmlMemoryDump();
    return doc;
}

bool isValidXmlTree(xmlDoc *doc, char* gpxSchemaFile) {
    xmlSchemaPtr schema = NULL;
    xmlSchemaParserCtxtPtr ctxt;
    int ret;    

    if (doc == NULL || gpxSchemaFile == NULL) {
        return false;
    }
    
    xmlLineNumbersDefault(1);

    ctxt = xmlSchemaNewParserCtxt(gpxSchemaFile);

    xmlSchemaSetParserErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
    schema = xmlSchemaParse(ctxt);
    xmlSchemaFreeParserCtxt(ctxt);
    //xmlSchemaDump(stdout, schema); //To print schema dump

    if (doc == NULL) {
        return false;
    } else {
        xmlSchemaValidCtxtPtr ctxt;

        ctxt = xmlSchemaNewValidCtxt(schema);
        xmlSchemaSetValidErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
        ret = xmlSchemaValidateDoc(ctxt, doc);
        xmlSchemaFreeValidCtxt(ctxt);
        //xmlFreeDoc(doc);
    }

    // free the resource
    if(schema != NULL) {
        xmlSchemaFree(schema);
    }

    xmlSchemaCleanupTypes();
    xmlCleanupParser();
    xmlMemoryDump();

    if (ret==0) {
        return true;
    }
    return false;
}

void doNothing (void* data) {
}

/* ******************************* A2 functions  - MUST be implemented *************************** */

//Module 1

/** Function to create an GPX object based on the contents of an GPX file.
 * This function must validate the XML tree generated by libxml against a GPX schema file
 * before attempting to traverse the tree and create an GPXdoc struct
 *@pre File name cannot be an empty string or NULL.
       File represented by this name must exist and must be readable.
 *@post Either:
        A valid GPXdoc has been created and its address was returned
		or 
		An error occurred, and NULL was returned
 *@return the pinter to the new struct or NULL
 *@param gpxSchemaFile - the name of a schema file
 *@param fileName - a string containing the name of the GPX file
**/
GPXdoc* createValidGPXdoc(char* fileName, char* gpxSchemaFile) {
    xmlDoc *doc = xmlReadFile(fileName, NULL, 0);
    if (isValidXmlTree(doc, gpxSchemaFile) == true) {
        xmlFreeDoc(doc);
        GPXdoc *gpxDoc = createGPXdoc(fileName);
        return gpxDoc;
    }
    xmlFreeDoc(doc);
    return NULL;
}

/** Function to validating an existing a GPXobject object against a GPX schema file
 *@pre 
    GPXdoc object exists and is not NULL
    schema file name is not NULL/empty, and represents a valid schema file
 *@post GPXdoc has not been modified in any way
 *@return the boolean aud indicating whether the GPXdoc is valid
 *@param doc - a pointer to a GPXdoc struct
 *@param gpxSchemaFile - the name of a schema file
 **/
bool validateGPXDoc(GPXdoc* doc, char* gpxSchemaFile) {
    xmlDoc *xmlDoc = gpxDoctoXml(doc);
    bool res = isValidXmlTree(xmlDoc, gpxSchemaFile);
    xmlFreeDoc(xmlDoc);
    return res;
}

/** Function to writing a GPXdoc into a file in GPX format.
 *@pre
    GPXdoc object exists, is valid, and and is not NULL.
    fileName is not NULL, has the correct extension
 *@post GPXdoc has not been modified in any way, and a file representing the
    GPXdoc contents in GPX format has been created
 *@return a boolean value indicating success or failure of the write
 *@param
    doc - a pointer to a GPXdoc struct
 	fileName - the name of the output file
 **/
bool writeGPXdoc(GPXdoc* doc, char* fileName) {
    xmlDoc *xmlDoc = gpxDoctoXml(doc);
    int result = xmlSaveFormatFileEnc(fileName, xmlDoc, "UTF-8", 1);

    xmlFreeDoc(xmlDoc);
    if (result==0) {
        return false;
    }
    
    return true;
}



//Module 2

/** Function that returns the length of a Route
 *@pre Route object exists, is not null, and has not been freed
 *@post Route object must not be modified in any way
 *@return length of the route in meters
 *@param rt - a pointer to a Route struct
**/
float getRouteLen(const Route *rt) {
    if (rt == NULL || rt->waypoints->length == 1) {
        return 0;
    }
    
    float dist = 0;
    ListIterator iter = createIterator(rt->waypoints);
    void * elem1;
    void * elem2;
    if ((elem1 = nextElement(&iter)) != NULL) {
        Waypoint* tmp1 = (Waypoint*)elem1;
        while ((elem2 = nextElement(&iter)) != NULL) {
            Waypoint* tmp2 = (Waypoint*)elem2;
            double dLat = (tmp2->latitude - tmp1->latitude) * M_PI /180.0;
            double dLon = (tmp2->longitude - tmp1->longitude) * M_PI /180.0;

            double tempLat1 = (tmp1->latitude) * M_PI /180.0;
            double tempLat2 = (tmp2->latitude) * M_PI /180.0;

            double tempDist = pow(sin(dLat/2), 2) + pow(sin(dLon/2), 2) * cos(tempLat1) * cos(tempLat2);
            tempDist = 2 * atan2(sqrt(tempDist), sqrt(1-tempDist));
            tempDist = tempDist * 6371 * pow(10, 3);

            dist += (float)tempDist;
            tmp1 = tmp2;
        }
    }
    return dist;
}

/** Function that returns the length of a Track
 *@pre Track object exists, is not null, and has not been freed
 *@post Track object must not be modified in any way
 *@return length of the track in meters
 *@param tr - a pointer to a Track struct
**/
float getTrackLen(const Track *tr) {
    if (tr == NULL) {
        return 0;
    }
    
    float dist = 0;
    ListIterator iter = createIterator(tr->segments);
    void * elem;
    void * elem1;
    void * elem2;
    int i = 0;
    Waypoint* prevWpt;

    while ((elem = nextElement(&iter)) != NULL) {//iterator of track segments
        TrackSegment* tmp = (TrackSegment*)elem;     

        ListIterator iter1 = createIterator(tmp->waypoints);
        if ((elem1 = nextElement(&iter1)) != NULL) {
            Waypoint* tmp1 = (Waypoint*)elem1;

            if (i>0) {//if there are more than one segments calculate the distance of adjacent points
                double dLat = (tmp1->latitude - prevWpt->latitude) * M_PI /180.0;
                double dLon = (tmp1->longitude - prevWpt->longitude) * M_PI /180.0;

                double tempLat1 = (prevWpt->latitude) * M_PI /180.0;
                double tempLat2 = (tmp1->latitude) * M_PI /180.0;

                double tempDist = pow(sin(dLat/2), 2) + pow(sin(dLon/2), 2) * cos(tempLat1) * cos(tempLat2);
                tempDist = 2 * atan2(sqrt(tempDist), sqrt(1-tempDist));
                tempDist = tempDist * 6371 * pow(10, 3);

                dist += (float)tempDist;
            }

            prevWpt = tmp1;//in case there are no more wpts in the segment but may have more segments
            //get the distance of the waypoints
            while ((elem2 = nextElement(&iter1)) != NULL) {
                Waypoint* tmp2 = (Waypoint*)elem2;
                double dLat = (tmp2->latitude - tmp1->latitude) * M_PI /180.0;
                double dLon = (tmp2->longitude - tmp1->longitude) * M_PI /180.0;

                double tempLat1 = (tmp1->latitude) * M_PI /180.0;
                double tempLat2 = (tmp2->latitude) * M_PI /180.0;

                double tempDist = pow(sin(dLat/2), 2) + pow(sin(dLon/2), 2) * cos(tempLat1) * cos(tempLat2);
                tempDist = 2 * atan2(sqrt(tempDist), sqrt(1-tempDist));
                tempDist = tempDist * 6371 * pow(10, 3);

                dist += (float)tempDist;
                tmp1 = tmp2;
                prevWpt = tmp2;
            }
        }
        i++;
    }
    
    return dist;
}

/** Function that rounds the length of a track or a route to the nearest 10m
 *@pre Length is not negative
 *@return length rounded to the nearest 10m
 *@param len - length
**/
float round10(float len) {
    int div = len/10;
    int rem = (int)len%10;
    return rem<5 ? div*10 : div*10+10;
}

/** Function that returns the number routes with the specified length, using the provided tolerance 
 * to compare route lengths
 *@pre GPXdoc object exists, is not null
 *@post GPXdoc object exists, is not null, has not been modified
 *@return the number of routes with the specified length
 *@param doc - a pointer to a GPXdoc struct
 *@param len - search route length
 *@param delta - the tolerance used for comparing route lengths
**/
int numRoutesWithLength(const GPXdoc* doc, float len, float delta) {
    if (doc == NULL || len < 0 || delta < 0) {
        return 0;
    }

    int total = 0;
    ListIterator iter = createIterator(doc->routes);
    void * elem;

    while ((elem = nextElement(&iter)) != NULL) {//iterate through each route in the list
        Route* tmp = (Route*)elem;
        float routeLen = getRouteLen(tmp);//get the route length
        if (routeLen - len <= delta && routeLen - len >= -delta) {//if its within the delta increment the total
            total++;
        }
    }
    return total;
}


/** Function that returns the number tracks with the specified length, using the provided tolerance 
 * to compare track lengths
 *@pre GPXdoc object exists, is not null
 *@post GPXdoc object exists, is not null, has not been modified
 *@return the number of tracks with the specified length
 *@param doc - a pointer to a GPXdoc struct
 *@param len - search track length
 *@param delta - the tolerance used for comparing track lengths
**/
int numTracksWithLength(const GPXdoc* doc, float len, float delta) {
    if (doc == NULL || len < 0 || delta < 0) {
        return 0;
    }

    int total = 0;
    ListIterator iter = createIterator(doc->tracks);
    void * elem;

    while ((elem = nextElement(&iter)) != NULL) {//iterate through each route in the list
        Track* tmp = (Track*)elem;
        float trkLen = getTrackLen(tmp);//get the route length
        if (trkLen - len <= delta && trkLen - len >= -delta) {//if its within the delta increment the total
            total++;
        }
    }
    return total;
}

/** Function that checks if the current route is a loop
 *@pre Route object exists, is not null
 *@post Route object exists, is not null, has not been modified
 *@return true if the route is a loop, false otherwise
 *@param route - a pointer to a Route struct
 *@param delta - the tolerance used for comparing distances between start and end points
**/
bool isLoopRoute(const Route* route, float delta) {
    if (route == NULL || delta < 0 || route->waypoints->length < 4) {
        return false;
    }

    Waypoint *firstWpt = (Waypoint *)getFromFront(route->waypoints);
    Waypoint *lastWpt = (Waypoint *)getFromBack(route->waypoints);

    double dLat = (lastWpt->latitude - firstWpt->latitude) * M_PI / 180.0;
    double dLon = (lastWpt->longitude - firstWpt->longitude) * M_PI / 180.0;

    double tempLat1 = (firstWpt->latitude) * M_PI / 180.0;
    double tempLat2 = (lastWpt->latitude) * M_PI / 180.0;

    double dist = pow(sin(dLat/2), 2) + pow(sin(dLon/2), 2) * cos(tempLat1) * cos(tempLat2);
    dist = 2 * atan2(sqrt(dist), sqrt(1-dist));
    dist = dist * 6371 * pow(10, 3);

    if (dist < delta && dist > -delta) {
        return true;
    }
    return false;
}


/** Function that checks if the current track is a loop
 *@pre Track object exists, is not null
 *@post Track object exists, is not null, has not been modified
 *@return true if the track is a loop, false otherwise
 *@param track - a pointer to a Track struct
 *@param delta - the tolerance used for comparing distances between start and end points
**/
bool isLoopTrack(const Track *tr, float delta) {
    if (tr == NULL || delta < 0) {
        return false;
    }

    int totalWpts = 0;
    ListIterator iter = createIterator(tr->segments);
    void * elem;
    while ((elem = nextElement(&iter)) != NULL){
		TrackSegment* tmp = (TrackSegment*)elem;
		totalWpts += tmp->waypoints->length;
	}
    if (totalWpts < 4) {
        return false;
    }
    

    Waypoint *firstWpt = (Waypoint *)getFromFront(((TrackSegment *)getFromFront(tr->segments))->waypoints);
    Waypoint *lastWpt = (Waypoint *)getFromBack(((TrackSegment *)getFromBack(tr->segments))->waypoints);

    double dLat = (lastWpt->latitude - firstWpt->latitude) * M_PI / 180.0;
    double dLon = (lastWpt->longitude - firstWpt->longitude) * M_PI / 180.0;

    double tempLat1 = (firstWpt->latitude) * M_PI / 180.0;
    double tempLat2 = (lastWpt->latitude) * M_PI / 180.0;

    double dist = pow(sin(dLat/2), 2) + pow(sin(dLon/2), 2) * cos(tempLat1) * cos(tempLat2);
    dist = 2 * atan2(sqrt(dist), sqrt(1-dist));
    dist = dist * 6371 * pow(10, 3);

    if (dist < delta && dist > -delta) {
        return true;
    }
    return false;
}


/** Function that returns all routes between the specified start and end locations
 *@pre GPXdoc object exists, is not null
 *@post GPXdoc object exists, is not null, has not been modified
 *@return a list of Route structs that connect the given sets of coordinates
 *@param doc - a pointer to a GPXdoc struct
 *@param sourceLat - latitude of the start location
 *@param sourceLong - longitude of the start location
 *@param destLat - latitude of the destination location
 *@param destLong - longitude of the destination location
 *@param delta - the tolerance used for comparing distances between waypoints 
*/
List* getRoutesBetween(const GPXdoc* doc, float sourceLat, float sourceLong, float destLat, float destLong, float delta) {
    if (doc == NULL|| doc->routes == NULL) {
        return NULL;    
    }
    List *rteList = initializeList(&routeToString, &doNothing, &compareRoutes);

    ListIterator iter = createIterator(doc->routes);
    void * elem;
    while ((elem = nextElement(&iter)) != NULL){//for every route
		Route* tmp = (Route*)elem;
        /**
         * check if the first latitude - source latitude is within delta
         * check if the first longitude - source longitude is within delta
         * check if the last latitude - dest latitude is within delta
         * check if the last longitude - dest longitude is within delta
         */
		if (((Waypoint *)getFromFront(tmp->waypoints))!=NULL && ((Waypoint *)getFromBack(tmp->waypoints))!=NULL &&((Waypoint *)getFromFront(tmp->waypoints))->latitude - sourceLat <= delta && ((Waypoint *)getFromFront(tmp->waypoints))->latitude - sourceLat >= -delta 
            && ((Waypoint *)getFromFront(tmp->waypoints))->longitude - sourceLong <= delta && ((Waypoint *)getFromFront(tmp->waypoints))->longitude - sourceLong >= -delta 
            && ((Waypoint *)getFromBack(tmp->waypoints))->latitude - destLat <= delta && ((Waypoint *)getFromBack(tmp->waypoints))->latitude - destLat >= -delta 
            && ((Waypoint *)getFromBack(tmp->waypoints))->longitude - destLong <= delta && ((Waypoint *)getFromBack(tmp->waypoints))->longitude - destLong >= -delta) {
            insertBack(rteList, tmp);
        }
	}

    if (rteList->length == 0) {
        freeList(rteList);
        return NULL;
    }
    return rteList;
}

/** Function that returns all Tracks between the specified start and end locations
 *@pre GPXdoc object exists, is not null
 *@post GPXdoc object exists, is not null, has not been modified
 *@return a list of Track structs that connect the given sets of coordinates
 *@param doc - a pointer to a GPXdoc struct
 *@param sourceLat - latitude of the start location
 *@param sourceLong - longitude of the start location
 *@param destLat - latitude of the destination location
 *@param destLong - longitude of the destination location
 *@param delta - the tolerance used for comparing distances between waypoints 
*/
List* getTracksBetween(const GPXdoc* doc, float sourceLat, float sourceLong, float destLat, float destLong, float delta) {
    if (doc == NULL || doc->tracks == NULL) {
        return NULL;    
    }
    List *trkList = initializeList(&trackToString, &doNothing, &compareTracks);

    ListIterator iter = createIterator(doc->tracks);
    void * elem;
    while ((elem = nextElement(&iter)) != NULL){//for every route
		Track* tmp = (Track*)elem;
        /**
         * check if the first latitude - source latitude is within delta
         * check if the first longitude - source longitude is within delta
         * check if the last latitude - dest latitude is within delta
         * check if the last longitude - dest longitude is within delta
         */
		if (getFromFront(tmp->segments)!=NULL && ((Waypoint *)getFromFront( ((TrackSegment *)getFromFront(tmp->segments))->waypoints ))!=NULL && getFromFront(tmp->segments)!=NULL && ((Waypoint *)getFromBack( ((TrackSegment *)getFromBack(tmp->segments))->waypoints ))!=NULL && ((Waypoint *)getFromFront( ((TrackSegment *)getFromFront(tmp->segments))->waypoints ))->latitude - sourceLat <= delta && ((Waypoint *)getFromFront( ((TrackSegment *)getFromFront(tmp->segments))->waypoints ))->latitude - sourceLat >= -delta 
            && ((Waypoint *)getFromFront( ((TrackSegment *)getFromFront(tmp->segments))->waypoints ))->longitude - sourceLong <= delta && ((Waypoint *)getFromFront( ((TrackSegment *)getFromFront(tmp->segments))->waypoints ))->longitude - sourceLong >= -delta 
            && ((Waypoint *)getFromBack( ((TrackSegment *)getFromBack(tmp->segments))->waypoints ))->latitude - destLat <= delta && ((Waypoint *)getFromBack( ((TrackSegment *)getFromBack(tmp->segments))->waypoints ))->latitude - destLat >= -delta 
            && ((Waypoint *)getFromBack( ((TrackSegment *)getFromBack(tmp->segments))->waypoints ))->longitude - destLong <= delta && ((Waypoint *)getFromBack( ((TrackSegment *)getFromBack(tmp->segments))->waypoints ))->longitude - destLong >= -delta) {

            insertBack(trkList, tmp);
        }
	}
    
    if (trkList->length == 0) {
        freeList(trkList);
        return NULL;
    }
    return trkList;
}


//Module 3

int intStrlen(int number) {
    int num = number;
    int len = 0;
    while (num != 0 ) {
        num = num/10;
        len++;
    }
    return len;
}

int noWptsinTrk(const Track *tr) {
    if (tr == NULL) {
        return -1;
    }
    int total=0;
    ListIterator iter = createIterator(tr->segments);
    void * elem;
    while ((elem = nextElement(&iter)) != NULL){//for every trksegment
        TrackSegment* tmp = (TrackSegment*)elem;
        ListIterator iter2 = createIterator(tmp->waypoints);
        void * elem2;
        while ((elem2 = nextElement(&iter2)) != NULL){//for every waypoint
            total++;
        }
    }
    return total;
}

/** Function to converting a Track into a JSON string
 *@pre Track is not NULL
 *@post Track has not been modified in any way
 *@return A string in JSON format
 *@param event - a pointer to a Track struct
 **/
char* trackToJSON(const Track *tr) {
    if (tr == NULL) {
        char *string = malloc(sizeof(char)*3);
        strcpy(string, "{}");
        return string;
    }
    
    float trackLen = round10(getTrackLen(tr));
    char loopStat[6];
    if (isLoopTrack(tr, 10.0) == true) {
        strcpy(loopStat, "true");
    } else {
        strcpy(loopStat, "false");
    }

    char *string;
    int noWpts = noWptsinTrk(tr);
    if (strcmp(tr->name, "")==0) {
        string = malloc(sizeof(char)*(50 + 4 + intStrlen(noWpts) + intStrlen(trackLen)));
        sprintf(string, "{\"name\":\"\",\"numPoints\":%d,\"len\":%.1f,\"loop\":%s}", noWpts, trackLen, loopStat);
    } else {
        string = malloc(sizeof(char)*(50 + strlen(tr->name) + intStrlen(noWpts) + intStrlen(trackLen)));
        sprintf(string, "{\"name\":\"%s\",\"numPoints\":%d,\"len\":%.1f,\"loop\":%s}", tr->name, noWpts, trackLen, loopStat);
    }
    return string;
}

/** Function to converting a Route into a JSON string
 *@pre Route is not NULL
 *@post Route has not been modified in any way
 *@return A string in JSON format
 *@param event - a pointer to a Route struct
 **/
char* routeToJSON(const Route *rt) {
    if (rt == NULL) {
        char *string = malloc(sizeof(char)*3);
        strcpy(string, "{}");
        return string;
    }
    
    float routeLen = round10(getRouteLen(rt));
    char loopStat[6];
    if (isLoopRoute(rt, 10.0) == true) {
        strcpy(loopStat, "true");
    } else {
        strcpy(loopStat, "false");
    }

    char *string;
    if (strcmp(rt->name, "") == 0) {
        string = malloc(sizeof(char)*(50 + 4 + intStrlen(rt->waypoints->length) + intStrlen(routeLen)));
        sprintf(string, "{\"name\":\"\",\"numPoints\":%d,\"len\":%.1f,\"loop\":%s}", rt->waypoints->length, routeLen, loopStat);
    } else {
        string = malloc(sizeof(char)*(50 + strlen(rt->name) + intStrlen(rt->waypoints->length) + intStrlen(routeLen)));
        sprintf(string, "{\"name\":\"%s\",\"numPoints\":%d,\"len\":%.1f,\"loop\":%s}",  rt->name, rt->waypoints->length, routeLen, loopStat);
    }

    return string;
}

/** Function to converting a list of Route structs into a JSON string
 *@pre Route list is not NULL
 *@post Route list has not been modified in any way
 *@return A string in JSON format
 *@param event - a pointer to a List struct
 **/
char* routeListToJSON(const List *list) {
    char *string = malloc(sizeof(char)*3);
    if (list == NULL || list->length == 0) {
        strcpy(string, "[]");
        return string;
    }

    char *tempString;
    int totalLen = 0;
    int i = 0;
    strcpy(string, "[");
    
    ListIterator iter = createIterator((List *)list);
    void * elem;
    while ((elem = nextElement(&iter)) != NULL){//for every route
		Route* tmp = (Route*)elem;

        if (i != 0) {
            strcat(string, ",");
        }
        
        tempString = routeToJSON(tmp);
        totalLen += strlen(tempString);
        string = realloc(string, sizeof(char)*(totalLen + 2 + list->length));
        strcat(string, tempString);
        free(tempString);

        i++;
	}
    strcat(string, "]");
    return string;
}

/** Function to converting a list of Route->Waypoint structs into a JSON string
 *@pre Route->Waypoint list is not NULL
 *@post Route->Waypoint list has not been modified in any way
 *@return A string in JSON format
 *@param event - a pointer to a List struct
 **/
char *rteWptListToJSON(const List *list) {
    char *string = malloc(sizeof(char)*3);
    if (list == NULL || list->length == 0) {
        strcpy(string, "[]");
        return string;
    }

    char *tempString;
    int totalLen = 0;
    int i = 0;
    strcpy(string, "[");
    
    ListIterator iter = createIterator((List *)list);
    void * elem;
    while ((elem = nextElement(&iter)) != NULL){//for every route
		Waypoint* tmp = (Waypoint*)elem;

        if (i != 0) {
            strcat(string, ",");
        }

        if (tmp == NULL) {
            tempString = malloc(sizeof(char)*3);
            strcpy(tempString, "{}");
        } else {
            if (strcmp(tmp->name, "") == 0) {
                tempString = malloc(sizeof(char)*(100 + 4 + intStrlen(tmp->longitude) + intStrlen(tmp->latitude)));
                sprintf(tempString, "{\"name\":\"\",\"index\":%d,\"lat\":%.7lf,\"lon\":%.7lf}", i, tmp->latitude, tmp->longitude);
            } else {
                tempString = malloc(sizeof(char)*(100 + 4 + strlen(tmp->name) + intStrlen(tmp->longitude) + intStrlen(tmp->latitude)));
                sprintf(tempString, "{\"name\":\"%s\",\"index\":%d,\"lat\":%.7lf,\"lon\":%.7lf}", tmp->name, i, tmp->latitude, tmp->longitude);
            }
        }

        totalLen += strlen(tempString);
        string = realloc(string, sizeof(char)*(totalLen + 2 + list->length));
        strcat(string, tempString);
        free(tempString);

        i++;
	}
    strcat(string, "]");
    return string;
}

/** Function to converting a list of Track structs into a JSON string
 *@pre Track list is not NULL
 *@post Track list has not been modified in any way
 *@return A string in JSON format
 *@param event - a pointer to a List struct
 **/
char* trackListToJSON(const List *list) {
    char *string = malloc(sizeof(char)*3);
    if (list == NULL || list->length == 0) {
        strcpy(string, "[]");
        return string;
    }

    char *tempString;
    int totalLen = 0;
    int i = 0;
    strcpy(string, "[");
    
    ListIterator iter = createIterator((List *)list);
    void * elem;
    while ((elem = nextElement(&iter)) != NULL){//for every route
		Track* tmp = (Track*)elem;

        if (i != 0) {
            strcat(string, ",");
        }
        
        tempString = trackToJSON(tmp);
        totalLen += strlen(tempString);
        string = realloc(string, sizeof(char)*(totalLen + 2 + list->length));
        strcat(string, tempString);
        free(tempString);

        i++;
	}
    strcat(string, "]");
    return string;
}

/** Function to converting a GPXdoc into a JSON string
 *@pre GPXdoc is not NULL
 *@post GPXdoc has not been modified in any way
 *@return A string in JSON format
 *@param event - a pointer to a GPXdoc struct
 **/
char* GPXtoJSON(const GPXdoc* gpx) {
    if (gpx == NULL) {
        char *string = malloc(sizeof(char)*3);
        strcpy(string, "{}");
        return string;
    }

    char *string = malloc(sizeof(char)*(80 + intStrlen(gpx->version) + strlen(gpx->creator) + intStrlen(gpx->waypoints->length) + intStrlen(gpx->routes->length) + intStrlen(gpx->tracks->length)));
    sprintf(string, "{\"version\":%.1lf,\"creator\":\"%s\",\"numWaypoints\":%d,\"numRoutes\":%d,\"numTracks\":%d}",  gpx->version, gpx->creator, gpx->waypoints->length, gpx->routes->length, gpx->tracks->length);
    return string;
}


// ***************************** Bonus A2 functions ********************************

/** Function to adding an Waypont struct to an existing Route struct
 *@pre arguments are not NULL
 *@post The new waypoint has been added to the Route's waypoint list
 *@return N/A
 *@param rt - a Route struct
 *@param pr - a Waypoint struct
 **/
void addWaypoint(Route *rt, Waypoint *pt) {
    if (rt == NULL || pt == NULL) {
        return;
    }
    insertBack(rt->waypoints, pt);
}

/** Function to adding an Route struct to an existing GPXdoc struct
 *@pre arguments are not NULL
 *@post The new route has been added to the GPXdoc's routes list
 *@return N/A
 *@param doc - a GPXdoc struct
 *@param rt - a Route struct
 **/
void addRoute(GPXdoc* doc, Route* rt) {
    if (doc == NULL || rt == NULL) {
        return;
    }
    insertBack(doc->routes, rt);
}

/** Function to converting a JSON string into an GPXdoc struct
 *@pre JSON string is not NULL
 *@post String has not been modified in any way
 *@return A newly allocated and initialized GPXdoc struct
 *@param str - a pointer to a string
 **/
GPXdoc* JSONtoGPX(const char* gpxString) {
    if (gpxString == NULL) {
        return NULL;
    }
    GPXdoc *rootDoc = malloc(sizeof(GPXdoc));
    if (rootDoc == NULL) {
        return NULL;
    }
    
    strcpy(rootDoc->namespace, "http://www.topografix.com/GPX/1/1");

    double ver;
    char str[strlen(gpxString)-20];
    sscanf(gpxString, "{\"version\":%lf,\"creator\":\"%s", &ver, str);
    rootDoc->version = ver;
    strcpy(str, gpxString+(25 + intStrlen(ver)));
    str[strcspn(str, "\"")] = '\0';
    rootDoc->creator = malloc(strlen(str)+1);
    strcpy(rootDoc->creator, str);

    rootDoc->waypoints = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
    rootDoc->routes = initializeList(&routeToString, &deleteRoute, &compareRoutes);
    rootDoc->tracks = initializeList(&trackToString, &deleteTrack, &compareTracks);

    return rootDoc;
}

/** Function to converting a JSON string into an Waypoint struct
 *@pre JSON string is not NULL
 *@post String has not been modified in any way
 *@return A newly allocated and initialized Waypoint struct
 *@param str - a pointer to a string
 **/
Waypoint* JSONtoWaypoint(const char* gpxString) {
    if (gpxString == NULL) {
        return NULL;
    }
    Waypoint *wpt = malloc(sizeof(Waypoint));
    if (wpt == NULL) {
        return NULL;
    }

    double lat, lon;
    sscanf(gpxString, "{\"lat\":%lf,\"lon\":%lf", &lat, &lon);
    wpt->latitude = lat;
    wpt->longitude = lon;
    wpt->name = malloc(sizeof(char)*3);
    strcpy(wpt->name, "");
    wpt->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);

    return wpt;
}

/** Function to converting a JSON string into an Route struct
 *@pre JSON string is not NULL
 *@post String has not been modified in any way
 *@return A newly allocated and initialized Route struct
 *@param str - a pointer to a string
 **/
Route* JSONtoRoute(const char* gpxString) {
    if (gpxString == NULL) {
        return NULL;
    }
    Route *rte = malloc(sizeof(Route));
    if (rte == NULL) {
        return NULL;
    }

    char str[strlen(gpxString)-9];
    strcpy(str, gpxString + 9);
    str[strcspn(str, "\"")] = '\0';

    rte->name = malloc(strlen(str)+1);
    strcpy(rte->name, str);

    rte->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);
    rte->waypoints = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);

    return rte;
}

//************glue functions*************

/**
 * function to be used to get the json string of the gpx doc from the filename
 * @return string of the json version of the gpx doc
 */
char *filenametogpxJSON(char *filename) {
    GPXdoc *doc = createValidGPXdoc(filename, "gpx.xsd");
    char *string = GPXtoJSON(doc);
    deleteGPXdoc(doc);
    return string;
}

/**
 * function to be used to get the Route list json string of the gpx doc from the filename
 * @return string of the json version of the gpx doc's route list
 */
char *filenametoRouteListJSON(char *filename) {
    GPXdoc *doc = createValidGPXdoc(filename, "gpx.xsd");
    char *string;
    if (doc != NULL) {
        string = routeListToJSON(doc->routes);
    } else {
        string = routeListToJSON(NULL);
    }
    deleteGPXdoc(doc);
    return string;
}

/**
 * function to be used to get the Wpt list json string of the a route gpx doc from the filename
 * @return string of the json version of the gpx doc's, route's, wpt list
 */
char *filenametoRteWptListJSON(char *filename, char *routeName) {
    GPXdoc *doc = createValidGPXdoc(filename, "gpx.xsd");
    char *string;
    if (doc != NULL) {
        Route *rt = getRoute(doc, routeName);
        if (rt != NULL) {
            string = rteWptListToJSON(rt->waypoints);
        } else {
            string = rteWptListToJSON(NULL);
        }
    } else {
        string = rteWptListToJSON(NULL);
    }
    deleteGPXdoc(doc);
    return string;
}

/**
 * function to be used to get the track list json string of the gpx doc from the filename
 * @return string of the json version of the gpx doc's track list
 */
char *filenametoTrackListJSON(char *filename) {
    GPXdoc *doc = createValidGPXdoc(filename, "gpx.xsd");
    char *string;
    if (doc != NULL) {
        string = trackListToJSON(doc->tracks);
    } else {
        string = trackListToJSON(NULL);
    }
    deleteGPXdoc(doc);
    return string;
}

/**
 * function to be used to create a gpx file based on filename.
 * @return int 1 on success and 0 of failure
 */
int createGPX(char *filename, char *gpxstring) {
    GPXdoc *doc = JSONtoGPX(gpxstring);
    if (validateGPXDoc(doc, "gpx.xsd") == true) {
        writeGPXdoc(doc, filename);
        deleteGPXdoc(doc);
        return 1;
    } else {
        deleteGPXdoc(doc);
        return 0;
    }
}

/**
 * function to be used to add a route to a given file
 * @return int 1 on success and 0 of failure
 */
int addRtetoFile(char *filename, char *rteString, char *wptString) {
    GPXdoc *doc = createValidGPXdoc(filename, "gpx.xsd");
    Route* rte = JSONtoRoute(rteString);
    if (strlen(wptString)>3) {
        char string[strlen(wptString)];
        memcpy(string, &wptString[1], strlen(wptString)-2);
        string[strlen(wptString)-2] = '\0';
        char temp[1000];
        int j = 0;
        
        for (int i=0; i<strlen(string); i++) {
            temp[i-j] = string[i];
            if (string[i]=='}') {
                temp[i+1]='\0';
                Waypoint *pt = JSONtoWaypoint(temp);
                addWaypoint(rte, pt);
                j = i+1;
                for (int k=0; k<1000; k++) {
                    temp[k]='\0';
                }
            }
        }
    }

    addRoute(doc, rte);

    if (validateGPXDoc(doc, "gpx.xsd") == true) {
        writeGPXdoc(doc, filename);
        deleteGPXdoc(doc);
        return 1;
    } else {
        deleteGPXdoc(doc);
        return 0;
    }
}

/**
 * function ges a Json of the routes between the path
 * @return the JSON formatted string of routes
 */
char *routeListJSONBwetweenPath(char *filename, float spLat, float spLon, float epLat, float epLon, float accuracy) {
    GPXdoc *doc = createValidGPXdoc(filename, "gpx.xsd");
    List* rteList = getRoutesBetween(doc, spLat, spLon, epLat, epLon, accuracy);
    char *string = routeListToJSON(rteList);
    deleteGPXdoc(doc);
    return string;
}

/**
 * function ges a Json of the tracks between the path
 * @return the JSON formatted string of tracks
 */
char *trackListJSONBwetweenPath(char *filename, float spLat, float spLon, float epLat, float epLon, float accuracy) {
    GPXdoc *doc = createValidGPXdoc(filename, "gpx.xsd");
    List* trkList = getTracksBetween(doc, spLat, spLon, epLat, epLon, accuracy);
    char *string = trackListToJSON(trkList);
    deleteGPXdoc(doc);
    return string;
}

int renameComp(char* filename, char* compName, char* newName) {
    GPXdoc *doc = createValidGPXdoc(filename, "gpx.xsd");
    Route *rt = getRoute(doc, compName);
    Track *tr = getTrack(doc, compName);
    if (rt != NULL) {
        strcpy(rt->name, newName);
        writeGPXdoc(doc, filename);
        deleteGPXdoc(doc);
        return 1;
    } else if (tr != NULL) {
        strcpy(tr->name, newName);
        writeGPXdoc(doc, filename);
        deleteGPXdoc(doc);
        return 1;
    }
    deleteGPXdoc(doc);
    return 0;
}

char *getrteOtherData(GPXdoc *doc, char*compName) {
    Route *rt = getRoute(doc, compName);
    ListIterator iter = createIterator(rt->otherData);
    void * elem;
    int i = 0;
    char *string = malloc(3+500*rt->otherData->length);
    strcpy(string, "[");
    while ((elem = nextElement(&iter)) != NULL){
		GPXData* tmp = (GPXData*)elem;
        char *tempstring = malloc(sizeof(char)*500);
        sprintf(tempstring, "{\"name\":\"%s\", \"val\":\"%s\"}", tmp->name, tmp->value);
        if (i != 0) {
            strcat(string, ",");
        }
        strcat(string, tempstring);
        free(tempstring);

        i++;
	}
    strcat(string, "]");
    return string;
}

char *gettrkOtherData(GPXdoc *doc, char* compName) {
    Track *tr = getTrack(doc, compName);
    ListIterator iter = createIterator(tr->otherData);
    void * elem;
    int i = 0;
    char *string = malloc(3+500*tr->otherData->length);
    strcpy(string, "[");
    while ((elem = nextElement(&iter)) != NULL){
		GPXData* tmp = (GPXData*)elem;
        char *tempstring = malloc(sizeof(char)*500);
        sprintf(tempstring, "{\"name\":\"%s\", \"val\":\"%s\"}", tmp->name, tmp->value);
        if (i != 0) {
            strcat(string, ",");
        }
        strcat(string, tempstring);
        free(tempstring);

        i++;
	}
    strcat(string, "]");
    return string;
}

char *details(char* filename, char* compName) {
    GPXdoc *doc = createValidGPXdoc(filename, "gpx.xsd");
    Route *rt = getRoute(doc, compName);
    Track *tr = getTrack(doc, compName);
    if (rt != NULL) {
        char * string = getrteOtherData(doc, compName);
        deleteGPXdoc(doc);
        return string;
    } else if (tr != NULL) {
        char * string = gettrkOtherData(doc, compName);
        deleteGPXdoc(doc);
        return string;
    }
    deleteGPXdoc(doc);
    char *string = malloc(3);
    strcpy(string, "[]");
    return string;
}