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


// --- Sekcja: IO (Zapis i odczyt) ---

// Mapa pomocnicza dla parsera
const std::map<std::string, ElementType> str_to_elem_type = {
    {"LOADING_RAMP", ElementType::RAMP},
    {"WORKER", ElementType::WORKER},
    {"STOREHOUSE", ElementType::STOREHOUSE},
    {"LINK", ElementType::LINK}
};

ParsedLineData parse_line(std::string line) {
    ParsedLineData data;
    std::istringstream iss(line);
    std::string token;

    // Pobierz typ elementu (pierwsze słowo)
    iss >> token;
    auto it = str_to_elem_type.find(token);
    if (it == str_to_elem_type.end()) {
        throw std::logic_error("Unknown element type: " + token);
    }
    data.element_type = it->second;

    // Pobierz parametry klucz=wartość
    while (iss >> token) {
        auto pos = token.find('=');
        if (pos != std::string::npos) {
            std::string key = token.substr(0, pos);
            std::string value = token.substr(pos + 1);
            data.parameters[key] = value;
        }
    }
    return data;
}

Factory load_factory_structure(std::istream& is) {
    Factory factory;
    std::string line;

    while (std::getline(is, line)) {
        // Pomiń puste linie i komentarze
        if (line.empty() || line[0] == ';' || line[0] == '#') continue;

        ParsedLineData p = parse_line(line);

        if (p.element_type == ElementType::RAMP) {
            factory.add_ramp(Ramp(
                std::stoi(p.parameters["id"]),
                std::stoi(p.parameters["delivery-interval"])
            ));
        } 
        else if (p.element_type == ElementType::WORKER) {
            PackageQueueType qt = PackageQueueType::FIFO;
            if (p.parameters["queue-type"] == "LIFO") {
                qt = PackageQueueType::LIFO;
            }
            std::unique_ptr<IPackageQueue> q = std::make_unique<PackageQueue>(qt);
            
            factory.add_worker(Worker(
                std::stoi(p.parameters["id"]),
                std::stoi(p.parameters["processing-time"]),
                std::move(q)
            ));
        } 
        else if (p.element_type == ElementType::STOREHOUSE) {
            factory.add_storehouse(Storehouse(
                std::stoi(p.parameters["id"])
            ));
        } 
        else if (p.element_type == ElementType::LINK) {
            // Format: src=ramp-1 dest=worker-1
            std::string src_full = p.parameters["src"];
            std::string dest_full = p.parameters["dest"];

            // Lambda do rozbijania "typ-id"
            auto parse_link_token = [](const std::string& s) {
                auto dash = s.find('-');
                return std::make_pair(s.substr(0, dash), std::stoi(s.substr(dash + 1)));
            };

            auto src = parse_link_token(src_full);
            auto dest = parse_link_token(dest_full);

            PackageSender* sender = nullptr;
            IPackageReceiver* receiver = nullptr;

            // Znajdź nadawcę
            if (src.first == "ramp") {
                auto it = factory.find_ramp_by_id(src.second);
                if (it != factory.ramp_cend()) sender = &(*it);
            } else if (src.first == "worker") {
                auto it = factory.find_worker_by_id(src.second);
                if (it != factory.worker_cend()) sender = &(*it);
            }

            // Znajdź odbiorcę
            if (dest.first == "worker") {
                auto it = factory.find_worker_by_id(dest.second);
                if (it != factory.worker_cend()) receiver = &(*it);
            } else if (dest.first == "storehouse") {
                auto it = factory.find_storehouse_by_id(dest.second);
                if (it != factory.storehouse_cend()) receiver = &(*it);
            }

            // Jeśli znaleziono obu, połącz ich
            if (sender && receiver) {
                sender->receiver_preferences_.add_receiver(receiver);
            }
        }
    }
    return factory;
}

void save_factory_structure(const Factory& factory, std::ostream& os) {
    // 1. Zapis Ramp
    for (auto it = factory.ramp_cbegin(); it != factory.ramp_cend(); ++it) {
        os << "LOADING_RAMP id=" << it->get_id() 
           << " delivery-interval=" << it->get_delivery_interval() << "\n";
    }

    // 2. Zapis Robotników
    for (auto it = factory.worker_cbegin(); it != factory.worker_cend(); ++it) {
        std::string q_type = (it->get_queue()->get_queue_type() == PackageQueueType::LIFO) ? "LIFO" : "FIFO";
        os << "WORKER id=" << it->get_id() 
           << " processing-time=" << it->get_processing_time() 
           << " queue-type=" << q_type << "\n";
    }

    // 3. Zapis Magazynów
    for (auto it = factory.storehouse_cbegin(); it != factory.storehouse_cend(); ++it) {
        os << "STOREHOUSE id=" << it->get_id() << "\n";
    }

    // 4. Zapis Linków (Połączeń)
    auto save_links = [&](const PackageSender& sender, std::string src_prefix) {
        for (const auto& pref : sender.receiver_preferences_.get_preferences()) {
            IPackageReceiver* recv = pref.first;
            std::string dest_prefix;
            
            if (recv->get_receiver_type() == ReceiverType::WORKER) dest_prefix = "worker";
            else dest_prefix = "storehouse";

            os << "LINK src=" << src_prefix << "-" << sender.get_id() 
               << " dest=" << dest_prefix << "-" << recv->get_id() << "\n";
        }
    };

    // Generowanie linków dla wszystkich nadawców
    for (auto it = factory.ramp_cbegin(); it != factory.ramp_cend(); ++it) save_links(*it, "ramp");
    for (auto it = factory.worker_cbegin(); it != factory.worker_cend(); ++it) save_links(*it, "worker");

    os.flush();
}