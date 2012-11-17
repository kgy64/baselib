#ifndef __DEBUGLEVELS_H__
#define __DEBUGLEVELS_H__

enum {
    DL_NONE     =   0x00000000,
    DL_CALLS    =   0x00000001,
    DL_CONFIG   =   0x00000002,
    DL_FILTER   =   0x00000004,
    DL_FILE     =   0x00000008,
    DL_SERIAL   =   0x00000010,
    DL_GPS      =   0x00000020,
    DL_GRAPHICS =   0x00000040,
    DL_ALL      =   0xffffffff
};

enum {
    DM_NONE     =   0x00000000,
    DM_SERIAL   =   0x00000001,
    DM_CONFIG   =   0x00000002,
    DM_MAIN     =   0x00000004,
    DM_FILTER   =   0x00000008,
    DM_FILE     =   0x00000010,
    DM_GPS      =   0x00000020,
    DM_DUCKTOR  =   0x00000040,
    DM_ALL      =   0xffffffff
};

#endif /* __DEBUGLEVELS_H__ */

/* * * * * * * * * * * * * End - of - File * * * * * * * * * * * * * * */
