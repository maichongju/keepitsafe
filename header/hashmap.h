#ifndef HASHMAP_H
#define HASHMAP_H

typedef enum
{
    type_int,
    type_char,
    type_str
} type;

typedef union DATA
{
    int i;
    char c;
    char *str;
} data;

typedef struct HASHMAPNODE
{
    data *key;
    data *val;
    struct HASHMAPNODE *next;
} hashmapnode;

typedef struct HASHMAP
{
    int size;
    type type_val;
    type type_key;
    hashmapnode *head;
    hashmapnode *tail;
} hashmap;

hashmap *hashmap_create(type typekey, type typeval);
hashmapnode *hashmap_node_create(type typekey, void *key, type typeval, void *val);
void hashmap_insert(hashmap *hmap, void *key, void *val);
void *hashmap_find_val(hashmap *hmap, void *key);
void hashmap_free(hashmap *map);
void hashmap_node_free(type typekey, type typeval, hashmapnode *node);
void display_hashmap(hashmap *map);

#endif