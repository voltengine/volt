#include <volt/pch.hpp>
#include <volt/paths.hpp>

#include <volt/config.hpp>

namespace fs = std::filesystem;

// C:/Program Files/{path}/ -> /opt/{path}/
// %USERPROFILE%/Documents/{path}/ -> ($XDG_DOCUMENTS_DIR | $HOME/Documents)/{path}/ -> $HOME/Documents/{path}/
// %USERPROFILE%/Pictures/{path}/ -> ($XDG_PICTURES_DIR | $HOME/Pictures)/{path}/ -> $HOME/Pictures/{path}/
// %USERPROFILE%/Videos/{path}/ -> ($XDG_VIDEOS_DIR | $HOME/Videos)/{path}/ -> $HOME/Movies/{path}/
// %USERPROFILE%/Saved Games/{path}/ -> ($XDG_DATA_HOME | $HOME/.local/shared)/{path}/saved/ -> $HOME/Library/Application Support/{path}/saves/
// %LOCALAPPDATA%/{path}/ -> ($XDG_DATA_HOME | $HOME/.local/shared)/{path}/data/ -> $HOME/Library/Application Support/{path}/data/

namespace volt::paths {

static fs::path get_app_prefix() {
	return fs::path(config::json()["volt"]["publisher"].get_ref<std::string &>()) /
			config::json()["volt"]["title"].get_ref<std::string &>();
}

fs::path data() {
#ifdef VOLT_DEVELOPMENT
	return fs::path(VOLT_DEVELOPMENT_PATH) / "cache" / "user-paths" / "data";
#elif defined(VOLT_PLATFORM_LINUX)
	char *xdg = std::getenv("XDG_DATA_HOME");
	if (!xdg) {
		return fs::path(std::getenv("HOME")) / ".local" /
			"shared" / get_app_prefix() / "data";
	} else
		return xdg / get_app_prefix() / "data";
#elif defined(VOLT_PLATFORM_MACOS)
	return fs::path(std::getenv("HOME")) / "Library" /
			"Application Support" / get_app_prefix() / "data";
#elif defined(VOLT_PLATFORM_WINDOWS)
	return std::getenv("LOCALAPPDATA") / get_app_prefix();
#endif
}

fs::path documents() {
#ifdef VOLT_DEVELOPMENT
	return fs::path(VOLT_DEVELOPMENT_PATH) / "cache" / "user-paths" / "documents";
#elif defined(VOLT_PLATFORM_LINUX)
	char *xdg = std::getenv("XDG_DOCUMENTS_DIR");
	if (!xdg) {
		return fs::path(std::getenv("HOME")) / "Documents" / get_app_prefix();
	} else
		return xdg / get_app_prefix();
#elif defined(VOLT_PLATFORM_MACOS)
	return fs::path(std::getenv("HOME")) / "Documents" / get_app_prefix();
#elif defined(VOLT_PLATFORM_WINDOWS)
	return fs::path(std::getenv("USERPROFILE")) / "Documents" / get_app_prefix();
#endif
}

fs::path pictures() {
#ifdef VOLT_DEVELOPMENT
	return fs::path(VOLT_DEVELOPMENT_PATH) / "cache" / "user-paths" / "pictures";
#elif defined(VOLT_PLATFORM_LINUX)
	char *xdg = std::getenv("XDG_PICTURES_DIR");
	if (!xdg) {
		return fs::path(std::getenv("HOME")) / "Pictures" / get_app_prefix();
	} else
		return xdg / get_app_prefix();
#elif defined(VOLT_PLATFORM_MACOS)
	return fs::path(std::getenv("HOME")) / "Pictures" / get_app_prefix();
#elif defined(VOLT_PLATFORM_WINDOWS)
	return fs::path(std::getenv("USERPROFILE")) / "Pictures" / get_app_prefix();
#endif
}

fs::path videos() {
#ifdef VOLT_DEVELOPMENT
	return fs::path(VOLT_DEVELOPMENT_PATH) / "cache" / "user-paths" / "videos";
#elif defined(VOLT_PLATFORM_LINUX)
	char *xdg = std::getenv("XDG_VIDEOS_DIR");
	if (!xdg) {
		return fs::path(std::getenv("HOME")) / "Videos" / get_app_prefix();
	} else
		return xdg / get_app_prefix();
#elif defined(VOLT_PLATFORM_MACOS)
	return fs::path(std::getenv("HOME")) / "Movies" / get_app_prefix();
#elif defined(VOLT_PLATFORM_WINDOWS)
	return fs::path(std::getenv("USERPROFILE")) / "Videos" / get_app_prefix();
#endif
}

fs::path music() {
#ifdef VOLT_DEVELOPMENT
	return fs::path(VOLT_DEVELOPMENT_PATH) / "cache" / "user-paths" / "music";
#elif defined(VOLT_PLATFORM_LINUX)
	char *xdg = std::getenv("XDG_MUSIC_DIR");
	if (!xdg) {
		return fs::path(std::getenv("HOME")) / "Music" / get_app_prefix();
	} else
		return xdg / get_app_prefix();
#elif defined(VOLT_PLATFORM_MACOS)
	return fs::path(std::getenv("HOME")) / "Music" / get_app_prefix();
#elif defined(VOLT_PLATFORM_WINDOWS)
	return fs::path(std::getenv("USERPROFILE")) / "Music" / get_app_prefix();
#endif
}

fs::path saves() {
#ifdef VOLT_DEVELOPMENT
	return fs::path(VOLT_DEVELOPMENT_PATH) / "cache" / "user-paths" / "saves";
#elif defined(VOLT_PLATFORM_LINUX)
	char *xdg = std::getenv("XDG_DATA_HOME");
	if (!xdg) {
		return fs::path(std::getenv("HOME")) / ".local" /
			"shared" / get_app_prefix() / "saves";
	} else
		return xdg / get_app_prefix() / "saves";
#elif defined(VOLT_PLATFORM_MACOS)
	return fs::path(std::getenv("HOME")) / "Library" /
			"Application Support" / get_app_prefix() / "saves";
#elif defined(VOLT_PLATFORM_WINDOWS)
	return fs::path(std::getenv("USERPROFILE")) / "Saved Games" / get_app_prefix();
#endif
}

}