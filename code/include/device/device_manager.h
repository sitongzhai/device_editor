//******************************************************************************
//* Author: Sitong Zhai
//* Affiliation: University of Toronto
//* Created: 2022-06-08
//* Last updated: 2022-06-08
//* This is only a demo used for how to design the new project wizard pages
//******************************************************************************

#ifndef DEVICE_DEVICE_MANAGER
#define DEVICE_DEVICE_MANAGER

#include <string>
#include <vector>
#include <map>

namespace eda {

  class DeviceManager {
  public:
    class DeviceDef {
    public:
      std::string family;
      std::string name;
      std::vector<std::string> packages;
      std::vector<std::string> speeds;
    };
    
  private:
    static DeviceManager* manager_;
    std::vector<std::string> families_;
    std::vector<std::vector<DeviceDef>> devices_;
    std::map<std::string, size_t> family_index_map_;

    DeviceManager() {}
    ~DeviceManager() {}

  public:
    static DeviceManager* manager() {
      return manager_;
    }
    static void load();
    static void release() {
      if (manager_ != NULL) delete manager_;
      manager_ = NULL;
    }
    std::vector<std::string> families() { return families_; }
    std::vector<std::vector<DeviceDef>> devices() { return devices_; }
    void addDevice(DeviceDef device) {
      std::string family = device.family;
      auto iter = family_index_map_.find(device.family);
      size_t index = families_.size();
      if (iter == family_index_map_.end()) {
        family_index_map_.insert(std::make_pair(family, families_.size()));
        families_.push_back(family);
        devices_.push_back(std::vector<DeviceDef>());
      } else {
        index = iter->second;
      }
      devices_[index].push_back(device);
    }
  };
}


#endif // !DEVICE_DEVICE_MANAGER
