#ifndef BEST_TIME_MANAGER_H
#define BEST_TIME_MANAGER_H

#include <fstream>
#include <string>

class BestTimeManager {
private:
    float bestTime;
    const char* filename = "best_time.txt";

    BestTimeManager() {
        std::ifstream in(filename);
        if (in.is_open()) {
            in >> bestTime;
            in.close();
        } else {
            bestTime = -1.0f;
        }
    }

public:
    static BestTimeManager& GetInstance() {
        static BestTimeManager instance;
        return instance;
    }

    void UpdateBestTime(float newTime) {
        if (bestTime < 0 || newTime < bestTime) {
            bestTime = newTime;
            std::ofstream out(filename);
            if (out.is_open()) {
                out << bestTime;
                out.close();
            }
        }
    }

    float GetBestTime() const {
        return bestTime;
    }

    BestTimeManager(const BestTimeManager&) = delete;
    void operator=(const BestTimeManager&) = delete;
};

#endif
