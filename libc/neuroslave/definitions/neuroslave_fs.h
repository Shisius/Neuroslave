#ifndef _NEUROSLAVE_FS_H_
#define _NEUROSLAVE_FS_H_

#include <string>
#include <vector>
#include <stdlib.h>
#include <dirent.h>

// Path
#define NEUROSLAVE_DATA_FOLDER "BCI"
#define NEUROSLAVE_USERS_FOLDER "Users"
#define NEUROSLAVE_PLAYLIST_FOLDER "Playlist"
#define NEUROSLAVE_EEG_FOLDER "EEG"
const std::string NEUROSLAVE_DATA_PATH = "/home/" + std::string(getenv("USER")) + "/" NEUROSLAVE_DATA_FOLDER "/";
const std::string NEUROSLAVE_USERS_PATH = NEUROSLAVE_DATA_PATH + NEUROSLAVE_USERS_FOLDER "/";
const std::string NEUROSLAVE_PLAYLIST_PATH = "/" NEUROSLAVE_PLAYLIST_FOLDER "/";
const std::string NEUROSLAVE_EEG_PATH = "/" NEUROSLAVE_EEG_FOLDER "/";

std::string neuroslave_playlist_path(const std::string & user);

std::string neuroslave_eeg_path(const std::string & user);

std::vector<std::string> neuroslave_get_users();

std::vector<std::string> neuroslave_get_playlist(const std::string & user);

#endif //_NEUROSLAVE_FS_H_
