#include "poset.h"
#include <vector>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <functional>
#include <cassert>

namespace {

#ifdef NDEBUG
    bool debug() {
		static const bool debug = false;
		return debug;
	}
#else
    bool debug() {
        static const bool debug = true;
        return debug;
    }
#endif

    using poset_id_t = unsigned long;
    using element_id_t = unsigned long long;
    using related_elements_t = std::unordered_set<element_id_t>;
    using dictionary_t = std::unordered_map<std::string, element_id_t>;
    using poset_graph_t = std::unordered_map<element_id_t, std::pair<related_elements_t, related_elements_t>>;
    using poset_t = std::pair<dictionary_t, poset_graph_t>;
    using poset_map_t = std::unordered_map<poset_id_t, poset_t>;

    enum relation_order_e {
        PREDECESSING,
        SUCCESSING
    };

    template<typename ... Args>
    inline void log_debug() {
        if (debug()) {
            std::cerr << std::endl;
        }
    }

    template<typename T, typename ... Args>
    inline void log_debug(T t, Args ... args) {
        if (debug()) {
            std::cerr << t;
            log_debug(args...);
        }
    }

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

    element_id_t insert_element_into_dictionary(dictionary_t& dictionary, const std::string& element_name) {
        element_id_t element_id = get_new_element_id();
        dictionary.insert({element_name, element_id});

        return element_id;
    }

    void remove_element_from_dictionary(dictionary_t& dictionary, const std::string& element_name) {
        dictionary.erase(element_name);
    }

    void insert_element_into_poset_graph(poset_graph_t& poset_graph, element_id_t element_id) {
        related_elements_t predecessors, successors;
        poset_graph.insert({element_id, {predecessors, successors}});
    }

    bool are_elements_related(poset_graph_t& poset_graph, element_id_t element1_id, element_id_t element2_id) {
        auto it = poset_graph.find(element1_id);
        related_elements_t& successors = it->second.second;

        return (successors.find(element2_id) != successors.end());
    }

    void add_relation(poset_graph_t& poset_graph, element_id_t element1_id, element_id_t element2_id) {
        auto it = poset_graph.find(element1_id);
        related_elements_t& element1_successors = it->second.second;
        element1_successors.insert(element2_id);

        it = poset_graph.find(element2_id);
        related_elements_t& element2_predecessors = it->second.first;
        element2_predecessors.insert(element1_id);
    }

    void add_relation_transitively(poset_graph_t& poset_graph, element_id_t element1_id, element_id_t element2_id) {
        auto it = poset_graph.find(element1_id);
        related_elements_t& element1_predecessors = it->second.first;

        for (element_id_t el: element1_predecessors) {
            add_relation(poset_graph, el, element2_id);
        }

        it = poset_graph.find(element2_id);
        related_elements_t& element2_successors = it->second.second;

        for (element_id_t el: element2_successors) {
            add_relation(poset_graph, element1_id, el);
        }

        add_relation(poset_graph, element1_id, element2_id);
    }

    void remove_relation(poset_graph_t& poset_graph, element_id_t element1_id, bool erase_from_element1,
                         element_id_t element2_id, bool erase_from_element2) {
        if (erase_from_element1) {
            auto it1 = poset_graph.find(element1_id);
            related_elements_t& element1_successors = it1->second.second;
            element1_successors.erase(element2_id);
        }

        if (erase_from_element2) {
            auto it2 = poset_graph.find(element2_id);
            related_elements_t& element2_predecessors = it2->second.first;
            element2_predecessors.erase(element1_id);
        }
    }

    bool can_remove_relation(poset_graph_t& poset_graph, element_id_t element1_id, element_id_t element2_id) {
        auto it1 = poset_graph.find(element1_id);
        assert(it1 != poset_graph.end());
        related_elements_t& element1_successors = it1->second.second;

        auto it2 = poset_graph.find(element2_id);
        assert(it2 != poset_graph.end());
        related_elements_t& element2_predecessors = it2->second.first;

        for (auto successor_id: element1_successors) {
            auto it3 = element2_predecessors.find(successor_id);

            // A non-empty intersection means that removing the relation
            // would violate poset conditions.
            if (it3 != element2_predecessors.end()) {
                return false;
            }
        }

        return true;
    }

    void remove_relations_iteratively(poset_graph_t& poset_graph, element_id_t element_id,
            related_elements_t& related_elements, relation_order_e relation_order) {
        for (auto it = related_elements.begin(); it != related_elements.end(); it = related_elements.erase(it)) {
            element_id_t related_element_id = *it;

            if (relation_order == PREDECESSING) {
                remove_relation(poset_graph, related_element_id, true, element_id, false);
            } else {
                remove_relation(poset_graph, element_id, false, related_element_id, true);
            }
        }
    }

