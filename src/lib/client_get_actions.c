#include "iec61850_client/client.h"

void query_log(client_t *c,
               const char *logRef,
               MmsValue *id,
               MmsValue *time,
               bool *moreFollows,
               result_t *res)
{
  uint64_t timestamp = MmsValue_getUtcTimeInMs(time);
  LinkedList logEntries =
    IedConnection_queryLogAfter(c->con, &c->error, logRef, id, timestamp, moreFollows);

  if (c->error == IED_ERROR_OK) {
    res->code = SUCCESS;
    result_writef(res, "Log Reference: %s\n", logRef);

    char buf[BUFSIZE + 1];
    LinkedList element = LinkedList_getNext(logEntries);
    while (element != NULL) {
      MmsJournalEntry entry = (MmsJournalEntry)LinkedList_getData(element);
      MmsValue_update(id, MmsJournalEntry_getEntryID(entry));
      MmsValue_printToBuffer(id, buf, BUFSIZE);
      result_writef(res, "Entry ID: %s,", buf);

      MmsValue_update(time, MmsJournalEntry_getOccurenceTime(entry));
      MmsValue_printToBuffer(time, buf, BUFSIZE);
      result_writef(res, " Time: %s\n", buf);

      LinkedList var = LinkedList_getNext(entry->journalVariables);
      while (var != NULL) {

        MmsJournalVariable varData = (MmsJournalVariable)LinkedList_getData(var);

        result_writef(res, "  variable: %s\n", MmsJournalVariable_getTag(varData));
        MmsValue_printToBuffer(MmsJournalVariable_getValue(varData), buf, BUFSIZE);
        result_writef(res, "  value: %s\n", buf);

        var = LinkedList_getNext(var);
      }
      element = LinkedList_getNext(element);
    }
  } else {
    res->code = LIBIEC61850_ERROR;
    flogf(c->log, "query to '%s' failed: %s\n", logRef, error_to_string(c->error));
  }

  if (logEntries) {
    LinkedList_destroyDeep(logEntries, (LinkedListValueDeleteFunction)MmsJournalEntry_destroy);
  }
}

void read_log(client_t *c, const char *lcbName, result_t *res)
{
  MmsValue *lcbValue = IedConnection_readObject(c->con, &c->error, lcbName, IEC61850_FC_LG);

  if ((c->error == IED_ERROR_OK) && (MmsValue_getType(lcbValue) != MMS_DATA_ACCESS_ERROR)) {
    char logRef[BUFSIZE + 1];
    MmsValue_printToBuffer(MmsValue_getElement(lcbValue, 1), logRef, BUFSIZE);
    MmsValue *oldEntryTm = MmsValue_getElement(lcbValue, 3);
    MmsValue *oldEntryId = MmsValue_getElement(lcbValue, 5);
    bool more = true;

    while (more) {
      query_log(c, logRef, oldEntryId, oldEntryTm, &more, res);
    }

  } else {
    res->code = LIBIEC61850_ERROR;
    flogf(c->log, "read failed: %s\n", error_to_string(c->error));
  }
}

void read_value(client_t *c, const char *daName, const char *fcString, result_t *res)
{
  da_access_t *da = NULL;
  MmsValue *mmsVal = NULL;

  da = create_data_attribute(daName, fcString);
  if (da != NULL) {
    mmsVal = IedConnection_readObject(c->con, &c->error, da->name, da->fc);
  } else {
    res->code = IEC61850_ACTIONS_ERROR;
    flogf(c->log, "unable to parse data attribute '%s' with fc '%s'\n", daName, fcString);
    return;
  }

  if (mmsVal && (c->error == IED_ERROR_OK)) {
    MmsDataAccessError err = MmsValue_getDataAccessError(mmsVal);
    if (MmsValue_getType(mmsVal) == MMS_DATA_ACCESS_ERROR) {
      flogf(c->log, "read '%s' value failed: access error\n", daName);
    } else {
      char val[BUFSIZE + 1];
      MmsValue_printToBuffer(mmsVal, val, BUFSIZE);
      char *type = MmsValue_getTypeString(mmsVal);
      char *fc = FunctionalConstraint_toString(da->fc);

      res->code = SUCCESS;
      result_writef(res, "%s: %s (type: %s, fc: %s)\n", da->name, val, type, fc);
    }
  } else {
    res->code = LIBIEC61850_ERROR;
    flogf(c->log, "read '%s' value failed: %s\n", da->name, error_to_string(c->error));
  }

  if (da) {
    destroy_data_attribute(da);
  }
  if (mmsVal) {
    MmsValue_delete(mmsVal);
  }
}

