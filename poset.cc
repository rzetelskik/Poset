#include "poset.h"
#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <functional>
//TODO do wyjebania
#include <cassert>


namespace {
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

    bool is_cstring_valid(char const *value) {
        return (value != nullptr);
    }

    std::optional<element_id_t> get_element_id_from_dictionary(const dictionary_t& dictionary, const std::string& name) {
        auto it = dictionary.find(name);
        if (it != dictionary.end()) {
            return it->second;
        }
        return std::nullopt;
    }

    std::optional<std::reference_wrapper<poset_t>> get_poset(poset_id_t id) {
        auto it = poset_map().find(id);
        if (it != poset_map().end()) {
            std::reference_wrapper<poset_t> poset = it->second;
            return poset;
        }
        return std::nullopt;
    }

    element_id_t insert_element_into_dictionary(dictionary_t& dictionary, const std::string& name) {
        element_id_t id = get_new_element_id();
        dictionary.insert({name, id});

        return id;
    }

    void insert_element_into_poset_graph(poset_graph_t& poset_graph, element_id_t element_id) {
        related_elements_t predecessors, successors;
        poset_graph.insert({element_id, {predecessors, successors}});
    }

    bool are_elements_related(poset_t& poset, element_id_t element1_id, element_id_t element2_id) {
        auto it = poset.second.find(element1_id);
        related_elements_t& successors = it->second.second;

        return (successors.find(element2_id) != successors.end());
    }

    void add_relation(poset_t& poset, element_id_t element1_id, element_id_t element2_id) {
        auto it = poset.second.find(element1_id);
        related_elements_t& first_successors = it->second.second;
        first_successors.insert(element2_id);

        it = poset.second.find(element2_id);
        related_elements_t& second_predecessors = it->second.first;
        second_predecessors.insert(element1_id);
    }

    void add_relation_transitively(poset_t& poset, element_id_t element1_id, element_id_t element2_id) {
        auto it = poset.second.find(element1_id);
        related_elements_t& preceeding_first = it->second.first;

        for (element_id_t el: preceeding_first) {
            add_relation(poset, el, element2_id);
        }

        it = poset.second.find(element2_id);
        related_elements_t& proceeding_second = it->second.second;

        for (element_id_t el: proceeding_second) {
            add_relation(poset, element1_id, el);
        }

        add_relation(poset, element1_id, element2_id);
    }

    void remove_relation(poset_t& poset, element_id_t element1_id, element_id_t element2_id) {
        //TODO sprawdzanie czy nie zaburzy relacji (przechodniosc)
        auto it = poset.second.find(element1_id);

        related_elements_t& successors = it->second.second;
        successors.erase(element2_id);

        it = poset.second.find(element2_id);

        related_elements_t& predecessors = it->second.first;
        predecessors.erase(element1_id);
    }

    void remove_from_poset_graph(poset_t& poset, element_id_t element_id) {
        auto it = poset.second.find(element_id);

        related_elements_t& predecessors = it->second.first;
        related_elements_t& successors = it->second.second;

        for (auto p_id: predecessors) {
            /*auto it2 = poset_graph.find(p);
            assert(it2 != poset_graph.end());
            predecessor = true;
            remove_x_from_elements_related_to_y(element_id, p, predecessor, poset_graph);*/
            remove_relation(poset, element_id, p_id);
        }
        for (auto s_id: successors) {
            /*auto it3 = poset_graph.find(s_id);
            assert(it3 != poset_graph.end());
            predecessor = false;
            remove_x_from_elements_related_to_y(element_id, s_id, predecessor, poset_graph);*/
            remove_relation(poset, element_id, s_id);
        }

        poset.second.erase(element_id);
    }
}

unsigned long jnp1::poset_new() {
    poset_id_t id = get_new_poset_id();
    poset_t poset;
    poset_map().insert({id, poset});
    return id;
}

size_t jnp1::poset_size(unsigned long id) {
    auto it = poset_map().find(id);
    return ((it != poset_map().end()) ? it->second.first.size() : 0);
}

bool jnp1::poset_insert(unsigned long id, char const *value) {
    auto poset_opt = get_poset(id);
    if (!poset_opt.has_value()) return false;

    poset_t& poset = poset_opt.value().get();

    if (!is_cstring_valid(value)) return false;
    std::string element_name(value);

    if (get_element_id_from_dictionary(poset.first, element_name).has_value()) return false;

    element_id_t element_id = insert_element_into_dictionary(poset.first, element_name);
    insert_element_into_poset_graph(poset.second, element_id);

    return true;
}

