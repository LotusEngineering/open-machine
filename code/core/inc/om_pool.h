#ifndef OM_POOL_H_
#define OM_POOL_H_

int om_pool_new(size_t block_size, size_t block_count);

void* om_pool_malloc(int pool_id);

void om_pool_free(int pool_id, void* p_mem);


#endif //OM_POOL_H_