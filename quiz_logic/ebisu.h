#ifndef EBISU_H_
#define EBISU_H_

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
    double alpha_;
    double beta_;
    double t_;
};

Ebisu::Ebisu(double alpha, double beta, double t)
    : alpha_(alpha), beta_(beta), t_(t) {}

double Ebisu::predictRecall(double elapsed, bool exact) const {
    double recall_probability = 0.0;
    if (exact) {
        recall_probability =
            std::exp(std::log(alpha_) - std::log(alpha_ + beta_) +
                     (alpha_ + beta_) * std::log((t_ + elapsed) / t_));
    } else {
        recall_probability =
            1.0 / (1.0 + std::pow((t_ + elapsed) / t_, alpha_ + beta_));
    }
    return recall_probability;
}

void Ebisu::updateRecall(double success, double total, double elapsed) {
    double alpha_posterior = alpha_ + success;
    double beta_posterior = beta_ + total - success;
    double t_posterior =
        (alpha_posterior / (alpha_posterior + beta_posterior)) *
        ((t_ + elapsed) / alpha_posterior);

    alpha_ = alpha_posterior;
    beta_ = beta_posterior;
    t_ = t_posterior;
}

double Ebisu::modelToPercentileDecay() const {
    double percentile_decay =
        std::pow(t_, 1.0 / (alpha_ + beta_)) /
        boost::math::beta(alpha_, beta_);
    return percentile_decay;
}

double Ebisu::percentileDecayToModel(double percentile_decay) {
    double alpha_posterior =
        (alpha_ + beta_) *
        (std::pow(percentile_decay * boost::math::beta(alpha_, beta_),
                  alpha_ + beta_) -
         1);
    double beta_posterior = alpha_ * ((t_ / percentile_decay) - 1);
    double t_posterior =
        percentile_decay * (alpha_posterior / (alpha_posterior + beta_posterior));

    alpha_ = alpha_posterior;
    beta_ = beta_posterior;
    t_ = t_posterior;

    return t_posterior;
}

double Ebisu::getAlpha() const { return alpha_; }

double Ebisu::getBeta() const { return beta_; }

double Ebisu::getT() const { return t_; }

void Ebisu::setAlpha(double newAlpha) { alpha_ = newAlpha; }

void Ebisu::setBeta(double newBeta) { beta_ = newBeta; }

void Ebisu::setT(double newT) { t_ = newT; }

#endif  // EBISU_H_