void traverse_files(client_t *c, const char *dirName, result_t *res)
{
  LinkedList rootDirectory = IedConnection_getFileDirectory(c->con, &c->error, NULL);
  if ((rootDirectory != NULL) && (c->error == IED_ERROR_OK)) {
    res->code = SUCCESS;
    LinkedList directoryEntry = LinkedList_getNext(rootDirectory);
    while (directoryEntry != NULL) {
      FileDirectoryEntry entry = (FileDirectoryEntry)directoryEntry->data;
      result_writef(res,
                    "%s %i\n",
                    FileDirectoryEntry_getFileName(entry),
                    FileDirectoryEntry_getFileSize(entry));

      directoryEntry = LinkedList_getNext(directoryEntry);
    }
  } else {
    flogf(c->log, "error getting files: %s\n", error_to_string(c->error));
  }

  if (rootDirectory) {
    LinkedList_destroyDeep(rootDirectory,
                           (LinkedListValueDeleteFunction)FileDirectoryEntry_destroy);
  }
}

void traverse_data_set_members(client_t *c, const char *dsName, bool readValues, result_t *res)
{
  bool isDeletable;
  LinkedList dataSetMembers =
    IedConnection_getDataSetDirectory(c->con, &c->error, dsName, &isDeletable);
  if ((dataSetMembers != NULL) && (c->error == IED_ERROR_OK)) {
    res->code = SUCCESS;
    result_writef(res, "%s %s\n", dsName, isDeletable ? "(DELETABLE)" : "(not deletable)");
    LinkedList member = LinkedList_getNext(dataSetMembers);
    while (member != NULL) {
      char *member_name = (char *)member->data;

      if (readValues) {
        read_value(c, member_name, NULL, res);
      } else {
        result_writef(res, "%s\n", member_name);
      }

      member = LinkedList_getNext(member);
    }
    res->code = SUCCESS;
  } else {
    res->code = LIBIEC61850_ERROR;
    flogf(c->log, "get failed: %s\n", error_to_string(c->error));
  }

  if (dataSetMembers) {
    LinkedList_destroy(dataSetMembers);
  }
}

void traverse_data_attributes(client_t *c,
                              const char *doName,
                              DataRequest dataRequest,
                              result_t *res)
{
  LinkedList dataAttributes = IedConnection_getDataDirectoryFC(c->con, &c->error, doName);
  if ((dataAttributes != NULL) && (c->error == IED_ERROR_OK)) {
    res->code = SUCCESS;
    LinkedList attribute = LinkedList_getNext(dataAttributes);
    while (attribute != NULL) {
      char *attributeName = (char *)attribute->data;
      char *qualifiedDaName = create_qualified_name(doName, '.', attributeName);
      if (!qualifiedDaName) {
        flogf(c->log, "error allocating qualified name for data attribute '%s'", attributeName);
        continue;
      }

      if (dataRequest == VALUES) {
        read_value(c, qualifiedDaName, NULL, res);
      } else {
        res->code = SUCCESS;
        result_writef(res, "%s\n", qualifiedDaName);
      }

      attribute = LinkedList_getNext(attribute);
      FunctionalConstraint fc;
      char *parsedName = parse_da_name(qualifiedDaName, &fc);
      traverse_data_attributes(c, parsedName, dataRequest, res);
    }
  } else {
    res->code = LIBIEC61850_ERROR;
    flogf(c->log, "get failed: %s\n", error_to_string(c->error));
  }

  if (dataAttributes) {
    LinkedList_destroy(dataAttributes);
  }
}

