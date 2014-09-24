#include "../common/common.h"

#include "../buildin/rapidxml.hpp"
#include "../buildin/rapidxml_print.hpp"

#include "xml_config.h"

namespace ygopro
{

	bool CommonConfig::LoadConfig(const std::wstring& name) {
        TextFile f(To<std::string>(name));
        rapidxml::xml_document<> doc;
        doc.parse<0>(f.Data());
        rapidxml::xml_node<>* root = doc.first_node();
        rapidxml::xml_node<>* config_node = root->first_node();
        while(config_node) {
            std::string config_name = config_node->name();
            rapidxml::xml_attribute<>* key_attr = config_node->first_attribute();
            rapidxml::xml_attribute<>* value_attr = key_attr->next_attribute();
            if(config_name == "integer") {
                std::string key = key_attr->value();
                long val = To<long>(value_attr->value());
                config_map[key] = val;
            } else if(config_name == "float") {
                std::string key = key_attr->value();
                double val = To<double>(value_attr->value());
                config_map[key] = val;
            } else if(config_name == "string") {
                std::string key = key_attr->value();
                std::wstring val = To<std::wstring>(std::string(value_attr->value()));
                config_map[key] = val;
            }
            config_node = config_node->next_sibling();
        }
        return true;
	}

	void CommonConfig::SaveConfig(const std::wstring& name) {
		if(config_map.size() == 0)
			return;
        rapidxml::xml_document<> doc;
        rapidxml::xml_node<>* rot = doc.allocate_node(rapidxml::node_pi, doc.allocate_string("xml version='1.0' encoding='utf-8'"));
        doc.append_node(rot);
        rapidxml::xml_node<>* root = doc.allocate_node(rapidxml::node_element, "Config", nullptr);
        for(auto& iter : config_map) {
            rapidxml::xml_node<>* config_node = nullptr;
            if(iter.second.val_type == 0) {
                config_node = doc.allocate_node(rapidxml::node_element, "integer", nullptr);
                config_node->append_attribute(doc.allocate_attribute("name", iter.first.c_str()));
                config_node->append_attribute(doc.allocate_attribute("value", To<std::string>((long)iter.second).c_str()));
            } else if(iter.second.val_type == 1) {
                config_node = doc.allocate_node(rapidxml::node_element, "float", nullptr);
                config_node->append_attribute(doc.allocate_attribute("name", iter.first.c_str()));
                config_node->append_attribute(doc.allocate_attribute("value", To<std::string>((double)iter.second).c_str()));
            } else {
                config_node = doc.allocate_node(rapidxml::node_element, "string", nullptr);
                config_node->append_attribute(doc.allocate_attribute("name", iter.first.c_str()));
                config_node->append_attribute(doc.allocate_attribute("value", To<std::string>((std::wstring&)iter.second).c_str()));
            }
            root->append_node(config_node);
        }
        std::ofstream out(To<std::string>(name).c_str());
        out << doc;
	}

}
