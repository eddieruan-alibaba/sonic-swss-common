#ifndef NEXTHOPGROUP_CAPI_H
#define NEXTHOPGROUP_CAPI_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct NextHopGroupFull NextHopGroupFull;

// Create a new (empty) NextHopGroupFull
NextHopGroupFull* nexthopgroup_create(void);

// Free the object
void nexthopgroup_free(NextHopGroupFull* obj);

// JSON APIs ---
char* nexthopgroup_to_json(NextHopGroupFull* obj);


#ifdef __cplusplus
}
#endif

#endif // NEXTHOPGROUP_CAPI_H