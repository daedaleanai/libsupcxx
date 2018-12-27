
add_library(
  supcxx STATIC

  # heap
  del_op.cc
  del_ops.cc
  del_opv.cc
  del_opvs.cc
  new_op.cc
  new_opnt.cc
  new_opv.cc
  new_opvnt.cc
  new_handler.cc

  # heap objects with pure virtuals, but not stack objects
  pure.cc

  # rtti
  dyncast.cc
  eh_aux_runtime.cc
  hash_bytes.cc
  tinfo.cc
  tinfo2.cc

  array_type_info.cc
  class_type_info.cc
  enum_type_info.cc
  function_type_info.cc
  fundamental_type_info.cc
  pbase_type_info.cc
  pmem_type_info.cc
  pointer_type_info.cc
  si_class_type_info.cc
  vmi_class_type_info.cc
  )
