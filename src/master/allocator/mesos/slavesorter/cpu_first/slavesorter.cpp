// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include "master/allocator/mesos/slavesorter/cpu_first/slavesorter.hpp"

namespace mesos {
namespace internal {
namespace master {
namespace allocator {


ResourceSlaveSorterCPUFirst::ResourceSlaveSorterCPUFirst() {}

ResourceSlaveSorterCPUFirst::~ResourceSlaveSorterCPUFirst() {}

bool ResourceSlaveSorterCPUFirst::_compare(SlaveID& l, SlaveID& r)
{
 CHECK(freeResources.contains(l));
 CHECK(freeResources.contains(r));
  const Resources lres = freeResources[l];
  const Resources rres = freeResources[r];
  if (lres.cpus().getOrElse(0) < rres.cpus().getOrElse(0)){
    return true;
  }
  else if (lres.cpus().getOrElse(0) > rres.cpus().getOrElse(0)) {
    return false;
  }

  if (lres.mem().getOrElse(0) < rres.mem().getOrElse(0)){
    return true;
  }else if (lres.mem().getOrElse(0) > rres.mem().getOrElse(0)) {
    return false;
  }

  return  (lres.disk().getOrElse(0) < rres.disk().getOrElse(0));
}

void ResourceSlaveSorterCPUFirst::sort(
  std::vector<SlaveID>::iterator begin, std::vector<SlaveID>::iterator end)
{
  std::sort(
    begin, end, [this](SlaveID l, SlaveID r) { return _compare(l, r); });
}

void ResourceSlaveSorterCPUFirst::add(
  const SlaveID& slaveId,
  const SlaveInfo& slaveInfo,
  const Resources& resources)
{
  // TODO(jabnouneo): refine
  // totalResources[slaveId] += resources.createStrippedScalarQuantity();
  if (!resources.empty()) {
    // Add shared resources to the total quantities when the same
    // resources don't already exist in the total.
    const Resources newShared =
      resources.shared().filter([this, slaveId](const Resource& resource) {
        return !total_.resources[slaveId].contains(resource);
      });

    total_.resources[slaveId] += resources;
    freeResources[slaveId] = resources;
    const Resources scalarQuantities =
      (resources.nonShared() + newShared).createStrippedScalarQuantity();

    total_.scalarQuantities += scalarQuantities;
  }
}

void ResourceSlaveSorterCPUFirst::remove(
  const SlaveID& slaveId, const Resources& resources)
{
  if (!resources.empty()) {
    CHECK(total_.resources.contains(slaveId));
    CHECK(total_.resources[slaveId].contains(resources))
      << total_.resources[slaveId] << " does not contain " << resources;

    total_.resources[slaveId] -= resources;
    freeResources[slaveId] -= allocatedResources[slaveId];

    // Remove shared resources from the total quantities when there
    // are no instances of same resources left in the total.
    const Resources absentShared =
      resources.shared().filter([this, slaveId](const Resource& resource) {
        return !total_.resources[slaveId].contains(resource);
      });

    const Resources scalarQuantities =
      (resources.nonShared() + absentShared).createStrippedScalarQuantity();

    CHECK(total_.scalarQuantities.contains(scalarQuantities));
    total_.scalarQuantities -= scalarQuantities;

    if (total_.resources[slaveId].empty()) {
      total_.resources.erase(slaveId);
      freeResources.erase(slaveId);

    }
  }
}

void ResourceSlaveSorterCPUFirst::allocated(
  const SlaveID& slaveId, const Resources& toAdd)
{
  // Add shared resources to the allocated quantities when the same
  // resources don't already exist in the allocation.
  const Resources sharedToAdd =
    toAdd.shared().filter([this, slaveId](const Resource& resource) {
      return !total_.resources[slaveId].contains(resource);
    });

  const Resources quantitiesToAdd =
    (toAdd.nonShared() + sharedToAdd).createStrippedScalarQuantity();
  total_.resources[slaveId] += quantitiesToAdd;
  freeResources[slaveId] -= quantitiesToAdd;
  allocatedResources[slaveId] += toAdd;
  total_.scalarQuantities += quantitiesToAdd;
}

// Specify that resources have been unallocated on the given slave.
void ResourceSlaveSorterCPUFirst::unallocated(
  const SlaveID& slaveId, const Resources& toRemove)
{
  // TODO(jabnouneo): refine and account for shared resources
  CHECK(allocatedResources.contains(slaveId));
  CHECK(allocatedResources.at(slaveId).contains(toRemove))
    << "Resources " << allocatedResources.at(slaveId) << " at agent " << slaveId
    << " does not contain " << toRemove;

  const Resources quantitiesToRemove =
    (toRemove.nonShared()).createStrippedScalarQuantity();
  allocatedResources[slaveId] -= quantitiesToRemove;
  freeResources[slaveId] += quantitiesToRemove;



  if (allocatedResources[slaveId].empty()) {
    allocatedResources.erase(slaveId);
  }
}

} // namespace allocator {
} // namespace master {
} // namespace internal {
} // namespace mesos {
