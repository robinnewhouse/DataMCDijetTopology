#ifndef WeightTool_h
#define WeightTool_h

#include <unordered_map>
#include <string>
#include <vector>
#include <memory>

class WeightTool {
public:
    WeightTool();

    virtual ~WeightTool(void) { };

    bool read_weights_file(const std::string& filepath);
    bool read_bugged_events_file(const std::string& filepath);

    std::unordered_map<int, float> m_nevents_map;
    std::unordered_map<int, float> m_xsection_map;
    std::unordered_map<int, float> m_filtereff_map;
    std::unordered_map<int, std::unique_ptr<std::vector<int>>> m_bugged_events_map;

    float get_xsection(int dsid) const;
    float get_nevents(int dsid) const;
    float get_filtereff(int dsid) const;
    float get_scale_factor(int dsid) const;

    bool is_bugged_event(int dsid, int event_number) const;
};

#endif // #ifdef WeightTool_h
