#ifndef _NEUROSLAVE_FS_H_
#define _NEUROSLAVE_FS_H_

#include <string.h>

// Path
#define NEUROSLAVE_DATA_FOLDER "BCI"
#define NEUROSLAVE_USERS_FOLDER "Users"
#define NEUROSLAVE_PLAYLIST_FOLDER "Playlist"
const char NEUROSLAVE_DATA_PATH[] = "~/" NEUROSLAVE_DATA_FOLDER "/";
const char NEUROSLAVE_USERS_PATH[] = "~/" NEUROSLAVE_DATA_FOLDER "/" NEUROSLAVE_USERS_FOLDER "/";
const char NEUROSLAVE_PLAYLIST_PATH[] = "/" NEUROSLAVE_PLAYLIST_FOLDER "/";

char * neuroslave_playlist_path(const char * user)
{
	char * playlist_path = malloc(strlen(NEUROSLAVE_USERS_PATH) + strlen(user) + strlen(NEUROSLAVE_PLAYLIST_PATH));
	strcpy(playlist_path, NEUROSLAVE_USERS_PATH);
	strcat(playlist_path, user);
	strcat(playlist_path, NEUROSLAVE_PLAYLIST_PATH);
	return playlist_path;
}

#endif //_NEUROSLAVE_FS_H_
