// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: base.proto

#ifndef PROTOBUF_INCLUDED_base_2eproto
#define PROTOBUF_INCLUDED_base_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3007000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3007001 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/inlined_string_field.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/generated_enum_reflection.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_base_2eproto

// Internal implementation detail -- do not use these members.
struct TableStruct_base_2eproto {
  static const ::google::protobuf::internal::ParseTableField entries[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::google::protobuf::internal::AuxillaryParseTableField aux[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::google::protobuf::internal::ParseTable schema[2]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::google::protobuf::internal::FieldMetadata field_metadata[];
  static const ::google::protobuf::internal::SerializationTable serialization_table[];
  static const ::google::protobuf::uint32 offsets[];
};
void AddDescriptors_base_2eproto();
namespace communication {
class Request;
class RequestDefaultTypeInternal;
extern RequestDefaultTypeInternal _Request_default_instance_;
class Responese;
class ResponeseDefaultTypeInternal;
extern ResponeseDefaultTypeInternal _Responese_default_instance_;
}  // namespace communication
namespace google {
namespace protobuf {
template<> ::communication::Request* Arena::CreateMaybeMessage<::communication::Request>(Arena*);
template<> ::communication::Responese* Arena::CreateMaybeMessage<::communication::Responese>(Arena*);
}  // namespace protobuf
}  // namespace google
namespace communication {

enum Protocol {
  heart_beat = 0,
  token_login = 1,
  directory_verification = 2,
  file_diff = 10,
  Protocol_INT_MIN_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::google::protobuf::int32>::min(),
  Protocol_INT_MAX_SENTINEL_DO_NOT_USE_ = std::numeric_limits<::google::protobuf::int32>::max()
};
bool Protocol_IsValid(int value);
const Protocol Protocol_MIN = heart_beat;
const Protocol Protocol_MAX = file_diff;
const int Protocol_ARRAYSIZE = Protocol_MAX + 1;

const ::google::protobuf::EnumDescriptor* Protocol_descriptor();
inline const ::std::string& Protocol_Name(Protocol value) {
  return ::google::protobuf::internal::NameOfEnum(
    Protocol_descriptor(), value);
}
inline bool Protocol_Parse(
    const ::std::string& name, Protocol* value) {
  return ::google::protobuf::internal::ParseNamedEnum<Protocol>(
    Protocol_descriptor(), name, value);
}
// ===================================================================

class Request :
    public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:communication.Request) */ {
 public:
  Request();
  virtual ~Request();

  Request(const Request& from);

  inline Request& operator=(const Request& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  Request(Request&& from) noexcept
    : Request() {
    *this = ::std::move(from);
  }

  inline Request& operator=(Request&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  static const ::google::protobuf::Descriptor* descriptor() {
    return default_instance().GetDescriptor();
  }
  static const Request& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const Request* internal_default_instance() {
    return reinterpret_cast<const Request*>(
               &_Request_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  void Swap(Request* other);
  friend void swap(Request& a, Request& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline Request* New() const final {
    return CreateMaybeMessage<Request>(nullptr);
  }

  Request* New(::google::protobuf::Arena* arena) const final {
    return CreateMaybeMessage<Request>(arena);
  }
  void CopyFrom(const ::google::protobuf::Message& from) final;
  void MergeFrom(const ::google::protobuf::Message& from) final;
  void CopyFrom(const Request& from);
  void MergeFrom(const Request& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  #if GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
  static const char* _InternalParse(const char* begin, const char* end, void* object, ::google::protobuf::internal::ParseContext* ctx);
  ::google::protobuf::internal::ParseFunc _ParseFunc() const final { return _InternalParse; }
  #else
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) final;
  #endif  // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const final;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      ::google::protobuf::uint8* target) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(Request* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return nullptr;
  }
  inline void* MaybeArenaPtr() const {
    return nullptr;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // bytes body = 3;
  void clear_body();
  static const int kBodyFieldNumber = 3;
  const ::std::string& body() const;
  void set_body(const ::std::string& value);
  #if LANG_CXX11
  void set_body(::std::string&& value);
  #endif
  void set_body(const char* value);
  void set_body(const void* value, size_t size);
  ::std::string* mutable_body();
  ::std::string* release_body();
  void set_allocated_body(::std::string* body);

  // .communication.Protocol proto = 1;
  void clear_proto();
  static const int kProtoFieldNumber = 1;
  ::communication::Protocol proto() const;
  void set_proto(::communication::Protocol value);

  // int32 id = 2;
  void clear_id();
  static const int kIdFieldNumber = 2;
  ::google::protobuf::int32 id() const;
  void set_id(::google::protobuf::int32 value);

  // @@protoc_insertion_point(class_scope:communication.Request)
 private:
  class HasBitSetters;

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::internal::ArenaStringPtr body_;
  int proto_;
  ::google::protobuf::int32 id_;
  mutable ::google::protobuf::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_base_2eproto;
};
// -------------------------------------------------------------------

class Responese :
    public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:communication.Responese) */ {
 public:
  Responese();
  virtual ~Responese();

  Responese(const Responese& from);

  inline Responese& operator=(const Responese& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  Responese(Responese&& from) noexcept
    : Responese() {
    *this = ::std::move(from);
  }

  inline Responese& operator=(Responese&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  static const ::google::protobuf::Descriptor* descriptor() {
    return default_instance().GetDescriptor();
  }
  static const Responese& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const Responese* internal_default_instance() {
    return reinterpret_cast<const Responese*>(
               &_Responese_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    1;

  void Swap(Responese* other);
  friend void swap(Responese& a, Responese& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline Responese* New() const final {
    return CreateMaybeMessage<Responese>(nullptr);
  }

  Responese* New(::google::protobuf::Arena* arena) const final {
    return CreateMaybeMessage<Responese>(arena);
  }
  void CopyFrom(const ::google::protobuf::Message& from) final;
  void MergeFrom(const ::google::protobuf::Message& from) final;
  void CopyFrom(const Responese& from);
  void MergeFrom(const Responese& from);
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  #if GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
  static const char* _InternalParse(const char* begin, const char* end, void* object, ::google::protobuf::internal::ParseContext* ctx);
  ::google::protobuf::internal::ParseFunc _ParseFunc() const final { return _InternalParse; }
  #else
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) final;
  #endif  // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const final;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      ::google::protobuf::uint8* target) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(Responese* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return nullptr;
  }
  inline void* MaybeArenaPtr() const {
    return nullptr;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // string msg = 3;
  void clear_msg();
  static const int kMsgFieldNumber = 3;
  const ::std::string& msg() const;
  void set_msg(const ::std::string& value);
  #if LANG_CXX11
  void set_msg(::std::string&& value);
  #endif
  void set_msg(const char* value);
  void set_msg(const char* value, size_t size);
  ::std::string* mutable_msg();
  ::std::string* release_msg();
  void set_allocated_msg(::std::string* msg);

  // bytes body = 4;
  void clear_body();
  static const int kBodyFieldNumber = 4;
  const ::std::string& body() const;
  void set_body(const ::std::string& value);
  #if LANG_CXX11
  void set_body(::std::string&& value);
  #endif
  void set_body(const char* value);
  void set_body(const void* value, size_t size);
  ::std::string* mutable_body();
  ::std::string* release_body();
  void set_allocated_body(::std::string* body);

  // int32 code = 1;
  void clear_code();
  static const int kCodeFieldNumber = 1;
  ::google::protobuf::int32 code() const;
  void set_code(::google::protobuf::int32 value);

  // int32 id = 2;
  void clear_id();
  static const int kIdFieldNumber = 2;
  ::google::protobuf::int32 id() const;
  void set_id(::google::protobuf::int32 value);

  // @@protoc_insertion_point(class_scope:communication.Responese)
 private:
  class HasBitSetters;

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::internal::ArenaStringPtr msg_;
  ::google::protobuf::internal::ArenaStringPtr body_;
  ::google::protobuf::int32 code_;
  ::google::protobuf::int32 id_;
  mutable ::google::protobuf::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_base_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// Request

// .communication.Protocol proto = 1;
inline void Request::clear_proto() {
  proto_ = 0;
}
inline ::communication::Protocol Request::proto() const {
  // @@protoc_insertion_point(field_get:communication.Request.proto)
  return static_cast< ::communication::Protocol >(proto_);
}
inline void Request::set_proto(::communication::Protocol value) {
  
  proto_ = value;
  // @@protoc_insertion_point(field_set:communication.Request.proto)
}

// int32 id = 2;
inline void Request::clear_id() {
  id_ = 0;
}
inline ::google::protobuf::int32 Request::id() const {
  // @@protoc_insertion_point(field_get:communication.Request.id)
  return id_;
}
inline void Request::set_id(::google::protobuf::int32 value) {
  
  id_ = value;
  // @@protoc_insertion_point(field_set:communication.Request.id)
}

// bytes body = 3;
inline void Request::clear_body() {
  body_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& Request::body() const {
  // @@protoc_insertion_point(field_get:communication.Request.body)
  return body_.GetNoArena();
}
inline void Request::set_body(const ::std::string& value) {
  
  body_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:communication.Request.body)
}
#if LANG_CXX11
inline void Request::set_body(::std::string&& value) {
  
  body_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:communication.Request.body)
}
#endif
inline void Request::set_body(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  
  body_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:communication.Request.body)
}
inline void Request::set_body(const void* value, size_t size) {
  
  body_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:communication.Request.body)
}
inline ::std::string* Request::mutable_body() {
  
  // @@protoc_insertion_point(field_mutable:communication.Request.body)
  return body_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* Request::release_body() {
  // @@protoc_insertion_point(field_release:communication.Request.body)
  
  return body_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void Request::set_allocated_body(::std::string* body) {
  if (body != nullptr) {
    
  } else {
    
  }
  body_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), body);
  // @@protoc_insertion_point(field_set_allocated:communication.Request.body)
}

// -------------------------------------------------------------------

// Responese

// int32 code = 1;
inline void Responese::clear_code() {
  code_ = 0;
}
inline ::google::protobuf::int32 Responese::code() const {
  // @@protoc_insertion_point(field_get:communication.Responese.code)
  return code_;
}
inline void Responese::set_code(::google::protobuf::int32 value) {
  
  code_ = value;
  // @@protoc_insertion_point(field_set:communication.Responese.code)
}

// int32 id = 2;
inline void Responese::clear_id() {
  id_ = 0;
}
inline ::google::protobuf::int32 Responese::id() const {
  // @@protoc_insertion_point(field_get:communication.Responese.id)
  return id_;
}
inline void Responese::set_id(::google::protobuf::int32 value) {
  
  id_ = value;
  // @@protoc_insertion_point(field_set:communication.Responese.id)
}

// string msg = 3;
inline void Responese::clear_msg() {
  msg_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& Responese::msg() const {
  // @@protoc_insertion_point(field_get:communication.Responese.msg)
  return msg_.GetNoArena();
}
inline void Responese::set_msg(const ::std::string& value) {
  
  msg_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:communication.Responese.msg)
}
#if LANG_CXX11
inline void Responese::set_msg(::std::string&& value) {
  
  msg_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:communication.Responese.msg)
}
#endif
inline void Responese::set_msg(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  
  msg_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:communication.Responese.msg)
}
inline void Responese::set_msg(const char* value, size_t size) {
  
  msg_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:communication.Responese.msg)
}
inline ::std::string* Responese::mutable_msg() {
  
  // @@protoc_insertion_point(field_mutable:communication.Responese.msg)
  return msg_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* Responese::release_msg() {
  // @@protoc_insertion_point(field_release:communication.Responese.msg)
  
  return msg_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void Responese::set_allocated_msg(::std::string* msg) {
  if (msg != nullptr) {
    
  } else {
    
  }
  msg_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), msg);
  // @@protoc_insertion_point(field_set_allocated:communication.Responese.msg)
}

// bytes body = 4;
inline void Responese::clear_body() {
  body_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& Responese::body() const {
  // @@protoc_insertion_point(field_get:communication.Responese.body)
  return body_.GetNoArena();
}
inline void Responese::set_body(const ::std::string& value) {
  
  body_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:communication.Responese.body)
}
#if LANG_CXX11
inline void Responese::set_body(::std::string&& value) {
  
  body_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:communication.Responese.body)
}
#endif
inline void Responese::set_body(const char* value) {
  GOOGLE_DCHECK(value != nullptr);
  
  body_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:communication.Responese.body)
}
inline void Responese::set_body(const void* value, size_t size) {
  
  body_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:communication.Responese.body)
}
inline ::std::string* Responese::mutable_body() {
  
  // @@protoc_insertion_point(field_mutable:communication.Responese.body)
  return body_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* Responese::release_body() {
  // @@protoc_insertion_point(field_release:communication.Responese.body)
  
  return body_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void Responese::set_allocated_body(::std::string* body) {
  if (body != nullptr) {
    
  } else {
    
  }
  body_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), body);
  // @@protoc_insertion_point(field_set_allocated:communication.Responese.body)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)

}  // namespace communication

namespace google {
namespace protobuf {

template <> struct is_proto_enum< ::communication::Protocol> : ::std::true_type {};
template <>
inline const EnumDescriptor* GetEnumDescriptor< ::communication::Protocol>() {
  return ::communication::Protocol_descriptor();
}

}  // namespace protobuf
}  // namespace google

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // PROTOBUF_INCLUDED_base_2eproto
