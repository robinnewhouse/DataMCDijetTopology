#include<TFileMerger.h>
#include<TTree.h>
#include<TString.h>
#include<iostream>

int main(int argc, char **argv)
{
    if(argc < 3) {
        std::cout << "Less than three arguments specified (expect output filename and at least one input file)." << std::endl;
        return 1;
    }

    TTree::SetMaxTreeSize(1LL*1024*1024*1024*1024); // Set max to 1TB
    TFileMerger merger(kFALSE,kFALSE); // hadd style
    merger.OutputFile(argv[1]);

    for(int i = 2; i < argc; ++i) {
        std::cout << argv[i] << std::endl;
        merger.AddFile(argv[i]);
    }
    merger.Merge();
}
