extern "C"
{
#include <iec61850_client/utils.h>
}

#include <gtest/gtest.h>

TEST(ParseBoolTest, HandlesGoodInput)
{
  bool val;
  EXPECT_TRUE(parse_bool("true", &val));
  EXPECT_TRUE(val);
  EXPECT_TRUE(parse_bool("false", &val));
  EXPECT_FALSE(val);
  EXPECT_TRUE(parse_bool("on", &val));
  EXPECT_TRUE(val);
  EXPECT_TRUE(parse_bool("off", &val));
  EXPECT_FALSE(val);
  EXPECT_TRUE(parse_bool("1", &val));
  EXPECT_TRUE(val);
  EXPECT_TRUE(parse_bool("0", &val));
  EXPECT_FALSE(val);
}

TEST(ParseBoolTest, HandlesBadInput)
{
  bool val;
  EXPECT_FALSE(parse_bool("", &val));
  EXPECT_FALSE(parse_bool("trueAAA", &val));
}

TEST(ErrorToString, HanndlesGoodInput)
{
  EXPECT_STREQ(error_to_string(IED_ERROR_OK), "success");
  EXPECT_STREQ(error_to_string(IED_ERROR_NOT_CONNECTED),
               "the service request can not be executed because the client is not yet connected");
  EXPECT_STREQ(error_to_string(IED_ERROR_ALREADY_CONNECTED),
               "connect service cannot execute because the client is already connected");
  EXPECT_STREQ(error_to_string(IED_ERROR_CONNECTION_LOST),
               "connection lost during service request");
  EXPECT_STREQ(
    error_to_string(IED_ERROR_SERVICE_NOT_SUPPORTED),
    "the service or some given parameters are not supported by the client stack or by the server");
  EXPECT_STREQ(error_to_string(IED_ERROR_CONNECTION_REJECTED), "connection rejected by server");
  EXPECT_STREQ(error_to_string(IED_ERROR_OUTSTANDING_CALL_LIMIT_REACHED),
               "cannot send request because outstanding call limit is reached");
  EXPECT_STREQ(error_to_string(IED_ERROR_USER_PROVIDED_INVALID_ARGUMENT),
               "API function has been called with an invalid argument");
  EXPECT_STREQ(error_to_string(IED_ERROR_ENABLE_REPORT_FAILED_DATASET_MISMATCH),
               "enable report failed due to dataset mismatch");
  EXPECT_STREQ(error_to_string(IED_ERROR_OBJECT_REFERENCE_INVALID),
               "the provided object reference is invalid (there is a syntactical error)");
  EXPECT_STREQ(error_to_string(IED_ERROR_UNEXPECTED_VALUE_RECEIVED),
               "received object is of unexpected type");
  EXPECT_STREQ(error_to_string(IED_ERROR_TIMEOUT),
               "communication to the server failed with a timeout");
  EXPECT_STREQ(error_to_string(IED_ERROR_ACCESS_DENIED), "access denied");
  EXPECT_STREQ(error_to_string(IED_ERROR_OBJECT_DOES_NOT_EXIST), "object does not exist");
  EXPECT_STREQ(error_to_string(IED_ERROR_OBJECT_EXISTS), "object already exists");
  EXPECT_STREQ(error_to_string(IED_ERROR_OBJECT_ACCESS_UNSUPPORTED),
               "the server does not support the requested access method");
  EXPECT_STREQ(error_to_string(IED_ERROR_TYPE_INCONSISTENT),
               "the server expected an object of another type");
  EXPECT_STREQ(error_to_string(IED_ERROR_TEMPORARILY_UNAVAILABLE),
               "the object or service is temporarily unavailable");
  EXPECT_STREQ(error_to_string(IED_ERROR_OBJECT_UNDEFINED),
               "the specified object is not defined in the server");
  EXPECT_STREQ(error_to_string(IED_ERROR_INVALID_ADDRESS), "the specified address is invalid");
  EXPECT_STREQ(error_to_string(IED_ERROR_HARDWARE_FAULT), "service failed due to a hardware fault");
  EXPECT_STREQ(error_to_string(IED_ERROR_TYPE_UNSUPPORTED),
               "the requested data type is not supported by the server");
  EXPECT_STREQ(error_to_string(IED_ERROR_OBJECT_ATTRIBUTE_INCONSISTENT),
               "the provided attributes are inconsistent");
  EXPECT_STREQ(error_to_string(IED_ERROR_OBJECT_VALUE_INVALID),
               "the provided object value is invalid");
  EXPECT_STREQ(error_to_string(IED_ERROR_OBJECT_INVALIDATED), "the object is invalidated");
  EXPECT_STREQ(error_to_string(IED_ERROR_MALFORMED_MESSAGE),
               "received an invalid response message from the server");
  EXPECT_STREQ(error_to_string(IED_ERROR_SERVICE_NOT_IMPLEMENTED), "service not implemented");
}

