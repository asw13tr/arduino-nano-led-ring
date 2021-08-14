// Copyright 2020 David Conran

#include "IRac.h"
#include "IRrecv.h"
#include "IRrecv_test.h"
#include "IRsend.h"
#include "IRsend_test.h"
#include "gtest/gtest.h"

// Tests for sendSymphony().

// Test sending typical data only.
TEST(TestSendSymphony, SendDataOnly) {
  IRsendTest irsend(kGpioUnused);
  irsend.begin();
  irsend.sendSymphony(0xD90);
  EXPECT_EQ(
      "f38000d50"
      "m1250s400m1250s400m400s1250m1250s400m1250s400m400s1250m400s1250m1250s400"
      "m400s1250m400s1250m400s1250"
      "m400s7850"
      "m1250s400m1250s400m400s1250m1250s400m1250s400m400s1250m400s1250m1250s400"
      "m400s1250m400s1250m400s1250"
      "m400s7850"
      "m1250s400m1250s400m400s1250m1250s400m1250s400m400s1250m400s1250m1250s400"
      "m400s1250m400s1250m400s1250"
      "m400s7850"
      "m1250s400m1250s400m400s1250m1250s400m1250s400m400s1250m400s1250m1250s400"
      "m400s1250m400s1250m400s1250"
      "m400s7850",
      irsend.outputStr());
}

// Tests for decodeSymphony().

// Decode normal Symphony messages.
TEST(TestDecodeSymphony, SyntheticSelfDecode) {
  IRsendTest irsend(kGpioUnused);
  IRrecv irrecv(kGpioUnused);
  irsend.begin();

  // Real-life Symphony code from an actual capture/decode.
  irsend.reset();
  irsend.sendSymphony(0x123);
  irsend.makeDecodeResult();
  EXPECT_TRUE(irrecv.decode(&irsend.capture));
  EXPECT_EQ(decode_type_t::SYMPHONY, irsend.capture.decode_type);
  EXPECT_EQ(kSymphonyBits, irsend.capture.bits);
  EXPECT_EQ(0x123, irsend.capture.value);
  EXPECT_EQ(0, irsend.capture.address);
  EXPECT_EQ(0, irsend.capture.command);
  EXPECT_FALSE(irsend.capture.repeat);
}

