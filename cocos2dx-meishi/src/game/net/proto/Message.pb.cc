// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: Message.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "Message.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
// @@protoc_insertion_point(includes)

namespace fgame {

void protobuf_ShutdownFile_Message_2eproto() {
}

#ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
void protobuf_AddDesc_Message_2eproto_impl() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

#else
void protobuf_AddDesc_Message_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

#endif
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_Message_2eproto);
}

#ifdef GOOGLE_PROTOBUF_NO_STATIC_INITIALIZER
GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AddDesc_Message_2eproto_once_);
void protobuf_AddDesc_Message_2eproto() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AddDesc_Message_2eproto_once_,
                 &protobuf_AddDesc_Message_2eproto_impl);
}
#else
// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_Message_2eproto {
  StaticDescriptorInitializer_Message_2eproto() {
    protobuf_AddDesc_Message_2eproto();
  }
} static_descriptor_initializer_Message_2eproto_;
#endif
bool CS_CMD_ID_IsValid(int value) {
  switch(value) {
    case 100:
    case 200:
    case 500:
    case 501:
    case 502:
    case 503:
    case 504:
    case 505:
    case 506:
    case 507:
    case 508:
    case 509:
    case 510:
    case 511:
    case 512:
    case 513:
    case 514:
    case 515:
    case 517:
    case 518:
    case 519:
    case 520:
    case 550:
    case 551:
    case 603:
    case 604:
    case 704:
    case 801:
    case 1000:
    case 1001:
    case 1002:
    case 1003:
    case 2000:
    case 2001:
    case 2002:
    case 2003:
    case 2004:
    case 2005:
    case 2006:
    case 2007:
    case 2008:
    case 2009:
    case 2010:
    case 2011:
    case 2012:
    case 2013:
    case 2014:
    case 2100:
    case 2101:
    case 2102:
    case 2103:
    case 2200:
    case 2201:
    case 3000:
    case 3001:
    case 3002:
    case 3003:
    case 3004:
    case 3005:
    case 3006:
    case 3007:
    case 3010:
    case 3011:
    case 3012:
    case 3020:
    case 3021:
    case 3101:
    case 3102:
    case 4001:
    case 4002:
    case 4003:
    case 4004:
    case 4005:
    case 5001:
    case 5002:
    case 5003:
    case 6001:
    case 6002:
    case 6003:
    case 6004:
    case 7000:
    case 7001:
    case 7002:
    case 7003:
    case 7004:
    case 7005:
    case 7006:
    case 7100:
    case 8001:
    case 8002:
    case 8100:
    case 8101:
    case 8102:
    case 8103:
    case 8104:
    case 8105:
    case 8106:
    case 8107:
    case 8108:
    case 8110:
    case 8111:
    case 8112:
    case 8113:
    case 8114:
    case 8115:
    case 8116:
    case 8117:
    case 8118:
    case 8201:
    case 8202:
    case 8203:
    case 8210:
    case 8211:
    case 8212:
    case 8223:
    case 8224:
    case 8225:
    case 8231:
    case 8232:
    case 8233:
    case 8241:
    case 8242:
    case 8250:
    case 8251:
    case 8252:
    case 8253:
    case 8254:
    case 8255:
    case 8256:
    case 8301:
    case 8302:
    case 8303:
    case 8304:
    case 8305:
    case 8306:
    case 8307:
    case 8401:
    case 8402:
    case 8403:
    case 8404:
    case 8405:
    case 8406:
    case 8407:
    case 8408:
    case 8409:
    case 8410:
    case 8411:
    case 8501:
    case 8502:
    case 8503:
    case 8504:
    case 8505:
    case 8506:
    case 8507:
    case 8508:
    case 8509:
    case 8601:
    case 8602:
    case 8603:
    case 8604:
    case 8605:
    case 8606:
    case 8700:
    case 8701:
    case 8702:
    case 8703:
    case 8704:
    case 8705:
    case 8706:
      return true;
    default:
      return false;
  }
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace fgame

// @@protoc_insertion_point(global_scope)
