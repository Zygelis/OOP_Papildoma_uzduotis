#include "header.h"

string isvalom_zodi(const string& zodis) {
    string isvalytas_zodis = zodis;
    isvalytas_zodis.erase(isvalytas_zodis.begin(), find_if(isvalytas_zodis.begin(), isvalytas_zodis.end(), ::isalpha));
    isvalytas_zodis.erase(find_if(isvalytas_zodis.rbegin(), isvalytas_zodis.rend(), ::isalpha).base(), isvalytas_zodis.end());
    transform(isvalytas_zodis.begin(), isvalytas_zodis.end(), isvalytas_zodis.begin(), ::tolower);
    return isvalytas_zodis;
}

bool ar_yra_URL(const string& zodis, const unordered_set<string>& url_pabaigos) {
    regex url_struktura(R"((https?|ftp):\/\/[^\s/$.?#].[^\s]*|www\.[^\s/$.?#].[^\s]*|HTTP:\/\/[^\s/$.?#].[^\s]*)");

    // Ziurime ar zodis atitinka URL struktura
    if (regex_match(zodis, url_struktura))
        return true;

    // Jei zodis nesutampa su URL struktura, tikriname ar jis baigiasi viena is URL pabaigu
    for (const auto& ending : url_pabaigos) {
        if (zodis.size() >= ending.size() && zodis.compare(zodis.size() - ending.size(), ending.size(), ending) == 0)
            return true;
    }

    return false;
}

int main() {
    ifstream tekstas("input\\vilniaus_testas.txt");
    ofstream zodziu_danzio_failas("output\\zodziai.txt");
    ofstream url_isvedimo_failas("output\\urls.txt");
    ofstream cross_reference_failas("output\\cross_reference.txt");
    ifstream url_pabaigu_failas("input\\url_pabaigos.txt");

    if (!tekstas.is_open() || !zodziu_danzio_failas.is_open() || !url_isvedimo_failas.is_open() ||
        !cross_reference_failas.is_open() || !url_pabaigu_failas.is_open()) {
        cerr << "Klaida atidarant failus!" << endl;
        return 0;
    }

    unordered_map<string, std::pair<int, vector<int>>> zodziu_pasirodymai;
    unordered_set<string> url_pabaigos;

    string url_pabaiga;
    while (getline(url_pabaigu_failas, url_pabaiga)) {
        url_pabaigos.insert(url_pabaiga);
    }

    string eilute;
    int n_eilutes = 0;

    while (getline(tekstas, eilute)) {
        n_eilutes++;

        istringstream iss(eilute);
        string word;

        while (iss >> word) {
            string isvalytas_zodis = isvalom_zodi(word);

            if (!isvalytas_zodis.empty() && !ar_yra_URL(isvalytas_zodis, url_pabaigos)) {
                zodziu_pasirodymai[isvalytas_zodis].first++;
                zodziu_pasirodymai[isvalytas_zodis].second.push_back(n_eilutes);
            }

            if (ar_yra_URL(word, url_pabaigos)) {
                url_isvedimo_failas << "URL: " << word << endl;
            }
        }
    }

    for (const auto& entry : zodziu_pasirodymai) {
        if (entry.second.first > 1) {
            zodziu_danzio_failas << "Zodis: " << entry.first << ", Pasikartojimu skaicius: " << entry.second.first << endl;

            // Panaikinam pasikartojancius eiluciu numerius ir surusiuojam juos didejimo tvarka kad butu lengviau surasti.
            unordered_set<int> unique_eilutes(entry.second.second.begin(), entry.second.second.end());
            vector<int> surusiuotos_eilutes(unique_eilutes.begin(), unique_eilutes.end());
            std::sort(surusiuotos_eilutes.begin(), surusiuotos_eilutes.end());

            cross_reference_failas << "Zodis: " << entry.first << ", Teksto eilutes: ";
            for (const auto& eilute : surusiuotos_eilutes) {
                cross_reference_failas << eilute << " ";
            }
            cross_reference_failas << endl;
        }
    }

    tekstas.close();
    zodziu_danzio_failas.close();
    url_isvedimo_failas.close();
    cross_reference_failas.close();
    url_pabaigu_failas.close();

    return 0;
}