#include "DataMCbackgroundTools/WeightTool.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

WeightTool::WeightTool(void) { }

bool
WeightTool::read_weights_file(const std::string& filepath)
{
	std::ifstream samples_file(filepath, std::ifstream::in);

	if (!samples_file.is_open()) {
		std::cout << "failed to open sample xsection file: " << filepath << std::endl;
		return false;
	}

	std::string line;
	int dsid;
	float nevents, xsection, filtereff;

	while (std::getline(samples_file, line))
	{
		if (line[0] != '#') {
			std::istringstream iss(line);
			if (!(iss >> dsid >> nevents >> xsection >> filtereff)) {
				std::cout << "failed to parse line: " << line << std::endl;
				m_nevents_map.clear();
				m_xsection_map.clear();
				m_filtereff_map.clear();
				return false;
			} else {
				m_nevents_map[dsid]   = nevents;
				m_xsection_map[dsid]  = xsection;
				m_filtereff_map[dsid] = filtereff;
			}
		}
	}

	return true;
}

bool
WeightTool::read_bugged_events_file(const std::string& filepath)
{
    std::ifstream bugs_file(filepath, std::ifstream::in);

    if (!bugs_file.is_open()) {
        std::cout << "failed to open bugged events file: " << filepath << std::endl;
        return false;
    }

    std::string line;
    int dsid, event_number;

    while (std::getline(bugs_file, line))
    {
        if (line[0] != '#') {
            std::istringstream iss(line);
            if (!(iss >> dsid >> event_number)) {
                std::cout << "failed to parse line: " << line << std::endl;
                m_bugged_events_map.clear();
                return false;
            } else {
                bool dsid_exists = m_bugged_events_map.find(dsid)
                    != m_bugged_events_map.end();

                if (!dsid_exists)
                    m_bugged_events_map[dsid] = std::unique_ptr<std::vector<int>>(
                            new std::vector<int>(0)
                            );

                m_bugged_events_map[dsid]->push_back(event_number);
            }
        } // end of processing single line
    } // end of reading all lines

    return true;
}

bool
WeightTool::is_bugged_event(int dsid, int event_number) const
{
    bool event_bugged = false;

    bool dsid_exists = m_bugged_events_map.find(dsid)
        != m_bugged_events_map.end();

    if (dsid_exists) {
        std::vector<int>* bugged_events = m_bugged_events_map.at(dsid).get();

        event_bugged = std::find(bugged_events->begin(),
                bugged_events->end(), event_number)
            != bugged_events->end();
    }

    return event_bugged;
}

float
WeightTool::get_nevents(int dsid) const
{
	auto it = m_nevents_map.find(dsid);

	if (it != m_nevents_map.end()) {
		return it->second;
	} else {
		return -1;
	}
}

float
WeightTool::get_xsection(int dsid) const
{
	auto it = m_xsection_map.find(dsid);

	if (it != m_xsection_map.end()) {
		return it->second;
	} else {
		return -1;
	}
}

float
WeightTool::get_filtereff(int dsid) const
{
	auto it = m_filtereff_map.find(dsid);

	if (it != m_filtereff_map.end()) {
		return it->second;
	} else {
		return -1;
	}
}

