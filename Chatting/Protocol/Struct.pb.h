// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: Struct.proto

#ifndef GOOGLE_PROTOBUF_INCLUDED_Struct_2eproto
#define GOOGLE_PROTOBUF_INCLUDED_Struct_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3021000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3021012 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/metadata_lite.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
#include "Enum.pb.h"
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_Struct_2eproto
PROTOBUF_NAMESPACE_OPEN
namespace internal {
class AnyMetadata;
}  // namespace internal
PROTOBUF_NAMESPACE_CLOSE

// Internal implementation detail -- do not use these members.
struct TableStruct_Struct_2eproto {
  static const uint32_t offsets[];
};
extern const ::PROTOBUF_NAMESPACE_ID::internal::DescriptorTable descriptor_table_Struct_2eproto;
namespace Protocol {
class Player;
struct PlayerDefaultTypeInternal;
extern PlayerDefaultTypeInternal _Player_default_instance_;
}  // namespace Protocol
PROTOBUF_NAMESPACE_OPEN
template<> ::Protocol::Player* Arena::CreateMaybeMessage<::Protocol::Player>(Arena*);
PROTOBUF_NAMESPACE_CLOSE
namespace Protocol {

// ===================================================================

class Player final :
    public ::PROTOBUF_NAMESPACE_ID::Message /* @@protoc_insertion_point(class_definition:Protocol.Player) */ {
 public:
  inline Player() : Player(nullptr) {}
  ~Player() override;
  explicit PROTOBUF_CONSTEXPR Player(::PROTOBUF_NAMESPACE_ID::internal::ConstantInitialized);

  Player(const Player& from);
  Player(Player&& from) noexcept
    : Player() {
    *this = ::std::move(from);
  }

  inline Player& operator=(const Player& from) {
    CopyFrom(from);
    return *this;
  }
  inline Player& operator=(Player&& from) noexcept {
    if (this == &from) return *this;
    if (GetOwningArena() == from.GetOwningArena()
  #ifdef PROTOBUF_FORCE_COPY_IN_MOVE
        && GetOwningArena() != nullptr
  #endif  // !PROTOBUF_FORCE_COPY_IN_MOVE
    ) {
      InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }

  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* descriptor() {
    return GetDescriptor();
  }
  static const ::PROTOBUF_NAMESPACE_ID::Descriptor* GetDescriptor() {
    return default_instance().GetMetadata().descriptor;
  }
  static const ::PROTOBUF_NAMESPACE_ID::Reflection* GetReflection() {
    return default_instance().GetMetadata().reflection;
  }
  static const Player& default_instance() {
    return *internal_default_instance();
  }
  static inline const Player* internal_default_instance() {
    return reinterpret_cast<const Player*>(
               &_Player_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  friend void swap(Player& a, Player& b) {
    a.Swap(&b);
  }
  inline void Swap(Player* other) {
    if (other == this) return;
  #ifdef PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() != nullptr &&
        GetOwningArena() == other->GetOwningArena()) {
   #else  // PROTOBUF_FORCE_COPY_IN_SWAP
    if (GetOwningArena() == other->GetOwningArena()) {
  #endif  // !PROTOBUF_FORCE_COPY_IN_SWAP
      InternalSwap(other);
    } else {
      ::PROTOBUF_NAMESPACE_ID::internal::GenericSwap(this, other);
    }
  }
  void UnsafeArenaSwap(Player* other) {
    if (other == this) return;
    GOOGLE_DCHECK(GetOwningArena() == other->GetOwningArena());
    InternalSwap(other);
  }

  // implements Message ----------------------------------------------

  Player* New(::PROTOBUF_NAMESPACE_ID::Arena* arena = nullptr) const final {
    return CreateMaybeMessage<Player>(arena);
  }
  using ::PROTOBUF_NAMESPACE_ID::Message::CopyFrom;
  void CopyFrom(const Player& from);
  using ::PROTOBUF_NAMESPACE_ID::Message::MergeFrom;
  void MergeFrom( const Player& from) {
    Player::MergeImpl(*this, from);
  }
  private:
  static void MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg);
  public:
  PROTOBUF_ATTRIBUTE_REINITIALIZES void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  const char* _InternalParse(const char* ptr, ::PROTOBUF_NAMESPACE_ID::internal::ParseContext* ctx) final;
  uint8_t* _InternalSerialize(
      uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const final;
  int GetCachedSize() const final { return _impl_._cached_size_.Get(); }

  private:
  void SharedCtor(::PROTOBUF_NAMESPACE_ID::Arena* arena, bool is_message_owned);
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(Player* other);

  private:
  friend class ::PROTOBUF_NAMESPACE_ID::internal::AnyMetadata;
  static ::PROTOBUF_NAMESPACE_ID::StringPiece FullMessageName() {
    return "Protocol.Player";
  }
  protected:
  explicit Player(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                       bool is_message_owned = false);
  public:

  static const ClassData _class_data_;
  const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*GetClassData() const final;

  ::PROTOBUF_NAMESPACE_ID::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  enum : int {
    kNameFieldNumber = 2,
    kIdFieldNumber = 1,
    kPlayerTypeFieldNumber = 3,
  };
  // string name = 2;
  void clear_name();
  const std::string& name() const;
  template <typename ArgT0 = const std::string&, typename... ArgT>
  void set_name(ArgT0&& arg0, ArgT... args);
  std::string* mutable_name();
  PROTOBUF_NODISCARD std::string* release_name();
  void set_allocated_name(std::string* name);
  private:
  const std::string& _internal_name() const;
  inline PROTOBUF_ALWAYS_INLINE void _internal_set_name(const std::string& value);
  std::string* _internal_mutable_name();
  public:

  // uint64 id = 1;
  void clear_id();
  uint64_t id() const;
  void set_id(uint64_t value);
  private:
  uint64_t _internal_id() const;
  void _internal_set_id(uint64_t value);
  public:

  // .Protocol.PlayerType playerType = 3;
  void clear_playertype();
  ::Protocol::PlayerType playertype() const;
  void set_playertype(::Protocol::PlayerType value);
  private:
  ::Protocol::PlayerType _internal_playertype() const;
  void _internal_set_playertype(::Protocol::PlayerType value);
  public:

  // @@protoc_insertion_point(class_scope:Protocol.Player)
 private:
  class _Internal;

  template <typename T> friend class ::PROTOBUF_NAMESPACE_ID::Arena::InternalHelper;
  typedef void InternalArenaConstructable_;
  typedef void DestructorSkippable_;
  struct Impl_ {
    ::PROTOBUF_NAMESPACE_ID::internal::ArenaStringPtr name_;
    uint64_t id_;
    int playertype_;
    mutable ::PROTOBUF_NAMESPACE_ID::internal::CachedSize _cached_size_;
  };
  union { Impl_ _impl_; };
  friend struct ::TableStruct_Struct_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// Player

// uint64 id = 1;
inline void Player::clear_id() {
  _impl_.id_ = uint64_t{0u};
}
inline uint64_t Player::_internal_id() const {
  return _impl_.id_;
}
inline uint64_t Player::id() const {
  // @@protoc_insertion_point(field_get:Protocol.Player.id)
  return _internal_id();
}
inline void Player::_internal_set_id(uint64_t value) {
  
  _impl_.id_ = value;
}
inline void Player::set_id(uint64_t value) {
  _internal_set_id(value);
  // @@protoc_insertion_point(field_set:Protocol.Player.id)
}

// string name = 2;
inline void Player::clear_name() {
  _impl_.name_.ClearToEmpty();
}
inline const std::string& Player::name() const {
  // @@protoc_insertion_point(field_get:Protocol.Player.name)
  return _internal_name();
}
template <typename ArgT0, typename... ArgT>
inline PROTOBUF_ALWAYS_INLINE
void Player::set_name(ArgT0&& arg0, ArgT... args) {
 
 _impl_.name_.Set(static_cast<ArgT0 &&>(arg0), args..., GetArenaForAllocation());
  // @@protoc_insertion_point(field_set:Protocol.Player.name)
}
inline std::string* Player::mutable_name() {
  std::string* _s = _internal_mutable_name();
  // @@protoc_insertion_point(field_mutable:Protocol.Player.name)
  return _s;
}
inline const std::string& Player::_internal_name() const {
  return _impl_.name_.Get();
}
inline void Player::_internal_set_name(const std::string& value) {
  
  _impl_.name_.Set(value, GetArenaForAllocation());
}
inline std::string* Player::_internal_mutable_name() {
  
  return _impl_.name_.Mutable(GetArenaForAllocation());
}
inline std::string* Player::release_name() {
  // @@protoc_insertion_point(field_release:Protocol.Player.name)
  return _impl_.name_.Release();
}
inline void Player::set_allocated_name(std::string* name) {
  if (name != nullptr) {
    
  } else {
    
  }
  _impl_.name_.SetAllocated(name, GetArenaForAllocation());
#ifdef PROTOBUF_FORCE_COPY_DEFAULT_STRING
  if (_impl_.name_.IsDefault()) {
    _impl_.name_.Set("", GetArenaForAllocation());
  }
#endif // PROTOBUF_FORCE_COPY_DEFAULT_STRING
  // @@protoc_insertion_point(field_set_allocated:Protocol.Player.name)
}

// .Protocol.PlayerType playerType = 3;
inline void Player::clear_playertype() {
  _impl_.playertype_ = 0;
}
inline ::Protocol::PlayerType Player::_internal_playertype() const {
  return static_cast< ::Protocol::PlayerType >(_impl_.playertype_);
}
inline ::Protocol::PlayerType Player::playertype() const {
  // @@protoc_insertion_point(field_get:Protocol.Player.playerType)
  return _internal_playertype();
}
inline void Player::_internal_set_playertype(::Protocol::PlayerType value) {
  
  _impl_.playertype_ = value;
}
inline void Player::set_playertype(::Protocol::PlayerType value) {
  _internal_set_playertype(value);
  // @@protoc_insertion_point(field_set:Protocol.Player.playerType)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)

}  // namespace Protocol

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // GOOGLE_PROTOBUF_INCLUDED_GOOGLE_PROTOBUF_INCLUDED_Struct_2eproto
