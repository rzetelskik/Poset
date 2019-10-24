#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>

using poset_id_t = unsigned long;
using element_id_t = unsigned long long;
using related_elements_t = std::unordered_set<element_id_t>;
using dictionary_t = std::unordered_map<std::string, element_id_t>;
using poset_graph_t = std::unordered_map<element_id_t, std::pair<related_elements_t, related_elements_t>>;
using poset_t = std::pair<dictionary_t, poset_graph_t>;
using poset_map_t = std::unordered_map<poset_id_t, poset_t>;

poset_map_t& poset_map() {
    static poset_map_t poset_map;
    return poset_map;
}

poset_id_t get_new_poset_id() {
    static poset_id_t id = 0;
    id++;
    return id;
}

element_id_t get_new_element_id() {
    static element_id_t id = 0;
    id++;
    return id;
}

unsigned long poset_new() {
    poset_id_t id = get_new_poset_id();
    poset_t poset;
    poset_map().insert({id, poset});
    return id;
}

//void poset_delete(unsigned long id) {
//    auto it = poset_map().find(id);
//    if (it != poset_map().end()){
//        poset_map().erase(it); // TODO kwestia usuniecia całego posetu z pamięci
//    }
//}

size_t poset_size(unsigned long id) {
    auto it = poset_map().find(id);
    return ((it != poset_map().end()) ? it->second.first.size() : 0);
}

std::optional<element_id_t> get_element_id_from_dictionary(const dictionary_t& dictionary, const std::string& name) {
    auto it = dictionary.find(name);
    if (it != dictionary.end()) {
        return it->second;
    }
    return std::nullopt;
}

element_id_t insert_element_into_dictionary(dictionary_t& dictionary, const std::string& name) {
    element_id_t id = get_new_element_id();
    dictionary.insert({name, id});

    return id;
}

void insert_element_into_poset_graph(poset_graph_t& poset_graph, element_id_t element_id) {
    related_elements_t preceeding, proceeding;
    poset_graph.insert({element_id, {preceeding, proceeding}});
}

bool poset_insert(unsigned long id, char const *value) {
    auto it = poset_map().find(id);
    if (it == poset_map().end()) {
        return false;
    }
    poset_t& poset = it->second;

    std::string element_name(value);
    if (get_element_id_from_dictionary(poset.first, element_name).has_value()) {
        return false;
    }

    element_id_t element_id = insert_element_into_dictionary(poset.first, element_name);
    insert_element_into_poset_graph(poset.second, element_id);

    return true;
}

bool poset_remove(unsigned long id, char const *value);

bool poset_add(unsigned long id, char const *value1, char const *value2);

bool poset_del(unsigned long id, char const *value1, char const *value2);

bool poset_test(unsigned long id, char const *value1, char const *value2);

void poset_clear(unsigned long id);


int main() {

    return 0;
}
