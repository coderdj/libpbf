# Generated by the protocol buffer compiler.  DO NOT EDIT!
# source: protocDef.proto

from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from google.protobuf import reflection as _reflection
from google.protobuf import descriptor_pb2
# @@protoc_insertion_point(imports)




DESCRIPTOR = _descriptor.FileDescriptor(
  name='protocDef.proto',
  package='pbf',
  serialized_pb='\n\x0fprotocDef.proto\x12\x03pbf\"\xd2\x01\n\x05\x45vent\x12\x0e\n\x06number\x18\x01 \x02(\x03\x12\x0c\n\x04time\x18\x02 \x02(\x04\x12#\n\x07\x63hannel\x18\x03 \x03(\x0b\x32\x12.pbf.Event.Channel\x1a\x85\x01\n\x07\x43hannel\x12\n\n\x02id\x18\x01 \x02(\x05\x12\x0e\n\x06module\x18\x03 \x01(\x05\x12%\n\x04\x64\x61ta\x18\x02 \x03(\x0b\x32\x17.pbf.Event.Channel.Data\x1a\x37\n\x04\x44\x61ta\x12\x0f\n\x07payload\x18\x01 \x02(\x0c\x12\x0c\n\x04time\x18\x02 \x01(\x03\x12\x10\n\x08integral\x18\x03 \x01(\x05\"\x9f\x01\n\x06Header\x12\x0e\n\x06zipped\x18\x01 \x02(\x08\x12\x11\n\tstartdate\x18\x02 \x02(\x03\x12\x14\n\x0c\x63reationdate\x18\x03 \x02(\x03\x12\x15\n\rrunidentifier\x18\x04 \x02(\t\x12\x12\n\nfilenumber\x18\x05 \x02(\x05\x12\x0f\n\x07runmode\x18\x06 \x01(\t\x12\x11\n\tstartedby\x18\x07 \x01(\t\x12\r\n\x05notes\x18\x08 \x01(\t')




_EVENT_CHANNEL_DATA = _descriptor.Descriptor(
  name='Data',
  full_name='pbf.Event.Channel.Data',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='payload', full_name='pbf.Event.Channel.Data.payload', index=0,
      number=1, type=12, cpp_type=9, label=2,
      has_default_value=False, default_value="",
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='time', full_name='pbf.Event.Channel.Data.time', index=1,
      number=2, type=3, cpp_type=2, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='integral', full_name='pbf.Event.Channel.Data.integral', index=2,
      number=3, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=180,
  serialized_end=235,
)

_EVENT_CHANNEL = _descriptor.Descriptor(
  name='Channel',
  full_name='pbf.Event.Channel',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='id', full_name='pbf.Event.Channel.id', index=0,
      number=1, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='module', full_name='pbf.Event.Channel.module', index=1,
      number=3, type=5, cpp_type=1, label=1,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='data', full_name='pbf.Event.Channel.data', index=2,
      number=2, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[_EVENT_CHANNEL_DATA, ],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=102,
  serialized_end=235,
)

_EVENT = _descriptor.Descriptor(
  name='Event',
  full_name='pbf.Event',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='number', full_name='pbf.Event.number', index=0,
      number=1, type=3, cpp_type=2, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='time', full_name='pbf.Event.time', index=1,
      number=2, type=4, cpp_type=4, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='channel', full_name='pbf.Event.channel', index=2,
      number=3, type=11, cpp_type=10, label=3,
      has_default_value=False, default_value=[],
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[_EVENT_CHANNEL, ],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=25,
  serialized_end=235,
)


_HEADER = _descriptor.Descriptor(
  name='Header',
  full_name='pbf.Header',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    _descriptor.FieldDescriptor(
      name='zipped', full_name='pbf.Header.zipped', index=0,
      number=1, type=8, cpp_type=7, label=2,
      has_default_value=False, default_value=False,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='startdate', full_name='pbf.Header.startdate', index=1,
      number=2, type=3, cpp_type=2, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='creationdate', full_name='pbf.Header.creationdate', index=2,
      number=3, type=3, cpp_type=2, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='runidentifier', full_name='pbf.Header.runidentifier', index=3,
      number=4, type=9, cpp_type=9, label=2,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='filenumber', full_name='pbf.Header.filenumber', index=4,
      number=5, type=5, cpp_type=1, label=2,
      has_default_value=False, default_value=0,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='runmode', full_name='pbf.Header.runmode', index=5,
      number=6, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='startedby', full_name='pbf.Header.startedby', index=6,
      number=7, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
    _descriptor.FieldDescriptor(
      name='notes', full_name='pbf.Header.notes', index=7,
      number=8, type=9, cpp_type=9, label=1,
      has_default_value=False, default_value=unicode("", "utf-8"),
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=238,
  serialized_end=397,
)

_EVENT_CHANNEL_DATA.containing_type = _EVENT_CHANNEL;
_EVENT_CHANNEL.fields_by_name['data'].message_type = _EVENT_CHANNEL_DATA
_EVENT_CHANNEL.containing_type = _EVENT;
_EVENT.fields_by_name['channel'].message_type = _EVENT_CHANNEL
DESCRIPTOR.message_types_by_name['Event'] = _EVENT
DESCRIPTOR.message_types_by_name['Header'] = _HEADER

class Event(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType

  class Channel(_message.Message):
    __metaclass__ = _reflection.GeneratedProtocolMessageType

    class Data(_message.Message):
      __metaclass__ = _reflection.GeneratedProtocolMessageType
      DESCRIPTOR = _EVENT_CHANNEL_DATA

      # @@protoc_insertion_point(class_scope:pbf.Event.Channel.Data)
    DESCRIPTOR = _EVENT_CHANNEL

    # @@protoc_insertion_point(class_scope:pbf.Event.Channel)
  DESCRIPTOR = _EVENT

  # @@protoc_insertion_point(class_scope:pbf.Event)

class Header(_message.Message):
  __metaclass__ = _reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _HEADER

  # @@protoc_insertion_point(class_scope:pbf.Header)


# @@protoc_insertion_point(module_scope)
