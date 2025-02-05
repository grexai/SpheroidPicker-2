#include "propertyreader.h"

propertyreader::propertyreader()
{
    //get settings from file
}
float f_param(std::map<std::string, std::string>& settings, std::string param)
{
    return static_cast<float>(atof(settings[param].c_str()));
    // LOCAL FUGGO 
}

int i_param(std::map<std::string, std::string>& settings, std::string param)
{
    return atoi(settings[param].c_str());
}

std::string s_param(std::map<std::string, std::string>& settings, std::string param)
{
    return settings[param];
}

template<typename Out>

void split(const std::string &s, char delim, Out result) {
    using namespace  std;
    stringstream ss;
    ss.str(s);
    string item;
    while (std::getline(ss, item, delim)) {
       *(result++) = item;
    }
}

std::vector<std::string> split(const std::string & s, char delim)
{
    using namespace std;
    vector<string> elems;
    split(s, delim, back_inserter(elems));
    return elems;
}

void propertyreader::read_settings(std::string settings_file_path,
                    std::map<std::string, std::string>& settings) {
        using namespace  std;
        std::cout << "Reading settings from file: " << settings_file_path << std::endl;

            ifstream input(settings_file_path);
            cout<< "settings_file_path" << endl;
            if (!input) {
                    throw 20;
                    cout<< "line" << endl;
            }
            std::string line;
            cout<< "line" << endl;
            while (getline(input, line)) {
                    std::vector<string> setting = split(line, '=');
                    if (setting.size() >= 2) {
                            if (setting[0][0] != '#') {
                                    settings[setting[0]] = setting[1];
                            }

                    }
            }

    }

void propertyreader::apply_settings(std::map<std::string, std::string>& settings) {
    cfg.port_pipette=s_param(settings,"ports.pipette");
    cfg.port_pressurecontrooler=s_param(settings,"ports.pressurecontroller");
    cfg.model_weights = s_param(settings,"dl.modelweights");
    cfg.textGraph = s_param(settings,"dl.textgraph");
    cfg.classesFile = s_param(settings,"dl.classes");
    cfg.matterport_graph = s_param(settings,"matterport.graph");
    cfg.matterport_folder = s_param(settings,"matterport.folder");
    cfg.wp_96_offset_X = s_param(settings,"offsets.wp96.x");
    cfg.wp_96_offset_Y = s_param(settings,"offsets.wp96.y");

}