bool jnp1::poset_remove(unsigned long id, char const *value) {
    auto poset_opt = get_poset(id);
    if (!poset_opt.has_value()) return false;

    if (!is_cstring_valid(value)) return false;
    std::string element_name(value);

    poset_t& poset = poset_opt.value().get();

    std::optional<element_id_t> element_id = get_element_id_from_dictionary(poset.first, element_name);
    if (!element_id.has_value()) return false;

    remove_from_poset_graph(poset, element_id.value());
    //TODO remove from dictionary

    return true;
}

bool jnp1::poset_add(unsigned long id, char const *value1, char const *value2) {
    auto poset_opt = get_poset(id);
    if (!poset_opt.has_value()) return false;

    poset_t& poset = poset_opt.value().get();

    if (!is_cstring_valid(value1) || !is_cstring_valid(value2)) return false;

    std::string element1_name(value1), element2_name(value2);
    if (element1_name == element2_name) return false;

    std::optional<element_id_t> element1_id, element2_id;
    element1_id = get_element_id_from_dictionary(poset.first, element1_name);
    element2_id = get_element_id_from_dictionary(poset.first, element2_name);

    if (!element1_id.has_value() || !element2_id.has_value() ||
        are_elements_related(poset, element1_id.value(), element2_id.value()) ||
        are_elements_related(poset, element2_id.value(), element1_id.value())) return false;

    add_relation_transitively(poset, element1_id.value(), element2_id.value());

    return true;
}

bool jnp1::poset_del(unsigned long id, char const *value1, char const *value2) {
    if (!is_cstring_valid(value1) || !is_cstring_valid(value2)) return false;
    std::string element1_name(value1), element2_name(value2);

    auto poset_opt = get_poset(id);
    if (!poset_opt.has_value()) return false;

    poset_t& poset = poset_opt.value().get();

    std::optional<element_id_t> element1_id, element2_id;
    element1_id = get_element_id_from_dictionary(poset.first, element1_name);
    element2_id = get_element_id_from_dictionary(poset.first, element2_name);

    if (!element1_id.has_value() || !element2_id.has_value() ||
        !are_elements_related(poset, element1_id.value(), element2_id.value())) return false;

    remove_relation(poset, element1_id.value(), element2_id.value());

    return true;
}

bool jnp1::poset_test(unsigned long id, char const *value1, char const *value2) {
    auto poset_opt = get_poset(id);
    if (!poset_opt.has_value()) return false;

    poset_t& poset = poset_opt.value().get();

    if (!is_cstring_valid(value1) || !is_cstring_valid(value2)) return false;

    std::string name1(value1), name2(value2);
    std::optional<element_id_t> element_id1, element_id2;
    element_id1 = get_element_id_from_dictionary(poset.first, name1);
    element_id2 = get_element_id_from_dictionary(poset.first, name2);

    if (!element_id1.has_value() || !element_id2.has_value()) return false;
    if (element_id1.value() == element_id2.value()) return true;

    return are_elements_related(poset, element_id1.value(), element_id2.value());
}

void jnp1::poset_clear(unsigned long id) {
    auto poset_opt = get_poset(id);
    if (!poset_opt.has_value()) return;

    poset_t& poset = poset_opt.value().get();

    poset.first.clear();
    poset.second.clear();
}


int main() {
    using namespace jnp1;

    poset_id_t poset_id = poset_new();
    assert(!poset_size(poset_id));
    assert(poset_insert(poset_id, "a"));
    assert(!poset_insert(poset_id, "a"));
    assert(poset_insert(poset_id, "b"));
    assert(poset_insert(poset_id, "c"));
    assert(poset_insert(poset_id, "d"));
    assert(poset_add(poset_id, "b", "c"));
    assert(poset_test(poset_id, "b", "c"));
    assert(!poset_test(poset_id, "c", "b"));
    assert(poset_test(poset_id, "b", "b"));
    assert(!poset_insert(poset_id, nullptr));
    assert(!poset_add(poset_id, "a", "a"));
    assert(!poset_add(poset_id, "c", "b"));
    assert(!poset_add(poset_id, "b", "c"));
    assert(poset_add(poset_id, "c", "d"));
    assert(poset_test(poset_id, "c", "d"));
    assert(poset_test(poset_id, "b", "d"));
    assert(poset_add(poset_id, "a", "b"));
    assert(poset_test(poset_id, "a", "b"));
    assert(poset_test(poset_id, "a", "d"));
    assert(!poset_test(poset_id, "d", "a"));
    poset_clear(poset_id);
    assert(!poset_test(poset_id, "a", "d"));
    assert(!poset_test(poset_id, "b", "c"));
    assert(!poset_test(poset_id, "b", "d"));
    assert(poset_insert(poset_id, "a"));
    return 0;
}
