/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _LIBUNWINDSTACK_MAP_INFO_H
#define _LIBUNWINDSTACK_MAP_INFO_H

#include <stdint.h>

#include <atomic>
#include <memory>
#include <mutex>
#include <string>

#include <unwindstack/Elf.h>
#include <unwindstack/Memory.h>

namespace unwindstack {

struct MapInfo {
  MapInfo(MapInfo* map_info, uint64_t start, uint64_t end, uint64_t offset, uint64_t flags,
          const char* name)
      : start(start),
        end(end),
        offset(offset),
        flags(flags),
        name(name),
        prev_map(map_info),
        load_bias(static_cast<uint64_t>(-1)) {}
  MapInfo(MapInfo* map_info, uint64_t start, uint64_t end, uint64_t offset, uint64_t flags,
          const std::string& name)
      : start(start),
        end(end),
        offset(offset),
        flags(flags),
        name(name),
        prev_map(map_info),
        load_bias(static_cast<uint64_t>(-1)) {}
  ~MapInfo() = default;

  uint64_t start = 0;
  uint64_t end = 0;
  uint64_t offset = 0;
  uint16_t flags = 0;
  std::string name;
  std::shared_ptr<Elf> elf;
  // This value is only non-zero if the offset is non-zero but there is
  // no elf signature found at that offset.
  uint64_t elf_offset = 0;
  // This value is the offset from the map in memory that is the start
  // of the elf. This is not equal to offset when the linker splits
  // shared libraries into a read-only and read-execute map.
  uint64_t elf_start_offset = 0;

  MapInfo* prev_map = nullptr;

  std::atomic_uint64_t load_bias;

  // This function guarantees it will never return nullptr.
  Elf* GetElf(const std::shared_ptr<Memory>& process_memory, ArchEnum expected_arch);

  uint64_t GetLoadBias(const std::shared_ptr<Memory>& process_memory);

  Memory* CreateMemory(const std::shared_ptr<Memory>& process_memory);

 private:
  MapInfo(const MapInfo&) = delete;
  void operator=(const MapInfo&) = delete;

  Memory* GetFileMemory();
  bool InitFileMemoryFromPreviousReadOnlyMap(MemoryFileAtOffset* memory);

  // Protect the creation of the elf object.
  std::mutex mutex_;
};

}  // namespace unwindstack

#endif  // _LIBUNWINDSTACK_MAP_INFO_H
