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

// --- Existing JSON APIs ---
char* nexthopgroup_to_json(const NextHopGroupFull* obj);
int nexthopgroup_from_json(const char* json_str, NextHopGroupFull** out_obj);

#ifdef __cplusplus
}
#endif

#endif // NEXTHOPGROUP_CAPI_H