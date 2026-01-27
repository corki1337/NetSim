//szablon klasy NodeCollection
//klasa Factory
//funkcje do zapisu i odczytu struktury fabryki z pliku

#ifndef FACTORY_HPP
#define FACTORY_HPP


#include "nodes.hpp"
#include <algorithm>
#include <stdexcept>
#include <istream>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <functional>
#include <set>

#include <ostream>

enum class ElementType{

    RAMP,
    WORKER,
    STOREHOUSE,
    LINK
};




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
  
    void add_worker(Worker&& worker) {workers.add(std::move(worker));}
    void remove_worker(ElementID id) {remove_receiver(workers, id);}
    NodeCollection<Worker>::iterator find_worker_by_id(ElementID id) {return workers.find_by_id(id);}
    NodeCollection<Worker>::const_iterator find_worker_by_id(ElementID id) const {return workers.find_by_id(id);}
    NodeCollection<Worker>::const_iterator worker_cbegin() const {return workers.cbegin();}
    NodeCollection<Worker>::const_iterator worker_cend() const {return workers.cend();}
  
    void add_storehouse(Storehouse&& storehouse) {storehouses.add(std::move(storehouse));}
    void remove_storehouse(ElementID id) {remove_receiver(storehouses, id);}
    NodeCollection<Storehouse>::iterator find_storehouse_by_id(ElementID id) {return storehouses.find_by_id(id);}
    NodeCollection<Storehouse>::const_iterator find_storehouse_by_id(ElementID id) const {return storehouses.find_by_id(id);}
    NodeCollection<Storehouse>::const_iterator storehouse_cbegin() const {return storehouses.cbegin();}
    NodeCollection<Storehouse>::const_iterator storehouse_cend() const {return storehouses.cend();}
    
    bool is_consistent() const;
    void do_deliveries(Time t);
    void do_package_passing();
    void do_work(Time t);
    
private:

    template <typename Node>
    void remove_receiver(NodeCollection<Node>& collection, ElementID id);


    NodeCollection<Ramp> ramps;
    NodeCollection<Worker> workers;
    NodeCollection<Storehouse> storehouses;
};


template <typename Node>
void Factory::remove_receiver(NodeCollection<Node>& collection, ElementID id){

    typename NodeCollection<Node>::iterator iterator = collection.find_by_id(id);

    if (iterator !=collection.end()) {

        IPackageReceiver* receiver_ptr = &(*iterator);
        for( auto& ramp: ramps){
            ramp.receiver_preferences_.remove_receiver(receiver_ptr);
        }
        for( auto& worker:workers)
            {worker.receiver_preferences_.remove_receiver(receiver_ptr);            
        }
    
        collection.remove_by_id(id);
    }
}


class ParsedLineData{
public:
    ElementType element_type;
    std::map<std::string, std::string> parameters;
};


ParsedLineData parse_line(std::string line);

Factory load_factory_structure(std::istream& is);








void save_factory_structure(const Factory& factory, std::ostream& os);

void generate_structure_report(const Factory& factory, std::ostream& os);
void generate_simulation_turn_report(const Factory& factory, std::ostream& os, Time t);

class IntervalReportNotifier{
public:
    IntervalReportNotifier(TimeOffset to) : to_(to) {}
    bool should_generate_report(Time t);
private:
    TimeOffset to_;
};

class SpecificTurnsReportNotifier{
public:
    SpecificTurnsReportNotifier(std::set<Time> turns) : turns_(turns) {}
    bool should_generate_report(Time t);
private:
    std::set<Time> turns_;
};

void simulate(Factory& f, TimeOffset d, std::function<void (Factory&, Time)> rf);


#endif