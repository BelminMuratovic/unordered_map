#include "UnorderedMap.hpp"
#include <iostream>

int main()
{
    UnorderedMap<std::string, std::string> mapa;
    mapa.emplace("kljuc1", "vrijednost1");
    mapa.emplace("kljuc2", "vrijednost2");
    mapa.emplace("kljuc3", "vrijednost3");
    mapa.emplace("kljuc4", "vrijednost4");
    mapa.emplace("kljuc5", "vrijednost5");
    mapa["kljuc6"] = "vrijednost6";

    for (auto it = mapa.begin(); it != mapa.end(); ++it)
    {
        std::cout << "Key: " << it->first << ", value: " << it->second << std::endl;
    }
    mapa["kljuc1"] = "nova vrijednost1";

    auto it = mapa.find("kljuc1");
    if (it != mapa.end())
    {
        std::cout << "Element pronadjen: " << it->second << std::endl;
    }
    else
    {
        std::cout << "Element ne postoji" << std::endl;
    }

    bool erased = mapa.erase("kljuc1");

    if (erased)
    {
        std::cout << "Element je izbrisan" << std::endl;
    }

    return 0;
}