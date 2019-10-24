#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <assert.h>

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

//size_t poset_size(unsigned long id) {
//    auto it = poset_map().find(id);
//    if (it != poset_map().end()){
//
//    }
//}

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


void remove_relation(element_id_t element1_id,element_id_t  element2_id,poset_graph_t poset_graph) {

    auto it1 = poset_graph.find(element1_id);
    assert(it1 != poset_graph.end());

    related_elements_t successors = it1->second.second;
    successors.erase(element2_id);

    it1 = poset_graph.find(element2_id);
    assert(it1 != poset_graph.end());

    related_elements_t predecessor = it1->second.first;
    successors.erase(element1_id);


}

bool remove_from_graph(element_id_t element_id, poset_graph_t poset_graph) {

    auto it1 = poset_graph.find(element_id);
    if (it1 == poset_graph.end())
        return false;

    related_elements_t predecessors =  it1->second.first;
    related_elements_t successors =  it1->second.second;

    // bool predecessor;
    for (auto p_id:predecessors) {
        /*auto it2 = poset_graph.find(p);
        assert(it2 != poset_graph.end());
        predecessor = true;
        remove_x_from_elements_related_to_y(element_id, p, predecessor, poset_graph);*/
        remove_relation(element_id, p_id, poset_graph);
    }
    for (auto s_id:successors) {
        /*auto it3 = poset_graph.find(s_id);
        assert(it3 != poset_graph.end());
        predecessor = false;
        remove_x_from_elements_related_to_y(element_id, s_id, predecessor, poset_graph);*/
        remove_relation(element_id, s_id, poset_graph);
    }
    poset_graph.erase(element_id);
    return true;
}


bool poset_remove(unsigned long id, char const *value) {

    if (value == nullptr)
        return false;
    std::string element(value);

    auto it1 = poset_map_t().find(id);
    if (it1 == poset_map_t().end()) {
        return false;
    }

    poset_graph_t poset_graph = it1->second.second;
    dictionary_t dictionary = it1->second.first;

    auto it2 = dictionary.find(element);
    if (it2 == dictionary.end())
        return false;

    element_id_t element_id = it2->second;

    return remove_from_graph(element_id, poset_graph);

    /*Jeżeli istnieje poset o identyfikatorze id i element value należy do tego
        zbioru, to usuwa element ze zbioru oraz usuwa wszystkie relacje tego
    elementu, a w przeciwnym przypadku nic nie robi. Wynikiem jest true, gdy
    element został usunięty, a false w przeciwnym przypadku.*/
}

bool poset_add(unsigned long id, char const *value1, char const *value2);



bool poset_del(unsigned long id, char const *value1, char const *value2){

    if (value1 == nullptr || value2 == nullptr)
        return false;
    std::string element1(value1);
    std::string element2(value2);

    auto it1 = poset_map_t().find(id);
    if (it1 == poset_map_t().end()) {
        return false;
    }

    poset_graph_t poset_graph = it1->second.second;
    dictionary_t dictionary = it1->second.first;

    auto it2 = dictionary.find(element1);
    if (it2 == dictionary.end())
        return false;
    element_id_t element1_id = it2->second;

    auto it3 = dictionary.find(element1);
    if (it3 == dictionary.end())
        return false;

    element_id_t element2_id = it3->second;

    remove_relation(element1_id, element2_id, poset_graph);
    return true;
}

bool poset_test(unsigned long id, char const *value1, char const *value2);

void poset_clear(unsigned long id);


int main() {

    return 0;
}
