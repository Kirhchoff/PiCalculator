#include <iostream>
#include <future>
#include <random>
#include <vector>
#include <algorithm>
#include <chrono>

using namespace std;

class PiGenerator {
public:
    float calculatePi(long long samples, int threads)
    {
        if(threads < 1 or samples < 1)
        {
            cout << endl << "Invalid input! Requested threads: " << threads << ", samples: " << samples << ".";
            return 0;
        }

        long long samplesPerThread, totalSamples;
        tie(samplesPerThread, totalSamples) = adjustSamples(samples, threads);

        vector<future<long long>> results(threads);
        generate(results.begin(), results.end(), [&](){
            return async(launch::async, [&](){
                return countPointsInsideCircle(samplesPerThread);
            });
        });

        long long totalPointsInCircle = 0;
        for_each(results.begin(), results.end(), [&](future<long long>& r){
            r.wait();
            totalPointsInCircle += r.get();
        });
        return double(totalPointsInCircle) / totalSamples * 4.0;
    }

private:
    pair<long long, long long> adjustSamples(long long samples, int threads)
    {
        long long samplesPerThread = std::max(1ll, samples/threads);
        return {samplesPerThread, samplesPerThread * threads};
    }

    struct Point {
        double x;
        double y;
    };

    long long countPointsInsideCircle(long long samples)
    {
        mt19937 rnGenerator(std::chrono::high_resolution_clock::now().time_since_epoch().count()); //using time beacause random_device doesn't work with MinGW apparently
        uniform_real_distribution<double> distribution(0.0, 1.0);

        long long pointsInside = 0;
        for(auto i = 0ll; i < samples; ++i)
        {
            if(isInsideCircle(randomPoint(rnGenerator, distribution)))
            {
                ++pointsInside;
            }
        }
        return pointsInside;
    }

    bool isInsideCircle(const Point& p) const
    {
        return p.x*p.x + p.y*p.y <= 1.0;
    }

    template<class RNEngine, class Distribution>
    Point randomPoint(RNEngine& engine, Distribution dist)
    {
        return Point{dist(engine), dist(engine)};
    }
};

int main()
{
    cout << PiGenerator{}.calculatePi(100'000'000, 4) << endl;
    return 0;
}

