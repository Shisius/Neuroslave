
#include "neuroslave_fs.h"

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
