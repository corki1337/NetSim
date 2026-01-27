#include "factory.hpp"

bool has_reachable_storehouse(const PackageSender* sender, std::map<const PackageSender*, NodeColor>& node_colors) {

    if (node_colors[sender] == NodeColor::VERIFIED) {
        return true;
    }    
    node_colors[sender] = NodeColor::VISITED; 

    if (sender->receiver_preferences_.get_preferences().empty()) {
        throw std::logic_error("No defined receivers");
    }
    bool has_other_receiver = false;

    for(const auto& receiver: sender->receiver_preferences_.get_preferences()){
    
        if(receiver.first->get_receiver_type() == ReceiverType::STOREHOUSE){
            has_other_receiver = true;
        }
        
        else if(receiver.first->get_receiver_type() == ReceiverType::WORKER){
            
            IPackageReceiver* receiver_ptr = receiver.first;
            auto worker_ptr = dynamic_cast<Worker*>(receiver_ptr);
            auto sendrecv_ptr =dynamic_cast<PackageSender*>(worker_ptr);

            if (worker_ptr == sender){
                continue;
            }
            has_other_receiver = true;

            if(node_colors[sendrecv_ptr] == NodeColor::UNVISITED){
                has_reachable_storehouse(sendrecv_ptr, node_colors);
            }
        }   
    }
    node_colors[sender] = NodeColor::VERIFIED;

    if (has_other_receiver) {
        return true;
    } 
    else {
        throw std::logic_error("No reachable magazine");
    }
}  

bool Factory::is_consistent() const {
    std::map<const PackageSender*, NodeColor> node_colors;

    for (const auto& ramp : ramps) {
        node_colors[&ramp] = NodeColor::UNVISITED;
    }
    for (const auto& worker : workers){
        node_colors[&worker] = NodeColor::UNVISITED;
    }

    try{        
        for (const auto& ramp : ramps) {
            has_reachable_storehouse(&ramp, node_colors);
        }
    }
    catch (const std::logic_error&) {
        return false;
    }

    return true;
}

void Factory::do_deliveries(Time t){
    for (auto& ramp: ramps){
        ramp.deliver_goods(t);
    }
}

void Factory::do_package_passing() {
    for (auto& ramp: ramps){
        ramp.send_package();
    }

    for (auto& worker: workers){
        worker.send_package();
    }
}

void Factory::do_work(Time t){
    for (auto& worker: workers){
        worker.do_work(t);
    }
}

ParsedLineData parse_line(std::string line) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream token_stream(line);
    char delimiter = ' ';

    while (std::getline(token_stream, token, delimiter)) {
        tokens.push_back(token);
    }

    ParsedLineData data;
    if (tokens.empty()) {
        throw std::invalid_argument("Empty line");
    }

    std::string tag = tokens[0];

    if (tag == "LOADING_RAMP") {
        data.element_type = ElementType::RAMP;
    } else if (tag == "WORKER") {
        data.element_type = ElementType::WORKER;
    } else if (tag == "STOREHOUSE") {
        data.element_type = ElementType::STOREHOUSE;
    } else if (tag == "LINK") {
        data.element_type = ElementType::LINK;
    } else {
        throw std::invalid_argument("Unknown tag: " + tag);
    }

    for (size_t i = 1; i < tokens.size(); ++i) {
        const std::string& t = tokens[i];
        size_t eq_pos = t.find('=');
        if (eq_pos != std::string::npos) {
            std::string key = t.substr(0, eq_pos);
            std::string value = t.substr(eq_pos + 1);
            data.parameters[key] = value;
        }
    }

    return data;
}

