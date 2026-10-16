// Paralelo.h - Reparte [0,n) en bloques, un hilo por bloque (COBEGIN ... COEND)
#pragma once
#include <algorithm>
#include <cstddef>
#include <thread>
#include <vector>

class Paralelo {
public:
    template <class F>
    static void rango(std::size_t n, F f) {
        if (n == 0) return;
        std::size_t h = std::max(1u, std::thread::hardware_concurrency());
        std::size_t hilos = std::min(h, n);
        std::size_t chunk = (n + hilos - 1) / hilos;
        std::vector<std::thread> ts;
        for (std::size_t t = 0; t < hilos; t++) {
            std::size_t b = t * chunk, e = std::min(n, b + chunk);
            if (b >= e) break;
            ts.emplace_back(f, b, e);
        }
        for (auto& t : ts) t.join();
    }
};
