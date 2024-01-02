#include "iec61850_client/client.h"

client_t client__create(FILE *output, FILE *log)
{
  client_t c;
  c.con = IedConnection_create();
  c.error = IED_ERROR_UNKNOWN;
  c.output = output;
  c.log = log;

  return c;
}

void client__connect(client_t *self, const char *hostname, int tcpPort)
{
  flogf(self->log, "attempting to connect to %s:%i\n", hostname, tcpPort);
  IedConnection_connect(self->con, &self->error, hostname, tcpPort);

  if (self->error != IED_ERROR_OK) {
    flogf(self->log, "connection failed: %s\n", error_to_string(self->error));
  } else {
    flogf(self->log, "connected to %s:%i\n", hostname, tcpPort);
  }
}

void client__disconnect(client_t *self)
{
  flogf(self->log, "disconnecting\n");
  IedConnection_destroy(self->con);
}
