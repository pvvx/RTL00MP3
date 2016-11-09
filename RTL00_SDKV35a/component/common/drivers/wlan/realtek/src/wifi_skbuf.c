#include <section_config.h>
#include <freertos/wrapper.h>
#undef MAX_SKB_BUF_NUM
#define MAX_SKB_BUF_NUM 16

// DO NOT modify this structure
struct skb_data {
	struct list_head list;
	unsigned char buf[MAX_SKB_BUF_SIZE];
	atomic_t ref;
};

SRAM_BD_DATA_SECTION
struct skb_data skb_data_pool[MAX_SKB_BUF_NUM];