TEST(FCFromString, HandlesGoodInput)
{
  EXPECT_EQ(fc_from_string("ALL"), IEC61850_FC_ALL);
  // not exhaustive because final conversion is the
  // responsibilty of libiec61850
  EXPECT_EQ(fc_from_string("DC"), IEC61850_FC_DC);
  EXPECT_EQ(fc_from_string("MX"), IEC61850_FC_MX);
}

TEST(FCFromString, HandlesBadInput)
{
  EXPECT_EQ(fc_from_string(NULL), IEC61850_FC_NONE);
  EXPECT_EQ(fc_from_string(""), IEC61850_FC_NONE);
  EXPECT_EQ(fc_from_string("X"), IEC61850_FC_NONE);
  EXPECT_EQ(fc_from_string("XX"), IEC61850_FC_NONE);
  EXPECT_EQ(fc_from_string("ALLL"), IEC61850_FC_NONE);
  EXPECT_EQ(fc_from_string("DCX"), IEC61850_FC_NONE);
  EXPECT_EQ(fc_from_string("DCXXXX"), IEC61850_FC_NONE);
}

TEST(ParseMmsRef, HandlesGoodInput)
{
  FunctionalConstraint fc;
  EXPECT_STREQ(parse_mms_ref("Foo/bar$MX$A$mag$f", &fc), "Foo/bar.A.mag.f");
  EXPECT_EQ(fc, IEC61850_FC_MX);
}

TEST(ParseMmsRef, HandlesBadInput)
{
  FunctionalConstraint fc;
  EXPECT_EQ(parse_mms_ref(NULL, NULL), nullptr);
  EXPECT_EQ(parse_mms_ref(NULL, &fc), nullptr);
  EXPECT_EQ(parse_mms_ref("", &fc), nullptr);
  EXPECT_EQ(parse_mms_ref("$MX$", &fc), nullptr);
  EXPECT_EQ(parse_mms_ref("$$", &fc), nullptr);
  EXPECT_EQ(parse_mms_ref("Foo/bar", &fc), nullptr);
  EXPECT_EQ(parse_mms_ref("Foo/bar$", &fc), nullptr);
  EXPECT_STREQ(parse_mms_ref("Foo/bar$$", &fc), "Foo/bar.");
  EXPECT_EQ(fc, IEC61850_FC_NONE);
}

TEST(ParseDaName, HandlesGoodInput)
{
  FunctionalConstraint fc;
  EXPECT_STREQ(parse_da_name("foo[ALL]", &fc), "foo");
  EXPECT_EQ(fc, IEC61850_FC_ALL);
  EXPECT_STREQ(parse_da_name("foo", &fc), "foo");
  // fc isn't modified if no new fc is provided
  EXPECT_EQ(fc, IEC61850_FC_ALL);
}

TEST(ParseDaName, HandlesBadInput)
{
  FunctionalConstraint fc;
  EXPECT_EQ(parse_da_name(NULL, NULL), nullptr);
  EXPECT_EQ(parse_da_name(NULL, &fc), nullptr);
  EXPECT_EQ(parse_da_name("", &fc), nullptr);
  EXPECT_EQ(parse_da_name("[", &fc), nullptr);
  EXPECT_EQ(parse_da_name("[MX]", &fc), nullptr);
  EXPECT_STREQ(parse_da_name("foo[", &fc), "foo");
  EXPECT_STREQ(parse_da_name("foo[]", &fc), "foo");
  EXPECT_STREQ(parse_da_name("foo[bar", &fc), "foo");
  EXPECT_STREQ(parse_da_name("foo[]bar", &fc), "foo");
}

TEST(CreateDa, HandlesGoodInput)
{
  da_access_t *da;

  da = create_data_attribute("Foo", "MX");
  EXPECT_STREQ(da->name, "Foo");
  EXPECT_EQ(da->fc, IEC61850_FC_MX);

  da = create_data_attribute("Foo[MX]", "");
  EXPECT_STREQ(da->name, "Foo");
  EXPECT_EQ(da->fc, IEC61850_FC_MX);

  da = create_data_attribute("Foo[MX]", NULL);
  EXPECT_STREQ(da->name, "Foo");
  EXPECT_EQ(da->fc, IEC61850_FC_MX);

  da = create_data_attribute("Foo[MX]", "DC");
  EXPECT_STREQ(da->name, "Foo");
  EXPECT_EQ(da->fc, IEC61850_FC_MX);

  da = create_data_attribute("Foo/bar$MX$A$mag$f", "");
  EXPECT_STREQ(da->name, "Foo/bar.A.mag.f");
  EXPECT_EQ(da->fc, IEC61850_FC_MX);

  da = create_data_attribute("Foo/bar$MX$A$mag$f", NULL);
  EXPECT_STREQ(da->name, "Foo/bar.A.mag.f");
  EXPECT_EQ(da->fc, IEC61850_FC_MX);

  da = create_data_attribute("Foo/bar$MX$A$mag$f", "DC");
  EXPECT_STREQ(da->name, "Foo/bar.A.mag.f");
  EXPECT_EQ(da->fc, IEC61850_FC_MX);

  destroy_data_attribute(da);
}

