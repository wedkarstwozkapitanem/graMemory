#include<algorithm>
#include<chrono>
#include<iostream>
#include<limits>
#include<locale.h>
#include<memory>
#include<random>
#include<string>
#include<thread>
#include<vector>
#include<fstream>
#include<array>
#include<map>


class graMemory;


int main() {
    setlocale(LC_CTYPE, "Polish");
    std::unique_ptr<graMemory> gra = std::make_unique<graMemory>();
    return EXIT_SUCCESS;
}


class graMemory {
public:
    graMemory() {
        this->rozgrywka();
    }

private:
    struct stanKarty {
        int ktory{};
        bool czyOdsloniety{ false };
    };

    const inline auto rysujKarty(const int& ileKart, const int& rozmiar, const std::string& ciog_znakow) const {
        std::ofstream plik("talia_kart_memory.txt");

        std::vector<std::vector<std::vector<char>>> karty(ileKart, std::vector<std::vector<char>>(rozmiar, std::vector<char>(rozmiar)));
        if (ciog_znakow.size() <= 1) return karty;
        std::random_device generator;
        std::mt19937 gen(generator());
        std::uniform_int_distribution<> losowa(0, ciog_znakow.size() - 1);
        for (size_t p{}; p < ileKart; ++p) {
            for (size_t x{ 0 }; x < rozmiar; ++x) {
                for (size_t y{ 0 }; y < rozmiar; ++y) {
                    karty[p][x][y] = ciog_znakow[losowa(gen)];
                    plik << karty[p][x][y];
                } plik << '\n';
            } plik << '\n';
        }
        plik.close();
        return karty;
    }
    void nowaGra(std::string* pseudomin, long long& rozmiar, std::string& ciog_znakow, long long& ileKart) {
        std::cout << reset << "Nazwa gracza 1: ";
        while (!(std::cin >> pseudomin[0])) {
            if (std::cin.fail()) oczyscBledy();
            std::cerr << czerwony << "Wprowadż poprawną nazwę jeszcze raz: " << reset;
        }
        std::cout << "Nazwa gracza 2: ";
        while (!(std::cin >> pseudomin[1])) {
            if (std::cin.fail()) oczyscBledy();
            std::cerr << czerwony << "Wprowadż poprawną nazwę jeszcze raz: " << reset;
        }
        std::cout << "Jakiego rozmiaru mają być karty?(nxn): ";
        while (!(std::cin >> rozmiar) || rozmiar < 3 || rozmiar > 80) {
            if (std::cin.fail()) oczyscBledy();
            std::cerr << czerwony << "Wprowadż poprawną liczbe oznaczająco rozmiar 1 karty który ma musi być większy od 2 i mniejszy od 80 jeszcze raz: " << reset;
        }
        std::cout << "Jakie ciog_znakowi mają utworzyć karte?np.abc: ";
        while (!(std::cin >> ciog_znakow) || ciog_znakow.size() <= 1) {
            if (std::cin.fail()) oczyscBledy();
            std::cerr << czerwony << "Wprowadż ciąg znaków większy od 1: " << reset;
        }
        std::cout << "Ile wylosować kart?: ";
        while (!(std::cin >> ileKart)) {
            if (std::cin.fail()) oczyscBledy();
            std::cerr << czerwony << "Wprowadż poprawną liczbe oznaczająco liczbę kart jeszcze raz: " << reset;
        }
    }
    const void rozgrywka() {
        long long punkty[2] = { 0,0 }, rozmiar{ 8 }, ileKart{ 8 }, ileOdkrytych{};
        std::string ciog_znakow{ "pixel" };
        auto karty = this->rysujKarty(ileKart, 0, " ");
        std::vector<stanKarty> kolejnosc;
        bool tura{ 0 };

        std::cout << zielony << "MEMORY: \n1)Nowa gra\n2)Wczytaj ostatnio zapisano gre\n3)Domyślna gra\nWybierz opcje (1-3): ";  short ustawienia{};
        while (!(std::cin >> ustawienia) || ustawienia < 1 || ustawienia > 3) {
            if (std::cin.fail()) oczyscBledy();
            std::cerr << czerwony << "Liczba musi być z zakresu 1-3.Wprowadż jeszcze raz: ";
        };

        for (char litera = 'a'; litera <= 'z'; ++litera) koloryLitery[litera] = kolory[litera % kolory.size()];



        std::string pseudomin[2] = { "nr.1","nr.2" };
        switch (ustawienia) {
        case 1:
            nowaGra(pseudomin, rozmiar, ciog_znakow, ileKart);
            break;
        case 2:
            if (!odczytajGre(ileKart, rozmiar, ciog_znakow, tura, ileOdkrytych, punkty, kolejnosc, karty, pseudomin)) nowaGra(pseudomin, rozmiar, ciog_znakow, ileKart);
            break;
        }

        karty = this->rysujKarty(ileKart, rozmiar, ciog_znakow);

        while (ciog_znakow.size() <= 1) {
            std::cout << "Wprowadż ciog_znakowi do utworzenia kart: "; std::cin >> ciog_znakow;
            karty = this->rysujKarty(ileKart, rozmiar, ciog_znakow);
        }
        if (!karty.size()) {
            std::cerr << "Brak kart";
            return;
        }

        if (kolejnosc.size() <= 1) kolejnosc.resize(2 * ileKart);


        if (ustawienia != 2) {
            for (size_t i{}; i < ileKart; ++i) {
                kolejnosc[i].ktory = i;
                kolejnosc[i + ileKart].ktory = i;
            }
            std::random_device generator;
            std::mt19937 los(generator());
            std::shuffle(kolejnosc.begin(), kolejnosc.end(), los);
        }

        zapiszGre(ileKart, rozmiar, ciog_znakow, tura, ileOdkrytych, punkty, kolejnosc, karty, pseudomin);
        while (ileOdkrytych != ileKart) {
            this->narysujodnowa(tura, karty, kolejnosc, punkty, pseudomin);

            std::cout << niebieski << "\n\nWprowadz 1 numer karty do odkrycia ";
            int numer1{};
            while (numer1 = this->pobierz(2 * ileKart)) {
                if (!kolejnosc[numer1 - 1].czyOdsloniety) {
                    kolejnosc[numer1 - 1].czyOdsloniety = true;
                    this->narysujodnowa(tura, karty, kolejnosc, punkty, pseudomin);
                    break;
                }
                else std::cout << czerwony << "To kartę już odsłoniłeś! Jeszcze raz";
            }
            std::cout << niebieski << "\n\nWprowadz 2 numer karty do odkrycia ";
            int numer2{};

            while (numer2 = this->pobierz(2 * ileKart)) {
                if (!kolejnosc[numer2 - 1].czyOdsloniety) {
                    kolejnosc[numer2 - 1].czyOdsloniety = true;
                    this->narysujodnowa(tura, karty, kolejnosc, punkty, pseudomin);
                    break;
                }
                else std::cout << czerwony << "To kartę już odsłoniłeś! Jeszcze raz";
            }

            kolejnosc[numer2 - 1].czyOdsloniety = true;
            this->narysujodnowa(tura, karty, kolejnosc, punkty, pseudomin);


            if (kolejnosc[numer1 - 1].ktory == kolejnosc[numer2 - 1].ktory) {
                std::cout << zielony << "Para kart!\n" << reset;
                ++ileOdkrytych;
                ++punkty[tura];
            }
            else {
                std::cout << czerwony << "\n\nNie pasuje!\n";

                for (int i = 3; i > 0; --i) {
                    std::cout << "Czas do zakrycia kart: " << i << " sekundy" << std::flush;
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                    std::cout << "\r";
                }
                std::cout << reset;
                kolejnosc[numer1 - 1].czyOdsloniety = false;
                kolejnosc[numer2 - 1].czyOdsloniety = false;
                tura = !tura;
            }
            zapiszGre(ileKart, rozmiar, ciog_znakow, tura, ileOdkrytych, punkty, kolejnosc, karty, pseudomin);
        }

        this->narysujodnowa(tura, karty, kolejnosc, punkty, pseudomin);
        punkty[0] > punkty[1] ? std::cout << "WYGRYWA " << pseudomin[0] : punkty[0] == punkty[1] ? std::cout << "REMIS" : std::cout << "WYGRYWA GRACZ " << pseudomin[1];
    }