void traverse_data_items(client_t *c, const char *lnName, DataRequest dataRequest, result_t *res)
{
  ACSIClass acsiClass;
  switch (dataRequest) {
    case DATA_OBJECTS:
    case DATA_ATTRIBUTES:
    case VALUES:
      acsiClass = ACSI_CLASS_DATA_OBJECT;
      break;
    case DATA_SETS:
    case DATA_SET_MEMBERS:
    case DATA_SET_VALUES:
      acsiClass = ACSI_CLASS_DATA_SET;
      break;
    case BUFFERED_REPORTS:
      acsiClass = ACSI_CLASS_BRCB;
      break;
    case UNBUFFERED_REPORTS:
      acsiClass = ACSI_CLASS_URCB;
      break;
    default:
      /* other cases should not appear unless called in error */
      return;
  }

  LinkedList data = IedConnection_getLogicalNodeDirectory(c->con, &c->error, lnName, acsiClass);
  if ((data != NULL) && (c->error == IED_ERROR_OK)) {
    res->code = SUCCESS;
    LinkedList item = LinkedList_getNext(data);
    while (item != NULL) {
      char *itemName = (char *)item->data;
      char *qualifiedName = create_qualified_name(lnName, '.', itemName);
      if (!qualifiedName) {
        flogf(c->log, "error allocating qualified name for '%s'", itemName);
        continue;
      }

      switch (dataRequest) {
        case DATA_SETS: {
          bool isDeletable;
          LinkedList dataSet =
            IedConnection_getDataSetDirectory(c->con, &c->error, qualifiedName, &isDeletable);

          res->code = SUCCESS;
          result_writef(
            res, "%s %s\n", qualifiedName, isDeletable ? "(DELETABLE)" : "(not deletable)");
          LinkedList_destroy(dataSet);
          break;
        }
        case DATA_SET_MEMBERS:
          traverse_data_set_members(c, qualifiedName, false, res);
          break;
        case DATA_SET_VALUES:
          traverse_data_set_members(c, qualifiedName, true, res);
          break;
        case UNBUFFERED_REPORTS:
          res->code = SUCCESS;
          result_writef(res, "Unbuffered Report: %s\n", itemName);
          break;
        case BUFFERED_REPORTS:
          res->code = SUCCESS;
          result_writef(res, "Buffered Report: %s\n", itemName);
          break;
        case DATA_OBJECTS:
          res->code = SUCCESS;
          result_writef(res, "%s\n", qualifiedName);
          break;
        case DATA_ATTRIBUTES:
        case VALUES:
          traverse_data_attributes(c, qualifiedName, dataRequest, res);
          break;
        default:
          /* should be unreachable based on previous switch */
          break;
      }

      item = LinkedList_getNext(item);
    }
  } else {
    res->code = LIBIEC61850_ERROR;
    flogf(c->log, "get failed: %s\n", error_to_string(c->error));
  }

  if (data) {
    LinkedList_destroy(data);
  }
}

void traverse_logical_nodes(client_t *c, const char *ldName, DataRequest dataRequest, result_t *res)
{
  LinkedList logicalNodes = IedConnection_getLogicalDeviceDirectory(c->con, &c->error, ldName);
  if ((logicalNodes != NULL) && (c->error == IED_ERROR_OK)) {
    res->code = SUCCESS;
    LinkedList node = LinkedList_getNext(logicalNodes);
    while (node != NULL) {
      char *lnName = (char *)node->data;
      char *qualifiedLnName = create_qualified_name(ldName, '/', lnName);
      if (!qualifiedLnName) {
        flogf(c->log, "error allocating qualified name for logical node '%s'", lnName);
        continue;
      }

      switch (dataRequest) {
        case LOGICAL_NODES:
          res->code = SUCCESS;
          result_writef(res, "%s\n", qualifiedLnName);
          break;
        default:
          traverse_data_items(c, qualifiedLnName, dataRequest, res);
          break;
      }
      node = LinkedList_getNext(node);
    }
  } else {
    res->code = LIBIEC61850_ERROR;
    flogf(c->log, "get failed: %s\n", error_to_string(c->error));
  }

  if (logicalNodes) {
    LinkedList_destroy(logicalNodes);
  }
}

void traverse_log_blocks(client_t *c, const char *ldName, result_t *res)
{
  const char *zeroBlock = "LLN0";
  char *lnZero = create_qualified_name(ldName, '/', zeroBlock);
  if (!lnZero) {
    res->code = ALLOCATION_FAILURE;
    flogf(c->log, "read failed: malloc error");
    return;
  }

  LinkedList logs =
    IedConnection_getLogicalNodeDirectory(c->con, &c->error, lnZero, ACSI_CLASS_LOG);
  if ((logs != NULL) && (c->error == IED_ERROR_OK)) {
    res->code = SUCCESS;
    LinkedList log = LinkedList_getNext(logs);
    while (log != NULL) {
      char *lcbName = (char *)LinkedList_getData(log);
      char *qualifiedLcbName = create_qualified_name(lnZero, '.', lcbName);
      if (!qualifiedLcbName) {
        flogf(c->log, "error creating qualified name for LCB '%s'", lcbName);
        continue;
      }

      result_writef(res, "%s:\n", qualifiedLcbName);
      traverse_data_attributes(c, qualifiedLcbName, VALUES, res);

      log = LinkedList_getNext(log);
    }
  } else {
    res->code = LIBIEC61850_ERROR;
    flogf(c->log, "get failed: %s\n", error_to_string(c->error));
  }

  if (logs) {
    LinkedList_destroy(logs);
  }
}