Factory load_factory_structure(std::istream& is) {
    Factory factory;
    std::string line;

    while (std::getline(is, line)) {
        if (line.empty() || line[0] == ';') {
            continue;
        }

        ParsedLineData data = parse_line(line);

        if (data.element_type == ElementType::RAMP) {
            ElementID id = std::stoi(data.parameters["id"]);
            TimeOffset di = std::stoi(data.parameters["delivery-interval"]);
            factory.add_ramp(Ramp(id, di));
        } 
        else if (data.element_type == ElementType::WORKER) {
            ElementID id = std::stoi(data.parameters["id"]);
            TimeOffset pt = std::stoi(data.parameters["processing-time"]);
            std::string qt_str = data.parameters["queue-type"];
            PackageQueueType qt = (qt_str == "LIFO") ? PackageQueueType::LIFO : PackageQueueType::FIFO;
            factory.add_worker(Worker(id, pt, std::make_unique<PackageQueue>(qt)));
        } 
        else if (data.element_type == ElementType::STOREHOUSE) {
            ElementID id = std::stoi(data.parameters["id"]);
            factory.add_storehouse(Storehouse(id));
        } 
        else if (data.element_type == ElementType::LINK) {
            std::string src_str = data.parameters["src"];
            std::string dest_str = data.parameters["dest"];

            std::string src_type_str = src_str.substr(0, src_str.find('-'));
            ElementID src_id = std::stoi(src_str.substr(src_str.find('-') + 1));

            std::string dest_type_str = dest_str.substr(0, dest_str.find('-'));
            ElementID dest_id = std::stoi(dest_str.substr(dest_str.find('-') + 1));

            IPackageReceiver* receiver = nullptr;
            if (dest_type_str == "worker") {
                auto it = factory.find_worker_by_id(dest_id);
                if (it != factory.worker_cend()) {
                    receiver = &(*it);
                }
            } else if (dest_type_str == "store") {
                auto it = factory.find_storehouse_by_id(dest_id);
                if (it != factory.storehouse_cend()) {
                    receiver = &(*it);
                }
            }

            if (receiver) {
                if (src_type_str == "ramp") {
                    auto it = factory.find_ramp_by_id(src_id);
                    if (it != factory.ramp_cend()) {
                        it->receiver_preferences_.add_receiver(receiver);
                    }
                } else if (src_type_str == "worker") {
                    auto it = factory.find_worker_by_id(src_id);
                    if (it != factory.worker_cend()) {
                        it->receiver_preferences_.add_receiver(receiver);
                    }
                }
            }
        }
    }
    return factory;
}

void save_factory_structure(const Factory& factory, std::ostream& os) {
    for (auto it = factory.ramp_cbegin(); it != factory.ramp_cend(); ++it) {
        os << "LOADING_RAMP id=" << it->get_id() << " delivery-interval=" << it->get_delivery_interval() << "\n";
    }

    for (auto it = factory.worker_cbegin(); it != factory.worker_cend(); ++it) {
        os << "WORKER id=" << it->get_id() << " processing-time=" << it->get_processing_duration() << " queue-type=";
        if (it->get_queue()->get_queue_type() == PackageQueueType::LIFO) {
            os << "LIFO";
        } else {
            os << "FIFO";
        }
        os << "\n";
    }

    for (auto it = factory.storehouse_cbegin(); it != factory.storehouse_cend(); ++it) {
        os << "STOREHOUSE id=" << it->get_id() << "\n";
    }

    auto sort_receivers = [](const auto& a, const auto& b) {
        IPackageReceiver* r1 = a.first;
        IPackageReceiver* r2 = b.first;
        if (r1->get_receiver_type() != r2->get_receiver_type()) {
            return r1->get_receiver_type() == ReceiverType::STOREHOUSE; 
        }
        return r1->get_id() < r2->get_id();
    };

    for (auto it = factory.ramp_cbegin(); it != factory.ramp_cend(); ++it) {
        auto prefs = it->receiver_preferences_.get_preferences();
        std::vector<std::pair<IPackageReceiver*, double>> sorted_receivers(prefs.begin(), prefs.end());
        std::sort(sorted_receivers.begin(), sorted_receivers.end(), sort_receivers);

        for (const auto& [receiver, prob] : sorted_receivers) {
            os << "LINK src=ramp-" << it->get_id() << " dest=";
            if (receiver->get_receiver_type() == ReceiverType::WORKER) {
                os << "worker-" << receiver->get_id();
            } else {
                os << "store-" << receiver->get_id();
            }
            os << "\n";
        }
    }

    for (auto it = factory.worker_cbegin(); it != factory.worker_cend(); ++it) {
        auto prefs = it->receiver_preferences_.get_preferences();
        std::vector<std::pair<IPackageReceiver*, double>> sorted_receivers(prefs.begin(), prefs.end());
        std::sort(sorted_receivers.begin(), sorted_receivers.end(), sort_receivers);

        for (const auto& [receiver, prob] : sorted_receivers) {
            os << "LINK src=worker-" << it->get_id() << " dest=";
            if (receiver->get_receiver_type() == ReceiverType::WORKER) {
                os << "worker-" << receiver->get_id();
            } else {
                os << "store-" << receiver->get_id();
            }
            os << "\n";
        }
    }
}

