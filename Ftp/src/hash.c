/*
 * @文件功能：
 * 建立hash表；
 */
#include "hash.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>

static hash_node_t ** hash_get_bucket(hash_t *hash, void *key);
static hash_node_t *hash_get_node_by_key(hash_t *hash, void *key, unsigned int key_size);
static void hash_clear_bucket(hash_t *hash, unsigned int index);


/* 建立一个hash表，并返回指向新建hash表的指针 */
hash_t* hash_alloc(unsigned int buckets, hashfunc_t hash_func)
{
    /* 分配hash表所需的内存空间 */
	hash_t *hash = (hash_t *)malloc(sizeof(hash_t));
	hash->buckets = buckets;	/* 桶的个数 */
	hash->hash_func = hash_func; /* hash函数 */
	int len = sizeof(hash_node_t *) * buckets; /* 链表数组占用的字节数 */
	hash->nodes = (hash_node_t **)malloc(len);/* 分配指向链表数组指针的内存空间 */
	memset(hash->nodes, 0, len);

	return hash;
}

/* 根据key查找value */
void* hash_lookup_value_by_key(hash_t *hash, void* key, unsigned int key_size)
{
	/* 先确认key所对应的节点 */
	hash_node_t *node = hash_get_node_by_key(hash, key, key_size);
	if(node == NULL)
		return NULL;
	else
		return node->value;
}

/* hash表新添加节点 */
void hash_add_entry(hash_t *hash, void *key, unsigned int key_size,
	void *value, unsigned int value_size)
{
	/* 1. 查找key所对应的bucket编号 */
	hash_node_t **buck = hash_get_bucket(hash, key);
	assert(buck != NULL);

	/* 2. 创建新节点 */
	hash_node_t *node = (hash_node_t *)malloc(sizeof(hash_node_t));
	memset(node, 0, sizeof(hash_node_t));
	node->key = malloc(key_size);
	node->value = malloc(value_size);
	memcpy(node->key, key, key_size);
	memcpy(node->value, value, value_size);

	/* 3. 在链表中头部插入新节点 */
	if(*buck != NULL)
	{
		hash_node_t *head = *buck; /* head是链表的第一个节点 */

		node->next = head;
		head->prev = node;
		*buck = node; /* 更新链表指针 */
	}
	else
	{
		*buck = node;
	}
}

/* 销毁指定的节点 */
void hash_free_entry(hash_t *hash, void *key, unsigned int key_size)
{
	/* 查找指定的节点 */
	hash_node_t *node = hash_get_node_by_key(hash, key, key_size);
	assert(node != NULL);

	free(node->key);
	free(node->value);

	/* 若删除节点后，更新链表 */
	if(node->prev != NULL)
	{
        /* 所删除的节点不是头节点 */
		node->prev->next = node->next;
	}
	else
	{
        /* 所删除的节点是头节点 */
		hash_node_t **buck = hash_get_bucket(hash, key);
		*buck = node->next;
	}
	if(node->next != NULL)
		node->next->prev = node->prev;

	free(node);
}

/* 清空hash表 */
void hash_clear_entry(hash_t *hash)
{
	unsigned int i;
	for(i = 0; i < hash->buckets; ++i)
	{
		hash_clear_bucket(hash, i);
		hash->nodes[i] = NULL;
	}
}

/* 销毁hash表 */
void hash_destroy(hash_t *hash)
{
	hash_clear_entry(hash);
	free(hash->nodes);
	free(hash);
}

/* 根据key获取bucket位置 */
static hash_node_t **hash_get_bucket(hash_t *hash, void *key)
{
    /* 由hash函数映射到相应的桶位置 */
	unsigned int pos = hash->hash_func(hash->buckets, key);
	assert(pos < hash->buckets);

	return &(hash->nodes[pos]);
}

/* 根据key获取node节点 */
static hash_node_t *hash_get_node_by_key(hash_t *hash, void *key, unsigned int key_size)
{
	/* 1. 获取key所对应的bucket位置 */
	hash_node_t **buck = hash_get_bucket(hash, key);
	assert(buck != NULL);

	/* 2. 遍历节点链表查找key所对应的节点 */
	hash_node_t *node = *buck; /* 指向链表第一个节点 */
	while(node != NULL && memcmp(node->key, key, key_size) != 0)
		node = node->next;

    /* 返回所对应的节点，可能包含NULL，表示找不到 */
	return node;
}

/* 清空某个bucket所对应的节点链表 */
static void hash_clear_bucket(hash_t *hash, unsigned int index)
{
	assert(index < hash->buckets); /* 防止越界 */
	hash_node_t *node = hash->nodes[index];
	hash_node_t *tmp = node;
	while(node)
	{
		tmp = node->next;
		free(node->key);
		free(node->value);
		free(node);
		node = tmp;
	}
	hash->nodes[index] = NULL;
}