void get_all(client_t *c, DataRequest dataRequest, result_t *res)
{
  bool getFileNames = dataRequest == FILES;

  LinkedList deviceList = IedConnection_getServerDirectory(c->con, &c->error, getFileNames);
  if ((deviceList != NULL) && (c->error == IED_ERROR_OK)) {
    res->code = SUCCESS;
    LinkedList device = LinkedList_getNext(deviceList);
    while (device != NULL) {
      char *ldName = (char *)device->data;

      switch (dataRequest) {
        case LOGICAL_DEVICES:
          res->code = SUCCESS;
          result_writef(res, "%s\n", ldName);
          break;
        case LOG_BLOCKS:
          traverse_log_blocks(c, ldName, res);
          break;
        case FILES: {
          FileDirectoryEntry entry = (FileDirectoryEntry)device->data;
          res->code = SUCCESS;
          result_writef(res,
                        "'%s' size: %i bytes\n",
                        FileDirectoryEntry_getFileName(entry),
                        FileDirectoryEntry_getFileSize(entry));
          break;
        }
        default:
          traverse_logical_nodes(c, ldName, dataRequest, res);
          break;
      }

      device = LinkedList_getNext(device);
    }
  } else {
    res->code = LIBIEC61850_ERROR;
    flogf(c->log, "get failed: %s\n", error_to_string(c->error));
  }

  if (deviceList && getFileNames) {
    LinkedList_destroyDeep(deviceList, (LinkedListValueDeleteFunction)FileDirectoryEntry_destroy);
  } else if (deviceList) {
    LinkedList_destroy(deviceList);
  }
}

result_t client__get_logical_devices(client_t *self)
{
  result_t res = result_create();
  flogf(self->log, "getting all logical devices\n");
  get_all(self, LOGICAL_DEVICES, &res);
  return res;
}

result_t client__get_logical_nodes(client_t *self, const char *ldName)
{
  result_t res = result_create();
  if (strcasecmp(ldName, "all") == 0) {
    flogf(self->log, "getting all logical nodes\n");
    get_all(self, LOGICAL_NODES, &res);
  } else {
    flogf(self->log, "getting logical nodes of '%s'\n", ldName);
    traverse_logical_nodes(self, ldName, LOGICAL_NODES, &res);
  }

  return res;
}

result_t client__get_data_objects(client_t *self, const char *lnName)
{
  result_t res = result_create();
  if (strcasecmp(lnName, "all") == 0) {
    flogf(self->log, "getting all data objects\n");
    get_all(self, DATA_OBJECTS, &res);
  } else {
    flogf(self->log, "getting data objects of '%s'\n", lnName);
    traverse_data_items(self, lnName, DATA_OBJECTS, &res);
  }

  return res;
}

result_t client__get_data_attributes(client_t *self, const char *doName)
{
  result_t res = result_create();
  if (strcasecmp(doName, "all") == 0) {
    flogf(self->log, "getting all data attributes\n");
    get_all(self, DATA_ATTRIBUTES, &res);
  } else {
    flogf(self->log, "getting data attributes of '%s'\n", doName);
    traverse_data_attributes(self, doName, DATA_ATTRIBUTES, &res);
  }

  return res;
}

result_t client__get_value(client_t *self, const char *daName, const char *fcString)
{
  result_t res = result_create();
  if (strcasecmp(daName, "all") == 0) {
    flogf(self->log, "reading all values\n");
    get_all(self, VALUES, &res);
  } else {
    flogf(self->log, "reading '%s'\n", daName);
    read_value(self, daName, fcString, &res);
  }

  return res;
}

result_t client__get_data_sets(client_t *self)
{
  result_t res = result_create();
  flogf(self->log, "getting data sets\n");
  get_all(self, DATA_SETS, &res);

  return res;
}

result_t client__get_data_set_members(client_t *self, const char *dsName, bool readVals)
{
  result_t res = result_create();
  if (strcasecmp(dsName, "all") == 0) {
    flogf(self->log, "getting all data set members%s\n", readVals ? " with values" : "");
    get_all(self, readVals ? DATA_SET_VALUES : DATA_SET_MEMBERS, &res);
  } else {
    flogf(
      self->log, "getting members of data set: '%s'%s\n", dsName, readVals ? " with values" : "");
    traverse_data_set_members(self, dsName, readVals, &res);
  }

  return res;
}

result_t client__get_reports(client_t *self)
{
  result_t res = result_create();
  flogf(self->log, "getting reports\n");
  get_all(self, BUFFERED_REPORTS, &res);
  get_all(self, UNBUFFERED_REPORTS, &res);

  return res;
}

result_t client__get_files(client_t *self)
{
  result_t res = result_create();
  flogf(self->log, "getting files\n");
  get_all(self, FILES, &res);

  return res;
}

result_t client__get_log_blocks(client_t *self)
{
  result_t res = result_create();
  flogf(self->log, "getting log control blocks\n");
  get_all(self, LOG_BLOCKS, &res);

  return res;
}

result_t client__get_log(client_t *self, const char *lcbName)
{
  result_t res = result_create();
  flogf(self->log, "reading log '%s'\n", lcbName);
  read_log(self, lcbName, &res);

  return res;
}
