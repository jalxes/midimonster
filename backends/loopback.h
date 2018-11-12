#include "midimonster.h"

int init();
static int backend_configure(char* option, char* value);
static int backend_configure_instance(instance* instance, char* option, char* value);
static uint32_t backend_interval();
static instance* backend_instance();
static channel* backend_channel(instance* instance, char* spec);
static int backend_set(instance* inst, size_t num, channel** c, channel_value* v);
static int backend_handle(size_t num, managed_fd* fds);
static int backend_start();
static int backend_shutdown();

typedef struct /*_loopback_instance_data*/ {
	size_t n;
    int value;
	char** name;
} loopback_instance;

typedef struct {
	uint64_t label;
} loopback_channel_ident;

