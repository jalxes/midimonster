#include <string.h>
#include "loopback.h"

#define BACKEND_NAME "loopback"
#define FLOW_UP "UP"
#define FLOW_DOWN "DOWN"

int init(){
	backend loopback = {
		.name = BACKEND_NAME,
		.conf = backend_configure,
		.create = backend_instance,
		.conf_instance = backend_configure_instance,
		.channel = backend_channel,
		.handle = backend_set,
		.process = backend_handle,
		.start = backend_start,
		.interval = backend_interval,
		.shutdown = backend_shutdown
	};

	//register backend
	if(mm_backend_register(loopback)){
		fprintf(stderr, "Failed to register loopback backend\n");
		return 1;
	}
	return 0;
}

static int backend_configure(char* option, char* value){
	//intentionally ignored
	return 0;
}

static uint32_t backend_interval(){
	return 2000;
}

static int backend_configure_instance(instance* inst, char* option, char* value){
	//intentionally ignored
	return 0;
}

static instance* backend_instance(){
	instance* i = mm_instance();
	if(!i){
		return NULL;
	}

	i->impl = calloc(1, sizeof(loopback_instance));
	if(!i->impl){
		fprintf(stderr, "Failed to allocate memory\n");
		return NULL;
	}

	return i;
}

static channel* backend_channel(instance* inst, char* spec){
	// loopback_instance* data = (loopback_instance*) inst->impl;
    loopback_channel_ident ident;
	if(!strncmp(spec, "up", 2)){
		ident.label = (uint64_t) FLOW_UP;
		return mm_channel(inst, (uint64_t) ident.label, 1);
	}
	
	if(!strncmp(spec, "down", 4)){
		ident.label = (uint64_t) FLOW_DOWN;
		return mm_channel(inst, ident.label, 1);
	}

	return NULL;
}

static int backend_set(instance* inst, size_t num, channel** c, channel_value* v){
	// size_t n;
	// for(n = 0; n < num; n++){
	// 	mm_channel_event(c[n], v[n]);
	// }
    size_t u;
    int spin = (int)v->normalised;

    if (spin > 0) {
        spin = spin > 1  ? 15 : 5;
    }


	loopback_instance* data;
    loopback_channel_ident ident;
	channel_value val;
	for(u = 0; u < num; u++){
		
        data = (loopback_instance*) c[u]->instance->impl;
		ident.label = c[u]->ident;

        if(ident.label == (uint64_t) FLOW_DOWN){
            data->value = data->value + spin;   
            if(data->value > 126){
                data->value = 126;   
            }
        }
        if(ident.label == (uint64_t) FLOW_UP){
            data->value = data->value - spin;   
            if(data->value < 1){
                data->value = 1;
            }
        }
        if(data->value > 1 & data->value < 126 ){
            fprintf(stderr, "label %s\n", ident.label);
            fprintf(stderr, "spin %d\n", spin);
            fprintf(stderr, "value %3d\n", data->value);
        }
        
        val.normalised = val.raw.u64 = (127 - data->value);

        mm_channel_event(c[u], val);
    }
	return 0;
}

static int backend_handle(size_t num, managed_fd* fds){
	//no events generated here
	return 0;
}

static int backend_start(){
	return 0;
}

static int backend_shutdown(){
	size_t n, u, p;
	instance** inst = NULL;
	loopback_instance* data = NULL;

	if(mm_backend_instances(BACKEND_NAME, &n, &inst)){
		fprintf(stderr, "Failed to fetch instance list\n");
		return 1;
	}

	for(u = 0; u < n; u++){
		data = (loopback_instance*) inst[u]->impl;
		for(p = 0; p < data->n; p++){
			free(data->name[p]);
		}
		free(data->name);
		free(inst[u]->impl);
	}

	free(inst);
	return 0;
}