void generate_structure_report(const Factory& factory, std::ostream& os) {
    os << "\n== LOADING RAMPS ==\n\n";

    std::vector<const Ramp*> sorted_ramps;
    for (auto it = factory.ramp_cbegin(); it != factory.ramp_cend(); ++it) {
        sorted_ramps.push_back(&(*it));
    }
    std::sort(sorted_ramps.begin(), sorted_ramps.end(), [](const Ramp* a, const Ramp* b) {
        return a->get_id() < b->get_id();
    });

    for (const auto* ramp : sorted_ramps) {
        os << "LOADING RAMP #" << ramp->get_id() << "\n";
        os << "  Delivery interval: " << ramp->get_delivery_interval() << "\n";
        os << "  Receivers:\n";

        auto prefs = ramp->receiver_preferences_.get_preferences();
        std::vector<IPackageReceiver*> receivers;
        for (const auto& pair : prefs) {
            receivers.push_back(pair.first);
        }
        std::sort(receivers.begin(), receivers.end(), [](IPackageReceiver* a, IPackageReceiver* b) {
            if (a->get_receiver_type() != b->get_receiver_type()) {
                return a->get_receiver_type() == ReceiverType::STOREHOUSE;
            }
            return a->get_id() < b->get_id();
        });

        for (const auto* recv : receivers) {
            os << "    ";
            if (recv->get_receiver_type() == ReceiverType::WORKER) {
                os << "worker #" << recv->get_id();
            } else {
                os << "storehouse #" << recv->get_id();
            }
            os << "\n";
        }
        os << "\n";
    }

    os << "\n== WORKERS ==\n\n";

    std::vector<const Worker*> sorted_workers;
    for (auto it = factory.worker_cbegin(); it != factory.worker_cend(); ++it) {
        sorted_workers.push_back(&(*it));
    }
    std::sort(sorted_workers.begin(), sorted_workers.end(), [](const Worker* a, const Worker* b) {
        return a->get_id() < b->get_id();
    });

    for (const auto* worker : sorted_workers) {
        os << "WORKER #" << worker->get_id() << "\n";
        os << "  Processing time: " << worker->get_processing_duration() << "\n";
        os << "  Queue type: " << (worker->get_queue()->get_queue_type() == PackageQueueType::LIFO ? "LIFO" : "FIFO") << "\n";
        os << "  Receivers:\n";

        auto prefs = worker->receiver_preferences_.get_preferences();
        std::vector<IPackageReceiver*> receivers;
        for (const auto& pair : prefs) {
            receivers.push_back(pair.first);
        }
        std::sort(receivers.begin(), receivers.end(), [](IPackageReceiver* a, IPackageReceiver* b) {
            if (a->get_receiver_type() != b->get_receiver_type()) {
                return a->get_receiver_type() == ReceiverType::STOREHOUSE;
            }
            return a->get_id() < b->get_id();
        });

        for (const auto* recv : receivers) {
            os << "    ";
            if (recv->get_receiver_type() == ReceiverType::WORKER) {
                os << "worker #" << recv->get_id();
            } else {
                os << "storehouse #" << recv->get_id();
            }
            os << "\n";
        }
        os << "\n";
    }

    os << "\n== STOREHOUSES ==\n\n";

    std::vector<const Storehouse*> sorted_storehouses;
    for (auto it = factory.storehouse_cbegin(); it != factory.storehouse_cend(); ++it) {
        sorted_storehouses.push_back(&(*it));
    }
    std::sort(sorted_storehouses.begin(), sorted_storehouses.end(), [](const Storehouse* a, const Storehouse* b) {
        return a->get_id() < b->get_id();
    });

    for (const auto* store : sorted_storehouses) {
        os << "STOREHOUSE #" << store->get_id() << "\n\n";
    }
}

