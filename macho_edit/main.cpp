#include <iostream>

#include "menu.h"
#include "patch_object.h"

__attribute__((noreturn)) void usage(void) {
	std::cout << "Usage: macho_edit binary_path\n";
	std::cout << "Or: macho_edit --patch-object binary_path (macos|ios|tvos|watchos) system_version_X.Y[.Z] sdk_version_X.Y[.Z]\n";

	exit(1);
}

int main(int argc, const char *argv[]) {
	if(argc > 2 && std::string(argv[1]) == "--patch-object") {
		return patch_object_main(argc, argv);
	} else if(argc != 2) {
		usage();
	}

	const char *binary_path = argv[1];

	MachO macho = MachO(binary_path);

	macho.print_description();

	while(main_menu(macho)) {
	}

    return 0;
}