TEST(CreateDa, HandlesBadInput)
{
  da_access_t *da;
  da = create_data_attribute("Foo", "");
  EXPECT_EQ(da, nullptr);

  da = create_data_attribute("Foo/bar$MX", "");
  EXPECT_EQ(da, nullptr);

  da = create_data_attribute("", "MX");
  EXPECT_EQ(da, nullptr);

  da = create_data_attribute("Foo", NULL);
  EXPECT_EQ(da, nullptr);

    da = create_data_attribute("Foo[", "");
  EXPECT_EQ(da, nullptr);

  da = create_data_attribute(NULL, "MX");
  EXPECT_EQ(da, nullptr);

  da = create_data_attribute(NULL, NULL);
  EXPECT_EQ(da, nullptr);

  destroy_data_attribute(da);
}

TEST(ParseMmsType, HandlesGoodInput)
{
  EXPECT_EQ(MmsType_from_string("bool"), MMS_BOOLEAN);
  EXPECT_EQ(MmsType_from_string("boolean"), MMS_BOOLEAN);
  EXPECT_EQ(MmsType_from_string("float"), MMS_FLOAT);
  EXPECT_EQ(MmsType_from_string("str"), MMS_VISIBLE_STRING);
  EXPECT_EQ(MmsType_from_string("string"), MMS_VISIBLE_STRING);
  EXPECT_EQ(MmsType_from_string("bitstring"), MMS_BIT_STRING);
  EXPECT_EQ(MmsType_from_string("octet_string"), MMS_OCTET_STRING);
  EXPECT_EQ(MmsType_from_string("unicode_string"), MMS_STRING);
  EXPECT_EQ(MmsType_from_string("int"), MMS_INTEGER);
  EXPECT_EQ(MmsType_from_string("integer"), MMS_INTEGER);
  EXPECT_EQ(MmsType_from_string("uint"), MMS_UNSIGNED);
  EXPECT_EQ(MmsType_from_string("unsigned"), MMS_UNSIGNED);
  EXPECT_EQ(MmsType_from_string("struct"), MMS_STRUCTURE);
  EXPECT_EQ(MmsType_from_string("structure"), MMS_STRUCTURE);
  EXPECT_EQ(MmsType_from_string("array"), MMS_ARRAY);
  EXPECT_EQ(MmsType_from_string("time"), MMS_UTC_TIME);
  EXPECT_EQ(MmsType_from_string("generalized_time"), MMS_GENERALIZED_TIME);
  EXPECT_EQ(MmsType_from_string("binary_time"), MMS_BINARY_TIME);
  EXPECT_EQ(MmsType_from_string("bcd"), MMS_BCD);
  EXPECT_EQ(MmsType_from_string("obj_id"), MMS_OBJ_ID);
}

TEST(ParseMmsType, HandlesBadInput)
{
  EXPECT_EQ(MmsType_from_string(""), MMS_DATA_ACCESS_ERROR);
  EXPECT_EQ(MmsType_from_string("foo"), MMS_DATA_ACCESS_ERROR);
}

TEST(CreateQualifiedName, HandlesGoodInput)
{
  EXPECT_STREQ(create_qualified_name("foo", '.', "bar"), "foo.bar");
  EXPECT_STREQ(create_qualified_name("foo", '/', "bar"), "foo/bar");
}

TEST(CreateQualifiedName, HandlesBadInput)
{
  EXPECT_EQ(create_qualified_name(NULL, '.', NULL), nullptr);
  EXPECT_EQ(create_qualified_name(NULL, '.', "bar"), nullptr);
  EXPECT_EQ(create_qualified_name("foo", '.', NULL), nullptr);
  EXPECT_STREQ(create_qualified_name("foo", '.', ""), "foo.");
  EXPECT_STREQ(create_qualified_name("", '.', "bar"), ".bar");
  EXPECT_EQ(create_qualified_name("foo", '\0', "bar"), nullptr);
}

class ResultTest : public testing::Test
{
protected:
  result_t res;
  void SetUp() override { res = result_create(); }
  void TearDown() override { result_destroy(&res); }
};

TEST_F(ResultTest, IsInitializedCorrectly)
{
  EXPECT_STREQ(res.message, "");
  EXPECT_EQ(res.code, UNKNOWN_ERROR);
}

TEST_F(ResultTest, CanWriteMsg)
{
  result_writef(&res, "%s", "foo");
  EXPECT_STREQ(res.message, "foo");
  result_writef(&res, "%s", "bar");
  EXPECT_STREQ(res.message, "foobar");
  result_writef(&res, "%s", "");
  EXPECT_STREQ(res.message, "foobar");
}
