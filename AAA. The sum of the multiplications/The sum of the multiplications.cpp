#include <iostream>

int main(int argc, char* argv[]) {
    int k = argc - 1;
    int* sizes = new int[k];
    for (int i = 0; i < k; ++i) {
        sizes[i] = atoi(argv[i + 1]);
    }
    int** indicators = new int* [k];
    for (int i = 0; i < k; ++i) {
        indicators[i] = new int[sizes[i]];
    }
    for (int i = 0; i < k; i++) {
        for (int j = 0; j < sizes[i]; ++j) {
            std::cin >> indicators[i][j];
        }
    }
    int* indexes = new int[k];
    for (int i = 0; i < k; ++i) {
        indexes[i] = 0;
    }
    long long sum = 0;
    while (true) {
        bool different = true;
        for (int i = 0; i < k - 1; ++i) {
            for (int j = i + 1; j < k; ++j) {
                if (indexes[i] == indexes[j]) {
                    different = false;
                    break;
                }
            }
            if (!different) {
                break;
            }
        }
        if (different) {
            long long composition = 1;
            for (int i = 0; i < k; ++i) {
                composition *= indicators[i][indexes[i]];
            }
            sum += composition;
        }
        int cnt = k - 1;
        while (cnt >= 0 && indexes[cnt] == sizes[cnt] - 1) {
            indexes[cnt] = 0;
            --cnt;
        }
        if (cnt < 0) {
            break;
        }
        ++indexes[cnt];
    }
    std::cout << sum << std::endl;
    for(int i = 0; i < k; ++i) {
        delete[] indicators[i];
    }
    delete[] indicators;
    delete[] indexes;
    delete[] sizes;
    return 0;
}