#include "include/system.hpp"

#include <fstream>
#include <algorithm>

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
  constexpr size_t NetDevAddrLen = 17;

  std::string mac;

  sysfs_class* cls = sysfs_open_class("net");
  if (cls) {
    dlist* devs = sysfs_get_class_devices(cls);

    sysfs_class_device* iter;
    dlist_for_each_data(devs, iter, sysfs_class_device) {
      if (strncmp(iter->name, LO.data(), LO.size()) != 0) {
        sysfs_attribute* attr = sysfs_get_classdev_attr(iter, "address");
        if (attr) {
          mac.insert(mac.end(), attr->value, attr->value + NetDevAddrLen);
          break;
        }
      }
    }
    sysfs_close_class(cls);
  }

  return mac;
}

static std::string get_cpu_serial()
{
  constexpr std::string_view Field = "Serial";

  std::string serial;
  std::ifstream cpuinfo("/proc/cpuinfo");

  if (cpuinfo)
  {
    std::string line;

    while (std::getline(cpuinfo, line)) {
      if (line > Field) {
        auto iter = std::find(line.rbegin(), line.rend(), ':');
        if (iter != line.rend()) {
          auto i = iter.base();
          while(*i == ' ') ++i;
          serial = std::string(i, line.end());
        }
      }
    }
  }

  cpuinfo.close();
  return serial;
}

core::system::info core::system::get_system_info()
{
  info result;
  result.motherboard_id = get_motherboard_serial();
  result.mac_address = get_mac_address();
  result.cpu_serial = get_cpu_serial();
  return result;
}