// Decode a real Symphony message.
TEST(TestDecodeSymphony, RealMessageDecode) {
  IRsendTest irsend(kGpioUnused);
  IRrecv irrecv(kGpioUnused);
  irsend.begin();

  // Real-life Symphony code from an actual capture/decode.
  // Ref: https://github.com/crankyoldgit/IRremoteESP8266/issues/1057#issue-577216614
  irsend.reset();
  const uint16_t button_1[95] = {
      1296, 412, 1294, 386, 420, 1224, 1322, 390, 1290, 390, 420, 1224, 452,
      1220, 1314, 394, 420, 1222, 482, 1190, 480, 1192, 452, 7960,
      1290, 420, 1290, 390, 418, 1226, 1318, 394, 1262, 416, 420, 1224, 454,
      1220, 1292, 416, 422, 1222, 450, 1222, 452, 1218, 454, 8208,
      1296, 414, 1292, 386, 418, 1226, 1292, 422, 1260, 420, 424, 1218, 454,
      1226, 1312, 390, 420, 1224, 454, 1220, 482, 1186, 454, 7960,
      1318, 392, 1264, 416, 392, 1252, 1318, 394, 1288, 394, 418, 1224, 452,
      1224, 1292, 422, 414, 1222, 458, 1214, 450, 1222, 454};
  irsend.sendRaw(button_1, 95, 38000);
  irsend.makeDecodeResult();
  EXPECT_TRUE(irrecv.decode(&irsend.capture));
  EXPECT_EQ(decode_type_t::SYMPHONY, irsend.capture.decode_type);
  EXPECT_EQ(kSymphonyBits, irsend.capture.bits);
  EXPECT_EQ(0xD90, irsend.capture.value);
  EXPECT_EQ(0x0, irsend.capture.address);
  EXPECT_EQ(0x0, irsend.capture.command);
  EXPECT_FALSE(irsend.capture.repeat);

  // Ref: https://github.com/crankyoldgit/IRremoteESP8266/issues/1057#issuecomment-596038442
  irsend.reset();
  const uint16_t power[23] = {
      1308, 368, 1310, 368, 448, 1222, 1310, 372, 1308, 400, 442, 1198, 472,
      1198, 1284, 396, 444, 1224, 470, 1200, 470, 1198, 472};
  irsend.sendRaw(power, 23, 38000);
  irsend.makeDecodeResult();
  EXPECT_TRUE(irrecv.decodeSymphony(&irsend.capture));
  EXPECT_EQ(decode_type_t::SYMPHONY, irsend.capture.decode_type);
  EXPECT_EQ(kSymphonyBits, irsend.capture.bits);
  EXPECT_EQ(0xD90, irsend.capture.value);
  EXPECT_EQ(0x0, irsend.capture.address);
  EXPECT_EQ(0x0, irsend.capture.command);
  EXPECT_FALSE(irsend.capture.repeat);

  irsend.reset();
  const uint16_t swing[23] = {
      1290, 418, 1286, 392, 422, 1248, 1284, 400, 1294, 386, 422, 1248, 422,
      1250, 444, 1228, 424, 1248, 446, 1226, 1258, 420, 446};
  irsend.sendRaw(swing, 23, 38000);
  irsend.makeDecodeResult();
  EXPECT_TRUE(irrecv.decode(&irsend.capture));
  EXPECT_EQ(decode_type_t::SYMPHONY, irsend.capture.decode_type);
  EXPECT_EQ(kSymphonyBits, irsend.capture.bits);
  EXPECT_EQ(0xD82, irsend.capture.value);
  EXPECT_EQ(0x0, irsend.capture.address);
  EXPECT_EQ(0x0, irsend.capture.command);
  EXPECT_FALSE(irsend.capture.repeat);
}

// Ref: https://github.com/crankyoldgit/IRremoteESP8266/issues/1057#issuecomment-596543641
TEST(TestDecodeSymphony, RealMessageSentViaLibrary) {
  IRsendTest irsend(kGpioUnused);
  IRrecv irrecv(kGpioUnused);
  irsend.begin();

  irsend.reset();
  // Generated by the library/ESP8266.
  const uint16_t rawdata_lib[47] = {
      1222, 430, 1250, 436, 410, 1242, 1274, 446, 1274, 412, 414, 1266, 410,
      1264, 1252, 436, 436, 1240, 410, 1270, 438, 1242, 410, 8012, 1254, 434,
      1258, 432, 438, 1240, 1276, 416, 1252, 434, 438, 1234, 412, 1270, 1250,
      442, 412, 1264, 438, 1238, 410, 1270, 438};
  irsend.sendRaw(rawdata_lib, 47, 38000);
  irsend.makeDecodeResult();
  EXPECT_TRUE(irrecv.decode(&irsend.capture));
  EXPECT_EQ(decode_type_t::SYMPHONY, irsend.capture.decode_type);
  EXPECT_EQ(kSymphonyBits, irsend.capture.bits);
  EXPECT_EQ(0xD90, irsend.capture.value);
  EXPECT_EQ(0x0, irsend.capture.address);
  EXPECT_EQ(0x0, irsend.capture.command);
  EXPECT_FALSE(irsend.capture.repeat);

  irsend.reset();
  // Generated by the real remote.
  const uint16_t rawdata_remote[47] = {
      1286, 396, 1286, 396, 446, 1226, 1288, 400, 1294, 388, 444, 1228, 446,
      1226, 1286, 396, 444, 1226, 448, 1226, 468, 1204, 448, 7968, 1286, 396,
      1286, 396, 470, 1202, 1286, 400, 1286, 396, 446, 1224, 448, 1226, 1288,
      396, 446, 1226, 472, 1200, 448, 1226, 472};
  irsend.sendRaw(rawdata_remote, 47, 38000);
  irsend.makeDecodeResult();
  EXPECT_TRUE(irrecv.decode(&irsend.capture));
  EXPECT_EQ(decode_type_t::SYMPHONY, irsend.capture.decode_type);
  EXPECT_EQ(kSymphonyBits, irsend.capture.bits);
  EXPECT_EQ(0xD90, irsend.capture.value);
  EXPECT_EQ(0x0, irsend.capture.address);
  EXPECT_EQ(0x0, irsend.capture.command);
  EXPECT_FALSE(irsend.capture.repeat);
}

