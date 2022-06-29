//
// Created by biba_bo on 2021-07-13.
//

#include "ballistic_calculator.h"

BallisticCalculator::BallisticCalculator() {
    _fireCorrectionsLineJSON = "";
    loadFromFile();
    loadFireCorrectionsFromFile();
}

double BallisticCalculator::getFocalPixelsCount() {
    return _focalPixelsCount;
}

void BallisticCalculator::planeDotsPlusDistance() {
    _focalPixelsCount = (_focalPixelsCount + BallisticConsts::STEP_FOCAL_PIXEL > BallisticConsts::MAX_FOCAL_PIXEL_COUNT) ?
            BallisticConsts::MAX_FOCAL_PIXEL_COUNT : _focalPixelsCount + BallisticConsts::STEP_FOCAL_PIXEL;
}

void BallisticCalculator::planeDotsMinusDistance() {
    _focalPixelsCount = (_focalPixelsCount - BallisticConsts::STEP_FOCAL_PIXEL < BallisticConsts::MIN_FOCAL_PIXEL_COUNT) ?
            BallisticConsts::MIN_FOCAL_PIXEL_COUNT : _focalPixelsCount - BallisticConsts::STEP_FOCAL_PIXEL;
}

void BallisticCalculator::loadFromFile() {
    std::fstream myFile("src/ballistic_info.dat", std::ios_base::in);
    if (myFile >> _focalPixelsCount) {
        if (_focalPixelsCount < BallisticConsts::MIN_FOCAL_PIXEL_COUNT || _focalPixelsCount > BallisticConsts::MAX_FOCAL_PIXEL_COUNT)
            _focalPixelsCount = BallisticConsts::INITIAL_FOCAL_PIXEL_COUNT;
    }
    myFile.close();
}

void BallisticCalculator::loadFireCorrectionsFromFile() {
    _fireCorrections.clear();

    std::fstream myFile("src/basic_ballistic_corrections.dat", std::ios_base::in);
    while (1) {
        FireCorrection correction;

        if (! (myFile >> correction.distance >> correction.yCorrection >> correction.xCorrection))
            break;

        _fireCorrections.push_back(correction);
    }

    myFile.close();

    _defaultFireCorrections.clear();
    std::fstream myFile2("src/default_basic_ballistic_corrections.dat", std::ios_base::in);
    while (1) {
        FireCorrection correction;

        if (! (myFile2 >> correction.distance >> correction.yCorrection >> correction.xCorrection))
            break;

        _defaultFireCorrections.push_back(correction);
    }

    myFile2.close();

    resetFireCorrectionsLineJSON();
}

void BallisticCalculator::saveFireCorrectionsToFile() {
    std::ofstream myFile;
    myFile.open("src/basic_ballistic_corrections.dat", std::ios::out | std::ofstream::trunc);
    for (int i = 0; i < _fireCorrections.size(); i++) {
        myFile << _fireCorrections[i].distance << " " << _fireCorrections[i].yCorrection << " " << _fireCorrections[i].xCorrection << std::endl;
    }
    myFile << std::endl;
    myFile.close();
}

void BallisticCalculator::saveToFile() {
// and save to file
    std::ofstream myFile;
    myFile.open("src/ballistic_info.dat", std::ios::out | std::ofstream::trunc);
    myFile << _focalPixelsCount << "\n" << std::endl;
    myFile.close();
}

void BallisticCalculator::resetFireCorrectionsLineJSON() {
    _fireCorrectionsLineJSON = "{\"FCA\":[";
    for (int i = 0; i < _fireCorrections.size(); i++) {
        _fireCorrectionsLineJSON += "{\"d\":" + std::to_string(_fireCorrections[i].distance)
                                    + ", \"v\":" + std::to_string(_fireCorrections[i].yCorrection)
                                    + ", \"h\":" + std::to_string(_fireCorrections[i].xCorrection);
        _fireCorrectionsLineJSON += (i != (_fireCorrections.size() - 1)) ? "}, " : "}";
    }
    _fireCorrectionsLineJSON += "]}";
}

FireCorrection BallisticCalculator::getCurrentCorrection() {
    double toPixel = (_focalPixelsCount * (double) WIDTH) / ((double) WIDTH_STREAM_FRAME);
    FireCorrection correction = _fireCorrections[_selectedCorrectionIndex];
    correction.xCorrection *= toPixel;
    correction.yCorrection *= toPixel;
    return correction;
}

void BallisticCalculator::setNextCorrection() {
    _selectedCorrectionIndex = (_selectedCorrectionIndex + 1 < _fireCorrections.size()) ? _selectedCorrectionIndex + 1 : _selectedCorrectionIndex;
    std::cout << "\nCorrection: " << _fireCorrections[_selectedCorrectionIndex].distance << " "
              << _fireCorrections[_selectedCorrectionIndex].xCorrection << " "
              << _fireCorrections[_selectedCorrectionIndex].yCorrection << "\n";
}

void BallisticCalculator::setPreviousCorrection() {
    _selectedCorrectionIndex = (_selectedCorrectionIndex - 1 >= 0) ? _selectedCorrectionIndex - 1 : _selectedCorrectionIndex;
    std::cout << "\nCorrection: " << _fireCorrections[_selectedCorrectionIndex].distance << " "
              << _fireCorrections[_selectedCorrectionIndex].xCorrection << " "
              << _fireCorrections[_selectedCorrectionIndex].yCorrection << "\n";
}

std::string BallisticCalculator::getFireCorrectionsJSON() {
    return _fireCorrectionsLineJSON;
}

int BallisticCalculator::getFireCorrectionIndex() {
    return _selectedCorrectionIndex;
}

void BallisticCalculator::setFireCorrectionIndex(int index) {
    _selectedCorrectionIndex = (index < 0 || index >= _fireCorrections.size()) ? _selectedCorrectionIndex : index;
}

void BallisticCalculator::fixFireCorrection(double dx, double dy) {
    double toPixel = (_focalPixelsCount * (double) WIDTH) / ((double) WIDTH_STREAM_FRAME);
    _fireCorrections[_selectedCorrectionIndex].xCorrection += (dx / toPixel);
    _fireCorrections[_selectedCorrectionIndex].yCorrection += (dy / toPixel);
    resetFireCorrectionsLineJSON();
    saveFireCorrectionsToFile();
}

void BallisticCalculator::setDefaultFireCorrectionTable() {
    _fireCorrections.clear();
    for (int i = 0; i < _defaultFireCorrections.size(); i++) {
        _fireCorrections.push_back(_defaultFireCorrections[i]);
    }
    resetFireCorrectionsLineJSON();
    saveFireCorrectionsToFile();
}

void BallisticCalculator::setDefaultFireCorrectionRecord(int index) {
    if (index < 0 || index >= _fireCorrections.size())
        return;

    _fireCorrections[index] = _defaultFireCorrections[index];
    resetFireCorrectionsLineJSON();
    saveFireCorrectionsToFile();
}
