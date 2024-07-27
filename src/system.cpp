#include "include/system.hpp"

#include <sysfs/libsysfs.h>

static std::string get_motherboard_serial()
{
  std::string serial;

  sysfs_class_device* dev = sysfs_open_class_device("dmi", "id");
  if (dev != nullptr) {
    sysfs_attribute* attr = sysfs_get_classdev_attr(dev, "board_serial");
    if (attr) {
      serial = attr->value;
    }
    sysfs_close_class_device(dev);
  }
  return serial;
}

static std::string get_mac_address()
{
  constexpr std::string_view LO = "lo";

  std::string mac;

  sysfs_class* cls = sysfs_open_class("net");
  if (cls) {
    dlist* devs = sysfs_get_class_devices(cls);

    dl_node* iter = devs->head;
    for (int i = 0; i <= devs->count; ++i) {
      if (iter->data == nullptr) {
        continue;
      }

      sysfs_class_device* dev = (sysfs_class_device*)iter->data;
      if (strncmp(dev->name, LO.data(), LO.size()) != 0) {
        sysfs_attribute* attr = sysfs_get_classdev_attr(dev, "address");
        if (attr) {
          mac = attr->value;
          break;
        }
      }
    }

    sysfs_close_class(cls);
  }

  return mac;
}

core::system::info core::system::get_system_info()
{
  info result;
  result.motherboard_id = get_motherboard_serial();
  result.mac_address = get_mac_address();
  return result;
}
