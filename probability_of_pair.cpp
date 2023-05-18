#include <iostream>
#include <vector>
#include <string>
#include <cmath>

using namespace std;

void validateInputOrThrow(int leftSideSize, int rightSideSize, long double probabilityOfMatch, int leftSideIndex,
                          int rightSideIndex) {
    if (leftSideSize <= 0) {
        throw std::invalid_argument("leftSideSize must be > 0");
    }
    if (rightSideSize <= 0) {
        throw std::invalid_argument("rightSideSize must be > 0");
    }
    if (leftSideIndex <= 0 || leftSideIndex > leftSideSize) {
        throw std::invalid_argument("leftSideIndex must be in [1..leftSideSize]");
    }
    if (rightSideIndex <= 0 || rightSideIndex > rightSideSize) {
        throw std::invalid_argument("rightSideIndex must be in [1..rightSideSize]");
    }
}

long double
calculateProbabilitySmart(int leftSideSize, int rightSideSize, long double probabilityOfMatch, int leftSideIndex,
                          int rightSideIndex, bool verbose = false) {
    validateInputOrThrow(leftSideSize, rightSideSize, probabilityOfMatch, leftSideIndex, rightSideIndex);
    vector<vector<vector<vector<long double>>>> dpS(
            leftSideSize + 1, vector<vector<vector<long double>>>(
                    leftSideSize + 1, vector<vector<long double>>(
                            rightSideSize + 1, vector<long double>(rightSideSize + 1)
                    )
            )
    );
    vector<vector<long double>> ans(leftSideSize + 1, vector<long double>(rightSideSize + 1));
    for (int rightIndex = 1; rightIndex <= rightSideSize; rightIndex++) {
        dpS[1][1][rightIndex][rightIndex] = powl(1 - probabilityOfMatch, rightIndex);
        dpS[1][0][rightIndex][rightIndex - 1] = 1 - dpS[1][1][rightIndex][rightIndex];
        ans[1][rightIndex] = (probabilityOfMatch) * powl(1 - probabilityOfMatch, rightIndex - 1);
    }

    for (int leftIndex = 1; leftIndex <= leftSideSize; leftIndex++) {
        dpS[leftIndex][leftIndex][1][1] = powl(1 - probabilityOfMatch, leftIndex);
        dpS[leftIndex][leftIndex - 1][1][0] = 1 - dpS[leftIndex][leftIndex][1][1];
        ans[leftIndex][1] = (probabilityOfMatch) * powl(1 - probabilityOfMatch, leftIndex - 1);
    }

    for (int leftIndex = 2; leftIndex <= leftSideSize; leftIndex++) {
        for (int rightIndex = 2; rightIndex <= rightSideSize; rightIndex++) {
            for (int restLeft = 0; restLeft <= leftIndex - 1; restLeft++) {
                int restRight = rightIndex - 1 - (leftIndex - restLeft);
                if (restRight < 0) {
                    continue;
                }
                ans[leftIndex][rightIndex] +=
                        dpS[leftIndex - 1][restLeft][rightIndex - 1][restRight]
                        * powl(1 - probabilityOfMatch, restLeft)
                        * powl(1 - probabilityOfMatch, restRight)
                        * (probabilityOfMatch);
                if (restLeft > 0 && restRight > 0) {
                    /**
                     * Девушка вышла за уже представленного мужчина
                     * Мужчина вышел за уже представленную женщину
                     */
                    dpS[leftIndex][restLeft - 1][rightIndex][restRight - 1] +=
                            dpS[leftIndex - 1][restLeft][rightIndex - 1][restRight]
                            * (1 - powl(1 - probabilityOfMatch, restLeft))
                            * (1 - powl(1 - probabilityOfMatch, restRight));
                }
                if (restLeft > 0) {
                    /**
                     * Девушка вышла за уже представленного мужчина
                     * Мужчина не сматчился
                     */
                    dpS[leftIndex][restLeft][rightIndex][restRight] +=
                            dpS[leftIndex - 1][restLeft][rightIndex - 1][restRight]
                            * (1 - powl(1 - probabilityOfMatch, restLeft))
                            * (powl(1 - probabilityOfMatch, restRight));
                }
                if (restRight > 0) {
                    /**
                    * Девушка не сматчилась
                    * Мужчина вышел за уже представленную девушку
                    */
                    dpS[leftIndex][restLeft][rightIndex][restRight] +=
                            dpS[leftIndex - 1][restLeft][rightIndex - 1][restRight]
                            * (1 - powl(1 - probabilityOfMatch, restRight))
                            * (powl(1 - probabilityOfMatch, restLeft));
                }

                /**
                 * Вновь пердставленные мужчина и женщина сошлись дру с другом
                 */
                dpS[leftIndex][restLeft][rightIndex][restRight] +=
                        dpS[leftIndex - 1][restLeft][rightIndex - 1][restRight]
                        * powl(1 - probabilityOfMatch, restLeft)
                        * powl(1 - probabilityOfMatch, restRight)
                        * (probabilityOfMatch);

                /**
                * Вновь пердставленные мужчина и женщина не сошлись ни с кем
                */
                dpS[leftIndex][restLeft + 1][rightIndex][restRight + 1] +=
                        dpS[leftIndex - 1][restLeft][rightIndex - 1][restRight]
                        * powl(1 - probabilityOfMatch, restLeft)
                        * powl(1 - probabilityOfMatch, restRight)
                        * (1 - probabilityOfMatch);
            }
        }
    }

    return ans[leftSideIndex][rightSideIndex];
}

int main() {
    cout << calculateProbabilitySmart(5, 3, 0.5, 1, 1);

    return 0;
}