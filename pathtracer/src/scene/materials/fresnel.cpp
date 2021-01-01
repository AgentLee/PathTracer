#include "fresnel.h"

float FresnelDielectric::EvaulateF(float cosThetaI) const
{
	cosThetaI = glm::clamp(cosThetaI, -1.f, 1.f);

	bool entering = cosThetaI > 0.f;

	if (!entering) {
		cosThetaI = std::abs(cosThetaI);

		// swap etaT and etaI
	}

	float sinThetaI = std::sqrt(std::max(0.f, 1 - cosThetaI * cosThetaI));

	float sinThetaT = etaI / etaT * sinThetaI;

	// swap since you couldn't do it before
	// because etaI and etaT are const
	if (!entering) {
		sinThetaT = etaT / etaI * sinThetaI;
	}

	// Total Internal Reflection
	if (sinThetaT >= 1.f) {
		return 1.f;
	}

	float cosThetaT = std::sqrt(std::max(0.f, 1 - sinThetaT * sinThetaT));

	float Rparl = ((etaT * cosThetaI) - (etaI * cosThetaT)) / ((etaT * cosThetaI) + (etaI * cosThetaT));
	float Rperp = ((etaI * cosThetaI) - (etaT * cosThetaT)) / ((etaI * cosThetaI) + (etaT * cosThetaT));

	float fr = Rparl * Rparl;
	fr += Rperp * Rperp;
	fr /= 2.f;

	return fr;
}

Color3f FresnelDielectric::Evaluate(float cosThetaI) const
{
    return Color3f(EvaulateF(cosThetaI));
}

float FresnelConductor::EvaulateF(float cosThetaI) const
{
	// TODO
	return 0.0f;
}

Color3f FresnelConductor::Evaluate(float cosThetaI) const
{
    // FrConductor
    cosThetaI = glm::clamp(cosThetaI, -1.f, 1.f);

    Color3f eta = etaT / etaI;
    Color3f etak = k / etaI;

    float cosThetaI2 = cosThetaI * cosThetaI;
    float sinThetaI2 = 1.f - cosThetaI2;
    Color3f eta2 = eta * eta;
    Color3f etak2 = etak * etak;

    Color3f t0 = eta2 - etak2 - sinThetaI2;
    Color3f a2plusb2 = Color3f(std::sqrt((t0 * t0 + 4.f * eta2 * etak2).x),
                               std::sqrt((t0 * t0 + 4.f * eta2 * etak2).y),
                               std::sqrt((t0 * t0 + 4.f * eta2 * etak2).z));
    Color3f t1 = a2plusb2 + cosThetaI2;
    Color3f a = Color3f(std::sqrt(0.5f * (a2plusb2 + t0).x),
                        std::sqrt(0.5f * (a2plusb2 + t0).y),
                        std::sqrt(0.5f * (a2plusb2 + t0).z));
    Color3f t2 = 2.f * cosThetaI * a;
    Color3f Rs = (t1 - t2) / (t1 + t2);

    Color3f t3 = cosThetaI2 * a2plusb2 + sinThetaI2 * sinThetaI2;
    Color3f t4 = t2 * sinThetaI2;
    Color3f Rp = Rs * (t3 - t4) / (t3 + t4);

    return 0.5f * (Rp + Rs);
}
