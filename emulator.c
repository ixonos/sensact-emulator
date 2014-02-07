#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "sensact/session.h"
#include "sensact/debug.h"
#include "sensact/plugin.h"
//#include "sensact/plugins/emulator_config.h"
// Above include file can be created for device specific configuration (for use with sa_register_devices() )

int emulator_connect(int device, void *config)
{
    //struct emulator_config_t *emulator_config = config;
    //struct emulator_t *emulator_data;

    //emulator_data = malloc(sizeof(struct emulator_t));
    //session[device].data = emulator_data;

    debug_printf("Connecting emulator device...\n");

    // Emualtor connect code here

    return SA_OK;
}

int emulator_disconnect(int device)
{
    //struct emulator_t *emulator_data = session[device].data;

    debug_printf("Disconnecting emulator device...\n");

    // Emualtor disconnect code here
    //free(emulator_data);

    // Mark device session as disconnected
    session[device].connected = false;

    return SA_OK;
}

int emulator_write(int device, char *data, int length, int timeout)
{
    //struct emulator_config_t *emulator_config = session[device].device->config;
    //struct emulator_t *emulator_data = session[device].data;

    return SA_OK;
}

int emulator_read(int device, char *data, int length, int timeout)
{
    //struct emulator_config_t *emulator_config = session[device].device->config;
    //struct emulator_t *emulator_data = session[device].data;

    return SA_OK;
}

int emulator_load(void)
{
    // Things to do on plugin load (if any)
    return SA_OK;
}

int emulator_unload(void)
{
    // Things to do on plugin unload (if any)
    return SA_OK;
}

struct sa_backend_t emulator_backend =
{
    .name = "emulator",
    .connect = emulator_connect,
    .disconnect = emulator_disconnect,
    .read = emulator_read,
    .write = emulator_write,
};

// Plugin configuration
struct sa_plugin_t emulator =
{
    .description = "Sensact emulator plugin",
    .version = "0.1",
    .author = "Michael Moeller",
    .license = "?",
    .load = emulator_load,
    .unload = emulator_unload,
    .backend = &emulator_backend,
};

sa_plugin_register(emulator);
