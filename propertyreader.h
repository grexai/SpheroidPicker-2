#ifndef PROPERTYREADER_H
#define PROPERTYREADER_H

#include <iostream>
#include <fstream>
#include <list>
#include <map>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>
typedef struct props{
    std::string port_pipette;
    std::string port_pressurecontrooler;
    std::string model_weights;
    std::string textGraph;
    std::string classesFile;
    std::string matterport_graph;
    std::string matterport_folder;
    std::string wp_96_offset_X;
    std::string wp_96_offset_Y;
}props;

class propertyreader
{
public:
    propertyreader();
    ~propertyreader();

    void apply_settings(std::map<std::string, std::string>& settings);
    void read_settings(std::string settings_file_path,
                       std::map<std::string, std::string>& settings);
    props cfg;
};

#endif // PROPERTYREADER_H
