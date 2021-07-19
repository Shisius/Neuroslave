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

std::string neuroslave_playlist_path(const std::string & user)
{
	return NEUROSLAVE_USERS_PATH + user + NEUROSLAVE_PLAYLIST_PATH;
}

std::string neuroslave_eeg_path(const std::string & user)
{
	return NEUROSLAVE_USERS_PATH + user + NEUROSLAVE_EEG_PATH;
}

std::vector<std::string> neuroslave_get_users()
{
	std::vector<std::string> users;

	struct dirent * user;
	DIR * users_dir = opendir(NEUROSLAVE_USERS_PATH.c_str());
	if (users_dir == NULL) {
		return users;
	}

	while ((user = readdir(users_dir)) != NULL) {
		if (isalnum(user->d_name[0]) != 0)
			users.push_back(std::string(user->d_name));
	}

	closedir(users_dir);
	return users;
}

std::vector<std::string> neuroslave_get_playlist(const std::string & user)
{
	std::vector<std::string> playlist;

	struct dirent * music;
	DIR * playlist_dir = opendir(neuroslave_playlist_path(user).c_str());
	if (playlist_dir == NULL) {
		return playlist;
	}

	while ((music = readdir(playlist_dir)) != NULL) {
		if (isalnum(music->d_name[0]) != 0)
			playlist.push_back(std::string(music->d_name));
	}

	closedir(playlist_dir);
	return playlist;
}

#endif //_NEUROSLAVE_FS_H_
