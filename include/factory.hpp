//szablon klasy NodeCollection
//klasa Factory
//funkcje do zapisu i odczytu struktury fabryki z pliku

#ifndef FACTORY_HPP
#define FACTORY_HPP


#include "nodes.hpp"


template <class Node>

class NodeCollection{
public:

    using container_t = typename std::list<Node>;
    using iterator = typename container_t::iterator;
    using const_iterator = typename container_t::const_iterator;


    const_iterator cbegin() const {return konteneruch_.cbegin();}
    const_iterator cend() const {return konteneruch_.cend();}
    const_iterator begin() const {return konteneruch_.begin();}
    const_iterator end() const {return konteneruch_.end();}
    iterator begin() {return konteneruch_.begin();}
    iterator end() {return konteneruch_.end();}

    void add(Node&& node) {konteneruch_.push_back(std::move(node));}
    void remove_by_id(ElementID id) {konteneruch_.remove_if([id](const Node& wenzel){return id == wenzel.get_id();});}
    iterator find_by_id(ElementID id) {return std::find_if(begin(), end(), [id](const Node& wenzel) {return id == wenzel.get_id();});}
    const_iterator find_by_id(ElementID id) const {return std::find_if(cbegin(), cend(), [id](const Node& wenzel) {return id == wenzel.get_id();});}





private:
    container_t konteneruch_;


};
















#endif