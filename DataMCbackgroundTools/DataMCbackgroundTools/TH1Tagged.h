#ifndef TH1Tagged_h
#define TH1Tagged_h

#include <TH1F.h>
#include <unordered_map>

class TH1Tagged {
private:
    TH1F* h_inclusive;
    std::unordered_map<std::string, TH1F*> h_tagged;
    std::unordered_map<std::string, TH1F*> h_vetoed;

public:
    TH1Tagged(std::string var_name_, float x_min_, float x_max_, float bin_spacing_);

    virtual ~TH1Tagged(void);

    const std::string var_name;
    const float x_min;
    const float x_max;
    const float bin_spacing;
    const int num_bins;

    bool fill_veto;

    void set_fill_veto(bool fill) { this->fill_veto = fill; }

    inline void fill(float val, float weight) {
        h_inclusive->Fill(val, weight);
    };

    void fill_tagged(const std::string tag_name, float val, float weight, bool tag_condition);

    void add(const TH1Tagged* ht);

    void write_all(void) const;

    ClassDef(TH1Tagged, 0);
};


#endif // #ifdef TH1Tagged_h
