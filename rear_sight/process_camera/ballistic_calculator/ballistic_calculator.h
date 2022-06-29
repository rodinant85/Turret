//
// Created by biba_bo on 2021-07-13.
//

#ifndef MACHINE_GUN_REAR_SIGHT_BALLISTIC_CALCULATOR_H
#define MACHINE_GUN_REAR_SIGHT_BALLISTIC_CALCULATOR_H

#include <fstream>
#include <vector>
#include <iostream>

#include "../rear_sight_processor/image_processing.h"

namespace BallisticConsts {
    constexpr double INITIAL_FOCAL_PIXEL_COUNT = 25.0;
    constexpr double MIN_FOCAL_PIXEL_COUNT = 1.0;
    constexpr double MAX_FOCAL_PIXEL_COUNT = 1584.0;
    constexpr double STEP_FOCAL_PIXEL = 0.20;
}

struct FireCorrection {
    int distance;
    double xCorrection;
    double yCorrection;
};

class BallisticCalculator {
private:
    double _focalPixelsCount;
    std::vector<FireCorrection> _fireCorrections;
    std::vector<FireCorrection> _defaultFireCorrections;
    int _selectedCorrectionIndex;
    std::string _fireCorrectionsLineJSON;

    void loadFromFile();
    void loadFireCorrectionsFromFile();
    void saveFireCorrectionsToFile();
    void resetFireCorrectionsLineJSON();

public:
    BallisticCalculator();

    double getFocalPixelsCount();
    void saveToFile();

    void planeDotsPlusDistance();
    void planeDotsMinusDistance();

    FireCorrection getCurrentCorrection();
    void setNextCorrection();
    void setPreviousCorrection();

    std::string getFireCorrectionsJSON();
    int getFireCorrectionIndex();
    void setFireCorrectionIndex(int index);
    void fixFireCorrection(double dx, double dy);

    void setDefaultFireCorrectionTable();
    void setDefaultFireCorrectionRecord(int index);
};


#endif //MACHINE_GUN_REAR_SIGHT_BALLISTIC_CALCULATOR_H