// Decode a real SM5021 generated message.
// Note: This used to fail because it had a "long" mark before the gap in mesg.
TEST(TestDecodeSymphony, Issue1105) {
  IRsendTest irsend(kGpioUnused);
  IRrecv irrecv(kGpioUnused);
  irsend.begin();

  // Ref: https://github.com/crankyoldgit/IRremoteESP8266/issues/1105#issuecomment-625327833
  irsend.reset();
  uint16_t rawData[191] = {
      1324, 372, 1322, 398, 448, 1218, 476, 1216, 478, 1216, 478, 1218, 478,
      1218, 476, 1218, 476, 1220, 474, 1220, 474, 1220, 1346, 7128, 1320, 402,
      1290, 404, 440, 1252, 442, 1254, 440, 1254, 440, 1254, 440, 1254, 440,
      1254, 440, 1254, 442, 1254, 440, 1254, 1288, 7186, 1288, 408, 1286, 408,
      438, 1254, 440, 1254, 440, 1254, 440, 1254, 440, 1254, 440, 1256, 440,
      1254, 440, 1254, 440, 1254, 1288, 7186, 1288, 408, 1286, 408, 438, 1254,
      440, 1256, 440, 1254, 440, 1254, 440, 1254, 440, 1254, 438, 1256, 438,
      1256, 440, 1254, 1288, 7188, 1288, 408, 1286, 408, 438, 1256, 438, 1256,
      440, 1254, 438, 1256, 438, 1256, 438, 1256, 438, 1256, 438, 1256, 440,
      1254, 1288, 7188, 1286, 408, 1286, 408, 440, 1254, 438, 1256, 438, 1256,
      438, 1256, 440, 1254, 438, 1254, 440, 1256, 438, 1256, 440, 1256, 438,
      8038, 1284, 408, 1286, 408, 438, 1282, 412, 1282, 414, 1280, 414, 1280,
      414, 1282, 414, 1280, 412, 1276, 412, 1286, 414, 1282, 412, 8062, 1260,
      434, 1262, 432, 414, 1280, 414, 1282, 412, 1282, 412, 1282, 412, 1282,
      414, 1280, 414, 1282, 412, 1282, 412, 1282, 412};  // UNKNOWN 827AA7B
  irsend.sendRaw(rawData, 191, 38000);
  irsend.makeDecodeResult();
  EXPECT_TRUE(irrecv.decode(&irsend.capture));
  EXPECT_EQ(decode_type_t::SYMPHONY, irsend.capture.decode_type);
  EXPECT_EQ(kSymphonyBits, irsend.capture.bits);
  EXPECT_EQ(0xC01, irsend.capture.value);
  EXPECT_EQ(0x0, irsend.capture.address);
  EXPECT_EQ(0x0, irsend.capture.command);
  EXPECT_FALSE(irsend.capture.repeat);
}

TEST(TestUtils, Housekeeping) {
  ASSERT_EQ("SYMPHONY", typeToString(decode_type_t::SYMPHONY));
  ASSERT_EQ(decode_type_t::SYMPHONY, strToDecodeType("SYMPHONY"));
  ASSERT_FALSE(hasACState(decode_type_t::SYMPHONY));
  ASSERT_FALSE(IRac::isProtocolSupported(decode_type_t::SYMPHONY));
  ASSERT_EQ(kSymphonyBits, IRsendTest::defaultBits(decode_type_t::SYMPHONY));
  ASSERT_EQ(kSymphonyDefaultRepeat,
            IRsendTest::minRepeats(decode_type_t::SYMPHONY));
}