    void remove_element_from_poset_graph(poset_graph_t& poset_graph, element_id_t element_id) {
        auto it = poset_graph.find(element_id);
        assert(it != poset_graph.end());

        related_elements_t& predecessors = it->second.first;
        related_elements_t& successors = it->second.second;

        remove_relations_iteratively(poset_graph, element_id, predecessors, PREDECESSING);
        remove_relations_iteratively(poset_graph, element_id, successors, SUCCESSING);

        poset_graph.erase(element_id);
    }
}

unsigned long jnp1::poset_new() {
    poset_id_t id = get_new_poset_id();
    poset_t poset;
    poset_map().insert({id, poset});

    log_debug("poset_new()");
    log_debug("poset_new: poset ", id, " created");

    return id;
}

void jnp1::poset_delete(unsigned long id) {
    log_debug("poset_delete(", id, ")");

    auto it = poset_map().find(id);
    if (it == poset_map().end()) {
        log_debug("poset_delete: poset ", id, " does not exist");
        return;
    }

    poset_map().erase(it);
    log_debug("poset_delete: poset ", id, " deleted");
}

size_t jnp1::poset_size(unsigned long id) {
    log_debug("poset_size(", id, ")");

    auto poset_opt = get_poset(id);
    if (!poset_opt.has_value()) {
        log_debug("poset_size: poset ", id, " does not exist");
        return 0;
    }

    size_t size = poset_opt.value().get().first.size();
    log_debug("poset_size: poset ", id, " contains ", size, " element(s)");

    return size;
}

bool jnp1::poset_insert(unsigned long id, char const *value) {
    log_debug("poset_insert(", id, ", \"", (is_cstring_valid(value) ? value : "NULL"), "\")");

    auto poset_opt = get_poset(id);
    if (!poset_opt.has_value()) {
        log_debug("poset_insert: poset ", id, " does not exist");
        return false;
    }
    poset_t& poset = poset_opt.value().get();

    if (!is_cstring_valid(value)) {
        log_debug("poset_insert: invalid value (NULL)");
        return false;
    }
    std::string element_name(value);

    if (get_element_id_from_dictionary(poset.first, element_name).has_value()) {
        log_debug("poset_insert: poset ", id, ", element \"", value, "\"", " already exists");
        return false;
    }

    element_id_t element_id = insert_element_into_dictionary(poset.first, element_name);
    insert_element_into_poset_graph(poset.second, element_id);
    log_debug("poset_insert: poset ", id, ", element \"", value, "\"", " inserted");

    return true;
}

bool jnp1::poset_remove(unsigned long id, char const *value) {
    log_debug("poset_remove(", id, ", \"", (is_cstring_valid(value) ? value : "NULL"), "\")");

    auto poset_opt = get_poset(id);
    if (!poset_opt.has_value()) {
        log_debug("poset_remove: poset ", id, " does not exis");
        return false;
    }

    if (!is_cstring_valid(value)) {
        log_debug("poset_remove: invalid value (NULL)");
        return false;
    }
    std::string element_name(value);

    poset_t& poset = poset_opt.value().get();

    std::optional<element_id_t> element_id = get_element_id_from_dictionary(poset.first, element_name);
    if (!element_id.has_value()) {
        log_debug("poset_remove: poset ", id, ", element \"", value, "\"", " does not exist");
        return false;
    }

    remove_element_from_poset_graph(poset.second, element_id.value());
    remove_element_from_dictionary(poset.first, element_name);
    log_debug("poset_remove: poset ", id, ", element \"", value, "\"", " removed");

    return true;
}

bool jnp1::poset_add(unsigned long id, char const *value1, char const *value2) {
    log_debug("poset_add(", id, ", \"", (is_cstring_valid(value1) ? value1 : "NULL"), "\", \"",
              (is_cstring_valid(value2) ? value2 : "NULL"), "\"", ")");

    auto poset_opt = get_poset(id);
    if (!poset_opt.has_value()) {
        log_debug("poset_add: poset ", id, " does not exist");
        return false;
    }

    if (!is_cstring_valid(value1)) {
        log_debug("poset_add: invalid value1 (NULL)");
        return false;
    }
    if (!is_cstring_valid(value2)) {
        log_debug("poset_add: invalid value2 (NULL)");
        return false;
    }

    poset_t& poset = poset_opt.value().get();
    std::string element1_name(value1), element2_name(value2);
    if (element1_name == element2_name) {
        log_debug("poset_add: poset ", id, ", relation (\"", value1, "\", \"", value2 , "\") cannot be added");
        return false;
    }

    std::optional<element_id_t> element1_id, element2_id;
    element1_id = get_element_id_from_dictionary(poset.first, element1_name);
    element2_id = get_element_id_from_dictionary(poset.first, element2_name);

    if (!element1_id.has_value() || !element2_id.has_value()) {
        log_debug("poset_add: poset ", id, ", element \"", value1, "\" or \"", value2 , "\" does not exist");
        return false;
    }

    if (are_elements_related(poset.second, element1_id.value(), element2_id.value()) ||
        are_elements_related(poset.second, element2_id.value(), element1_id.value())) {
        log_debug("poset_add: poset ", id, ", relation (\"", value1, "\", \"", value2 , "\") cannot be added");
        return false;
    }

    add_relation_transitively(poset.second, element1_id.value(), element2_id.value());
    log_debug("poset_add: poset ", id, ", relation (\"", value1, ",\" \"", value2 , "\") added");

    return true;
}

