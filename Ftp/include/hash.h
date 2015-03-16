#ifndef HASH_H
#define HASH_H

//hash函数
typedef unsigned int (*hashfunc_t)(unsigned int, void*);

typedef struct hash_node
{
	void *key;	//关键字
	void *value;	//要存储的信息
	struct hash_node *prev;	
	struct hash_node *next;
} hash_node_t;


//hash表的定义
typedef struct hash
{
	unsigned int buckets;	//bucket个数
	hashfunc_t hash_func;	//hash函数
	hash_node_t **nodes;	//链表数组
} hash_t;


hash_t* hash_alloc(unsigned int buckets, hashfunc_t hash_func);
void* hash_lookup_value_by_key(hash_t *hash, void* key, unsigned int key_size);
void hash_add_entry(hash_t *hash, void *key, unsigned int key_size,
	void *value, unsigned int value_size);
void hash_free_entry(hash_t *hash, void *key, unsigned int key_size);
void hash_clear_entry(hash_t *hash);
void hash_destroy(hash_t *hash);

#endif	/* HASH_H */
