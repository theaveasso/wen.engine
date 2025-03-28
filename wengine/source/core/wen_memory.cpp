#include "wen/core/wen_memory.hpp"

#include "wen/core/wen_logger.hpp"
#include "wen/platform/wen_platform.hpp"

struct memory_stats_t {
  uint64_t total_allocated;
  uint64_t tagged_allocations[MEMTAG_MAX_TAGS];
};

static struct memory_stats_t stats {};

static const char* memtags[MEMTAG_MAX_TAGS] = {
    "UNKNOWN",     "ARRAY",  "APPLICATION",
    "BST",         "DICT",   "ENTITY",
    "ENTITY_NODE", "JOB",    "MATERIAL_INSTANCE",
    "RING_QUEUE",  "STRING", "SCENE",
    "TEXTURE",     "VECTOR", "RENDERER",

};

void wen_meminit() {
  pm_memzero(&stats, sizeof(stats));
}

void wen_memcleanup() {
  // TODO:
}

void* wen_memalloc(uint64_t size_, memory_tag_t tag_) {
  if (tag_ == memory_tag_t::MEMTAG_UNKNOWN) {
    WEN_WARN(
        "wen_allocate called using MEMTAG_UNKNOWN. re-class this allocation.");
  }

  stats.total_allocated += size_;
  stats.tagged_allocations[tag_] += size_;

  // TODO: MEMORY ALIGNMENT
  void* block = pm_memalloc(size_, false);
  pm_memzero(block, size_);

  return block;
}

void wen_memfree(void* mem_, uint64_t size_, memory_tag_t tag_) {
  if (tag_ == memory_tag_t::MEMTAG_UNKNOWN) {
    WEN_WARN(
        "wen_allocate called using MEMTAG_UNKNOWN. re-class this allocation.");
  }

  stats.total_allocated -= size_;
  stats.tagged_allocations[tag_] -= size_;

  // TODO: MEMORY ALIGNMENT
  pm_memfree(mem_, false);
}

void* wen_memzero(void* mem_, uint64_t len) {
  return pm_memzero(mem_, len);
}

void* wen_memcpy(void* dst, const void* src, uint64_t len) {
  return pm_memcpy(dst, src, len);
}

void* wen_memset(void* dst_, int32_t val_, uint64_t dwords_) {
  return pm_memset(dst_, val_, dwords_);
}

std::string get_mem_usage_str() {
  std::string buffer = "WEN System memory usage: \n";

  for (uint64_t i = 0; i < MEMTAG_MAX_TAGS; ++i) {
    char  unit[4] = "XIB";
    float amount  = 1.0f;

    if (stats.tagged_allocations[i] >= GIB(1)) {
      unit[0] = 'G';
      amount  = static_cast<float>(stats.tagged_allocations[i]) / GIB(1);
    } else if (stats.tagged_allocations[i] >= MIB(1)) {
      unit[0] = 'M';
      amount  = static_cast<float>(stats.tagged_allocations[i]) / MIB(1);
    } else if (stats.tagged_allocations[i] >= KIB(1)) {
      unit[0] = 'K';
      amount  = static_cast<float>(stats.tagged_allocations[i]) / KIB(1);
    } else {
      unit[0] = 'B';
      unit[1] = 0;
      amount  = static_cast<float>(stats.tagged_allocations[i]);
    }
    buffer += memtags[i] + (": " + std::to_string(amount) + unit + "\n");
  }
  return buffer;
}
