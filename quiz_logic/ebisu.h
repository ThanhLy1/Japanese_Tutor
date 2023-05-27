#ifndef EBISU_H
#define EBISU_H

#include <cmath>
#include <boost/math/special_functions/beta.hpp>

class Ebisu {
public:
    Ebisu(double alpha = 3.0, double beta = 1.0, double t = 1.0);

    double predictRecall(double elapsed, bool exact = false) const;
    void updateRecall(double success, double total, double elapsed);
    double modelToPercentileDecay() const;
    double percentileDecayToModel(double percentileDecay);

    double getAlpha() const;
    double getBeta() const;
    double getT() const;
    void setAlpha(double newAlpha);
    void setBeta(double newBeta);
    void setT(double newT);

private:
    double alpha;
    double beta;
    double t;
};

Ebisu::Ebisu(double alpha, double beta, double t)
    : alpha(alpha), beta(beta), t(t) {}

double Ebisu::predictRecall(double elapsed, bool exact) const {
    double recallProbability = 0.0;
    if (exact) {
        recallProbability =
            std::exp(std::log(alpha) - std::log(alpha + beta) +
                     (alpha + beta) * std::log((t + elapsed) / t));
    } else {
        recallProbability =
            1.0 / (1.0 + std::pow((t + elapsed) / t, alpha + beta));
    }
    return recallProbability;
}

void Ebisu::updateRecall(double success, double total, double elapsed) {
    double alphaPosterior = alpha + success;
    double betaPosterior = beta + total - success;
    double tPosterior =
        (alphaPosterior / (alphaPosterior + betaPosterior)) *
        ((t + elapsed) / alphaPosterior);

    alpha = alphaPosterior;
    beta = betaPosterior;
    t = tPosterior;
}

double Ebisu::modelToPercentileDecay() const {
    double percentileDecay =
        std::pow(t, 1.0 / (alpha + beta)) /
        boost::math::beta(alpha, beta);
    return percentileDecay;
}

double Ebisu::percentileDecayToModel(double percentileDecay) {
    double alphaPosterior =
        (alpha + beta) *
        (std::pow(percentileDecay * boost::math::beta(alpha, beta),
                  alpha + beta) -
         1);
    double betaPosterior = alpha * ((t / percentileDecay) - 1);
    double tPosterior =
        percentileDecay * (alphaPosterior / (alphaPosterior + betaPosterior));

    alpha = alphaPosterior;
    beta = betaPosterior;
    t = tPosterior;

    return tPosterior;
}

double Ebisu::getAlpha() const { return alpha; }

double Ebisu::getBeta() const { return beta; }

double Ebisu::getT() const { return t; }

void Ebisu::setAlpha(double newAlpha) { alpha = newAlpha; }

void Ebisu::setBeta(double newBeta) { beta = newBeta; }

void Ebisu::setT(double newT) { t = newT; }

#endif
