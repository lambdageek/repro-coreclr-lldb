#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

#include "hostfxr.h"

static const char *
get_own_path(void)
{
	Dl_info info;
	if (dladdr(&get_own_path, &info) == 0) {
		fprintf(stderr, "dladdr failed: %s\n", dlerror());
		abort();
	}
	return info.dli_fname;
}

static char*
get_runtime_fullpath(const char *self_fullpath, const char* component)
{
	size_t m = strlen(component);
	const char *sep = NULL;
	if ((sep = strrchr(self_fullpath, '/')) == NULL) {
		fprintf(stderr, "can't find trailing slash in '%s'\n", self_fullpath);
		abort();
	}
	size_t n = 1 + (size_t)(sep - self_fullpath); // keep the trailing '/'
	char *result = malloc(sizeof(char) * (n + m +1)); // add a nul
	strncpy(result, self_fullpath, n);
	strncpy(result + n, component, m);
	result[n+m] = 0;
	return result;
}


static void*
load_hostfxr(const char *hostfxr_path)
{
	void *lib = dlopen (hostfxr_path, RTLD_LOCAL);
	if (!lib) {
		fprintf(stderr, "can't open %s due to %s\n", hostfxr_path, dlerror());
		abort();
	}
	return lib;
}

static
void error_writer(const char* msg)
{
	fprintf(stderr, "host says: %s\n", msg);
}

static void
start_runtime(const char *selfpath, void* hostfxr_lib, const char* runtimeconfig)
{
	void *sym = dlsym(hostfxr_lib, "hostfxr_set_error_writer");
	if (!sym) {
		fprintf(stderr, "can't get hostfxr_set_error_writer due to %s\n", dlerror());
		abort();
	}
	hostfxr_set_error_writer_fn set_error_writer = (hostfxr_set_error_writer_fn)sym;
	set_error_writer(&error_writer);
	sym = dlsym(hostfxr_lib, "hostfxr_initialize_for_dotnet_command_line");
	if (!sym) {
		fprintf(stderr, "can't get hostfxr_initialize_for_dotnet_command_line due to %s\n", dlerror());
		abort();
	}
	hostfxr_initialize_for_dotnet_command_line_fn init_fn = (hostfxr_initialize_for_dotnet_command_line_fn)sym;
	hostfxr_handle handle = {0,};
	struct hostfxr_initialize_parameters params = {
		.size = sizeof(params),
		.host_path = NULL,
		.dotnet_root = get_runtime_fullpath(selfpath, ""),
	};
	printf ("running with dotnet root %s\n", params.dotnet_root);
	char *sayHelloPath = get_runtime_fullpath(selfpath, "SayHello.dll");
	int32_t result = 0;
	const char *argv[] = {
		sayHelloPath,
	};
	int argc = sizeof(argv)/sizeof(argv[0]);
	if ((result = init_fn(argc, argv, &params, &handle)) != 0) {
		fprintf (stderr, "hostfxr_initialize_for_dotnet_command_line returned 0x%08x\n", result);
		abort();
	}
	free((void*)params.dotnet_root);
	free(sayHelloPath);
	printf ("coreclr initialized\n");
	sym = dlsym(hostfxr_lib, "hostfxr_run_app");
	if (!sym) {
		fprintf(stderr, "can't get hostfxr_run_app due to %s\n", dlerror());
		abort();
	}
	hostfxr_run_app_fn run = (hostfxr_run_app_fn)sym;
	run(handle);
	printf ("hostfxr_run_app finished\n");
}

//  lldb::PluginInitialize(lldb::SBDebugger)
int _ZN4lldb16PluginInitializeENS_10SBDebuggerE(void)
{
	printf ("Hello from C\n");
	const char* selfpath = get_own_path();
	char *hostfxrpath = get_runtime_fullpath(selfpath, "libhostfxr.dylib");
	fprintf(stderr, "hostfxr path is %s\n", hostfxrpath);
	//char *hostfxr_path = build_path(basepath, "libhostfxr.dylib");
	void *hostfxr_lib = load_hostfxr(hostfxrpath);
	free(hostfxrpath);
	char *runtimeconfig = get_runtime_fullpath(selfpath, "SayHello.runtimeconfig.json");
	start_runtime(selfpath, hostfxr_lib, runtimeconfig);
	free(runtimeconfig);

	dlclose(hostfxr_lib);
	return 1;
}