    const inline void narysujodnowa(const bool& tura, const std::vector<std::vector<std::vector<char>>>& karty, const std::vector<stanKarty>& kolejnosc, const long long* punkty, const std::string* pseudomin) const {
        oczyscKonsole();
        std::cout << zolty;
        this->wyswietlkarty(karty, kolejnosc);
        std::cout << zielony << "\033[36mPunkty gracza " << pseudomin[0] << ": " << punkty[0] << " | Punkty gracza " << pseudomin[1] << ": " << punkty[1] << "\nTura gracza: " << pseudomin[tura] << reset;
    }

    const inline void wyswietlkarty(const std::vector<std::vector<std::vector<char>>>& karty, const std::vector<stanKarty>& kolejnosc) const {
        const size_t rozmiar = karty[0].size();
        const size_t szerokosc_konsoli = 94;

        const size_t szerokosc_karty = rozmiar + 2 + 1;

        const size_t limit_na_rzad = szerokosc_konsoli / szerokosc_karty;

        size_t liczbawyswietlonych{};
        while (liczbawyswietlonych < kolejnosc.size()) {
            for (size_t i{}; i <= rozmiar + 1; ++i) {
                size_t karty_w_rzedzie = 0;
                for (size_t p = liczbawyswietlonych; p < kolejnosc.size() && karty_w_rzedzie < limit_na_rzad; ++p) {
                    ++karty_w_rzedzie;
                    for (size_t j{}; j <= rozmiar + 1; ++j) {
                        if (p >= 9 && j == 2 && i == 1) continue;
                        if (j == 1 && i == 1) std::cout << (p + 1);
                        else if (i == 0 || i == rozmiar + 1) std::cout << "-";
                        else if (j == 0 || j == rozmiar + 1) std::cout << "|";
                        else kolejnosc[p].czyOdsloniety ? std::cout << kolorDlaLitery(karty[kolejnosc[p].ktory][i - 1][j - 1]) << karty[kolejnosc[p].ktory][i - 1][j - 1] << zolty : std::cout << '*';
                    }
                    std::cout << ' ';
                }
                std::cout << '\n';
            }
            liczbawyswietlonych += limit_na_rzad;
            std::cout << '\n';
        }
    }