void generate_simulation_turn_report(const Factory& factory, std::ostream& os, Time t) {
    os << "=== [ Turn: " << t << " ] ===\n\n";
    os << "== WORKERS ==\n\n";

    std::vector<const Worker*> sorted_workers;
    for (auto it = factory.worker_cbegin(); it != factory.worker_cend(); ++it) {
        sorted_workers.push_back(&(*it));
    }
    std::sort(sorted_workers.begin(), sorted_workers.end(), [](const Worker* a, const Worker* b) {
        return a->get_id() < b->get_id();
    });

    for (const auto* worker : sorted_workers) {
        os << "WORKER #" << worker->get_id() << "\n";

        os << "  PBuffer: ";
        const auto& pbuffer = worker->get_processing_buffer();
        if (pbuffer.has_value()) {
            os << "#" << pbuffer->get_id() << " (pt = " << (t - worker->get_package_processing_start_time() + 1) << ")";
        } else {
            os << "(empty)";
        }
        os << "\n";

        os << "  Queue: ";
        auto it_queue = worker->cbegin();
        if (it_queue == worker->cend()) {
            os << "(empty)";
        } else {
            bool first = true;
            for (; it_queue != worker->cend(); ++it_queue) {
                if (!first) os << ", ";
                os << "#" << it_queue->get_id();
                first = false;
            }
        }
        os << "\n";

        os << "  SBuffer: ";
        const auto& sbuffer = worker->get_sending_buffer();
        if (sbuffer.has_value()) {
            os << "#" << sbuffer->get_id();
        } else {
            os << "(empty)";
        }
        os << "\n\n";
    }

    os << "== STOREHOUSES ==\n\n";

    std::vector<const Storehouse*> sorted_storehouses;
    for (auto it = factory.storehouse_cbegin(); it != factory.storehouse_cend(); ++it) {
        sorted_storehouses.push_back(&(*it));
    }
    std::sort(sorted_storehouses.begin(), sorted_storehouses.end(), [](const Storehouse* a, const Storehouse* b) {
        return a->get_id() < b->get_id();
    });

    for (const auto* store : sorted_storehouses) {
        os << "STOREHOUSE #" << store->get_id() << "\n";
        os << "  Stock: ";
        auto it_stock = store->cbegin();
        if (it_stock == store->cend()) {
            os << "(empty)";
        } else {
            bool first = true;
            for (; it_stock != store->cend(); ++it_stock) {
                if (!first) os << ", ";
                os << "#" << it_stock->get_id();
                first = false;
            }
        }
        os << "\n\n";
    }
}


bool IntervalReportNotifier::should_generate_report(Time t){
    return (t - 1) % to_ == 0;
}

bool SpecificTurnsReportNotifier::should_generate_report(Time t){
    return turns_.find(t) != turns_.end();
}

void simulate(Factory& f, TimeOffset d, std::function<void (Factory&, Time)> rf){
    if(!f.is_consistent()){
        throw std::logic_error("Network is inconsistent");
    }
    for(Time t = 1; t <= d; ++t){
        f.do_deliveries(t);
        f.do_package_passing();
        f.do_work(t);
        rf(f, t);
    }
}