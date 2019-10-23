#include <iostream>
#include <vector>
#include <bits/stdc++.h>
#include <string>
#include <utility>

using poset_id_t = unsigned long;
using element_id_t = unsigned long long;
using related_elements_t = std::unordered_set<element_id_t>;
using dictionary_t = std::unordered_map<std::string, element_id_t>;
using poset_t = std::pair<dictionary_t, std::unordered_map<element_id_t, std::pair<related_elements_t, related_elements_t>>>;
using poset_map_t = std::map<poset_id_t, poset_t>;

poset_map_t& poset_map() {
    static poset_map_t ans;
    return ans;
}

poset_id_t get_new_poset_id() {
    static poset_id_t id = 0;
    id++;
    return id;
}

unsigned long poset_new() {
    poset_id_t id = get_new_poset_id();
    poset_t poset;
    poset_map_t().insert({id, poset});
    return id;
}

void poset_delete(unsigned long id) {
    auto it = poset_map_t().find(id);
    if (it != poset_map_t().end()){
        poset_map_t().erase(it); // TODO kwestia usuniecia całego posetu z pamięci
    }
}

size_t poset_size(unsigned long id){
    auto it = poset_map_t().find(id);
    if (it != poset_map_t().end()){

    }
}

bool poset_insert(unsigned long id, char const *value);

bool poset_remove(unsigned long id, char const *value);

bool poset_add(unsigned long id, char const *value1, char const *value2);

bool poset_del(unsigned long id, char const *value1, char const *value2);

bool poset_test(unsigned long id, char const *value1, char const *value2);

void poset_clear(unsigned long id);


int main() {

    return 0;
}