    const inline int pobierz(const long long& ileKart) {
        int liczba{};
        while (true) {
            std::cout << "(1-" << ileKart << "): ";
            if (std::cin >> liczba) {
                if (liczba > 0 && liczba <= ileKart) return liczba;
                else std::cout << czerwony << "Numer karty musi być w przedziale od 1 do " << ileKart << ". Spróbuj ponownie. ";
            }
            else {
                oczyscBledy();
                std::cout << czerwony << "Proszę wprowadzić poprawną liczbę całkowitą. ";
            }
        } std::cout << reset;
    }

    const inline void zapiszGre(const long long& ileKart, const size_t& rozmiar, const std::string& ciog_znakow, const bool& tura, const long long& ileOdkrytych, const long long* punkty, const std::vector<stanKarty>& kolejnosc, const std::vector<std::vector<std::vector<char>>>& karty, const std::string* pseudomin) const {
        std::ofstream plik("pamiec_gry_memory.txt");
        if (plik.good()) {
            plik << pseudomin[0] << ' ' << pseudomin[1] << '\n' << ileKart << ' ' << rozmiar << ' ' << ciog_znakow << '\n'<< punkty[0] << " " << punkty[1] << '\n'<< tura << '\n' << ileOdkrytych << '\n';
            for (const auto& i : kolejnosc) plik << i.ktory << ' ' << i.czyOdsloniety << '\n';
            plik << '\n';
        }
        else std::cerr << czerwony << "Błąd podczas próby zapisu!\n";
        plik.close();
    }


    const inline bool odczytajGre(long long& ileKart, long long& rozmiar, std::string& ciog_znakow, bool& tura, long long& ileOdkrytych, long long* punkty, std::vector<stanKarty>& kolejnosc, std::vector<std::vector<std::vector<char>>>& karty, std::string* pseudomin) const {
        std::ifstream plik("pamiec_gry_memory.txt");
        if (plik.good()) {
            plik >> pseudomin[0] >> pseudomin[1] >> ileKart >> rozmiar >> ciog_znakow >> punkty[0] >> punkty[1] >> tura >> ileOdkrytych;
            kolejnosc.resize(2 * ileKart);
            for (int i{}; i < 2 * ileKart; ++i) plik >> kolejnosc[i].ktory >> kolejnosc[i].czyOdsloniety;
            plik.close();
        }
        else {
            std::cerr << czerwony << "Błąd podczas próby odczytu!\n";
            plik.close();
            return false;
        }
        return true;
    }

    const inline void oczyscBledy() {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    const inline void oczyscKonsole() const {
#ifdef _WIN32
        system("cls");
#else
        std::cout << "\033[2J\033[1;1H";
#endif
    }

    const std::array<std::string, 5> kolory = {
        "\033[31m", // czerwony
        "\033[32m", // zielony
        "\033[0m",  // reset
        "\033[34m", // niebieski
        "\033[33m"  // żółty
    };


    const std::string inline kolorDlaLitery(char litera) const {
        if (koloryLitery.find(litera) != koloryLitery.end()) return koloryLitery.at(litera);
        return "\033[0m"; // Domyślny kolor (reset)
    }

    const std::string& czerwony = kolory[0];
    const std::string& zielony = kolory[1];
    const std::string& reset = kolory[2];
    const std::string& niebieski = kolory[3];
    const std::string& zolty = kolory[4];
    std::map<char, std::string> koloryLitery;
};
