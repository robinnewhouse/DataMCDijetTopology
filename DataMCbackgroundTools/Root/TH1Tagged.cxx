#include "DataMCbackgroundTools/TH1Tagged.h"

#include <iostream>
#include <unordered_map>

TH1Tagged::TH1Tagged(
        std::string var_name_,
        float x_min_,
        float x_max_,
        float bin_spacing_)
    :
        var_name(var_name_),
        x_min(x_min_),
        x_max(x_max_),
        bin_spacing(bin_spacing_),
        num_bins( (x_max - x_min) / bin_spacing ),
        fill_veto(false)
{
    std::string name = this->var_name;
    h_inclusive = new TH1F(name.c_str(), name.c_str(), num_bins, x_min, x_max);
    h_inclusive->Sumw2();
}

TH1Tagged::~TH1Tagged(void)
{
    delete h_inclusive;
    for (auto& h : this->h_tagged)
        delete h.second;
    for (auto& h : this->h_vetoed)
        delete h.second;
}

void TH1Tagged::fill_tagged(const std::string tag_name, float val,
        float weight, bool tag_condition)
{
    if (tag_condition) {
        bool tagged_histogram_exists = h_tagged.find(tag_name) != h_tagged.end();

        if(!tagged_histogram_exists) {
            std::string name = this->var_name + "_" + tag_name;
            h_tagged.emplace(tag_name, new TH1F(name.c_str(), name.c_str(), this->num_bins, this->x_min, this->x_max));
            h_tagged.at(tag_name)->Sumw2();
        }

        h_tagged.at(tag_name)->Fill(val, weight);

    } else if (this->fill_veto) {

        bool vetoed_histogram_exists = h_vetoed.find(tag_name) != h_vetoed.end();

        if (!vetoed_histogram_exists) {
            std::string name = this->var_name + "_" + tag_name + "_VETO";
            h_vetoed.emplace(tag_name, new TH1F(name.c_str(), name.c_str(), this->num_bins, this->x_min, this->x_max));
            h_vetoed.at(tag_name)->Sumw2();
        }

        h_vetoed.at(tag_name)->Fill(val, weight);
    }
}

void TH1Tagged::write_all(void) const
{
    auto write_histogram = [](const TH1* h) {
        if (h->GetEntries() > 0) {
            h->Write();
        } else {
            std::cout << "WARNING: empty histogram (" << h->GetName() << ") not written to file." << std::endl;
        }
    };

    write_histogram(h_inclusive);

    for (auto& h : this->h_tagged)
        write_histogram(h.second);

    for (auto& h : this->h_vetoed)
        write_histogram(h.second);
}

void TH1Tagged::add(const TH1Tagged* ht)
{
    this->h_inclusive->Add(ht->h_inclusive);

    for (const auto& h : ht->h_tagged) {
        bool tagged_histogram_exists = this->h_tagged.find(h.first) != this->h_tagged.end();

        if(!tagged_histogram_exists) {
            std::string name = this->var_name + "_" + h.first;
            this->h_tagged.emplace(h.first, new TH1F(name.c_str(), name.c_str(), this->num_bins, this->x_min, this->x_max));
            this->h_tagged.at(h.first)->Sumw2();
        }

        this->h_tagged.at(h.first)->Add(h.second);
    }

    for (const auto& h : ht->h_vetoed) {
        bool vetoed_histogram_exists = this->h_vetoed.find(h.first) != this->h_vetoed.end();

        if (!vetoed_histogram_exists) {
            std::string name = this->var_name + "_" + h.first + "_VETO";
            this->h_vetoed.emplace(h.first, new TH1F(name.c_str(), name.c_str(), this->num_bins, this->x_min, this->x_max));
            this->h_vetoed.at(h.first)->Sumw2();
        }

        this->h_vetoed.at(h.first)->Add(h.second);
    }
}
