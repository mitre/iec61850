#include "iec61850_client/client.h"

bool control_object(client_t *self,
                    ControlObjectClient *controlClient,
                    MmsValue *mmsVal,
                    uint64_t opTime,
                    bool sbo,
                    bool sboWithValue)
{
  /* TimeActivated-Operate is not implemented in
   * libiec61850 at this time. In the `ControlObjectClient`
   * creation function, the parameter `hasTimeActivatedMode`
   * is hard-coded as false. This parameter is checked before
   * executing a time-activated operate and because it is
   * always false, the time delay argument is ignored.
   */
  opTime = 0;

  bool success = false;
  bool selected = !sbo && !sboWithValue;
  if (sboWithValue) {
    selected = ControlObjectClient_selectWithValue(*controlClient, mmsVal);
    if (!selected) {
      flogf(self->log, "control failed: Select With Value failed\n");
    }
  } else if (sbo) {
    selected = ControlObjectClient_select(*controlClient);
    if (!selected) {
      flogf(self->log, "control failed: Select failed\n");
    }
  }

  if (selected) {
    success = ControlObjectClient_operate(*controlClient, mmsVal, opTime);
  }

  return success;
}

result_t client__control_bool(client_t *self,
                              const char *doName,
                              bool ctlVal,
                              uint64_t opTime,
                              bool sbo,
                              bool sboWithValue)
{
  result_t res = result_create();
  MmsValue *mmsVal = MmsValue_newBoolean(ctlVal);
  ControlObjectClient controlClient = ControlObjectClient_create(doName, self->con);
  flogf(self->log, "controlling: %s -> %s\n", doName, ctlVal ? "true" : "false");

  if (mmsVal && controlClient) {
    if (control_object(self, &controlClient, mmsVal, opTime, sbo, sboWithValue)) {
      res.code = SUCCESS;
      result_writef(&res, "Control succeeded: %s -> %s\n", doName, ctlVal ? "true" : "false");
      flogf(self->log, "control succeeded\n");
    } else {
      res.code = IEC61850_ACTIONS_ERROR;
      flogf(self->log, "control failed\n");
    }
  } else {
    flogf(self->log, "control failed: '%s' may not be a valid data object\n", doName);
  }

  if (controlClient) {
    ControlObjectClient_destroy(controlClient);
  }
  if (mmsVal) {
    MmsValue_delete(mmsVal);
  }

  return res;
}

result_t client__control_int(client_t *self,
                             const char *doName,
                             int ctlVal,
                             uint64_t opTime,
                             bool sbo,
                             bool sboWithValue)
{
  result_t res = result_create();
  MmsValue *mmsVal = MmsValue_newInteger(ctlVal);
  ControlObjectClient controlClient = ControlObjectClient_create(doName, self->con);
  flogf(self->log, "controlling: %s -> %i\n", doName, ctlVal);

  if (mmsVal && controlClient) {
    if (control_object(self, &controlClient, mmsVal, opTime, sbo, sboWithValue)) {
      res.code = SUCCESS;
      result_writef(&res, "Control succeeded: %s -> %i\n", doName, ctlVal);
      flogf(self->log, "control succeeded\n");
    } else {
      res.code = IEC61850_ACTIONS_ERROR;
      flogf(self->log, "control failed\n");
    }
  } else {
    flogf(self->log, "control failed: '%s' may not be a valid data object\n", doName);
  }

  if (controlClient) {
    ControlObjectClient_destroy(controlClient);
  }
  if (mmsVal) {
    MmsValue_delete(mmsVal);
  }

  return res;
}

result_t client__control_uint(client_t *self,
                              const char *doName,
                              int ctlVal,
                              uint64_t opTime,
                              bool sbo,
                              bool sboWithValue)
{
  result_t res = result_create();
  MmsValue *mmsVal = MmsValue_newUnsigned(ctlVal);
  ControlObjectClient controlClient = ControlObjectClient_create(doName, self->con);
  flogf(self->log, "controlling: %s -> %i\n", doName, ctlVal);

  if (mmsVal && controlClient) {
    if (control_object(self, &controlClient, mmsVal, opTime, sbo, sboWithValue)) {
      res.code = SUCCESS;
      result_writef(&res, "Control succeeded: %s -> %i\n", doName, ctlVal);
      flogf(self->log, "control succeeded\n");
    } else {
      res.code = IEC61850_ACTIONS_ERROR;
      flogf(self->log, "control failed\n");
    }
  } else {
    flogf(self->log, "control failed: '%s' may not be a valid data object\n", doName);
  }

  if (controlClient) {
    ControlObjectClient_destroy(controlClient);
  }
  if (mmsVal) {
    MmsValue_delete(mmsVal);
  }

  return res;
}

result_t client__control_float(client_t *self,
                               const char *doName,
                               float ctlVal,
                               uint64_t opTime,
                               bool sbo,
                               bool sboWithValue)
{
  result_t res = result_create();
  MmsValue *mmsVal = MmsValue_newFloat(ctlVal);
  ControlObjectClient controlClient = ControlObjectClient_create(doName, self->con);
  flogf(self->log, "controlling: %s -> %f\n", doName, ctlVal);

  if (mmsVal && controlClient) {
    if (control_object(self, &controlClient, mmsVal, opTime, sbo, sboWithValue)) {
      res.code = SUCCESS;
      result_writef(&res, "Control succeeded: %s -> %f\n", doName, ctlVal);
      flogf(self->log, "control succeeded\n");
    } else {
      res.code = IEC61850_ACTIONS_ERROR;
      flogf(self->log, "control failed\n");
    }
  } else {
    flogf(self->log, "control failed: '%s' may not be a valid data object\n", doName);
  }

  if (controlClient) {
    ControlObjectClient_destroy(controlClient);
  }
  if (mmsVal) {
    MmsValue_delete(mmsVal);
  }

  return res;
}

result_t client__control_string(client_t *self,
                                const char *doName,
                                const char *ctlVal,
                                uint64_t opTime,
                                bool sbo,
                                bool sboWithValue)
{
  result_t res = result_create();
  MmsValue *mmsVal = MmsValue_newVisibleString(ctlVal);
  ControlObjectClient controlClient = ControlObjectClient_create(doName, self->con);
  flogf(self->log, "controlling: %s -> %s\n", doName, ctlVal);

  if (mmsVal && controlClient) {
    if (control_object(self, &controlClient, mmsVal, opTime, sbo, sboWithValue)) {
      res.code = SUCCESS;
      result_writef(&res, "Control succeeded: %s -> %s\n", doName, ctlVal);
      flogf(self->log, "control succeeded\n");
    } else {
      res.code = IEC61850_ACTIONS_ERROR;
      flogf(self->log, "control failed\n");
    }
  } else {
    flogf(self->log, "control failed: '%s' may not be a valid data object\n", doName);
  }

  if (controlClient) {
    ControlObjectClient_destroy(controlClient);
  }
  if (mmsVal) {
    MmsValue_delete(mmsVal);
  }

  return res;
}