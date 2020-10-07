/**
 * @brief Hashmap utility. Each hashmap contact a link list of 
 * HashMapNode where they contain string of key and value.
 * insert O(1)
 * delete O(n)
 * find O(n)
 * @version 1.0
 * @date 2020/09/07
 * @author Chongju Mai
 * 
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "header/hashmap.h"

/**
 * @brief create a new hashmap, reutrn the hashmap as pointer.
 * When finish used hashmap, need to call method freeHashMap()
 * @return {hashmap *} pointer of the hashmap pointer
 */
hashmap *hashmap_create(type typekey, type typeval)
{
    hashmap *hmap = malloc(sizeof(hashmap));
    hmap->size = 0;
    hmap->head = NULL;
    hmap->tail = NULL;
    hmap->type_key = typekey;
    hmap->type_val = typeval;
    return hmap;
}

/**
 * @brief create new HashMapNode for the given key-value.
 * 
 * @param key {char *} key of the node
 * @param val {char *} val of the node
 * @return {hashmapnode *} pointer of the new hashmapnode
 * 
 */
hashmapnode *hashmap_node_create(type typekey, void *key, type typeval, void *val)
{
    data *datakey = malloc(sizeof(data));
    data *dataval = malloc(sizeof(data));
    hashmapnode *node = malloc(sizeof(hashmapnode));
    node->key = datakey;
    node->val = dataval;
    node->next = NULL;

    switch (typekey)
    {
    case type_int:
        datakey->i = *(int *)key;
        break;
    case type_char:
        datakey->c = *(char *)key;
        break;
    case type_str:
    {
        char *str = (char *)key;
        datakey->str = calloc(strlen(str) + 1, sizeof(char));
        strcpy(datakey->str, str);
        break;
    }
    default:
        break;
    }
    switch (typeval)
    {
    case type_int:
        dataval->i = *(int *)val;
        break;
    case type_char:
        dataval->c = *(char *)val;
        break;
    case type_str:
    {
        char *str = (char *)val;
        dataval->str = calloc(strlen(str) + 1, sizeof(char));
        strcpy(dataval->str, str);
        break;
    }
    default:
        break;
    }

    return node;
}

/**
 * @brief insert the given key-value to the hash map. 
 * O(1) Execute time
 * 
 * @param hmap {hashmap *} hashmap need to be insert
 * @param typekey {type} key type
 * @param key {char *} key for the key-value
 * @param typeval {type} value type
 * @param val {char *} value for the key-value 
 */
void hashmap_insert(hashmap *hmap, void *key, void *val)
{
    hashmapnode *node = hashmap_node_create(hmap->type_key, key, hmap->type_val, val);
    // Empty hashmap, insert to the head and the tail
    if (hmap->size == 0)
    {
        hmap->head = node;
        hmap->tail = node;
    }
    else
    {
        hmap->tail->next = node;
        hmap->tail = node;
    }
    hmap->size++;
}

/**
 * @brief Find the value of the given key in the hashmap, if 
 * given key is not found, NULL will be return
 * O(n)
 * 
 * @param hmap {hashmap *} target hashmap
 * @param key {void *} target key
 * @return char* value of the key if found, NULL other wise
 */
void *hashmap_find_val(hashmap *hmap, void *key)
{
    void *val = NULL;
    data *result = NULL;
    bool match = false;
    if (hmap->size > 0)
    {
        hashmapnode *current = hmap->head;
        while (current != NULL && !match)
        {
            switch (hmap->type_key)
            {
            case type_int:
                if (*(int *)key == current->key->i)
                {
                    result = current->val;
                    match = true;
                }
                break;
            case type_char:
                if (*(char *)key == current->key->c)
                {
                    result = current->val;
                    match = true;
                }
            case type_str:
                if (strcmp((char *)key, current->key->str) == 0)
                {
                    result = current->val;
                    match = true;
                }
            default:
                break;
            }
            current = current->next;
        }
        if (match)
        {
            switch (hmap->type_val)
            {
            case type_int:
                val = (void *)&result->i;
                break;
            case type_char:
                val = (void *)&result->c;
                break;
            case type_str:
                val = (void *)result->str;
                break;
            default:
                break;
            }
        }
    }
    return val;
}
/**
 * @brief Free hashmap node as it need it
 * 
 * @param typekey {type} key type
 * @param typeval {type} value type
 * @param node {hashmapnode*} node need to be free 
 */
void hashmap_node_free(type typekey, type typeval, hashmapnode *node)
{
    //free key str if needed
    if (typekey == type_str)
    {
        free(node->key->str);
    }
    if (typeval == type_str)
    {
        free(node->val->str);
    }
    free(node->key);
    free(node->val);
    free(node);
}

/**
 * @brief Free the given hashmap, including everysingle node
 * 
 * @param hmap {hashmap *} target hashmap
 */
void hashmap_free(hashmap *hmap)
{
    if (hmap != NULL)
    {
        if (hmap->size > 0)
        {
            if (hmap->size == 1)
            {
                hashmap_node_free(hmap->type_key, hmap->type_val, hmap->head);
                hmap->head = NULL;
                hmap->tail = NULL;
            }
            else
            {
                hashmapnode *prev = hmap->head;
                hashmapnode *current = prev->next;
                while (current != NULL)
                {
                    hashmap_node_free(hmap->type_key, hmap->type_val, prev);
                    prev = current;
                    current = current->next;
                }
                hashmap_node_free(hmap->type_key, hmap->type_val, prev);
            }
        }
        free(hmap);
    }
}

/**
 * @brief display the given hashmap to stdout
 * 
 * @param map {hashmap*} target hashmap
 */
void display_hashmap(hashmap *map)
{
    if (map->size > 0)
    {
        type valtype = map->type_val;
        type keytype = map->type_key;

        printf("size:%i\n", map->size);
        hashmapnode *current = map->head;
        while (current != NULL)
        {
            printf("{");
            if (keytype == type_int)
            {
                printf("%i,", current->key->i);
            }
            else if (keytype == type_char)
            {
                printf("%c,", current->key->c);
            }
            else if (keytype == type_str)
            {
                printf("%s,", current->key->str);
            }

            if (valtype == type_int)
            {
                printf("%i} ", current->val->i);
            }
            else if (valtype == type_char)
            {
                printf("%c} ", current->val->c);
            }
            else if (valtype == type_str)
            {
                printf("%s} ", current->val->str);
            }
            current = current->next;
        }
        printf("\n");
    }
    else
    {
        printf("Empty map\n");
    }
}