bool jnp1::poset_del(unsigned long id, char const *value1, char const *value2) {
    log_debug("poset_del(", id, ", \"", (is_cstring_valid(value1) ? value1 : "NULL"), "\", \"",
              (is_cstring_valid(value2) ? value2 : "NULL"), "\"", ")");

    auto poset_opt = get_poset(id);
    if (!poset_opt.has_value()) {
        log_debug("poset_del: poset ", id, " does not exist");
        return false;
    }

    if (!is_cstring_valid(value1)) {
        log_debug("poset_del: invalid value1 (NULL)");
        return false;
    }
    if (!is_cstring_valid(value2)) {
        log_debug("poset_del: invalid value2 (NULL)");
        return false;
    }

    poset_t& poset = poset_opt.value().get();
    std::string element1_name(value1), element2_name(value2);

    std::optional<element_id_t> element1_id, element2_id;
    element1_id = get_element_id_from_dictionary(poset.first, element1_name);
    element2_id = get_element_id_from_dictionary(poset.first, element2_name);

    if (!element1_id.has_value() || !element2_id.has_value()) {
        log_debug("poset_del: poset ", id, ", element \"", value1, "\" or \"", value2 , "\" does not exist");
        return false;
    }

    if (!are_elements_related(poset.second, element1_id.value(), element2_id.value()) ||
        !can_remove_relation(poset.second, element1_id.value(), element2_id.value())) {
        log_debug("poset_del: poset ", id, ", relation (\"", value1, "\", \"", value2 , "\") cannot be deleted");
        return false;
    }

    remove_relation(poset.second, element1_id.value(), true, element2_id.value(), true);
    log_debug("poset_del: poset ", id, ", relation (\"", value1, ",\" \"", value2 , "\") deleted");

    return true;
}

bool jnp1::poset_test(unsigned long id, char const *value1, char const *value2) {
    log_debug("poset_test(", id, ", \"", (is_cstring_valid(value1) ? value1 : "NULL"), "\", \"",
              (is_cstring_valid(value2) ? value2 : "NULL"), "\"", ")");

    auto poset_opt = get_poset(id);
    if (!poset_opt.has_value()) {
        log_debug("poset_test: poset ", id, " does not exist");
        return false;
    }

    if (!is_cstring_valid(value1)) {
        log_debug("poset_test: invalid value1 (NULL)");
        return false;
    }
    if (!is_cstring_valid(value2)) {
        log_debug("poset_test: invalid value2 (NULL)");
        return false;
    }

    poset_t& poset = poset_opt.value().get();
    std::string name1(value1), name2(value2);

    std::optional<element_id_t> element_id1, element_id2;
    element_id1 = get_element_id_from_dictionary(poset.first, name1);
    element_id2 = get_element_id_from_dictionary(poset.first, name2);

    if (!element_id1.has_value() || !element_id2.has_value()) {
        log_debug("poset_test: poset ", id, ", element \"", value1, "\" or \"", value2 , "\" does not exist");
        return false;
    }
    if (element_id1.value() == element_id2.value()) {
        log_debug("poset_test: poset ", id, ", element \"", value1, "\" or \"", value2 , "\" exist");
        return true;
    }

    if (!are_elements_related(poset.second, element_id1.value(), element_id2.value())) {
        log_debug("poset_test: poset ", id, ", element \"", value1, "\" or \"", value2 , "\" does not exist");
        return false;
    }
    log_debug("poset_test: poset ", id, ", element \"", value1, "\" or \"", value2 , "\" exists");

    return true;
}

void jnp1::poset_clear(unsigned long id) {
    log_debug("poset_clear(", id, ")");

    auto poset_opt = get_poset(id);
    if (!poset_opt.has_value()) {
        log_debug("poset_clear: poset ", id, " does not exist");
        return;
    }

    poset_t& poset = poset_opt.value().get();

    poset.first.clear();
    poset.second.clear();

    log_debug("poset_clear: poset ", id, " cleared");
}