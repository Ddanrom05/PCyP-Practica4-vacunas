#include "Fecha.h"
#include <algorithm>
#include <cstdio>
#include "Texto.h"
using namespace std;

Fecha::Fecha(int d, int m, int a) : d_(d), m_(m), a_(a) {}

bool Fecha::bisiesto(int a) { return (a % 4 == 0 && a % 100 != 0) || a % 400 == 0; }

int Fecha::diasMes(int m, int a) {
    static const int t[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    return (m == 2 && bisiesto(a)) ? 29 : t[m - 1];
}

bool Fecha::valida() const {
    return a_ >= 1900 && a_ <= 2100 && m_ >= 1 && m_ <= 12 && d_ >= 1 && d_ <= diasMes(m_, a_);
}

long long Fecha::dias() const {
    long long y = a_ - (m_ <= 2);
    long long era = (y >= 0 ? y : y - 399) / 400;
    long long yoe = y - era * 400;
    long long doy = (153 * (m_ + (m_ > 2 ? -3 : 9)) + 2) / 5 + d_ - 1;
    long long doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return era * 146097 + doe - 719468;
}

string Fecha::str() const {
    char b[16];
    snprintf(b, sizeof b, "%02d/%02d/%04d", d_, m_, a_);
    return b;
}

bool Fecha::parsear(const string& s0, Fecha& f) {
    auto p = Texto::dividir(Texto::trim(s0), '/');
    if (p.size() != 3) return false;
    for (auto& x : p) {
        x = Texto::trim(x);
        if (x.empty() || x.size() > 4) return false;
        for (char c : x) if (!isdigit((unsigned char)c)) return false;
    }
    if (p[2].size() != 4) return false;
    Fecha t(stoi(p[0]), stoi(p[1]), stoi(p[2]));
    if (!t.valida()) return false;
    f = t;
    return true;
}

Fecha Fecha::sumarMes() const {
    int m = m_ + 1, a = a_;
    if (m > 12) { m = 1; a++; }
    return Fecha(min(d_, diasMes(m, a)), m, a);
}

int Fecha::mesesHasta(const Fecha& ref) const {
    return (ref.a_ - a_) * 12 + (ref.m_ - m_) - (ref.d_ < d_ ? 1 : 0);
}

bool Fecha::mismoMes(const Fecha& o) const { return m_ == o.m_ && a_ == o.a_; }

bool Fecha::operator==(const Fecha& o) const { return dias() == o.dias(); }
bool Fecha::operator<(const Fecha& o) const { return dias() < o.dias(); }
bool Fecha::operator<=(const Fecha& o) const { return dias() <= o.dias(); }
bool Fecha::operator>(const Fecha& o) const { return dias() > o.dias(); }
bool Fecha::operator>=(const Fecha& o) const { return dias() >= o.dias(); }