# -*- coding: utf-8 -*-
# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: filec.proto

import sys
_b=sys.version_info[0]<3 and (lambda x:x) or (lambda x:x.encode('latin1'))
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import symbol_database as _symbol_database
# @@protoc_insertion_point(imports)

_sym_db = _symbol_database.Default()




DESCRIPTOR = _descriptor.FileDescriptor(
  name='filec.proto',
  package='communication',
  syntax='proto3',
  serialized_options=None,
  serialized_pb=_b('\n\x0b\x66ilec.proto\x12\rcommunication\";\n\nCreatedMsg\x12\x0c\n\x04path\x18\x01 \x01(\t\x12\x0e\n\x06is_dir\x18\x02 \x01(\x08\x12\x0f\n\x07\x63ontent\x18\x03 \x01(\x0c\",\n\x0bModifiedMsg\x12\x0c\n\x04path\x18\x01 \x01(\t\x12\x0f\n\x07\x63ontent\x18\x02 \x01(\x0c\")\n\tDeleteMsg\x12\x0e\n\x06is_dir\x18\x01 \x01(\x08\x12\x0c\n\x04path\x18\x02 \x01(\t\"4\n\x08MovedMsg\x12\x0e\n\x06is_dir\x18\x01 \x01(\x08\x12\x0b\n\x03src\x18\x02 \x01(\t\x12\x0b\n\x03\x64st\x18\x03 \x01(\tb\x06proto3')
)




_CREATEDMSG = _descriptor.Descriptor(
  name='CreatedMsg',
  full_name='communication.CreatedMsg',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='path', full_name='communication.CreatedMsg.path', index=0,
      number=1, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='is_dir', full_name='communication.CreatedMsg.is_dir', index=1,
      number=2, type=8, cpp_type=7, label=1,
      has_default_value=False, default_value=False,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='content', full_name='communication.CreatedMsg.content', index=2,
      number=3, type=12, cpp_type=9, label=1,
      has_default_value=False, default_value=_b(""),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=30,
  serialized_end=89,
)


_MODIFIEDMSG = _descriptor.Descriptor(
  name='ModifiedMsg',
  full_name='communication.ModifiedMsg',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='path', full_name='communication.ModifiedMsg.path', index=0,
      number=1, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='content', full_name='communication.ModifiedMsg.content', index=1,
      number=2, type=12, cpp_type=9, label=1,
      has_default_value=False, default_value=_b(""),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=91,
  serialized_end=135,
)


_DELETEMSG = _descriptor.Descriptor(
  name='DeleteMsg',
  full_name='communication.DeleteMsg',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='is_dir', full_name='communication.DeleteMsg.is_dir', index=0,
      number=1, type=8, cpp_type=7, label=1,
      has_default_value=False, default_value=False,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='path', full_name='communication.DeleteMsg.path', index=1,
      number=2, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=137,
  serialized_end=178,
)


_MOVEDMSG = _descriptor.Descriptor(
  name='MovedMsg',
  full_name='communication.MovedMsg',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='is_dir', full_name='communication.MovedMsg.is_dir', index=0,
      number=1, type=8, cpp_type=7, label=1,
      has_default_value=False, default_value=False,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='src', full_name='communication.MovedMsg.src', index=1,
      number=2, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
    _descriptor.FieldDescriptor(
      name='dst', full_name='communication.MovedMsg.dst', index=2,
      number=3, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=_b("").decode('utf-8'),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      serialized_options=None, file=DESCRIPTOR),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  serialized_options=None,
  is_extendable=False,
  syntax='proto3',
  extension_ranges=[],
  oneofs=[
  ],
  serialized_start=180,
  serialized_end=232,
)

DESCRIPTOR.message_types_by_name['CreatedMsg'] = _CREATEDMSG
DESCRIPTOR.message_types_by_name['ModifiedMsg'] = _MODIFIEDMSG
DESCRIPTOR.message_types_by_name['DeleteMsg'] = _DELETEMSG
DESCRIPTOR.message_types_by_name['MovedMsg'] = _MOVEDMSG
_sym_db.RegisterFileDescriptor(DESCRIPTOR)

CreatedMsg = _reflection.GeneratedProtocolMessageType('CreatedMsg', (_message.Message,), dict(
  DESCRIPTOR = _CREATEDMSG,
  __module__ = 'filec_pb2'
  # @@protoc_insertion_point(class_scope:communication.CreatedMsg)
  ))
_sym_db.RegisterMessage(CreatedMsg)

ModifiedMsg = _reflection.GeneratedProtocolMessageType('ModifiedMsg', (_message.Message,), dict(
  DESCRIPTOR = _MODIFIEDMSG,
  __module__ = 'filec_pb2'
  # @@protoc_insertion_point(class_scope:communication.ModifiedMsg)
  ))
_sym_db.RegisterMessage(ModifiedMsg)

DeleteMsg = _reflection.GeneratedProtocolMessageType('DeleteMsg', (_message.Message,), dict(
  DESCRIPTOR = _DELETEMSG,
  __module__ = 'filec_pb2'
  # @@protoc_insertion_point(class_scope:communication.DeleteMsg)
  ))
_sym_db.RegisterMessage(DeleteMsg)

MovedMsg = _reflection.GeneratedProtocolMessageType('MovedMsg', (_message.Message,), dict(
  DESCRIPTOR = _MOVEDMSG,
  __module__ = 'filec_pb2'
  # @@protoc_insertion_point(class_scope:communication.MovedMsg)
  ))
_sym_db.RegisterMessage(MovedMsg)


# @@protoc_insertion_point(module_scope)
