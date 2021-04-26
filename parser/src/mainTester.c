#include "GPXParser.h"

int main(int argc, char const *argv[]) {
    GPXdoc *doc = createValidGPXdoc("testFiles/Memorial_Forest_Loop(1rt_0trk_0seg_430m).gpx", "gpx.xsd");
    if (doc == NULL) {
        deleteGPXdoc(doc);
        printf("failed1\n");
    }

    // char *string = GPXdocToString(doc);
    // printf("%s\n", string);

    printf("valid %d\n", validateGPXDoc(doc, "gpx.xsd"));
    writeGPXdoc(doc, "temp.xml");
    printf("valid %d\n", validateGPXDoc(doc, "gpx.xsd"));

    // char string[10000];
    // string = GPXtoJSON(doc);

    // printf("%s\n", string);
    printf("%s\n%s\n%s\n", GPXtoJSON(doc), routeListToJSON(doc->routes), trackListToJSON(doc->tracks));

    int rlen = getRouteLen(getFromFront(doc->routes));
    int tlen = getTrackLen(getFromFront(doc->tracks));

    printf("\nr:%d t:%d\nno r:%d\nno t:%d\n", rlen, tlen, numRoutesWithLength(doc, rlen, 10), numTracksWithLength(doc, tlen, 10));
    deleteGPXdoc(doc);

    doc = createValidGPXdoc("temp.xml", "gpx.xsd");
    if (doc == NULL) {
        printf("failed2\n");
    }
    deleteGPXdoc(doc);
    
    //printf("%lf %lf %lf %lf\n", round10(413), round10(424.5), round10(435), round10(449));

    doc = JSONtoGPX("{\"version\":1.1,\"creator\":\"WebTool\"}");
    printf("%s\n%lf\n%s\n", doc->namespace, doc->version, doc->creator);

    Waypoint *wpt = JSONtoWaypoint("{\"lat\":43.537299,\"lon\":-80.218267}");
    printf("\n%lf %lf\n", wpt->latitude, wpt->longitude);
    insertBack(doc->waypoints, wpt);

    Route *rte = JSONtoRoute("{\"name\":\"Reynolds Walk\"}");
    printf("\n%s\n", rte->name);
    insertBack(doc->routes, rte);

    deleteGPXdoc(doc);

    // char *string = GPXdocToString(doc);
    // printf("%s\n", string);
    // free(string);

    //printf("%d\n%d\n%d\n%d\n%d\n", getNumWaypoints(doc), getNumRoutes(doc), getNumTracks(doc), getNumSegments(doc), getNumGPXData(doc));

    // Track *wpt = getTrack(doc, "Some other trail");
    // string = trackToString(wpt);
    // if (string == NULL) {
    //     printf(" test\n");
    // } else {
    //     printf("%s\n", string);
    //     free(string);
    // }


    return 0;
}
