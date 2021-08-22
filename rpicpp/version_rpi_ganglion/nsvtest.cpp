
#include "neuroslave_ctl.h"

int main()
{
	// std::string user = "Hep";

	// std::cout << neuroslave_playlist_path(user) << std::endl;

	// std::vector<std::string> users = neuroslave_get_users();

	// std::vector<std::string> playlist = neuroslave_get_playlist(users[0]);

	// for (auto & u : users) {
	// 	std::cout << u << std::endl;
	// }

	// for (auto & m : playlist) {
	// 	std::cout << m << std::endl;
	// }

	// NeuroslaveSession nsvs;
	// NeuroslaveSession nsvs1;
	// nsvs.tag = "frrr";
	// nsvs.user_name = "ant";
	// nsvs1.tag = "gggh";
	// nsvs1.user_name = "chan";
	// std::string js = nsvs.to_json();
	// std::cout << js << std::endl;
	// //js = "{\"tag\":\"123\"}";
	// nsvs1.from_json(js);
	// std::cout << nsvs1.to_json() << std::endl;

	NeuroslaveController nsvctl;
	nsvctl.launch();
	nsvctl.wait_die();
	nsvctl.terminate();

	return 0;
}
