//szablon klasy NodeCollection
//klasa Factory
//funkcje do zapisu i odczytu struktury fabryki z pliku

#ifndef FACTORY_HPP
#define FACTORY_HPP


#include "nodes.hpp"
#include <algorithm>

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

class Factory{
public:

    void add_ramp(Ramp&& rampa) {ramps.add(std::move(rampa));}
    void remove_ramp(ElementID id) {ramps.remove_by_id(id);}
    NodeCollection<Ramp>::iterator find_ramp_by_id(ElementID id) {return ramps.find_by_id(id);}
    NodeCollection<Ramp>::const_iterator find_ramp_by_id(ElementID id) const {return ramps.find_by_id(id);}
    NodeCollection<Ramp>::const_iterator ramp_cbegin() const {return ramps.cbegin();}
    NodeCollection<Ramp>::const_iterator ramp_cend() const {return ramps.cend();}
  
    //TODO Dla workerów i storehouses jak wyzej
    //TODO Pozostale funkcje takie jak:
    //is_consistent(void) : bool {query}
    //do_deliveries(Time) : void
    //do_package_passing(void) : void
    //do_work(Time) : void
private:

    template <typename Node>
    void remove_receiver(NodeCollection<Node>& collection, ElementID id);


    NodeCollection<Ramp> ramps;
    NodeCollection<Worker> workers;
    NodeCollection<Storehouse> storehouses;
};














#endif