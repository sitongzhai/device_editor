//******************************************************************************
//* Author: Sitong Zhai
//* Affiliation: University of Toronto
//* Created: 2022-06-08
//* Last updated: 2022-06-08
//* This is only a demo used for how to design the new project wizard pages
//******************************************************************************

#include "device/device_manager.h"

namespace eda {

  DeviceManager* DeviceManager::manager_ = NULL;

  void DeviceManager::load() {
    release();
    manager_ = new DeviceManager();

    DeviceDef device;
    device.family = "Kintex7";
    device.name = "325t";
    device.packages.push_back("ffg900");
    device.speeds.push_back("-1");
    device.speeds.push_back("-2");
    device.speeds.push_back("-3");
    manager_->addDevice(device);

    DeviceDef device_1;
    device_1.family = "Virtex7";
    device_1.name = "690t";
    device_1.speeds.push_back("-1");
    device_1.speeds.push_back("-2");
    device_1.speeds.push_back("-3");
    manager_->addDevice(device_1);
  }

}