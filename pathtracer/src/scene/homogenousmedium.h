//#pragma once

//#include "medium.h"

//// This uses the Henyey-Greenstein Phase Function for scattering in the medium.
//class HomogenousMedium : public Medium
//{
//public:
//    HomogenousMedium(const Color3f &sigmaA, const Color3f &sigmaS, float g) :
//        sigmaA(sigmaA), sigmaS(sigmaS), sigmaT(sigmaA + sigmaS), g(g) {}

//    Color3f Tr(const Ray &ray, Sampler &sampler) const;

//    Color3f Sample(const Ray &ray, Sampler &sampler, MediumIntersection *mi) const;

//    const Color3f sigmaA, sigmaS, sigmaT;
//    const float g;
//};
