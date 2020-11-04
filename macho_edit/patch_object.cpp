#include <iostream>

#include "main.h"
#include "macho.h"
#include "macros.h"

int patch_object_main(int argc, const char *argv[]) {
	if (argc != 6) {
		usage();
	}
	const char *binary_path = argv[2];
	std::string system = argv[3];
	const char *version = argv[4];
	const char *sdk = argv[5];
	
	MachO macho = MachO(binary_path);
	
	uint32_t arch = 0;
	if(macho.n_archs > 1) {
		std::cout << "This binary has more than one arch :(\n";
		exit(1);
	}
	
	uint32_t cmd = LC_VERSION_MIN_MACOSX;
	if (system == "ios") {
		cmd = LC_VERSION_MIN_IPHONEOS;
	} else if (system == "tvos") {
		cmd = LC_VERSION_MIN_TVOS;
	} else if (system == "watchos") {
		cmd = LC_VERSION_MIN_WATCHOS;
	} else if (system != "macos") {
		usage();
	}

	const auto parseVersion = [](const char *value) {
		auto major = 0;
		auto minor = 0;
		auto patch = 0;
		if (sscanf(value, "%d.%d.%d", &major, &minor, &patch) != 3
			&& sscanf(value, "%d.%d", &major, &minor) != 2) {
			usage();
		} else if (major <= 0
			|| major > 255
			|| minor < 0
			|| minor >= 255
			|| patch < 0
			|| patch > 255) {
			usage();
		}
		return (uint32_t(major) << 16) | (uint32_t(minor) << 8) | (uint32_t(patch));
	};
	
	uint32_t magic = macho.archs[arch].mach_header.magic;
	uint32_t cmdsize = sizeof(version_min_command);
	auto lc = version_min_command();
	lc.cmd = SWAP32(cmd, magic);
	lc.cmdsize = SWAP32(cmdsize, magic);
	lc.version = parseVersion(version);
	lc.sdk = parseVersion(sdk);
	macho.insert_load_command(arch, (load_command*)&lc);
	
	return 0;
}
